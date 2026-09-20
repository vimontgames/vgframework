# vgframework MCP server — level design

MCP server that lets Claude Code drive the **active scene** of the vgframework editor
for level-design: list objects, read/write transforms, clone the reference box.

See `../docs/architecture.md` for the design and `../docs/data-contract.md` for the schema.

## How it works

```
Claude Code ──stdio──> this server ──writes──> <dir>/commands.jsonl
                                     <──reads── <dir>/state.json
                                                      ▲       │
                                        the mcpbridge plugin, in-process in the editor
```

`<dir>` defaults to `mcp/` under the process working directory; override with `VG_MCP_DIR`.
Both sides must point at the **same** folder.

## Setup

```bash
cd mcp-server
npm install
npm run build
```

### 1. Enable the bridge in the engine

The `mcpbridge` plugin is compiled into every build but stays completely inert unless
the environment variable `VG_MCP_BRIDGE` is set when the editor starts:

```bat
set VG_MCP_BRIDGE=1
vgframework_win64_msvc_dx12_release.exe        ::  (or editor.exe)
```

On startup the log shows: `[MCPBridge] enabled - watching "…/mcp/commands.jsonl"`.

### 2. Register the server with Claude Code

```bash
claude mcp add vgframework-leveldesign -- node /absolute/path/to/mcp-server/dist/index.js
```

Set `VG_MCP_DIR` in the command if the editor's working directory is not the repo root:

```bash
claude mcp add vgframework-leveldesign --env VG_MCP_DIR=D:/GitHUB_Repo/mcp -- node .../dist/index.js
```

## Tools

| Tool | Params | Purpose |
|---|---|---|
| `engine_status` | — | Is the editor running and reachable? |
| `list_objects` | `name_contains?`, `parent_id?`, `max_depth?`, `limit?` | Browse one level at a time, or search the whole tree by name |
| `get_transform` | `object_id` | One object's local transform |
| `set_transform` | `object_id`, `position?`, `rotation?`, `scale?` | Move / rotate / resize an existing object |
| `spawn_object` | `source_id`, `name?`, `parent_id?`, `position?`, `rotation?`, `scale?` | Clone an object (e.g. the 1×1×1 box) |
| `save_world` | — | Persist the world (freezes ids, keeps edits across reload) |

`position` / `scale` are `{x?,y?,z?}` — missing components are left unchanged.
`rotation` is a unit quaternion `{x,y,z,w}`; omit it for axis-aligned boxes.

## World conventions (important)

- The engine world is **Z-UP**: X and Y span the ground plane, **Z is height**. Units are metres.
  (The FBX importer's `target_axes.up = POSITIVE_Y` is an import-time conversion setting — it does
  *not* describe the runtime world. Gravity is `(0, 0, -9.81)`.)
- Reference primitive: **`data/Prefabs/Box_Base.prefab`**, whose **pivot is at the centre of its base**.
  So `position.z` is the ground level and `scale.z` is the full height — no half-height offset.
  A 8 × 8 × 20 m building is `scale {x:8, y:8, z:20}` at `position {z:0}`.
- Yaw of angle `a` around the vertical axis: `rotation {x:0, y:0, z:sin(a/2), w:cos(a/2)}`.
- `list_objects` / `get_transform` also return `bounds_local` (un-scaled mesh bounds — shows where the
  pivot sits) and `size_world` (`bounds × scale`, the real footprint). **Read them before laying out**
  rather than assuming dimensions.

## Notes / limitations (V1)

- Transform is **local** (relative to parent).
- Newly `spawn_object`-ed objects only get a stable `object_id` after `save_world`.
- `list_objects` is always bounded (see the table above): a full recursive listing of a real
  scene is ~9.7 MB of JSON. Default is one level deep; use `name_contains` to search.
- Rotation is validated: a +45° yaw sent through the bridge reads back as +45° in the editor's
  own Inspector (which decomposes via the engine's `Float4x4ToTRS`).
- If the editor asserts on `ScreenSpaceAmbient - Editor 0` at startup, that is an upstream WIP
  GTAO issue, not the bridge — see `../docs/PHASE3-4-IMPLEMENTATION.md` § "Piège connu".
- Latency is one engine frame + file I/O (fine for batch placement, not for dragging).
