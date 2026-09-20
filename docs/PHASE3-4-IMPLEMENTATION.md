# Phases 3-4 — Implémentation (module moteur + serveur MCP)

État : **compilé Debug + Release, 0 erreur / 0 warning** (solution complète) et **validé contre le moteur réel**
via le protocole MCP complet sur la scène `Aiguelongue` (7832 objets) — 2026-09-06.
Reste à jouer : le scénario « plan de ville » depuis Claude Code (§ Phase 5).

---

## Fichiers ajoutés (100 % additifs)

### Module moteur — `src/mcpbridge/`
| Fichier | Rôle |
|---|---|
| `IMCPBridge.h` | Interface plugin (`core::IPlugin`) : `Init(IEngine*, Singletons&)`, `Deinit()`, `Tick()`, `IsEnabled()` |
| `MCPBridge.{h,cpp}` | Plugin + `core::Singleton`. Transport fichier : poll `mcp/commands.jsonl` (mtime), écrit `mcp/state.json` (atomique via `MoveFileEx`). Gate runtime : variable d'env `VG_MCP_BRIDGE` |
| `SceneBridge.{h,cpp}` | Accès scène : `list_objects`, `get_transform`, `set_transform`, `spawn_object`, `save_world`. Résolution d'id via `core::IFactory::FindByUID`. Décomposition/recomposition matricielle maison (position + scale exacts) |
| `Json.{h,cpp}` | Mini-JSON autonome (parse + serialize), sous-ensemble du contrat |
| `Precomp.{h,cpp}`, `mcpbridge.def` | Boilerplate plugin (mirroir de `src/physics`, `src/audio`) |

### Build
| Fichier | Rôle |
|---|---|
| `sharpmake/vg.mcpbridge.sharpmake.cs` | **Nouveau** projet `MCPBridge` (`Type.DynamicLibrary`, dépend de `Core`). Auto-inclus par `main.sharpmake.cs` |

### Serveur MCP — `mcp-server/` (Node + TypeScript)
| Fichier | Rôle |
|---|---|
| `src/bridgeClient.ts` | Transport fichier côté serveur : écrit `commands.jsonl`, poll `state.json`, compacte le fichier de commandes, gère `ENGINE_NOT_RUNNING` / `TIMEOUT` |
| `src/index.ts` | Serveur `@modelcontextprotocol/sdk` (stdio). 6 tools : `engine_status`, `list_objects`, `get_transform`, `set_transform`, `spawn_object`, `save_world` |
| `package.json`, `tsconfig.json`, `README.md` | Setup |

## Fichiers existants modifiés (additif uniquement — aucune ligne existante supprimée/altérée)

| Fichier | Diff |
|---|---|
| `sharpmake/vg.solution.sharpmake.cs` | +1 ligne : `conf.AddProject<MCPBridge>(target);` |
| `sharpmake/vg.engine.sharpmake.cs` | +1 ligne : `conf.Defines.Add("VG_ENABLE_MCPBRIDGE");` |
| `sharpmake/vg.data.sharpmake.cs` | +1 ligne : exclusion `mcpbridge` du projet utilitaire `Version` (comme tous les autres modules) |
| `src/engine/Engine.cpp` | +34 lignes, 5 blocs, **tous `#if VG_ENABLE_MCPBRIDGE`** : include, pointeur statique, create+Init (si `getenv("VG_MCP_BRIDGE")`), `Tick()` dans `RunOneFrame`, `Deinit()` |
| `.gitignore` | +1 : `mcp/` |

Retirer la ligne de `vg.engine.sharpmake.cs` + celle de `vg.solution.sharpmake.cs` et régénérer ⇒ le moteur est identique à l'état d'origine (le `#if` neutralise tout le code dans `Engine.cpp`).

---

## Boucle d'intégration

`Engine::RunOneFrame()` → (après les updates, avant `m_editor->RunOneFrame()`) → `g_mcpBridge->Tick()`.
Appelé **chaque frame en mode éditeur, y compris hors Play**. `Tick()` ne fait rien tant que
`mcp/commands.jsonl` n'a pas changé de date de modification (coût quasi nul).

## Protocole (rappel, détail dans `docs/data-contract.md`)

- `mcp/commands.jsonl` : append-only, une commande JSON/ligne `{id, tool, args}`. Écrit par le serveur, lu par le moteur.
- `mcp/state.json` : `{engine:{running,playing}, lastProcessedId, results:[{id,tool,ok,data|error}]}`. Écrit par le moteur (atomique), lu par le serveur.
- Le serveur compacte `commands.jsonl` (supprime les lignes `id <= lastProcessedId`) avant chaque envoi.
- Testé de bout en bout avec un faux moteur (script Node) : list / set / propagation d'erreur ✅.

---

## Piège connu — l'éditeur crashe au démarrage (GTAO)

**Symptôme** : au lancement de `editor.exe` / `vgframework_win64_msvc_dx12_*.exe`, deux assertions puis crash :
`Texture resource "ScreenSpaceAmbient - Editor 0" does not exist in FrameGraph` (`FrameGraph.cpp:164`)
puis `RWTexture "ScreenSpaceAmbient - Editor 0" does not exist in FrameGraph` (`UserPass.hpp:230`).

**Cause** : fonctionnalité amont **en chantier** (commits `9c8c688a SSAO WIP`, `9715a2b1 GTAO`), activée par
défaut dans l'état committé. `LitView::RegisterFrameGraph` (`LitView.hpp:142-144`) n'ajoute la passe qui
**crée** la texture que si `m_lightingMode == Deferred` **et** `GetScreenSpaceAmbient() != None` ; quand la
ressource manque au `Render` alors que la condition est vraie, le `Setup` de la passe SSA n'a pas tourné.

**Sans rapport avec le bridge MCP** : sans la variable d'environnement `VG_MCP_BRIDGE`, `g_mcpBridge` reste
`nullptr` et les trois points d'appel dans `Engine.cpp` sont des branches mortes.

**Contournement appliqué** : `Editor.xml:35` `m_screenSpaceAmbient` **`GTAO` → `None`** (vérifié : l'éditeur
se lance normalement). Pour revenir en arrière : `git checkout -- Editor.xml`.

---

## Phase 5 — mode d'emploi

```powershell
# 1. serveur MCP (une fois)
cd D:\GitHUB_Repo\mcp-server ; npm install ; npm run build

# 2. enregistrer le serveur dans Claude Code (une fois)
#    -e et non --env ; le VG_MCP_DIR doit pointer le MÊME dossier que celui du moteur
claude mcp add vgframework -e VG_MCP_DIR=D:/GitHUB_Repo/mcp -- node D:/GitHUB_Repo/mcp-server/dist/index.js
#    puis REDÉMARRER Claude Code : une session déjà lancée ne voit pas un serveur ajouté après coup

# 3. lancer l'éditeur AVEC le bridge (à chaque session)
cd D:\GitHUB_Repo ; $env:VG_MCP_BRIDGE = "1" ; .\editor.exe
#    log attendu : [MCPBridge] enabled - watching "…/mcp/commands.jsonl"
#    le dossier mcp/ doit se créer avec un state.json contenant "running": true
```

4. Poser **à la main** un `data/Prefabs/Box_Base.prefab` dans la scène : `spawn_object` **clone un objet déjà
   présent**, il ne sait pas instancier un prefab depuis le disque (hors périmètre V1).
5. Dans Claude Code, en langage naturel : « vérifie que le moteur répond », « liste les objets »,
   « cherche Box_Base », puis le scénario grille.

**Rien n'est écrit sur disque tant que `save_world` n'est pas appelé** — recharger la scène annule tout.
Attention : il n'y a **pas de `delete_object`** en V1, le ménage se fait à la main dans l'éditeur.

### Checklist Phase 6

- [x] `docs/engine-analysis.md` complet avec citations
- [x] `docs/architecture.md` (ADR) cohérent avec Phase 0
- [x] `docs/OPEN_QUESTIONS.md` à jour — #1 à #4 tous résolus
- [x] Module bridge compilé sans modifier de fichier existant *(hors 4 câblages additifs validés, tous `#if`-gardés)*
- [x] Serveur MCP fonctionnel, 6 tools **testés contre le moteur réel** via le protocole MCP complet
- [x] Convention de rotation vérifiée contre l'Inspector du moteur (`OPEN_QUESTIONS.md` #4)
- [x] Build Debug **et** Release OK, 0/0, avec et sans `VG_MCP_BRIDGE` défini à l'exécution
- [ ] Scénario « plan de ville » 5×5 joué de bout en bout depuis Claude Code *(nécessite un `Box_Base` posé à la main dans la scène)*

---

## Bugs trouvés pendant la Phase 5

Le passage sur une vraie scène (`Aiguelongue`, 7832 objets) a révélé quatre défauts. **Trois étaient les miens** — aucun n'aurait été vu sur une scène jouet.

| # | Défaut | Où | Correction |
|---|---|---|---|
| 1 | Le heartbeat réécrivait `results` **vide** : tout client plus lent que ~2 s perdait sa réponse | `MCPBridge::writeState` | `results` est toujours republié ; `m_recentResults` stocke des `Json` au lieu de chaînes re-parsées |
| 2 | `list_objects` renvoyait **9,7 Mo** de JSON — inutilisable via MCP | `SceneBridge::listObjects` | modes *browse* (profondeur 1 par défaut) / *search* (`name_contains`), + `parent_id`, `max_depth`, `limit`, `child_count`. **9,7 Mo → 10 Ko** |
| 3 | Signe de rotation inversé : `decompose` lisait la **transposée** de ce qu'écrivait `recompose` | `SceneBridge.cpp` | termes antisymétriques pris en `(r_ij − r_ji)`. ⚠️ Un test à 90° ne peut **pas** détecter ce bug (boîte symétrique) — tester à 45° |
| 4 | `World::AddScene` teste `nullptr == m_activeScene` sur un **tableau** (`World.h:95`) → condition toujours fausse, aucune scène n'est jamais marquée active au chargement d'un monde | `src/engine/World/World.cpp:171` — **amont** | non corrigé (règle d'or). Contourné : le bridge énumère `GetSceneCount`/`GetScene`. Correctif amont si souhaité : `m_activeScene[typeIndex]` |

Le bug 4 passait inaperçu parce que seule la vue Prefab appelle `SetActiveScene` explicitement (`ImGuiPrefabView.hpp:193`).
