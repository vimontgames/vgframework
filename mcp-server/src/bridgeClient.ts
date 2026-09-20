import { promises as fs } from "node:fs";
import * as path from "node:path";

// One command line written to commands.jsonl
interface Command {
  id: number;
  tool: string;
  args: Record<string, unknown>;
}

// One entry in state.json "results"
interface CommandResult {
  id: number;
  tool: string;
  ok: boolean;
  data?: unknown;
  error?: { code: string; message: string; object_id?: string };
}

interface EngineState {
  engine?: { running?: boolean; playing?: boolean };
  lastProcessedId?: number;
  results?: CommandResult[];
}

export class BridgeError extends Error {
  code: string;
  constructor(code: string, message: string) {
    super(message);
    this.code = code;
    this.name = "BridgeError";
  }
}

/**
 * File-based transport to the in-engine mcpbridge plugin (docs/architecture.md, option B).
 * Writes <dir>/commands.jsonl (append-only) and polls <dir>/state.json for results.
 */
export class BridgeClient {
  private readonly dir: string;
  private readonly commandsPath: string;
  private readonly statePath: string;
  private nextId = 1;
  private idReady: Promise<void>;

  constructor(dir?: string) {
    this.dir = dir ?? process.env.VG_MCP_DIR ?? path.resolve(process.cwd(), "mcp");
    this.commandsPath = path.join(this.dir, "commands.jsonl");
    this.statePath = path.join(this.dir, "state.json");
    this.idReady = this.initId();
  }

  get directory(): string {
    return this.dir;
  }

  private async initId(): Promise<void> {
    // Resume id numbering above whatever is already in the command file so a
    // restarted server never reuses an id the engine may have processed.
    try {
      const text = await fs.readFile(this.commandsPath, "utf8");
      let max = 0;
      for (const line of text.split("\n")) {
        const t = line.trim();
        if (!t) continue;
        try {
          const parsed = JSON.parse(t) as Command;
          if (typeof parsed.id === "number" && parsed.id > max) max = parsed.id;
        } catch {
          /* ignore malformed line */
        }
      }
      this.nextId = max + 1;
    } catch {
      this.nextId = 1;
    }
  }

  private async readState(): Promise<EngineState | null> {
    try {
      const text = await fs.readFile(this.statePath, "utf8");
      return JSON.parse(text) as EngineState;
    } catch {
      return null;
    }
  }

  /** Trim commands.jsonl to only lines the engine has not acknowledged yet. */
  private async compactCommandFile(lastProcessedId: number): Promise<void> {
    if (lastProcessedId <= 0) return;
    let text: string;
    try {
      text = await fs.readFile(this.commandsPath, "utf8");
    } catch {
      return;
    }
    const kept: string[] = [];
    for (const line of text.split("\n")) {
      const t = line.trim();
      if (!t) continue;
      try {
        const parsed = JSON.parse(t) as Command;
        if (typeof parsed.id === "number" && parsed.id > lastProcessedId) kept.push(t);
      } catch {
        /* drop malformed */
      }
    }
    const next = kept.length ? kept.join("\n") + "\n" : "";
    await fs.writeFile(this.commandsPath, next, "utf8");
  }

  /**
   * Send a command and wait for the engine to report its result.
   * @throws BridgeError on engine errors, engine-not-running, or timeout.
   */
  async call(
    tool: string,
    args: Record<string, unknown>,
    timeoutMs = 15000,
  ): Promise<unknown> {
    await this.idReady;
    await fs.mkdir(this.dir, { recursive: true });

    const pre = await this.readState();
    if (pre?.lastProcessedId) {
      await this.compactCommandFile(pre.lastProcessedId).catch(() => {});
    }

    const id = this.nextId++;
    const line = JSON.stringify({ id, tool, args } satisfies Command) + "\n";
    await fs.appendFile(this.commandsPath, line, "utf8");

    const deadline = Date.now() + timeoutMs;
    let sawState = pre !== null;

    while (Date.now() < deadline) {
      await delay(120);
      const state = await this.readState();
      if (!state) continue;
      sawState = true;

      const hit = state.results?.find((r) => r.id === id);
      if (hit) {
        if (hit.ok) return hit.data ?? null;
        const e = hit.error;
        throw new BridgeError(e?.code ?? "UNKNOWN", e?.message ?? "command failed");
      }
    }

    if (!sawState) {
      throw new BridgeError(
        "ENGINE_NOT_RUNNING",
        `No response from the engine. Is the vgframework editor running with VG_MCP_BRIDGE set, ` +
          `and writing to "${this.statePath}"?`,
      );
    }
    throw new BridgeError("TIMEOUT", `Engine did not answer command #${id} (${tool}) within ${timeoutMs}ms`);
  }

  async status(): Promise<{ running: boolean; playing: boolean; dir: string }> {
    const state = await this.readState();
    return {
      running: state?.engine?.running === true,
      playing: state?.engine?.playing === true,
      dir: this.dir,
    };
  }
}

function delay(ms: number): Promise<void> {
  return new Promise((r) => setTimeout(r, ms));
}
