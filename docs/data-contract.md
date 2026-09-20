# Phase 2 — Contrat de données (schéma partagé bridge ↔ serveur MCP)

Basé sur `docs/engine-analysis.md`. Format d'échange : JSON.
Le moteur stocke le transform en `float4x4` ; **le contrat reste en TRS + quaternion**, la conversion se fait dans le module C++ (`src/core/Math/Math.h:123-132` + hlslpp).

---

## Objet

```jsonc
{
  "object_id": "1945741270",      // string décimale d'un UID u32 (IObject::GetUID). Stable dans la session.
  "name": "Box1X1M_03",
  "path": "Root/Blocks/Box1X1M_03", // chemin lisible (noms de GameObject), pour debug — NON garanti unique
  "primitive_type": "cube",        // "cube" si le mesh est le prefab de référence, sinon "unknown"
  "enabled": true,                 // InstanceFlags::Enabled
  "static": false,                 // InstanceFlags::Static
  "transform": {
    "position": { "x": 0.0, "y": 0.0, "z": 0.0 },       // translation locale (m[3].xyz), unités moteur = mètres
    "rotation": { "x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0 }, // quaternion local (identité = 0,0,0,1)
    "scale":    { "x": 1.0, "y": 1.0, "z": 1.0 }         // échelle locale par axe
  }
}
```

- `transform` est **local** (relatif au parent). Le champ `space` optionnel (`"local"` | `"world"`) pourra être ajouté en V1.1 ; V1 = local uniquement.
- **Convention d'axes : le monde est Z-UP.** X et Y = plan du sol, **Z = hauteur**, unités en mètres (preuves : `engine-analysis.md` §11). Un cap (yaw) d'angle `a` = quaternion `{x:0, y:0, z:sin(a/2), w:cos(a/2)}`. Le serveur MCP ne réinterprète pas les axes.
- **Primitive de référence : `data/Prefabs/Box_Base.prefab`**, dont le **pivot est au centre de sa base** → `position.z` = niveau du sol et `scale.z` = hauteur totale, sans offset de demi-hauteur.

Deux champs supplémentaires sont retournés par `list_objects` / `get_transform` pour ne rien avoir à deviner :

```jsonc
"bounds_local": {                    // bornes du mesh dans l'espace local, NON scalées
  "min":  { "x": 0, "y": 0, "z": 0 },   //   -> révèle la position du pivot dans le mesh
  "max":  { "x": 1, "y": 1, "z": 1 },
  "size": { "x": 1, "y": 1, "z": 1 }
},
"size_world": { "x": 8, "y": 8, "z": 20 }   // bounds_local.size * scale = emprise réelle
```

---

## Tools MCP → commandes bridge (1:1)

### `list_objects`

**Toujours borné.** Un listing récursif complet est inutilisable : mesuré sur `Aiguelongue.scene` (7832 objets), il produisait un `state.json` de **9,7 Mo**. Deux modes, tous deux plafonnés :

| Mode | Déclencheur | Comportement |
|---|---|---|
| **browse** (défaut) | pas de `name_contains` | descend jusqu'à `max_depth` (défaut **1** = enfants directs) |
| **search** | `name_contains` non vide | parcourt **tout** le sous-arbre, ne retourne que les noms contenant la sous-chaîne (insensible à la casse) |

Entrée — tout est optionnel :
```jsonc
{
  "name_contains": "box",      // active le mode search
  "parent_id": "1093025436",   // part de cet objet au lieu des racines de scène
  "max_depth": 1,              // mode browse uniquement ; 0 = illimité
  "limit": 200,                // plafond d'objets retournés
  "detail": "compact"          // "compact" (défaut) | "full"
}
```
Sortie :
```jsonc
{
  // absent quand parent_id est fourni. root_id ajouté le 2026-09-12 : la racine n'est
  // jamais listée comme un objet, et c'est le SEUL moyen de viser une scène vide (qui
  // n'a rien à lister) — à passer en parent_id de create_group / spawn_objects.
  "scenes": [{ "name": "City", "root_id": "3000000002", "child_count": 0 }],
  "objects": [ /* Objet[] ci-dessus, + "path" et "child_count" */ ],
  "returned": 8,               // taille de objects
  "matched": 8,                // total des correspondances, même au-delà de limit
  "truncated": false,
  "mode": "browse"
}
```
`child_count` permet de savoir s'il faut descendre. Résultat mesuré en mode browse sur la même scène : **10 Ko** au lieu de 9,7 Mo.

**`detail` (ajouté 2026-09-12).** En `compact` (défaut) un objet ne porte que `object_id`, `name`, `path`, `child_count` ; côté serveur MCP la réponse est rendue **une ligne par objet, sans dump JSON** — c'est ce qui faisait exploser la limite de tokens de l'outil dès ~170 objets. `detail: "full"` rajoute `enabled`, `transform`, `bounds_local` et `size_world` (~40 lignes de JSON par objet) : à réserver à une poignée d'objets, sinon utiliser `get_transform`, qui est toujours complet.

**Énumère toutes les scènes du monde principal** (`IWorld::GetSceneCount` / `GetScene`) et **non** `GetActiveScene` : `World::AddScene` teste `nullptr == m_activeScene` sur un **tableau** (`src/engine/World/World.cpp:171`, membre déclaré `World.h:95`), condition toujours fausse — aucune scène n'est donc jamais marquée active au chargement d'un monde. Bug amont non corrigé ici (règle d'or) ; l'énumération le contourne et gère au passage les mondes multi-scènes.

### `get_transform`
Entrée : `{ "object_id": "1945741270" }`
Sortie : l'Objet complet, ou erreur (voir plus bas).
Résolution : `Kernel::getFactory()->FindByUID(id)` (`src/core/IFactory.h:69`).

### `set_transform`
Entrée — **tous les sous-champs indépendamment optionnels** :
```jsonc
{
  "object_id": "1945741270",
  "position": { "x": 4.0, "z": 2.0 },   // y absent => y inchangé ; composantes idem
  "rotation": { "x": 0, "y": 0.7071, "z": 0, "w": 0.7071 },
  "scale":    { "x": 8.0, "y": 20.0, "z": 8.0 }
}
```
Sémantique :
- champ absent (`position`/`rotation`/`scale`) → cette partie du transform est conservée ;
- sous-champ absent (`position.y`) → composante conservée ;
- le module lit la matrice locale, la décompose (`Float4x4ToTRS`), applique les deltas fournis, recompose (`TRSToFloat4x4(T, quat, S)`), `SetLocalMatrix`, puis **`OnLocalMatrixChanged(false, true)`** (`src/core/IInstance.h:37`).
Sortie : `{ "object_id": "...", "transform": { ...état final... } }`

### `spawn_object` *(candidat V1 — cf. OQ #3)*
Entrée :
```jsonc
{
  "source_id": "1945741270",     // UID d'un objet à cloner (défaut : le prefab Box1X1M de la scène)
  "name": "Box1X1M_07",           // optionnel
  "parent_id": "114612410",       // optionnel, défaut = parent de la source
  "transform": { "position": {...}, "rotation": {...}, "scale": {...} }
}
```
Sortie : `{ "object_id": "<nouvel UID>", ...Objet }`
Impl : `FindByUID(source_id)->Instanciate()` → `AddChild` → `SetLocalMatrix` (`src/core/Object/Object.cpp:302`, `src/engine/Selection/Selection.cpp:335`).

### `spawn_objects` *(lot — ajouté 2026-09-12)*
Même clonage que `spawn_object`, mais **N copies en une commande** : un aller-retour au lieu de N. Mesuré : **150 clones en 0,23 s**, là où 150 `spawn_object` en parallèle déclenchaient des timeouts client à 15 s (le moteur traitait quand même la commande → doublons).
```jsonc
{
  "source_id": "1284883740",
  "parent_id": "3518068110",     // optionnel, défaut = parent de la source ; typiquement un create_group
  "items": [                     // 1 entrée = 1 clone ; placement relatif au parent
    { "name": "Bat_00", "position": {...}, "rotation": {...}, "scale": {...} }
  ]
}
```
Sortie : `{ "parent_id", "count", "spawned": [{ object_id, name }], "failed"? }`. Un échec sur un item n'interrompt pas le lot : il est reporté dans `failed`.

### `create_group` *(ajouté 2026-09-12)*
Crée un `GameObject` vide servant de dossier, pour ne pas laisser des centaines d'objets à plat sous la racine de scène.
```jsonc
{ "name": "Quartier_Nord", "parent_id": "...", "position": {...} }  // parent_id défaut = racine de la 1re scène
```
Impl : `IFactory::CreateObject("GameObject", name, parent)` → `RegisterUID()` → `parent->AddChild()` → `Release()` (même séquence que l'éditeur, `ImGuiGameObjectSceneEditorMenu.hpp:629`). Groupe posé à l'identité : les coordonnées des enfants restent égales aux coordonnées monde. Groupes imbriquables.

### `delete_object` *(ajouté 2026-09-12)*
```jsonc
{ "object_id": "278952507" }              // ou { "object_ids": ["...", "..."] }
```
Détache l'objet de son parent (`IGameObject::RemoveChild`) : la dernière référence tombe et **tout le sous-arbre** part avec lui (supprimer un groupe supprime son contenu). Sortie `{ "deleted": [{object_id, name}], "count", "failed"? }` — un id invalide ou une racine de scène est refusé sans interrompre le lot.

Deux écarts assumés par rapport au `Delete` de l'éditeur :
- **aucune entrée undo/redo** n'est créée → pas de Ctrl-Z après une suppression par le bridge (l'éditeur, lui, garde l'objet vivant dans sa pile d'undo, `Editor.cpp:796`) ;
- l'objet est retiré de la sélection avant destruction (`ISelection::Remove`) pour ne pas laisser un pointeur mort dans l'éditeur.

### `create_scene` *(ajouté 2026-09-12)*
```jsonc
{ "name": "City", "folder": "data/Scenes" }   // folder optionnel
```
Équivalent de **SceneList > New Scene** de l'éditeur : `IWorldResource::CreateSceneResource(file, BaseSceneType::Scene)` (`ImGuiSceneList.hpp:675`), qui écrit un `.scene` contenant un unique `Root`, enregistre la ressource et déclenche son chargement.

Deux conséquences à connaître :
- **le chargement est asynchrone** → la scène n'est pas encore dans le monde au retour de l'appel ; enchaîner sur `list_objects` pour lire son `root_id` ;
- **le monde ne mémorise la nouvelle scène qu'après `save_world`** (sinon le `.scene` existe sur disque mais `Game.world` ne le référence pas).

Garde-fous (`CreateSceneResource` écrase sans prévenir) : refus si le fichier existe déjà (`ALREADY_EXISTS`), si une scène du même nom est déjà chargée, ou si le nom contient un séparateur de chemin / joker (`INVALID_VALUE`).

Sortie : `{ "name", "file", "loading": true, "note" }`.

### `save_world`
Sauvegarde **chaque scène puis le fichier monde**, dans l'ordre du bouton « Save All » de l'éditeur (`ImGuiEditorView.hpp:85-95`).

> **Piège corrigé le 2026-09-12.** La V1 n'appelait que `IEngine::SaveWorld()`, qui ne réécrit que `data/Worlds/*.world` — un fichier de **références de scènes, sans aucun GameObject**. Le tool renvoyait `saved: true` alors que rien du travail de level design n'atteignait le disque. Le contenu vit dans les `.scene`, écrits uniquement par `IEngine::SaveScene(sceneRes)`, énumérables via `IWorldResource::GetSceneResourceCount/GetSceneResource` (`src/engine/IWorldResource.h:28-29`).

Sortie : `{ "saved", "world_saved", "scenes_saved", "scenes": [{ file, saved }] }`.

**Round-trip validé le 2026-09-12** (scène `Aiguelongue`) : groupe + 5 boîtes créés par le bridge → `save_world` → redémarrage de l'éditeur → objets présents, **UID identiques**, transforms exacts. Vérifié aussi : après suppression des objets de test et resauvegarde, le `.scene` est **bit à bit identique** à sa version d'avant-test (le sérialiseur ne produit pas de bruit de diff).

---

## Erreurs (jamais de crash silencieux — plan Phase 3)

```jsonc
{ "error": { "code": "OBJECT_NOT_FOUND", "message": "No object with UID 123", "object_id": "123" } }
```

| code | quand |
|---|---|
| `OBJECT_NOT_FOUND` | `FindByUID` renvoie `nullptr` |
| `NOT_A_GAMEOBJECT` | l'UID désigne un objet non `IGameObject` (Component, Resource…) |
| `INVALID_VALUE` | NaN/Inf, quaternion non normalisable, scale nul |
| `NO_ACTIVE_SCENE` | `GetActiveScene(Scene)` == null |
| `ENGINE_BUSY` | moteur en cours de (dé)chargement de monde |
| `NOT_SUPPORTED` | commande inconnue / hors périmètre V1 |

Côté serveur MCP : si `state.json` ne bouge pas après N secondes (moteur non lancé) → renvoyer une erreur explicite `ENGINE_NOT_RUNNING`, pas un timeout muet (plan Phase 4).

---

## Enveloppe de transport (option B — fichier)

`commands.jsonl` (une ligne par commande) :
```jsonc
{ "id": 42, "tool": "set_transform", "args": { "object_id": "1945741270", "scale": { "x": 8, "y": 20, "z": 8 } } }
```

`state.json` :
```jsonc
{
  "engine": { "running": true, "playing": false, "loading": false },
  "lastProcessedId": 42,
  "results": [
    { "id": 42, "ok": true, "data": { "object_id": "1945741270", "transform": { } } }
  ],
  "scene": { "world": "Game", "scene": "Aiguelongue", "objects": [ ] }
}
```
`results` = réponses aux commandes depuis le dernier snapshot (borné, ex. 100 dernières). `scene.objects` rempli seulement si un `list_objects` récent l'a demandé (évite d'écrire toute la scène chaque frame).
