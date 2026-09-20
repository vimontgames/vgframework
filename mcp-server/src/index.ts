#!/usr/bin/env node
import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";
import { BridgeClient, BridgeError } from "./bridgeClient.js";

const client = new BridgeClient();

const vec3 = z
  .object({ x: z.number().optional(), y: z.number().optional(), z: z.number().optional() })
  .describe("Any missing component is left unchanged.");

const quat = z
  .object({ x: z.number(), y: z.number(), z: z.number(), w: z.number() })
  .describe(
    "Unit quaternion. Identity is {x:0,y:0,z:0,w:1}. The world is Z-up, so a heading (yaw) of " +
      "angle a radians around the vertical axis is {x:0, y:0, z:sin(a/2), w:cos(a/2)}. " +
      "For axis-aligned boxes, leave it out.",
  );

/** Shared world-convention preamble injected into the placement tool descriptions. */
const WORLD_CONVENTIONS =
  "WORLD CONVENTIONS: the engine is Z-UP — X and Y span the ground plane, Z is height. " +
  "Units are metres. The reference primitive is the prefab data/Prefabs/Box_Base.prefab, " +
  "whose PIVOT IS AT THE CENTRE OF ITS BASE: set position.z to the ground level and scale.z to " +
  "the full height (no half-height offset). A 8x8x20 m building is scale {x:8,y:8,z:20} at " +
  "position {z:0}. list_objects/get_transform also return bounds_local (un-scaled mesh bounds, " +
  "which show where the pivot sits) and size_world (bounds x scale, the real footprint) — read " +
  "them once before laying anything out instead of assuming dimensions.";

type ToolResult = { content: Array<{ type: "text"; text: string }>; isError?: boolean };

function ok(payload: unknown, note?: string): ToolResult {
  const body = typeof payload === "string" ? payload : JSON.stringify(payload, null, 2);
  return { content: [{ type: "text", text: note ? `${note}\n\n${body}` : body }] };
}

function fail(err: unknown): ToolResult {
  const msg = err instanceof BridgeError ? `[${err.code}] ${err.message}` : String(err);
  return { content: [{ type: "text", text: `Error: ${msg}` }], isError: true };
}

function buildTransform(a: {
  position?: Record<string, number>;
  rotation?: Record<string, number>;
  scale?: Record<string, number>;
}): Record<string, unknown> | undefined {
  const t: Record<string, unknown> = {};
  if (a.position) t.position = a.position;
  if (a.rotation) t.rotation = a.rotation;
  if (a.scale) t.scale = a.scale;
  return Object.keys(t).length ? t : undefined;
}

const server = new McpServer({
  name: "vgframework-leveldesign",
  version: "0.1.0",
});

server.registerTool(
  "engine_status",
  {
    title: "Engine status",
    description:
      "Check whether the vgframework editor is running and reachable through the MCP bridge. " +
      "Call this first if other tools time out.",
    inputSchema: {},
  },
  async () => {
    const s = await client.status();
    return ok(
      s,
      s.running
        ? "Engine is running and answering."
        : `Engine not detected. Start the vgframework editor with the VG_MCP_BRIDGE environment variable set. Exchange dir: ${s.dir}`,
    );
  },
);

server.registerTool(
  "list_objects",
  {
    title: "List scene objects",
    description:
      "Browse or search the scene graph. Returns each GameObject's stable object_id (UID), name, path, " +
      "child_count, local transform, bounds_local and size_world.\n" +
      "ALWAYS BOUNDED — a real scene holds thousands of objects, so this never dumps the whole tree:\n" +
      "  • BROWSE (default): returns only the direct children of the scene roots. Pass parent_id to step " +
      "into a node, or max_depth to go deeper. Follow child_count to know where to descend.\n" +
      "  • SEARCH: pass name_contains to walk the entire tree and return only matching names.\n" +
      "The header line lists every scene of the world with its ROOT_ID — pass that as parent_id " +
      "(to list) or as the parent of a create_group to work in a specific scene, which is the only " +
      "way to reach an empty scene since it has nothing to list.\n" +
      "Start with a plain call to see the top-level groups, then drill down or search. " +
      WORLD_CONVENTIONS,
    inputSchema: {
      name_contains: z
        .string()
        .optional()
        .describe("Case-insensitive substring on the object name. Switches to search mode (whole tree)."),
      parent_id: z
        .string()
        .optional()
        .describe("List below this object instead of the scene roots."),
      max_depth: z
        .number()
        .int()
        .optional()
        .describe("Browse mode only. 1 = direct children (default), 0 = unlimited."),
      limit: z.number().int().optional().describe("Max objects returned (default 200)."),
      detail: z
        .enum(["compact", "full"])
        .optional()
        .describe(
          "compact (default): id, name, path, child_count — one line per object. " +
            "full: adds transform, bounds_local and size_world, which is ~40 lines of JSON per " +
            "object, so only ask for it on a handful of objects (or use get_transform).",
        ),
    },
  },
  async (args) => {
    try {
      const req: Record<string, unknown> = {};
      if (args.name_contains) req.name_contains = args.name_contains;
      if (args.parent_id) req.parent_id = args.parent_id;
      if (args.max_depth !== undefined) req.max_depth = args.max_depth;
      if (args.limit !== undefined) req.limit = args.limit;
      if (args.detail) req.detail = args.detail;

      const data = (await client.call("list_objects", req)) as {
        scenes?: Array<{ name: string; root_id?: string; child_count?: number }>;
        returned?: number;
        matched?: number;
        truncated?: boolean;
        mode?: string;
        detail?: string;
        objects?: Array<{ object_id: string; name: string; path?: string; child_count?: number }>;
      };

      const lines = (data.objects ?? [])
        .map(
          (o) =>
            `  ${o.object_id.padStart(10)}  ${o.path ?? o.name}` +
            (o.child_count ? `  (${o.child_count} children)` : ""),
        )
        .join("\n");

      const where = data.scenes?.length
        ? `scene(s) ${data.scenes
            .map((s) => `${s.name} [root ${s.root_id ?? "?"}, ${s.child_count ?? 0} children]`)
            .join(", ")}`
        : "subtree";
      const head =
        `${data.mode ?? "browse"} in ${where} — ${data.returned ?? 0} of ${data.matched ?? 0} match(es)` +
        (data.truncated ? " (truncated, raise limit or narrow the search)" : "");

      // In compact mode the one-line-per-object rendering IS the answer: dumping the raw
      // JSON as well is what used to blow the tool's token budget past ~170 objects.
      if (data.detail !== "full") return ok(`${head}:\n${lines}`);

      return ok(data, `${head}:\n${lines}`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "get_transform",
  {
    title: "Get object transform",
    description:
      "Return the local transform of one object (position, rotation quaternion, scale) plus its bounds_local " +
      "and size_world. " + WORLD_CONVENTIONS,
    inputSchema: { object_id: z.string().describe("Stable object id (UID) from list_objects") },
  },
  async ({ object_id }) => {
    try {
      return ok(await client.call("get_transform", { object_id }));
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "set_transform",
  {
    title: "Set object transform",
    description:
      "Update the local transform of one existing object. position / rotation / scale are independently optional; " +
      "omitted parts (and omitted x/y/z components) keep their current value. Non-uniform scale on the reference " +
      "box produces any rectangular volume. " + WORLD_CONVENTIONS,
    inputSchema: {
      object_id: z.string().describe("Stable object id (UID) from list_objects"),
      position: vec3.optional(),
      rotation: quat.optional(),
      scale: vec3.optional(),
    },
  },
  async ({ object_id, position, rotation, scale }) => {
    try {
      const args: Record<string, unknown> = { object_id };
      if (position) args.position = position;
      if (rotation) args.rotation = rotation;
      if (scale) args.scale = scale;
      return ok(await client.call("set_transform", args));
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "spawn_object",
  {
    title: "Spawn (clone) an object",
    description:
      "Clone an existing object (typically a Box_Base instance already in the scene) and place the copy. " +
      "Returns the new object_id. By default the clone is parented next to the source; pass parent_id (a " +
      "create_group id) to file it under a group. Use spawn_objects instead as soon as you need more than " +
      "one or two copies. Newly spawned objects only get a persistent id once the world is saved " +
      "(see save_world). " + WORLD_CONVENTIONS,
    inputSchema: {
      source_id: z.string().describe("object_id of the object to clone (e.g. the 1x1x1 reference box)"),
      name: z.string().optional().describe("Name for the new object"),
      parent_id: z.string().optional().describe("object_id of the parent (defaults to the source's parent)"),
      position: vec3.optional(),
      rotation: quat.optional(),
      scale: vec3.optional(),
    },
  },
  async ({ source_id, name, parent_id, position, rotation, scale }) => {
    try {
      const args: Record<string, unknown> = { source_id };
      if (name) args.name = name;
      if (parent_id) args.parent_id = parent_id;
      const t = buildTransform({ position, rotation, scale });
      if (t) args.transform = t;
      return ok(await client.call("spawn_object", args));
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "spawn_objects",
  {
    title: "Spawn many clones at once",
    description:
      "Batch version of spawn_object: one call clones the same source N times, each with its own name and " +
      "placement. ALWAYS PREFER THIS over repeated spawn_object calls when laying out more than a couple of " +
      "boxes — it is one engine round trip instead of N, and avoids the timeouts that parallel single spawns " +
      "hit. Pair it with create_group + parent_id so the result lands under one node instead of hundreds of " +
      "siblings at the scene root. Positions are relative to the parent. " +
      WORLD_CONVENTIONS,
    inputSchema: {
      source_id: z.string().describe("object_id of the object to clone (e.g. the 1x1x1 reference box)"),
      parent_id: z
        .string()
        .optional()
        .describe("object_id of the parent for every clone (defaults to the source's parent). Use a create_group id."),
      items: z
        .array(
          z.object({
            name: z.string().optional().describe("Name for this clone"),
            position: vec3.optional(),
            rotation: quat.optional(),
            scale: vec3.optional(),
          }),
        )
        .min(1)
        .describe("One entry per clone to create."),
    },
  },
  async ({ source_id, parent_id, items }) => {
    try {
      const args: Record<string, unknown> = { source_id, items };
      if (parent_id) args.parent_id = parent_id;

      const data = (await client.call("spawn_objects", args, 60000)) as {
        count?: number;
        parent_id?: string;
        spawned?: Array<{ object_id: string; name: string }>;
        failed?: Array<{ name?: string; error?: { code: string; message: string } }>;
      };

      const lines = (data.spawned ?? [])
        .map((o) => `  ${o.object_id.padStart(10)}  ${o.name}`)
        .join("\n");
      const head = `spawned ${data.count ?? 0} object(s) under parent ${data.parent_id ?? "?"}`;
      const failures = data.failed?.length
        ? `\n${data.failed.length} failed: ${data.failed
            .map((f) => `${f.name ?? "?"} (${f.error?.code ?? "?"})`)
            .join(", ")}`
        : "";

      return ok(`${head}:\n${lines}${failures}`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "create_group",
  {
    title: "Create an empty group node",
    description:
      "Create an empty GameObject to be used as a folder, then pass its object_id as parent_id to " +
      "spawn_object / spawn_objects so generated content is grouped by category (one node per district, " +
      "building, layer...) instead of hundreds of objects flat at the scene root. Children's positions are " +
      "relative to the group, so a group left at the origin keeps child coordinates equal to world " +
      "coordinates. Groups can be nested by passing another group's id as parent_id. " +
      WORLD_CONVENTIONS,
    inputSchema: {
      name: z.string().describe("Name of the group node"),
      parent_id: z
        .string()
        .optional()
        .describe("object_id of the parent (defaults to the root of the first scene)"),
      position: vec3.optional(),
      rotation: quat.optional(),
      scale: vec3.optional(),
    },
  },
  async ({ name, parent_id, position, rotation, scale }) => {
    try {
      const args: Record<string, unknown> = { name };
      if (parent_id) args.parent_id = parent_id;
      if (position) args.position = position;
      if (rotation) args.rotation = rotation;
      if (scale) args.scale = scale;

      const data = (await client.call("create_group", args)) as { object_id?: string; name?: string };
      return ok(`group "${data.name}" created — object_id ${data.object_id} (use it as parent_id)`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "create_scene",
  {
    title: "Create a new scene",
    description:
      "Add a new empty scene to the world — the editor's SceneList > New Scene. Use it to build " +
      "something in its own scene instead of dropping it into the level the user is working on. " +
      "Writes data/Scenes/<name>.scene (refuses to overwrite an existing file or a scene name " +
      "already loaded) and the scene appears in the editor's scene list.\n" +
      "TWO FOLLOW-UPS MATTER: the scene loads asynchronously, so call list_objects right after to " +
      "read its root_id from the scenes header, and use that id as parent_id for everything you " +
      "put in it; and the world only remembers the new scene after save_world.",
    inputSchema: {
      name: z.string().describe("Scene name, also the file name (no path, no extension). E.g. \"City\""),
      folder: z
        .string()
        .optional()
        .describe("Destination folder, relative to the project (default \"data/Scenes\")"),
    },
  },
  async ({ name, folder }) => {
    try {
      const args: Record<string, unknown> = { name };
      if (folder) args.folder = folder;
      const data = (await client.call("create_scene", args)) as { name?: string; file?: string };
      return ok(
        `scene "${data.name}" created (${data.file}). It loads asynchronously: call list_objects to ` +
          `read its root_id, then save_world to record it in the world file.`,
      );
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "create_world",
  {
    title: "Create a new world",
    description:
      "Create a new empty .world file and open it in the editor — the editor's File > World > New. " +
      "Writes data/Worlds/<name>.world (refuses to overwrite an existing file).\n" +
      "THIS REPLACES THE WORLD THE EDITOR CURRENTLY HAS OPEN: the engine holds a single world at a " +
      "time, so unsaved changes to the current world are lost and getting back to it means File > " +
      "World > Open in the editor. Ask the user before calling it. The previous .world file on disk " +
      "is not modified, and neither is the world the standalone game loads at startup (Engine.xml's " +
      "Start World), so the game keeps working as before.\n" +
      "The new world loads asynchronously and starts with no scene: call create_scene right after, " +
      "retrying while it answers WORLD_NOT_READY, then save_world.",
    inputSchema: {
      name: z.string().describe("World name, also the file name (no path, no extension). E.g. \"City\""),
      folder: z
        .string()
        .optional()
        .describe("Destination folder, relative to the project (default \"data/Worlds\")"),
    },
  },
  async ({ name, folder }) => {
    try {
      const args: Record<string, unknown> = { name };
      if (folder) args.folder = folder;
      const data = (await client.call("create_world", args)) as {
        name?: string;
        file?: string;
        previous_world?: string;
      };
      return ok(
        `world "${data.name}" created (${data.file}) and opened in the editor, replacing ` +
          `"${data.previous_world}". It is empty and loads asynchronously: call create_scene ` +
          `(retry while it answers WORLD_NOT_READY), then save_world.`,
      );
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "delete_object",
  {
    title: "Delete objects",
    description:
      "Remove one or several objects from the scene, with their whole subtree — deleting a group deletes " +
      "everything under it. Give object_id for one, or object_ids for a batch. " +
      "THIS CANNOT BE UNDONE from the editor (no Ctrl-Z entry is created) and the change only reaches disk " +
      "on the next save_world. Scene root objects cannot be deleted.",
    inputSchema: {
      object_id: z.string().optional().describe("Single object to delete"),
      object_ids: z.array(z.string()).optional().describe("Batch of objects to delete"),
    },
  },
  async ({ object_id, object_ids }) => {
    try {
      const args: Record<string, unknown> = {};
      if (object_ids?.length) args.object_ids = object_ids;
      else if (object_id) args.object_id = object_id;
      else return fail(new BridgeError("INVALID_VALUE", "give object_id or object_ids"));

      const data = (await client.call("delete_object", args, 60000)) as {
        count?: number;
        deleted?: Array<{ object_id: string; name: string }>;
        failed?: Array<{ code?: string; message?: string; object_id?: string }>;
      };

      const lines = (data.deleted ?? []).map((o) => `  ${o.object_id.padStart(10)}  ${o.name}`).join("\n");
      const failures = data.failed?.length
        ? `\n${data.failed.length} failed: ${data.failed
            .map((f) => `${f.object_id ?? "?"} (${f.code ?? "?"}: ${f.message ?? ""})`)
            .join(", ")}`
        : "";

      return ok(`deleted ${data.count ?? 0} object(s):\n${lines}${failures}`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "save_world",
  {
    title: "Save the world and its scenes",
    description:
      "Persist every scene of the world and the world file itself to disk — the editor's 'Save All'. " +
      "Nothing the bridge creates or edits survives a reload until this is called, and newly spawned " +
      "objects only get a frozen id here. Overwrites the .scene files on disk, so ask before calling it " +
      "on a scene the user cares about.",
    inputSchema: {},
  },
  async () => {
    try {
      const data = (await client.call("save_world", {}, 60000)) as {
        saved?: boolean;
        world_saved?: boolean;
        scenes_saved?: number;
        scenes?: Array<{ file: string; saved: boolean }>;
      };
      const files = (data.scenes ?? []).map((s) => `  ${s.saved ? "ok  " : "FAIL"}  ${s.file}`).join("\n");
      return ok(
        `${data.saved ? "saved" : "PARTIAL SAVE"} — world file ${data.world_saved ? "ok" : "FAILED"}, ` +
          `${data.scenes_saved ?? 0} scene(s):\n${files}`,
      );
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "set_material",
  {
    title: "Assign a material",
    description:
      "Point the material slots of an object's MeshComponent at a .mat file. Without this every clone keeps " +
      "the material of whatever it was cloned from, so anything built out of one reference box comes out " +
      "monochrome — use it to vary roofs, walls, props.\n" +
      "The MeshComponent is looked up in the CHILDREN too, which is where it sits on a prefab instance. " +
      "Omit `slot` to paint every slot of the mesh, or give it to target one material ID (a mesh with 2 IDs " +
      "has slots 0 and 1 — list_objects/create_prefabs tell you how many). Takes object_ids for a batch.\n" +
      "Nothing reaches disk until save_world.",
    inputSchema: {
      object_id: z.string().optional().describe("Object to repaint"),
      object_ids: z.array(z.string()).optional().describe("Batch of objects to repaint"),
      material: z
        .string()
        .describe("Path to the .mat, relative to the project. E.g. \"data/Materials/Grass/Grass_Plastic.mat\""),
      slot: z
        .number()
        .int()
        .min(0)
        .optional()
        .describe("Material slot (material ID) to set. Omit to set them all."),
    },
  },
  async ({ object_id, object_ids, material, slot }) => {
    try {
      const args: Record<string, unknown> = { material };
      if (object_ids?.length) args.object_ids = object_ids;
      else if (object_id) args.object_id = object_id;
      else return fail(new BridgeError("INVALID_VALUE", "give object_id or object_ids"));
      if (slot !== undefined) args.slot = slot;

      const data = (await client.call("set_material", args, 60000)) as { count?: number };
      return ok(data, `${data.count ?? 0} object(s) now use ${material}.`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "set_enabled",
  {
    title: "Show or hide objects",
    description:
      "Enable or disable an object and its whole subtree — the editor's checkbox. Every scene of a world " +
      "shares one coordinate space and they are drawn together, so disabling the scene root of a level you " +
      "are not working on is the way to stop it sitting on top of what you build (pass its root_id, which " +
      "list_objects reports in the scenes header).\n" +
      "A child of a disabled parent stays invisible, so the result reports enabled_in_hierarchy as well. " +
      "Nothing reaches disk until save_world.",
    inputSchema: {
      object_id: z.string().optional().describe("Object to show or hide"),
      object_ids: z.array(z.string()).optional().describe("Batch of objects"),
      enabled: z.boolean().describe("true to show, false to hide"),
    },
  },
  async ({ object_id, object_ids, enabled }) => {
    try {
      const args: Record<string, unknown> = { enabled };
      if (object_ids?.length) args.object_ids = object_ids;
      else if (object_id) args.object_id = object_id;
      else return fail(new BridgeError("INVALID_VALUE", "give object_id or object_ids"));

      const data = (await client.call("set_enabled", args, 60000)) as { count?: number };
      return ok(data, `${data.count ?? 0} object(s) ${enabled ? "enabled" : "disabled"}.`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "select_object",
  {
    title: "Select objects in the editor",
    description:
      "Put objects in the editor's selection, so what the bridge just built can be framed with the editor's " +
      "own focus shortcut and inspected in the Inspector. Use it to show the user the result of a batch of " +
      "edits instead of describing where to look.\n" +
      "This is the reachable half of \"point the camera at it\": the viewport camera is private editor state " +
      "re-fed to the view every frame, so the bridge cannot move it without modifying the editor.\n" +
      "Pass add:true to extend the current selection, or clear:true with no id to deselect everything.",
    inputSchema: {
      object_id: z.string().optional().describe("Object to select"),
      object_ids: z.array(z.string()).optional().describe("Batch of objects to select"),
      add: z.boolean().optional().describe("Add to the current selection instead of replacing it"),
      clear: z.boolean().optional().describe("With no id: clear the selection"),
    },
  },
  async ({ object_id, object_ids, add, clear }) => {
    try {
      const args: Record<string, unknown> = {};
      if (object_ids?.length) args.object_ids = object_ids;
      else if (object_id) args.object_id = object_id;
      else if (clear) args.clear = true;
      else return fail(new BridgeError("INVALID_VALUE", "give object_id, object_ids, or clear:true"));
      if (add) args.add = true;

      const data = (await client.call("select_object", args, 60000)) as { count?: number; cleared?: boolean };
      return ok(data, data.cleared ? "selection cleared." : `${data.count ?? 0} object(s) selected.`);
    } catch (e) {
      return fail(e);
    }
  },
);

server.registerTool(
  "instantiate_prefab",
  {
    title: "Place a prefab in the scene",
    description:
      "Put a .prefab file into the scene — the editor's Add Prefab. This is what spawn_object CANNOT do: " +
      "spawn_object only clones an object already present in the world, so a prefab with no existing instance " +
      "was unreachable. Use this to lay out a prefab library (or anything create_prefabs produced).\n" +
      "Give `items` to place several at once in a single engine round trip, or a single position/rotation/scale. " +
      "Positions are relative to the parent, so pair it with create_group (or pass a scene root_id as parent_id) " +
      "to keep the result tidy.\n" +
      "The prefab CONTENT loads asynchronously: the object and its transform exist on return, its children appear " +
      "a moment later — call list_objects again to see them. Nothing reaches disk until save_world.\n" +
      "WORLD CONVENTIONS: Z-UP (X and Y span the ground, Z is height), metres.",
    inputSchema: {
      prefab: z
        .string()
        .describe("Path to the .prefab, relative to the project. E.g. \"data/Prefabs/Giraphon/Giraphon.prefab\""),
      parent_id: z
        .string()
        .optional()
        .describe("object_id of the parent (defaults to the first scene root — pass a scene root_id to target another scene)"),
      name: z.string().optional().describe("Name of the instance (defaults to the prefab file name)"),
      position: vec3.optional(),
      rotation: quat.optional(),
      scale: vec3.optional(),
      items: z
        .array(
          z.object({
            name: z.string().optional(),
            position: vec3.optional(),
            rotation: quat.optional(),
            scale: vec3.optional(),
          }),
        )
        .optional()
        .describe("Batch: one entry per instance. Replaces the single placement above."),
    },
  },
  async ({ prefab, parent_id, name, position, rotation, scale, items }) => {
    try {
      const args: Record<string, unknown> = { prefab };
      if (parent_id) args.parent_id = parent_id;
      if (items?.length) args.items = items;
      else {
        if (name) args.name = name;
        if (position) args.position = position;
        if (rotation) args.rotation = rotation;
        if (scale) args.scale = scale;
      }

      const data = (await client.call("instantiate_prefab", args, 60000)) as {
        count?: number;
        created?: Array<{ object_id: string; name: string }>;
        parent_id?: string;
      };

      return ok(
        data,
        `${data.count ?? 0} instance(s) of ${prefab} placed. Content loads asynchronously — ` +
          `call list_objects again to see the children, and save_world to persist.`,
      );
    } catch (e) {
      return fail(e);
    }
  },
);

type CreatePrefabsResult = {
  staged?: number;
  pending?: number;
  created?: Array<{
    mesh?: string;
    prefab?: string;
    written?: boolean;
    materials?: Array<{ name?: string; file?: string; albedo?: string; normal?: string; pbr?: string }>;
  }>;
  warnings?: string[];
};

server.registerTool(
  "create_prefabs",
  {
    title: "Create prefabs from FBX files",
    description:
      "Turn every .fbx found in a folder into a ready-to-use prefab: data/Prefabs/<Mesh>/<Mesh>.prefab " +
      "holding a MeshComponent, plus one data/Materials/<Mesh>/<Material>.mat per material ID of the FBX. " +
      "Accepts either a single mesh folder (data/Meshes/Giraphon) or a folder holding one folder per mesh.\n" +
      "Material names come from the FBX itself, and textures are picked up NEXT TO THE FBX by suffix: " +
      "_BaseColor/_Color/_Albedo -> Albedo, _Normal -> Normal, _OcclusionRoughnessMetallic/_ORM/_PBR -> PBR. " +
      "A material whose name matches no texture is still written, with a warning - rename the FBX material " +
      "or the textures so they agree. Each .mat uses the Default shader, Opaque, cull Back, UV0, tiling 1, offset 0.\n" +
      "NOTHING IS OVERWRITTEN by default: an existing file is written as <name>-01, -02... so the result can be " +
      "compared with a hand-made asset. Pass overwrite:true to replace in place instead.\n" +
      "Importing an FBX is asynchronous, so this polls the engine until every model has resolved.",
    inputSchema: {
      folder: z
        .string()
        .describe("Folder to scan, relative to the project. E.g. \"data/Meshes/Giraphon\""),
      prefab_folder: z
        .string()
        .optional()
        .describe("Where prefabs are written (default \"data/Prefabs\")"),
      material_folder: z
        .string()
        .optional()
        .describe("Where materials are written (default \"data/Materials\")"),
      overwrite: z
        .boolean()
        .optional()
        .describe("Replace existing files instead of adding a -01 suffix (default false)"),
    },
  },
  async ({ folder, prefab_folder, material_folder, overwrite }) => {
    try {
      const args: Record<string, unknown> = { folder };
      if (prefab_folder) args.prefab_folder = prefab_folder;
      if (material_folder) args.material_folder = material_folder;
      if (overwrite) args.overwrite = true;

      const first = (await client.call("create_prefabs", args, 60000)) as CreatePrefabsResult;
      const staged = first.staged ?? 0;

      // The first call only registers the meshes; importing happens on engine frames.
      const created: NonNullable<CreatePrefabsResult["created"]> = [];
      const warnings: string[] = [...(first.warnings ?? [])];
      let pending = first.pending ?? 0;

      const deadlineMs = Date.now() + 120000;
      while (pending > 0 && Date.now() < deadlineMs) {
        await new Promise((r) => setTimeout(r, 1000));
        const next = (await client.call("create_prefabs", args, 60000)) as CreatePrefabsResult;
        created.push(...(next.created ?? []));
        warnings.push(...(next.warnings ?? []));
        pending = next.pending ?? 0;
      }

      if (pending > 0)
        return fail(
          new BridgeError(
            "ENGINE_BUSY",
            `${pending} mesh(es) still importing after 120 s — call create_prefabs again to finish them`,
          ),
        );

      const note =
        `${created.length} prefab(s) written from ${staged} FBX` +
        (warnings.length ? `, ${warnings.length} warning(s)` : "");

      return ok({ created, warnings }, note);
    } catch (e) {
      return fail(e);
    }
  },
);

async function main(): Promise<void> {
  const transport = new StdioServerTransport();
  await server.connect(transport);
  // stderr is safe for logs; stdout is the MCP channel.
  console.error(`[vgframework-mcp] ready. Exchange dir: ${client.directory}`);
}

main().catch((err) => {
  console.error("[vgframework-mcp] fatal:", err);
  process.exit(1);
});
