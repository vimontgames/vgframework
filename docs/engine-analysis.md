# Phase 0 — Analyse du moteur (vgframework)

> Reconnaissance en **lecture seule**. Aucun fichier moteur modifié.
> Toutes les réponses sont sourcées `fichier:ligne` (chemins relatifs à la racine du repo).
> Vérifié le 2026-09-06 sur la branche `master` (commit `08ef0d8c`).

Le moteur est **VG Framework** : C++ ~94 %, HLSL ~5 %, C# (Sharpmake uniquement), Python (exporters DCC uniquement).
Architecture modulaire : chaque sous-système est une **DLL plugin** (`core`, `gfx`, `renderer`, `engine`, `physics`, `audio`, `editor`, `game`) exposant un `extern "C" CreateNew()`.

---

## 1. Représentation des objets de scène

**Scene graph hiérarchique** (pas d'ECS). Hiérarchie :

```
World  (core::IWorld)                      src/core/IWorld.h:33
 └─ Scene / Prefab  (core::IBaseScene)     src/core/IBaseScene.h:23
     └─ GameObject "Root"  (core::IGameObject)   src/core/IBaseScene.h:28  (GetRoot)
         ├─ GameObject enfant …            src/core/IGameObject.h:24  (GetChildren)
         └─ Component[]                    src/core/IGameObject.h:35  (GetComponents)
```

- `World` = conteneur de scènes ; scène active par type : `IWorld::GetActiveScene(BaseSceneType)` — `src/core/IWorld.h:49`.
- `IBaseScene::GetRoot()` → `IGameObject *` racine — `src/core/IBaseScene.h:29`.
- `GameObject` : `class GameObject : public IGameObject` — `src/core/GameObject/GameObject.h:18` ; enfants `m_children`, composants `m_components` — `src/core/GameObject/GameObject.h:142-143`.
- Implémentation concrète : `src/core/GameObject/GameObject.cpp`.

### Transform

Il n'existe **pas** de classe `Transform` / `Position` / `Rotation` / `Scale` séparée.
Le transform est porté par la classe de base `core::Instance` (parent de `IGameObject`) sous forme de **deux matrices 4×4** :

```cpp
float4x4  m_local  = float4x4::identity();   // src/core/Instance/Instance.h:74
float4x4  m_global = float4x4::identity();   // src/core/Instance/Instance.h:75
```

API publique (`src/core/IInstance.h:31-37`) :

| Méthode | Rôle |
|---|---|
| `SetLocalMatrix(const float4x4 &)` | écrit le transform local |
| `GetLocalMatrix() const` | lit le transform local |
| `SetGlobalMatrix(const float4x4 &)` | écrit en espace monde (converti en local en interne) |
| `GetGlobalMatrix() const` | lit en espace monde |
| `OnLocalMatrixChanged(bool recomputeParents, bool recomputeChildren)` | **à appeler après toute écriture** pour propager aux enfants + notifier le renderer/physique |

`GameObject` redéfinit `OnLocalMatrixChanged` — `src/core/GameObject/GameObject.h:118` / `.cpp`.

`float4x4` provient de **hlslpp** (`extern/hlslpp`), convention **row-major, vecteurs-ligne** : la translation est la **4ᵉ ligne** `m[3].xyz` (confirmé `src/engine/Selection/Selection.cpp:197` `T.xyz += m[3].xyz`).

---

## 2. Format de la rotation

**Matrice** (`float4x4`), pas de quaternion ni d'angles d'Euler stockés.
Sérialisation scène : les 16 flottants bruts (`Ix..Iw Jx..Jw Kx..Kw Tx..Tw`), ex. `data/Scenes/BLAStest.scene:8`.

Helpers de conversion disponibles dans `src/core/Math/Math.h` (implémentation `Math.inl` / `Math.cpp`) :

| Fonction | Signature | `Math.h` |
|---|---|---|
| `Float4x4ToTRS` | `(const float4x4&, float3& T, float3& R_euler, float3& S)` | `:123` |
| `TRSToFloat4x4` | `(const float3& T, const float3& R_euler, const float3& S)` | `:126` |
| `TRSToFloat4x4` | `(const float3& T, quaternion R, float3 S)` | `:129` |
| `extractRotation` | `(const float4x4&) → float3x3` | `:132` |
| `clearScale` / `clearRotation` / `clearTranslation` | décomposition partielle | `:50-96` |

`quaternion` existe comme type hlslpp (`src/core/Math/Math.h:346`, `slerpShortestPath` `:165`) mais **n'est pas** le format de stockage du transform.

> **Conséquence pour le bridge** : le contrat MCP peut rester en quaternion ; la conversion quaternion↔matrice se fait **côté moteur** via `TRSToFloat4x4(T, quaternion, S)` et, en lecture, via `Float4x4ToTRS` (Euler) puis Euler→quaternion, ou une extraction quaternion directe à écrire dans le module bridge (hlslpp fournit `float4x4 → quaternion`).

---

## 3. Identifiant d'objet stable

**Oui — UID `u32` stable et résolvable globalement.**

- Type : `using UID = core::u32;` — `src/core/IObject.h:41`.
- Tout `IObject` porte : `GetUID()`, `SetUID()`, `RegisterUID()`, `HasValidUID()`, plus `GetOriginalUID()` (traçabilité prefab) — `src/core/IObject.h:65-74`.
- **Registre global** dans la Factory (singleton `Kernel::getFactory()`) :
  - `IObject * IFactory::FindByUID(UID) const` — `src/core/IFactory.h:69`  ← **résolution id → objet pour le bridge**
  - `const UIDObjectHash & GetUIDObjects() const` (`unordered_map<UID, IObject*>`) — `src/core/IFactory.h:64,68`
  - `UID RegisterUID(IObject*)` — `src/core/IFactory.h:66`
- Implémentation : `src/core/Object/Factory.{h,cpp}` (`m_uidObjectHash` — `Factory.h:89`).

### Persistance de l'UID

- Format de scène **actuel** : l'UID est sérialisé par objet — `data/Scenes/Aiguelongue.scene` contient 3379 `m_uid` (un par objet).
- `m_uid value="0"` = « non assigné » → un nouvel UID est généré au chargement (`getNewUID` — `src/core/Object/Factory.h:74`). C'est le cas des prefabs neufs (`data/Prefabs/Box1X1M.prefab:7`) et de l'ancien format (`data/Scenes/BLAStest.scene` n'a aucun `m_uid`).

> **Conséquence** : un UID est stable **au sein d'une session** et **entre chargements si la scène a été sauvegardée** après attribution. Un objet créé à l'exécution (non sauvegardé) reçoit un nouvel UID au prochain chargement. Pour le scénario Phase 5, **sauvegarder la scène** après placement fige les UID.

---

## 4. Couche de scripting existante

**Aucune couche de scripting runtime.** 

- Le C# du repo sert **uniquement** à Sharpmake (génération de projets) — `sharpmake/*.sharpmake.cs`.
- Le Python du repo sert **uniquement** aux exporters DCC — `data/Scripts/3dsMAX/`, `data/Scripts/Blender/`.
- La logique de jeu s'écrit en **C++** via des `Behaviour` / `Component` compilés — `src/core/Component/Behaviour/Behaviour.h`, projet exemple `projects/game/src/`.
- Enregistrement des classes : auto-registration statique `AutoRegisterClassInfo::registerClasses(factory)` déclenchée au chargement de chaque DLL — `projects/game/src/Game.cpp:66`.

> **Conséquence** : l'option « brancher le bridge depuis un script existant » (Phase 1 option A) est **écartée** : il n'y a pas de script.

---

## 5. Console de commandes / debug commands runtime

**Quasi inexistant.** `src/editor/ImGui/Window/Console/ImGuiConsole.{h,cpp}` est une **console de log** ImGui.
`ImGuiConsole::execute()` — `ImGuiConsole.cpp:422` — ne reconnaît que `CLEAR`, `HELP`, `HISTORY` en dur (`:439-452`). Pas de table de commandes enregistrables, pas d'accès scène.

> **Conséquence** : pas de canal « commande console » exploitable.

---

## 6. Réseau / IPC existant

**Aucun.** Recherche `socket`, `WSAStartup`, `listen(`, `recv(`, `httplib`, `WebSocket`, pipe nommé → **0 résultat** dans `src/`.
Le seul `LoadLibrary` hors plugins est le compilateur de shaders DXC (`src/gfx/Shader/dxc/ShaderCompiler_dxc.hpp:33`).
Pas de serveur de remote-debug, live-reload distant ni profiling réseau (Optick est local).

> **Conséquence** : rien à réutiliser (Phase 1 option D écartée). Tout canal réseau serait à créer.

---

## 7. Boucle de vie / hot-connect

- Exécutables à la racine : `vgframework_win64_msvc_dx12_{debug,release}.exe` ; `editor.exe` et `game.exe` sont des **copies** post-build (cf. mémoire *build-toolchain-constraints*).
- Boucle : `Engine::RunOneFrame()` — `src/engine/Engine.cpp:838` — appelée en continu par l'application. Elle tourne **en mode éditeur** que l'on soit en Play ou non.
- Ordre par frame (`Engine.cpp:919-996`) : `FixedUpdate` → `physics->Update` → `game->Update` → `world->update` → `LateUpdate` → **`ToolUpdate` (si une vue toolmode est visible, `Engine.cpp:979` / `anyToolmodeViewVisible()` `:1021`)** → `editor->RunOneFrame` → `renderer->RunOneFrame`.
- **`game->ToolUpdate(dt)` et `world->toolUpdate()` sont appelés chaque frame dans l'éditeur, hors Play** — `Engine.cpp:983-991`. C'est le point d'ancrage idéal pour un bridge qui doit fonctionner sans lancer le jeu.
- **Pas de hot-connect** : aucun IPC (cf. §6). Le process tourne en boucle vivante mais on ne peut s'y attacher que par du code chargé **dans** le process. Un changement de code moteur impose un rebuild + relance ; un changement de *données* (scène, `commands.json`) non.

---

## 8. Duplication / instanciation de nœud par code

**Oui — fonction interne de clonage existante et déjà exposée.**

- `IObject::Instanciate(InstanciateFlags = 0)` — déclaré `src/core/IObject.h:84`, défini `src/core/Object/Object.cpp:302`.
  Implémentation : `Kernel::getFactory()->Instanciate(this, nullptr, flags)` — copie profonde de propriétés + **nouvel UID** — `src/core/Object/Object.cpp:309`.
- `IFactory::Instanciate(const IObject*, IObject* parent, CopyPropertyFlags)` — `src/core/IFactory.h:60`.
- Utilisé par le copier-coller de l'éditeur : `ISelection::DuplicateGameObjects()` — `src/core/ISelection.h` / `src/engine/Selection/Selection.cpp:321`, qui appelle `go->Instanciate()` puis `parentGameObject->AddChild(newGO, index+1)` — `Selection.cpp:335,396`.
- Rattachement : `IGameObject::AddChild(IGameObject*, uint index = -1)` — `src/core/IGameObject.h:21`.
- Création par nom de classe : `IFactory::CreateObject(className, name, parent)` — `src/core/IFactory.h:44` ; `IGameObject::AddComponent(const char* className, name)` — `src/core/IGameObject.h:33`.

> **Conséquence** : le point bloquant §2 du plan se résout **en faveur de (a)**. V1 peut inclure un `spawn_object(source_uid, transform)` = `FindByUID(source_uid)->Instanciate()` + `AddChild` + `SetLocalMatrix`. Le « cube de référence » est le prefab `data/Prefabs/Box1X1M.prefab` (déjà présent dans le repo).

---

## 9. Système de build

- **Sharpmake** génère une **solution Visual Studio 2022** (`vgframework_vs2022.sln`).
- Génération : `sharpmake/generate_projects_Windows_msvc.bat` (le variant sans `_msvc` échoue, pas de LLVM — cf. mémoire *build-toolchain-constraints*).
- Build : `MSBuild.exe vgframework_vs2022.sln /p:Configuration=Debug /p:Platform="Win64 MSVC DX12" /m`.
- Un projet = un fichier `sharpmake/vg.<nom>.sharpmake.cs`. **Tous les `vg.*.sharpmake.cs` sont auto-inclus** : `[module: Sharpmake.Include("vg.*.sharpmake.cs")]` — `sharpmake/main.sharpmake.cs:5`.
- **MAIS** chaque projet doit être ajouté explicitement dans `Solution.ConfigureAll` : `conf.AddProject<Xxx>(target)` — `sharpmake/vg.solution.sharpmake.cs:44-72` (commentaire `:43` : « All projects must be explicitly added here »).
- Modèle de plugin minimal : `sharpmake/vg.game.sharpmake.cs` (18 lignes) + `projects/game/src/Game.{h,cpp,def}` avec `Game.def` exportant `CreateNew`.
- Chargement runtime : `Plugin::createInternal()` — `src/core/Plugin/Plugin.cpp:62` — `LoadLibraryExA` + `GetProcAddress("CreateNew")`. Cherche d'abord `build/bin/<platform>/<config> <compiler>[ dx12]/<nom>.dll` puis `bin/…`.
- **Il n'y a pas de scan de dossier de plugins** : chaque DLL est chargée par un appel explicite `Plugin::create<T>("nom")` (ex. `src/engine/Engine.cpp:386,413,420,429`). Le nom du plugin `game` vient de `EngineOptions::GetProjectPath()` (`src/engine/EngineOptions.cpp:224`), les autres sont en dur.

> **Conséquence** : voir `docs/OPEN_QUESTIONS.md` #1 — un nouveau module bridge implique **1 ligne** dans `vg.solution.sharpmake.cs` (fichier existant) + **1 point de chargement** runtime. Ni l'un ni l'autre n'est un « point d'extension » propre : ce sont les câblages minimaux à faire valider.

---

## 10. Sérialisation de scène

- Format : **XML** maison via `tinyxml2` (`extern/tinyxml2`), API `IFactory::LoadFromXML` / `SaveToXML` / `SerializeFromXML` / `SerializeToXML` — `src/core/IFactory.h:46-50`.
- Fichiers : `data/Worlds/*.world`, `data/Scenes/*.scene`, `data/Prefabs/*.prefab` — tous du XML `<Root><Object class="…"><Property …/>`.
- Un `GameObject` sérialise : `m_name`, `m_uid`, `m_flags`, `m_color`, **`m_local` (Float4x4, 16 floats)**, `m_tags`, `m_components`, `m_children` — cf. `data/Prefabs/Box1X1M.prefab`, `data/Scenes/Aiguelongue.scene`.
- Sauvegarde depuis l'API moteur : `IEngine::SaveWorld()` / `SaveWorldAs()` / `SaveScene(IResource*)` — `src/engine/IEngine.h:100-105`.
- `ObjectRuntimeFlags::NotSerialized` (`src/core/IObject.h:25`) : un objet instancié à l'exécution avec ce flag n'est **pas** sauvegardé → à ne PAS mettre si l'on veut que le placement MCP persiste.

> **Conséquence** : les modifications faites via le bridge (matrice locale) sont persistées telles quelles si l'on appelle `SaveWorld`/`SaveScene`. Le contrat de test Phase 5 (« vérifier après sauvegarde/rechargement ») est réalisable.

---

## 11. Conventions du monde (axes, unités, primitive de référence)

*(ajouté après validation utilisateur + vérification dans les données, 2026-09-06)*

### Le monde est **Z-up**

| Preuve | Source |
|---|---|
| Gravité par défaut `float3(0, 0, -9.81)` | `src/physics/Options/PhysicsOptions.h:44`, `Physics.xml:14` |
| Sur les 1435 transforms de la scène ville `Aiguelongue.scene` : étendue X = 170,5 / Y = 237,0 / **Z = 38,5** → X et Y forment le plan du sol, Z est la verticale | `data/Scenes/Aiguelongue.scene` |

- **X, Y = plan du sol ; Z = hauteur.** Unités = mètres.
- ⚠️ Piège : l'importeur FBX force `opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y` (`src/renderer/Importer/FBX/UFBXImporter/UFBXImporter.cpp:38`). Ce réglage concerne la conversion d'axes **à l'import du fichier** et ne reflète pas la convention du monde. Ne pas s'y fier : **le monde runtime est Z-up**.
- Conséquence pour la rotation : un cap (yaw) d'angle `a` autour de la verticale est le quaternion `{x:0, y:0, z:sin(a/2), w:cos(a/2)}`.

### Primitive de référence : `data/Prefabs/Box_Base.prefab`

- Structure : `Root` (GameObject vide) → enfant `Box_Base` portant un `MeshComponent` → `data/Meshes/Box/BOX_Base/BOX_Base.fbx`.
- **Pivot au centre de la base du mesh** (et non au centre du volume). Donc :
  - `position.z` = niveau du sol, directement ;
  - `scale.z` = hauteur totale ; **pas d'offset de demi-hauteur à appliquer**.
  - Un immeuble 8 × 8 × 20 m ⇒ `scale {x:8, y:8, z:20}` à `position {z:0}`.
- `Box1X1M` (créé le 2026-09-06) est remplacé par `Box_Base` dans ce rôle.

### Dimensions : mesurées à l'exécution, pas devinées

Le cache cuit (`cache/data/Meshes/.../*.bin`) n'est pas exploitable de façon fiable pour retrouver l'AABB. Le bridge expose donc les bornes réelles via `IGameObject::TryGetAABB()` (`src/core/GameObject/GameObject.cpp:822`, AABB dans l'espace local **non scalé**, agrégée composants + enfants) :

- `bounds_local` `{min, max, size}` → montre où tombe le pivot dans le mesh et la taille de référence ;
- `size_world` = `bounds_local.size × scale` → l'emprise réelle.

---

## Synthèse pour les phases suivantes

| Question plan | Verdict |
|---|---|
| ECS / scene graph | Scene graph hiérarchique `World → Scene → GameObject → Component` |
| Format rotation | `float4x4` (matrice) ; helpers TRS/quaternion dispo côté moteur |
| ID stable | **Oui**, `UID u32` + `IFactory::FindByUID()` ; persisté si scène sauvegardée |
| Scripting | **Aucun** runtime → option A écartée |
| Console commandes | Non exploitable |
| IPC existant | **Aucun** → option D écartée, à créer |
| Boucle / hot-connect | Boucle éditeur vivante (`ToolUpdate` chaque frame hors Play) ; pas de hot-connect |
| Duplication par code | **Oui**, `IObject::Instanciate()` + `AddChild()` → point bloquant §2 résolu en (a) |
| Build | Sharpmake→VS2022 ; nouveau projet = nouveau `vg.*.sharpmake.cs` (auto-inclus) + 1 ligne solution + 1 point de chargement |
| Sérialisation | XML tinyxml2 ; `m_local` sérialisé ; persistance OK via `SaveWorld`/`SaveScene` |

**Options de communication retenues comme réalistes (détaillées Phase 1) :**
- **B — fichier de commandes** (`commands.json` / `state.json`) poll dans `ToolUpdate`. Le moins invasif fonctionnellement.
- **C — socket TCP loopback** dans un thread du module bridge. Latence faible, un peu plus de code.

Dans les deux cas le **point d'ancrage dans la boucle** est le sujet de `OPEN_QUESTIONS.md` #1.
