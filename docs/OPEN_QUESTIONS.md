# Questions ouvertes — à valider avant d'écrire du code moteur (Phase 3)

Statut : **Phases 0 à 4 terminées et compilées (Debug + Release, 0/0).** Cf. `docs/PHASE3-4-IMPLEMENTATION.md`.
Questions #1–#3 tranchées par l'utilisateur le 2026-09-06, #1 implémentée :
- **#1 → Option 1**, implémentée. Câblage réel = **3 fichiers de build existants** (un de plus que prévu) + `Engine.cpp`, tout sous `#if VG_ENABLE_MCPBRIDGE`, gate runtime par variable d'env `VG_MCP_BRIDGE` :
  - `sharpmake/vg.solution.sharpmake.cs` : `conf.AddProject<MCPBridge>(target);` (1 ligne)
  - `sharpmake/vg.engine.sharpmake.cs` : `conf.Defines.Add("VG_ENABLE_MCPBRIDGE");` (1 ligne)
  - `sharpmake/vg.data.sharpmake.cs` : `SourceFilesExcludeRegex.Add(@".*\\mcpbridge(\.*)?");` dans le projet `Version` (1 ligne — sinon le projet utilitaire `version` ramasse les `.cpp` du nouveau module, comme pour tous les autres modules déjà listés là)
  - `src/engine/Engine.cpp` : include gardé + pointeur statique + create/Init + Tick + Deinit (~25 lignes, 5 emplacements, tous `#if VG_ENABLE_MCPBRIDGE`, aucune ligne existante modifiée)
- **#2 → contrat quaternion** confirmé.
- **#3 → `spawn_object` inclus en V1** — fait.

Historique des questions ci-dessous.

---

## #1 — Point d'insertion du bridge dans la boucle moteur (BLOQUANT)

### Constat (cf. `engine-analysis.md` §4, §5, §7, §9)

Il n'existe **aucun point d'extension propre** pour brancher du code externe sans toucher au cœur :
- pas de scripting runtime ;
- pas de système de commandes console enregistrables ;
- pas de scan d'un dossier de plugins — chaque DLL est chargée par un appel explicite `Plugin::create<T>("nom")` codé en dur (`src/engine/Engine.cpp`) ;
- l'auto-registration de classes (`AutoRegisterClassInfo`) ne se déclenche que si la DLL qui contient ces classes est chargée.

Un module bridge additif (`src/mcpbridge/…` + `sharpmake/vg.mcpbridge.sharpmake.cs`) est **entièrement du code neuf**, mais pour qu'il tourne il faut au minimum :

| # | Modif | Fichier | Ampleur | Statut règle d'or |
|---|---|---|---|---|
| a | `conf.AddProject<MCPBridge>(target);` | `sharpmake/vg.solution.sharpmake.cs` (existant) | 1 ligne | ⚠️ ajout dans un fichier existant |
| b | Chargement de la DLL + tick chaque frame | 1 point dans `src/engine/Engine.cpp` **ou** `src/application/…` (existant) | ~3–5 lignes | ⚠️ ajout dans un fichier existant |

> Le fichier `vg.mcpbridge.sharpmake.cs` lui-même est auto-inclus (`sharpmake/main.sharpmake.cs:5`) → aucune modif de build pour être *compilé*, seulement pour être *lié à la solution et chargé*.

### Options proposées (à trancher par l'utilisateur)

- **Option 1 — Assumer les 2 ajouts (a) + (b).**
  Diff minimal, purement additif (aucune ligne existante supprimée/modifiée). C'est la voie recommandée.
  Diff exact proposé pour (b), à valider :
  ```cpp
  // src/engine/Engine.cpp, dans Engine::init() après la création des autres plugins
  #if VG_ENABLE_MCPBRIDGE
      m_mcpBridge = Plugin::create<mcpbridge::IMCPBridge>("mcpbridge"); // no-op si DLL absente
  #endif
  // ... dans RunOneFrame(), dans le bloc ToolUpdate déjà existant (Engine.cpp:979-993) :
  #if VG_ENABLE_MCPBRIDGE
      if (m_mcpBridge) m_mcpBridge->Tick();
  #endif
  ```
  Gardé derrière `#if VG_ENABLE_MCPBRIDGE` (défini seulement par le projet bridge) → **zéro impact** sur le build normal quand le bridge est désactivé (checklist Phase 6).

- **Option 2 — Héberger le bridge dans le projet `game` existant.**
  `projects/game/` est déjà chargé par l'éditeur (`Engine.cpp:424`) et a un `ToolUpdate`. Mais cela modifie `projects/game/src/Game.cpp` (fichier existant) → **plus** invasif que l'option 1, et couple le bridge au jeu. Non recommandé.

- **Option 3 — Process externe uniquement, pas de code moteur.**
  Le serveur MCP lit/écrit directement les fichiers `.scene` XML sur disque. **Rejeté** : ne fonctionne pas sur une scène chargée en mémoire (pas de rechargement à chaud), donc pas de feedback visuel live — contredit le cas d'usage.

### Décision attendue

➡️ **Quelle option ?** (défaut recommandé : Option 1)
➡️ Si Option 1 : le point de chargement va dans `engine` ou dans `application` ?

---

## #2 — Contrat de rotation (résolu, à confirmer)

Le moteur stocke le transform en `float4x4` (pas de quaternion). Le plan (Phase 2) recommande de garder le **quaternion** dans le contrat MCP et de convertir côté bridge.

➡️ **Confirmation** : contrat MCP en quaternion `{x,y,z,w}`, conversion dans le module C++ via hlslpp + `TRSToFloat4x4` (`src/core/Math/Math.h:129`). OK ?

*(Aucune action requise si d'accord — c'est l'hypothèse retenue dans `docs/data-contract.md`.)*

---

## #3 — Point bloquant §2 du plan : pool vs création dynamique (résolu)

`engine-analysis.md` §8 : `IObject::Instanciate()` existe et est déjà utilisé par le copier-coller éditeur.
→ La **création dynamique (a)** est possible. V1 peut inclure `spawn_object`.

➡️ **Confirmation de périmètre V1** : inclut-on `spawn_object` (clone du prefab `Box1X1M`) dès V1, ou on s'en tient strictement à `list/get/set_transform` sur objets existants pour la première itération ?

*(Le plan Phase 5 penche pour inclure le spawn si (a) est vrai. Hypothèse retenue : `spawn_object` inclus en V1, mais implémenté après validation des 3 fonctions de base.)*

---

## #4 — Cube de référence + convention de rotation — **RÉSOLU (2026-09-06)**

✅ **Primitive de référence = `data/Prefabs/Box_Base.prefab`** (et non `Box1X1M`), tranché par l'utilisateur.
- **Pivot au centre de la base** du mesh → `position.z` = niveau du sol, `scale.z` = hauteur totale, aucun offset de demi-hauteur.
- **Monde Z-up** : X/Y = plan du sol, Z = hauteur (confirmé par la gravité `(0,0,-9.81)` et l'étendue des 1435 transforms d'`Aiguelongue.scene` : X 170 / Y 237 / **Z 38**). Le `target_axes.up = POSITIVE_Y` de l'importeur FBX ne concerne que la conversion à l'import — ne pas s'y fier.
- Dimensions : **mesurées à l'exécution** plutôt que devinées — le bridge renvoie `bounds_local` et `size_world` via `IGameObject::TryGetAABB()`.

✅ **Convention de rotation validée.** Le module décompose/recompose le transform lui-même (`SceneBridge.cpp`, base I/J/K **en lignes** = axes locaux ; quaternion via Shepperd) sans passer par `TRSToFloat4x4(T, quaternion, S)` (`Math.h:129`), dont la surcharge quaternion s'est révélée incohérente avec le reste du moteur (translation placée en 4ᵉ **colonne** au lieu de 4ᵉ ligne).

Deux vérifications faites en Phase 5, dans cet ordre — la première ne suffisait pas :

1. **Round-trip** (set puis get). A d'abord échoué : envoyé `z=+0.7071`, relu `z=−0.7071`. Cause : `decompose` lisait la matrice **transposée** de ce que `recompose` écrivait, ce qui négate l'axe. Corrigé en prenant les termes antisymétriques comme `(r_ij − r_ji)` au lieu de `(r_ji − r_ij)` ; les termes symétriques sont inchangés.
   ⚠️ Un test à **90° ne peut pas révéler une erreur de signe** (une boîte est symétrique) — le test doit se faire à 45°.
2. **Signe vs le moteur.** Le round-trip seul ne prouve rien : il est cohérent avec lui-même. Source de vérité indépendante utilisée : l'Inspector de l'éditeur, qui affiche les angles d'Euler via `Float4x4ToTRS` — **du code moteur, pas celui du bridge**. Yaw envoyé `+45°` → Inspector affiche **`+45`**. Conventions alignées.
