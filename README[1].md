# Gestion des vols et réservations — Cotonou

Application console en C++17 pour gérer les passagers, les vols, les réservations et les paiements d’une agence de voyages à Cotonou.

## Fonctionnalités

Le menu de l’application permet de :

1. Enregistrer un passager avec son nom, son téléphone, son adresse e-mail facultative et son numéro de pièce d’identité.
2. Ajouter un vol avec son code, son origine, sa destination, sa date et heure de départ, sa capacité et son tarif en FCFA.
3. Afficher les vols disponibles et leurs places restantes.
4. Rechercher un vol par code, ville ou date.
5. Réserver un vol pour un passager.
6. Enregistrer un paiement et confirmer une réservation.
7. Annuler une réservation.
8. Consulter la liste des réservations.
9. Afficher le détail d’une réservation.
10. Quitter l’application.

## Règles de gestion

- Le passager doit être enregistré avant de réserver.
- Une réservation concerne un seul passager et un seul vol.
- Un passager ne peut pas avoir plusieurs réservations actives pour le même vol.
- Une réservation annulée ne bloque pas une nouvelle réservation pour ce vol.
- La capacité du vol est contrôlée à la réservation et au paiement.
- La place est comptée comme occupée lorsque le paiement confirme la réservation.
- Une réservation annulée ne peut pas être payée et libère sa place.
- Le paiement est simulé : aucun service bancaire n’est connecté et aucun remboursement n’est calculé.

## Analyse et modélisation

### Préambule

Le projet vise à simplifier la gestion des voyages d’une agence située à Cotonou. L’application permet d’enregistrer les voyageurs, de gérer les vols proposés et de suivre les réservations jusqu’au paiement ou à l’annulation.

### 1. Quelles sont les classes nécessaires ?

Le modèle comprend les entités suivantes :

- **Passager** : personne qui souhaite voyager.
- **Vol** : trajet aérien proposé par l’agence.
- **Réservation** : demande d’un passager pour un vol.
- **Paiement** : règlement associé à une réservation.
- **AgenceVoyages** : composant qui gère les passagers, les vols et les réservations.
- **StatutReservation** : énumération des états `EnAttente`, `Confirmee` et `Annulee`.

### 2. Quels sont les attributs et les méthodes de chaque classe ?

- **Passager** — Attributs : identifiant, nom, téléphone, e-mail et numéro de passeport ou de pièce d’identité.
- **Vol** — Attributs : identifiant, code, origine, destination, départ, capacité, prix et état actif.
- **Réservation** — Attributs : identifiant, identifiants du passager et du vol, statut, date de création et date de paiement éventuelle. Ses opérations permettent de confirmer ou d’annuler la réservation.
- **Paiement** — Attributs : identifiant de la réservation, montant, mode de paiement et date du paiement.
- **AgenceVoyages** — Gère les collections de passagers, de vols, de réservations et de paiements. Ses opérations permettent d’enregistrer un passager, d’ajouter ou rechercher un vol, de réserver, de payer, d’annuler et de consulter les réservations.

### 3. Quelles sont les relations entre ces classes ?

- Un passager peut effectuer plusieurs réservations, mais chaque réservation concerne un seul passager.
- Un vol peut être lié à plusieurs réservations, mais chaque réservation concerne un seul vol.
- Une réservation peut ne pas être payée ou être liée à un paiement.
- L’agence gère les passagers, les vols et les réservations.
- Chaque réservation possède un statut défini par `StatutReservation`.

### 4. Quelles sont les multiplicités ?

- **Passager — Réservation** : un passager peut avoir `0..*` réservations ; chaque réservation a `1` passager.
- **Vol — Réservation** : un vol peut avoir `0..*` réservations ; chaque réservation concerne `1` vol.
- **Réservation — Paiement** : une réservation peut avoir `0..1` paiement ; chaque paiement concerne `1` réservation.
- **AgenceVoyages — Passager** : l’agence gère `0..*` passagers.
- **AgenceVoyages — Vol** : l’agence gère `0..*` vols.
- **AgenceVoyages — Réservation** : l’agence gère `0..*` réservations.

### 5. Diagramme de classes UML

Le diagramme est disponible dans `docs/diagramme-classes.png`. Son fichier PlantUML modifiable se trouve dans `docs/diagramme-classes.puml`.

Le paiement fait passer une réservation de l’état « en attente » à l’état « confirmée ». L’annulation libère la place. Le sujet ne précisant pas les règles de remboursement, cette version ne les calcule pas.

## Captures d’écran

### Enregistrement d’un passager

![Capture console de l’enregistrement d’un passager](docs/capture-enregistrement-passager.png)

### Ajout d’un vol

![Capture console de l’ajout d’un vol](docs/capture-ajout-vol.png)

## Fichiers présents dans le projet

- `main.cpp` : code source C++ de l’application.
- `docs/diagramme-classes.png` : diagramme UML.
- `docs/diagramme-classes.puml` : source PlantUML du diagramme.
- `docs/capture-enregistrement-passager.png` : capture de l’enregistrement d’un passager.
- `docs/capture-ajout-vol.png` : capture de l’ajout d’un vol.

## Limite

Les données sont gardées en mémoire pendant l’exécution seulement. Elles ne sont pas sauvegardées après la fermeture du programme.

