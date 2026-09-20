# Plan — MCP Bridge pour moteur 3D custom (assistance Level Design)

> **Destinataire de ce document : Claude Code.**
> Ce fichier est un brief d'implémentation. Exécute les phases dans l'ordre. Ne saute pas la Phase 0 : elle conditionne toutes les décisions techniques qui suivent. Si une information manque pour continuer une phase, **arrête-toi et documente la question dans `docs/OPEN_QUESTIONS.md`** plutôt que de deviner ou d'improviser une solution.

---

## 0. Contexte et objectif

Le moteur 3D est un moteur custom (C++ ~94%, HLSL ~5%, C# <1%, Python <1%) développé par l'auteur. L'objectif est de pouvoir piloter le moteur depuis Claude Code via un serveur MCP (Model Context Protocol), pour accélérer la construction de niveaux (Level Design / LD) — par exemple poser un plan de ville en plaçant des primitives (cubes, etc.) selon une grille de rues/blocks.

**Portée V1 (ce document) :** lire et modifier la **position / rotation / échelle** d'objets déjà présents dans une scène active. Rien d'autre.

## 1. Règle d'or — non négociable

**Aucun fichier existant du moteur ne doit être modifié.** Toute intégration se fait via du code additif :
- nouveaux fichiers, nouveau dossier/module isolé, nouveau projet dans la solution/CMake ;
- si un point d'extension existant (plugin API, hook de scripting, système d'entités déjà extensible) permet de brancher le bridge sans toucher au cœur, l'utiliser en priorité ;
- si aucun point d'extension n'existe et qu'un minimum de câblage est réellement inévitable (ex: enregistrer un nouveau système dans la boucle de frame), le signaler explicitement dans `docs/OPEN_QUESTIONS.md` avec le diff proposé, et attendre validation avant de committer.

Avant et après chaque phase impliquant du code moteur : `git status` / `git diff` pour prouver qu'aucun fichier existant n'a été touché (uniquement des ajouts).

## 2. Point bloquant identifié dès maintenant — à vérifier en priorité en Phase 0

Le cas d'usage cible ("construire un plan de ville") implique de poser un nombre arbitraire de volumes, pas seulement de repositionner un petit nombre d'objets déjà placés à la main.

**Mise à jour : un cube de référence 1m × 1m × 1m (mesh unitaire) résout la question « qu'est-ce qu'une primitive ».** Un scale non-uniforme (X/Y/Z indépendants) sur ce cube unique suffit à produire n'importe quel volume rectangulaire — bâtiment, mur, séparateur de rue — sans avoir besoin que le moteur sache générer plusieurs types de primitives (sphère, cylindre, etc.). V1 peut donc reposer sur un seul mesh de référence.

Ce qui reste à trancher est donc reformulé : **le moteur peut-il dupliquer/instancier un nœud de scène existant par du code externe** (plutôt que « créer une géométrie à partir de rien ») ? C'est une question généralement plus facile à résoudre : un copier-coller d'objet dans l'éditeur repose presque toujours sur une fonction interne de duplication de nœud, qui est un candidat naturel à exposer sans toucher à la structure du moteur.

- **(a)** Le moteur expose une fonction de duplication/instanciation de nœud (scripting, commande console, API de scène). → V1 peut inclure `spawn_object(source_object_id, transform)` qui clone le cube de référence et applique un nouveau transform.
- **(b)** Aucun moyen de dupliquer sans passer par l'éditeur/UI. → V1 se limite à repositionner un **pool de cubes de référence pré-placés manuellement** (le même cube 1×1×1, dupliqué à la main N fois dans l'éditeur), que le MCP redimensionne/repositionne/masque un par un. Limitation acceptable pour un premier prototype, à communiquer clairement à l'utilisateur.

**Ne pas supposer (a) par défaut.** La Phase 0 doit trancher factuellement, avec preuve à l'appui (fichier + ligne de code).

## 3. Hors-scope V1 (explicitement exclu, ne pas implémenter)

- Création/suppression de mesh custom (hors primitives basiques)
- Matériaux, textures, éclairage, physique, gameplay/scripting de logique
- Undo/redo, historique de modifications
- Édition multi-utilisateur/concurrente
- Tout ce qui touche au build/packaging du jeu final

---

## Phase 0 — Reconnaissance du moteur (lecture seule, aucun code écrit)

Objectif : cartographier le moteur sans y toucher, pour que les phases suivantes reposent sur des faits vérifiés et non des suppositions.

Produire `docs/engine-analysis.md` répondant, **avec citation du fichier source et des lignes concernées** pour chaque réponse :

1. **Représentation des objets de scène** : ECS ? scene graph ? liste de nodes ? Où sont définies les classes/structures `Transform`, `Position`, `Rotation`, `Scale` ?
2. **Format de la rotation** : quaternion, angles d'Euler, ou matrice ? (critique pour éviter des bugs de conversion silencieux)
3. **Identifiant d'objet stable** : chaque objet a-t-il un ID/GUID stable, ou seulement un pointeur/index qui peut changer d'une frame à l'autre ou d'un chargement à l'autre ? Si non → à signaler, un ID stable est un prérequis pour un bridge fiable.
4. **Couche de scripting existante** : le C# et/ou Python présents dans le repo exposent-ils déjà une API de la scène (lister/modifier des objets) ? Si oui, c'est le chemin d'intégration le plus rapide et le moins invasif.
5. **Console de commandes / debug commands runtime** : existe-t-il un système de commandes exécutables au runtime (souvent présent dans les moteurs custom pour le debug) ?
6. **Réseau / IPC existant** : y a-t-il déjà un serveur (RPC, HTTP, socket) pour du remote debugging, du live-reload de shaders, du profiling distant, etc. ? Réutilisable ou non ?
7. **Boucle de vie** : le moteur tourne-t-il en mode éditeur avec une boucle vivante interrogeable en continu, ou uniquement en exécutable compilé/jeu lancé ? Peut-on se connecter à un process déjà lancé (hot-connect), ou faut-il relancer à chaque changement ?
8. **Duplication/instanciation de nœud par code** : existe-t-il une fonction interne du type `DuplicateNode()`, `CloneObject()`, `Instantiate(sourceId, transform)` utilisée par le copier-coller de l'éditeur ou par du contenu de démo ? C'est la fonction clé à trouver — plus généralement disponible qu'une génération de primitive from-scratch (voir point bloquant §2).
9. **Système de build** : CMake, solution Visual Studio, autre ? Comment ajouter un nouveau module/projet sans modifier les fichiers de build existants (nouvelle cible liée en plus, pas de modification des cibles actuelles) ?
10. **Sérialisation de scène** : format de sauvegarde (JSON, binaire propriétaire, autre) ? Utile pour vérifier que les modifications faites via le bridge persistent bien après sauvegarde/rechargement.

---

## Phase 1 — Décision d'architecture : couche de communication

À documenter dans `docs/architecture.md` (format ADR), **après** la Phase 0, en fonction de ce qui a été découvert.

### Options (invasivité croissante)

| Option | Description | Quand la choisir |
|---|---|---|
| **A. Bridge via scripting existant** | Si le moteur expose déjà du C#/Python scriptable, exposer les 3 fonctions cibles directement depuis un script, zéro nouveau code moteur en C++ | Si Phase 0 confirme l'existence d'une couche de scripting fonctionnelle |
| **B. Fichier de commandes (polling)** | Le moteur lit un `commands.json` (watcher ou poll à chaque frame) et écrit un `state.json` en retour | Le plus simple à ajouter sans toucher au réseau, latence plus élevée, bon choix de prototype si aucune option plus légère n'existe |
| **C. Pipe nommé / socket TCP loopback** | Petit thread serveur ajouté dans un module additif, protocole JSON simple | Si le moteur tourne en process persistant (éditeur) et qu'on veut une latence faible |
| **D. Réutilisation d'un canal réseau/debug existant** | Si Phase 0 a trouvé un serveur RPC/debug déjà présent | Le moins de code à écrire, mais dépend fortement de ce qui existe |

**Recommandation par défaut si rien de spécifique n'est trouvé en Phase 0 : Option B (fichier de commandes)** pour le prototype initial — le moins invasif, le plus rapide à valider de bout en bout, migration vers C possible ensuite si la latence gêne l'usage interactif.

Documenter le choix retenu, les alternatives écartées et pourquoi (format ADR classique : Contexte / Décision / Options considérées / Conséquences).

---

## Phase 2 — Contrat de données (schéma partagé)

Schéma JSON minimal, à ajuster selon les réponses de la Phase 0 (notamment le format de rotation) :

```json
{
  "object_id": "string — identifiant stable, pas un pointeur volatile",
  "name": "string",
  "primitive_type": "cube | sphere | cylinder | plane | unknown",
  "transform": {
    "position": { "x": 0.0, "y": 0.0, "z": 0.0 },
    "rotation": { "x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0 },
    "scale": { "x": 1.0, "y": 1.0, "z": 1.0 }
  }
}
```

- Si le moteur utilise des angles d'Euler en interne, garder le quaternion comme contrat MCP (plus robuste) et faire la conversion côté bridge moteur, pas côté serveur MCP.
- Toutes les valeurs de `set_transform` doivent être **optionnelles indépendamment** (on peut vouloir changer seulement `scale` sans toucher à `position`).

---

## Phase 3 — Module bridge côté moteur (code additif C++)

- Nouveau dossier isolé, nom proposé : `Engine/Modules/MCPBridge/` (chemin exact à adapter à l'arborescence réelle découverte en Phase 0).
- 3 fonctions exposées pour V1 :
  - `list_objects()` → liste des objets de la scène active avec leur transform (schéma ci-dessus)
  - `get_transform(object_id)`
  - `set_transform(object_id, { position?, rotation?, scale? })`
- Intégration dans la boucle moteur : uniquement via un point d'extension existant si Phase 0 en a trouvé un (ex: système enregistrable, subscriber d'events). Sinon, documenter précisément le point d'insertion minimal nécessaire dans `docs/OPEN_QUESTIONS.md` avant de l'appliquer.
- Gestion d'erreurs : objet introuvable, ID invalide, valeurs hors plage → retour d'erreur structuré, jamais un crash silencieux.

**Signaler immédiatement si :**
- Pas d'identifiant stable par objet (bloquant, voir Phase 0 point 3)
- Pas de moyen d'itérer la liste des objets de la scène depuis l'extérieur du moteur

---

## Phase 4 — Serveur MCP

- Process séparé, aucune modification du moteur ici (c'est un client externe qui parle au bridge de la Phase 3).
- Stack recommandée : Node.js + TypeScript avec le SDK officiel MCP (`@modelcontextprotocol/sdk`). **Vérifier la documentation officielle actuelle sur modelcontextprotocol.io avant d'écrire le code** : l'API du SDK évolue, ne pas se fier à un exemple mémorisé qui pourrait être obsolète.
- 3 tools MCP exposés, correspondant 1:1 aux fonctions du bridge :
  - `list_objects` — pas de paramètre
  - `get_transform` — paramètre `object_id`
  - `set_transform` — paramètres `object_id`, `position?`, `rotation?`, `scale?`
- Chaque tool doit avoir une description claire et des schémas de paramètres explicites (types, obligatoire/optionnel) — c'est ce que Claude Code lira pour savoir quand et comment les appeler.
- Timeout et gestion de la perte de connexion au moteur (le moteur peut ne pas être lancé) → message d'erreur clair plutôt qu'un timeout muet.

---

## Phase 5 — Cas de test V1 : plan de ville simple

Scénario de validation de bout en bout :
1. Scène de test avec un pool de cubes de référence 1×1×1 déjà placés (nombre à définir selon la résolution du point bloquant §2). Chaque bâtiment de la ville est obtenu en scalant ce même cube non-uniformément (ex: 8×8×20 pour un immeuble), pas en changeant de mesh.
2. Demander à Claude Code, via le chat Claude Code (pas ce document) : *« Dispose les cubes disponibles en grille 5×5 espacée de 2 unités pour former un plan de blocks de ville, garde une rue de large 1 unité entre chaque ligne. »*
3. Vérifier visuellement dans le moteur que le résultat correspond.
4. Vérifier qu'aucun fichier existant du moteur n'a été modifié (`git status`).

Si le point bloquant §2 est résolu en faveur de (a) (création de primitives possible), ce scénario peut être étendu à un nombre d'objets arbitraire plutôt qu'un pool fixe — à traiter en V1.1, pas en V1.

---

## Phase 6 — Validation finale

- Checklist avant de considérer V1 terminé :
  - [ ] `docs/engine-analysis.md` complet avec citations
  - [ ] `docs/architecture.md` (ADR) rédigé et cohérent avec Phase 0
  - [ ] `docs/OPEN_QUESTIONS.md` à jour, vide si tout est résolu
  - [ ] Module bridge compilé sans modifier de fichier existant (diff vérifié)
  - [ ] Serveur MCP fonctionnel, testé avec les 3 tools depuis Claude Code
  - [ ] Scénario de la Phase 5 validé visuellement dans le moteur
  - [ ] Aucune régression sur le build/run normal du moteur sans le bridge activé

---

## Roadmap V1.1+ (hors scope de ce document, à ne pas anticiper maintenant)

- `create_primitive(type, transform)` si Phase 0 le permet
- `delete_object(object_id)`
- Opérations batch (déplacer/créer N objets en un seul appel, pour limiter les allers-retours)
- Snap-to-grid côté serveur MCP (utilitaire, pas dans le moteur)
- Sauvegarde de scène déclenchable depuis MCP

---

## Arborescence de fichiers attendue en fin de V1

```
docs/
  engine-analysis.md
  architecture.md
  OPEN_QUESTIONS.md
Engine/Modules/MCPBridge/        (nom/chemin à adapter, voir Phase 3)
  ...
mcp-server/
  package.json
  src/
    index.ts
    tools/
      list_objects.ts
      get_transform.ts
      set_transform.ts
```

---

## Questions ouvertes à remonter à l'utilisateur (ne pas trancher seul)

1. Le point bloquant §2 : primitives pré-placées (pool) vs. création dynamique — dépend de ce que la Phase 0 trouve.
2. Format de rotation interne du moteur (Euler vs quaternion) — impacte le schéma de données.
3. Existence ou non d'un point d'extension propre pour brancher le bridge sans toucher à la boucle de frame existante.
4. Si aucune option de communication n'est vraiment non-invasive (Phase 1), présenter le diff minimal proposé avant de l'appliquer.
