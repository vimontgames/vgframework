# Quickstart — construire un quartier depuis Claude Code

Scénario de bout en bout : lancer le moteur, connecter Claude, poser un petit quartier.
Durée : ~10 min la première fois, ~1 min les fois suivantes.

> **Rien n'est écrit sur disque tant que `save_world` n'est pas appelé.** Recharger la scène
> dans l'éditeur annule tout ce qui suit. C'est le filet de sécurité de ce scénario.

---

## A. Installation (une seule fois)

```powershell
# 1. compiler le serveur MCP
cd D:\GitHUB_Repo\mcp-server
npm install
npm run build

# 2. l'enregistrer auprès de Claude Code
#    -e et non --env. VG_MCP_DIR doit designer le MEME dossier que celui du moteur.
claude mcp add vgframework -e VG_MCP_DIR=D:/GitHUB_Repo/mcp -- node D:/GitHUB_Repo/mcp-server/dist/index.js
```

Vérification :

```powershell
claude mcp list      # attendu : vgframework: ... - ✔ Connected
```

⚠️ **Redémarre Claude Code après cet enregistrement.** Une session déjà lancée ne voit pas
un serveur MCP ajouté après son démarrage. `/exit` puis `claude`, ensuite `/mcp` doit lister
`vgframework` avec ses 10 tools (`engine_status`, `list_objects`, `get_transform`,
`set_transform`, `spawn_object`, `spawn_objects`, `create_group`, `create_scene`,
`delete_object`, `save_world`). Idem après un `npm run build` du serveur : la session en cours garde
l'ancienne version chargée.

---

## B. Lancer le moteur (à chaque session)

La variable d'environnement doit être posée **dans le terminal qui lance l'exe**, et le
répertoire courant doit être la racine du repo (le bridge crée `mcp/` à côté).

```powershell
cd D:\GitHUB_Repo
$env:VG_MCP_BRIDGE = "1"
.\editor.exe
```

Deux signes que le bridge est actif :

1. dans la console de l'éditeur : `[MCPBridge] enabled - watching ".../mcp/commands.jsonl"`
2. le fichier `D:\GitHUB_Repo\mcp\state.json` existe et contient `"running": true`

Sans la variable, le plugin n'est pas chargé du tout et tous les tools répondront
`ENGINE_NOT_RUNNING`.

---

## C. Vérifier la liaison

Dans Claude Code :

> Vérifie que le moteur répond, puis liste les objets de la scène.

Attendu sur la scène de démarrage `Aiguelongue` :

```
browse in scene(s) Aiguelongue — 8 of 8 match(es):
   114612410  Aiguelongue/Cameras       (3 children)
  1177627180  Aiguelongue/UI            (6 children)
  3646148306  Aiguelongue/Environment   (2 children)
  1093025436  Aiguelongue/Level         (7 children)
  ...
```

`list_objects` ne descend que d'un niveau par défaut — un listing récursif complet de cette
scène fait 9,7 Mo de JSON. Pour explorer : « montre-moi ce qu'il y a sous Level »
(`parent_id`) ou « cherche les objets dont le nom contient X » (`name_contains`).

---

## D. Poser la graine — seule action manuelle

`spawn_object` **clone un objet déjà présent dans la scène**. Il ne sait pas instancier un
prefab depuis le disque (hors périmètre V1). Il faut donc un premier `Box_Base` à la main.

Dans l'éditeur : glisse `data/Prefabs/Box_Base.prefab` dans la hiérarchie, sous `Level` par
exemple, et pose-le dans une zone dégagée. Note approximativement où il est.

Puis, dans Claude Code :

> Trouve l'objet Box_Base et donne-moi son id, sa position et ses dimensions.

Tu obtiendras son `object_id` (l'UID, à réutiliser ensuite) et son `bounds_local`, qui
révèle la taille réelle du mesh et la position du pivot.

---

## E. Un premier essai minuscule (recommandé)

Avant les 9 bâtiments, valide la mécanique sur trois boîtes :

> À partir du Box_Base, crée 3 immeubles alignés sur l'axe X, espacés de 12 m,
> de 8×8 m d'emprise et de hauteurs 10, 18 et 6 m. Pose-les au niveau du sol.

Regarde le résultat dans l'éditeur. Si les proportions sont bonnes, enchaîne.

---

## F. Le quartier

> Construis un petit quartier à partir du Box_Base.
> Grille 3×3 de blocs, chaque bloc fait 8×8 m d'emprise, avec 4 m de rue entre les blocs
> (donc un pas de 12 m). Le coin du quartier part de la position actuelle du Box_Base.
> Donne à chaque bâtiment une hauteur différente entre 6 et 25 m pour casser la monotonie.
> Tous les bâtiments posés au niveau du sol.
> Nomme-les Bat_00 à Bat_08.

### Ce que Claude doit appliquer (conventions du moteur)

Ces règles sont déjà dans les descriptions des tools, mais autant les connaître pour
relire ce qu'il fait :

| Règle | Conséquence |
|---|---|
| Le monde est **Z-up** | X et Y = plan du sol, **Z = hauteur** |
| Pivot de `Box_Base` **au centre de sa base** | `position.z = 0` pose au sol ; `scale.z` = hauteur totale, **sans offset de demi-hauteur** |
| Unités en mètres | un bâtiment 8×8×20 = `scale {x:8, y:8, z:20}` |
| Cap (yaw) d'angle `a` | `rotation {x:0, y:0, z:sin(a/2), w:cos(a/2)}` |

Un bâtiment de 20 m se fait donc avec `scale {x:8, y:8, z:20}` à `position {z:0}` — et non
`z:10`, réflexe naturel avec un pivot centré, mais faux ici.

### Rangement et débit (depuis le 2026-09-12)

Pour un quartier, Claude doit d'abord créer un groupe (`create_group`) puis poser les
bâtiments en **un seul** `spawn_objects` avec ce groupe en `parent_id` — 150 clones en
0,23 s, et la scène garde un seul nœud au lieu de 150 objets à plat. Si ce n'est pas ce
qu'il fait, dis-le explicitement :

> Range tout ça dans un groupe « Quartier_Nord » et crée les bâtiments en un seul lot.

### Variantes à tester

> Fais varier l'orientation : donne à un bâtiment sur trois un cap de 90°.

> Élargis les rues à 6 m et refais la grille.

> Liste les bâtiments que tu viens de créer avec leurs hauteurs.

---

## G. Garder ou jeter

- **Jeter** : recharge la scène dans l'éditeur. Rien n'a touché le disque.
- **Garder** : « Sauvegarde le monde. » (tool `save_world`, qui écrit les `.scene` **et** le
  `.world`). Round-trip vérifié le 2026-09-12 : après redémarrage de l'éditeur, objets,
  hiérarchie, transforms et **UID** sont identiques.

⚠️ À connaître **avant** de sauvegarder :

- `save_world` **écrase les `.scene` sur disque** — sur une scène qui compte, commite ou
  sauvegarde une copie avant ;
- un objet créé n'obtient un `object_id` stable qu'**après** la sauvegarde. Avant, son id
  change à chaque rechargement de scène ;
- `delete_object` existe depuis le 2026-09-12, mais **sans undo** : pas de Ctrl-Z dans
  l'éditeur après une suppression par le bridge (supprimer un groupe emporte son contenu).

---

## Dépannage

| Symptôme | Cause probable |
|---|---|
| Les tools n'apparaissent pas dans `/mcp` | session Claude Code pas redémarrée après `claude mcp add` |
| `ENGINE_NOT_RUNNING` | éditeur pas lancé, ou lancé sans `VG_MCP_BRIDGE`, ou `VG_MCP_DIR` ≠ dossier du moteur |
| `NO_ACTIVE_SCENE` | aucun monde chargé, ou commande envoyée pendant le démarrage |
| `OBJECT_NOT_FOUND` | id périmé : l'éditeur a rechargé la scène. Refais un `list_objects` |
| L'éditeur crashe sur `ScreenSpaceAmbient - Editor 0` | bug GTAO amont — voir `PHASE3-4-IMPLEMENTATION.md` § « Piège connu » |
| Les objets ne bougent pas visuellement | vérifie que l'éditeur a bien le focus sur une vue toolmode |

Tout retirer : `claude mcp remove vgframework`.
