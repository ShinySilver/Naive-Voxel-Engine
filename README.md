# iVy

![A very random screenshot from a very experimental build](/resources/screenshots/experimental_build_1.png?raw=true)

Bon du coup... On a notre compilation avec CMake
qui marche. Aucune idée de comment on active l'opti,
mais bon au moins on produit un executable.

Pour l'instant on charge:
 - glad
 - glfw3
 - pthread

On va avoir besoin de:
 - glm

Et peut-être de:
 - enet

Au niveau de l'architecture, je veux rester kiss le
plus longtemps possible. Dans l'idée, ce qui est dans client est
utilisé uniquement dans le thread principal, dans server ce qui est
dans les autres threads, et dans common les enums partagés. C'est 
server qui est chargé de maintenir la sync avec la remote - ou non.
Il a donc son propre jeu de paquets

Dans client :
 - un namespace qui gère le contexte openGl créé par glfw
 - un namespace qui gère la machine à état interne du client. En
   gros, c'est la mainloop, le namespace gère les events glfw qu'il
   peut transformer en events clients qui sont envoyés à server.
   Il gère aussi les renderers (lecture de common), le meshing, etc.

Dans common :
 - le serveur écrit, le client lit
 - c'est une zone pour la communication entre client (mainthread) et
   server (multithreads). Request queues, lists, mutex, etc, etc...
 - un dossier data/world pour la description des entités / du monde dans des
   structs. Avec des refs à un entityType

Dans server :
 - un namespace pour la liste des paquets
 - un namespace qui gère la machine à état interne du serveur.
   Il gère les physiques, la worldgen et le ticking des entités/mécaniques.
   C'est également le cas échéant le garant de la synchronisation
   avec un hôte remote si il est slave. Ainsi, il forward les events et requêtes
   clients à l'hôte. À l'inverse si il est host, il forward les requêtes
   du client remote dans common.
