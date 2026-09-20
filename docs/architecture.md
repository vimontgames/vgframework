# ADR 001 — Couche de communication du bridge MCP

Statut : **Accepté et implémenté** (Phases 3-4, cf. `docs/PHASE3-4-IMPLEMENTATION.md`)
Date : 2026-09-06
Basé sur : `docs/engine-analysis.md` (Phase 0)

> Écart d'implémentation vs. cette ADR : la conversion de rotation ne passe pas par
> `TRSToFloat4x4(T, quaternion, S)` (`Math.h:129`) — cette surcharge s'est révélée
> incohérente avec le reste du moteur (translation en 4ᵉ colonne). Le module fait sa
> propre décomposition/recomposition matricielle (base I/J/K en lignes). Position et
> scale sont exacts ; la rotation reste à valider visuellement (`OPEN_QUESTIONS.md` #4).

---

## Contexte

Piloter la scène active du moteur vgframework depuis Claude Code via un serveur MCP, pour du level design (poser/redimensionner des cubes de référence). Périmètre V1 : `list_objects`, `get_transform`, `set_transform` (+ `spawn_object` candidat, cf. OQ #3).

Contraintes issues de la Phase 0 :
- **Aucun fichier moteur existant modifié** (sauf câblage minimal validé, OQ #1).
- Pas de scripting runtime, pas de console de commandes, **pas d'IPC existant** → tout canal est à créer.
- Le moteur tourne en boucle éditeur vivante ; `ToolUpdate` est appelé chaque frame hors Play (`src/engine/Engine.cpp:983-991`) → bon endroit pour pomper un canal.
- Objets adressables par `UID u32` via `IFactory::FindByUID()` (`src/core/IFactory.h:69`).
- Transform = `float4x4` ; conversions TRS/quaternion dispo (`src/core/Math/Math.h:123-132`).

---

## Décision

**Adopter l'option B (fichier de commandes JSON) pour V1**, implémentée dans un **module bridge C++ additif** (`src/mcpbridge/` + `sharpmake/vg.mcpbridge.sharpmake.cs`), pompé depuis le hook `ToolUpdate` de la boucle moteur.

Le serveur MCP (Node/TypeScript, process séparé) écrit `mcp/commands.jsonl` et lit `mcp/state.json`. Le module moteur fait l'inverse.

Migration vers l'option C (socket TCP loopback) prévue et non bloquante (voir *Conséquences*).

### Schéma

```
Claude Code ──stdio──> serveur MCP (Node)  ──écrit──> <projet>/mcp/commands.jsonl
                             (process séparé)  <──lit──  <projet>/mcp/state.json
                                                              ▲          │
                                             fichiers sur disque         │
                                                              │          ▼
   moteur (process éditeur) : module mcpbridge.dll  ──lit commands, applique, écrit state──
      └─ tick appelé depuis Engine::RunOneFrame() bloc ToolUpdate  (câblage OQ #1)
```

### Emplacement des fichiers d'échange

`<GetProjectPath()>/mcp/` — à côté du projet de jeu chargé, hors `data/` versionné lourd.
Fallback : `<cwd>/mcp/` (racine repo). Répertoire créé par le module s'il n'existe pas. Ajouté au `.gitignore`.

### Format

- `commands.jsonl` : une commande JSON par ligne (append-only), champ `id` monotone. Le module traite les lignes non encore vues (curseur persistant en mémoire + `lastProcessedId` écrit dans `state.json`), puis **tronque** le fichier quand toutes les commandes sont traitées et que le serveur MCP a accusé réception (`ack` dans un `commands.ack` ou simplement : le module tronque après lecture, le serveur n'attend pas de persistance).
- `state.json` : snapshot complet écrit à chaque frame où (a) une commande a été appliquée, ou (b) toutes les N frames si `list_objects` a été demandé. Écriture atomique (`state.json.tmp` + rename).

---

## Options considérées

| Option | Latence | Code moteur | Robustesse | Verdict |
|---|---|---|---|---|
| **A — scripting existant** | — | nul | — | ❌ impossible : pas de scripting (analysis §4) |
| **B — fichier de commandes** | ~1 frame + I/O disque (10–30 ms typiques, borné par le poll) | module additif + hook `ToolUpdate` | élevée (pas de socket, survit aux redémarrages, débuggable à la main) | ✅ **retenu V1** |
| **C — socket TCP loopback** | <1 ms | module additif + hook + **thread réseau** (`WSAStartup`, accept, buffers) | moyenne (gestion déco, ports, pare-feu Windows) | ⏭️ V1.1 si la latence de B gêne l'usage interactif |
| **D — canal debug existant** | — | — | — | ❌ impossible : aucun IPC existant (analysis §6) |

### Pourquoi B plutôt que C d'emblée

1. **Moins de surface de code moteur** : pas de thread, pas de gestion de socket → le module bridge reste ~200 lignes, entièrement mono-thread dans le tick `ToolUpdate` (pas de synchro avec la scène à gérer).
2. **Débuggabilité** : on peut inspecter/éditer `commands.jsonl` et `state.json` à la main pendant le développement.
3. **Le cas d'usage tolère la latence** : le level design par lots (« pose 25 cubes en grille ») n'est pas sensible à 30 ms ; c'est un aller-retour, pas du drag temps réel.
4. **Chemin de migration propre** : l'interface interne du module (`applyCommand(Command&) → Result`, `buildState() → json`) est identique pour B et C ; seul le transport change. Passer à C = ajouter un thread qui appelle les mêmes fonctions.

### Pourquoi un module C++ et pas un process 100 % externe

Un process externe qui patche les `.scene` XML sur disque ne serait pas répercuté sur la scène **déjà chargée en mémoire** (pas de rechargement à chaud — analysis §7), donc aucun retour visuel. Le pilotage live impose du code dans le process moteur.

---

## Conséquences

### Positives
- Aucune dépendance réseau ; fonctionne même si le moteur est lancé/relancé plusieurs fois.
- Le module est désactivable par `#if VG_ENABLE_MCPBRIDGE` → build normal intact (checklist Phase 6).
- `set_transform` / `list` fonctionnent **en mode éditeur sans Play** (hook `ToolUpdate`).

### Négatives / risques
- **Câblage minimal inévitable** dans 2 fichiers existants (OQ #1) — à faire valider avant commit.
- Latence bornée par la fréquence de poll (1 frame) + I/O disque ; acceptable V1, à surveiller.
- `commands.jsonl` append-only : prévoir la troncature pour éviter la croissance illimitée.
- Écriture de `state.json` chaque frame si mal borné → n'écrire que sur changement ou sur demande explicite.
- UID non stable pour objets créés non sauvegardés (analysis §3) → `spawn_object` doit renvoyer l'UID attribué, et on recommande `SaveWorld` après une session de placement.

### Suivi
- ADR 002 (à écrire si migration) : passage au transport socket loopback.
- Le choix du langage/SDK du serveur MCP est traité en Phase 4 (Node + `@modelcontextprotocol/sdk`, vérifier la doc courante avant d'coder).
