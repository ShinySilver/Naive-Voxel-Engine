# iVy

![A very random screenshot from a very experimental build](/resources/screenshots/experimental_build_1.png?raw=true)

Un moteur de jeu voxel, designé avec comme objectif la pleine utilisation du GPU, du multithreading, et le support du multijoueur.
L'objectif final est de générer des continents volants, de les afficher dans leur totalité (avec du LOD). L'objectif secondaire est d'être capable de créer voxel par voxel des machines - mongolfières, dirigeables, chacun ayant sa propre "grille" de voxel.

L'objectif est ambitieux, mais c'est avant tout un projet "pour le fun" sur le long terme. On verra bien ce qu'on arrive à faire.

Nous utilisons:
 - glad: Une implémentation OpenGL
 - glfw3: Fenêtre, clavier, souris
 - pthread: Multithreading
 - glm: Algèbre et calcul matriciel sur le CPU

Et peut-être qu'un jour on utilisera:
 - enet: réseau
 - imgui: ui de debug
 - wangle: réseau

Au niveau de l'architecture, je veux suivre KISS le plus longtemps possible (edit: ça a pas duré longtemps). Dans l'idée, on coupe le code en trois morceaux. Ce qui est dans client est utilisé uniquement par le thread principal, ce qui est dans server est utilisé uniquement par les threads du server, et les classes et structures de données partagées sont dans common.

Le thread client communique avec les thread server via "client_networking.hpp" qui est dans client, et inversement pour les threads server via "server-networking.hpp". Dans l'état actuel, client_networking et server_networking trichent et communiquent directement sans utiliser le réseau, mais ça sera facile à modifier.
