
cell/chunk - plusieurs états: chargée, à activer, à désactiver, en attente

render thread:
*      passe les cell "à activer" dans la liste "chargée" dont le rendu sera fait.
*      load les entités des cells quand elles sont mises dans "chargée"
*      repasse dans la queue "attente" les cells "à désactiver"

1..n client worker thread:
*      gère la hashmap des cells.
*      passe les cells du cache dans "à activer" et "à désactiver" selon la pos de la camera
*      preload les entités des cells (dont meshing) avant de les mettre dans "à activer"
*      demande de manière async au "world" les cells "à activer" qui ne sont pas en cache.

world thread:
*      forward au server les demandes de cell (thread réseau "client")
*      en mode 'host', le world thread et le main server thread ne font qu'un
  
main server thread
*      reçoit les demandes de cell (thread réseau "serveur")
*      ajoute les demandes à la "query queue"

1..n server worker thread
*      récupère des demandes de chunk.
*      va faire des ios pour charger les chunks pré-existant
*      fait de la worldgen pour les nouveaux chunks