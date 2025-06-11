# Notre décodeur JPEG à nous

Bienvenue sur la page d'accueil de _votre_ projet JPEG, un grand espace de liberté, sous le regard bienveillant de vos enseignants préférés.
Le sujet sera disponible dès le lundi 5 mai à l'adresse suivante : [https://formationc.pages.ensimag.fr/projet/jpeg/jpeg/](https://formationc.pages.ensimag.fr/projet/jpeg/jpeg/).

Comme indiqué lors de l'amphi de présentation, vous devrez organiser un point d'étape avec vos enseignants pour valider cette architecture logicielle.
Cette page d'accueil servira de base à cette discussion. En pratique, vous pouvez reprendre son contenu comme bon vous semble, mais elle devra au moins comporter les infos suivantes :

1. des informations sur le découpage des fonctionnalités du projet en modules, en spécifiant les données en entrée et sortie de chaque étape ;
2. (au moins) un dessin des structures de données de votre projet (format libre, ça peut être une photo d'un dessin manuscrit par exemple) ;
3. une répartition des tâches au sein de votre équipe de développement, comportant une estimation du temps consacré à chacune d'elle (là encore, format libre, du truc cracra fait à la main, au joli Gantt chart).

Rajouter **régulièrement** des informations sur l'avancement de votre projet est aussi **une très bonne idée** (prendre 10 min tous les trois chaque matin pour résumer ce qui a été fait la veille, établir un plan d'action pour la journée qui commence et reporter tout ça ici, par exemple).

# Planning prévisionnel 

| Version | Nom de code   | Caractéristiques | Temps estimé |
| :---:   | :---   	  | :--- | :--- |
| 1 	  | Invader 	  | Décodeur d'images 8x8 en niveaux de gris | J+ 4 |
| 2 	  | Noir et blanc | Extension à des images grises comportant plusieurs blocs | J + 6 |
| 3 	  | Couleur 	  | Extension à des images en couleur | J+8 |
| 4	  | Sous-ech 	  | Extension avec des images avec sous-échantionnage | J +10 |

# Droit dépôt git

Si vous avez besoin de droit supplémentaire sur votre dépôt git, venez voir les profs au premier étage du bâtiment E.


# Liens utiles

- Bien former ses messages de commits : [https://www.conventionalcommits.org/en/v1.0.0/](https://www.conventionalcommits.org/en/v1.0.0/) ;
- Problème relationnel au sein du groupe ? Contactez [Pascal](https://fr.wikipedia.org/wiki/Pascal,_le_grand_fr%C3%A8re) !
- Besoin de prendre l'air ? Le [Mont Rachais](https://fr.wikipedia.org/wiki/Mont_Rachais) est accessible à pieds depuis le bâtiment E !
- Un peu juste sur le projet à quelques heures de la deadline ? Le [Montrachet](https://www.vinatis.com/achat-vin-puligny-montrachet) peut faire passer l'envie à certains de vos profs de vous mettre une tôle !


# Un Résumé de notre travail dès le démarrage du projet logiciel:

Dans le cadre de notre projet de décodage d’images JPEG, nous avons progressivement construit un décodeur capable de lire, interpréter et convertir les fichiers JPEG en images au format PPM et PGM. Le projet a été mené selon une progression structurée, répartie en plusieurs phases que nous détaillons ci-dessous.

- Phase 1 : Préparation de la stratégie de travail et création des modules de base

Nous avons commencé par planifier notre approche, en définissant les rôles de chacun et en mettant en place les modules essentiels (lecture binaire, arbres de Huffman, IQZZ, IDCT, lecture de l'entete , ecriture PPM/PGM, etc ..).

- Phase 2 : Première version du programme jpeg2ppm et décodage de l’image invader.jpg

Cette étape a permis la mise en œuvre d’une première version fonctionnelle de notre décodeur, capable de traiter une image simple (un seul bloc 8x8 en noir et blanc).

- Phase 3 : Gestion des troncatures et prise en charge des images en niveaux de gris à multiples blocs et des images couleur (non échantillonnées)

Nous avons ajouté la gestion des troncatures , et étendu la compatibilité du décodeur aux images en niveaux de gris à multiples blocs et aux images couleur non échantillonnées.

- Phase 4 : Implémentation de l’upsampling et décodage de toutes les images proposées, ainsi que d’exemples supplémentaires

Grâce à l’implémentation de l’upsampling, notre décodeur a pu gérer l’ensemble des images fournies, ainsi que d'autres images JPEG externes (muhammad_ali.jpeg, peak.jpeg, Mona_Lisa.jpg).

- Phase 5 : Factorisation du code et tentatives d’optimisation

Nous avons ensuite travaillé à rendre le code plus modulaire et lisible, tout en cherchant à améliorer les performances.