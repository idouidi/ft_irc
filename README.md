         ───────────────────────────────────────────────────────────────────────────────
         ─██████████████─██████████████────██████████─████████████████───██████████████─
         ─██░░░░░░░░░░██─██░░░░░░░░░░██────██░░░░░░██─██░░░░░░░░░░░░██───██░░░░░░░░░░██─
         ─██░░██████████─██████░░██████────████░░████─██░░████████░░██───██░░██████████─
         ─██░░██─────────────██░░██──────────██░░██───██░░██────██░░██───██░░██─────────
         ─██░░██████████─────██░░██──────────██░░██───██░░████████░░██───██░░██─────────
         ─██░░░░░░░░░░██─────██░░██──────────██░░██───██░░░░░░░░░░░░██───██░░██─────────
         ─██░░██████████─────██░░██──────────██░░██───██░░██████░░████───██░░██─────────
         ─██░░██─────────────██░░██──────────██░░██───██░░██──██░░██─────██░░██─────────
         ─██░░██─────────────██░░██────────████░░████─██░░██──██░░██████─██░░██████████─
         ─██░░██─────────────██░░██────────██░░░░░░██─██░░██──██░░░░░░██─██░░░░░░░░░░██─
         ─██████─────────────██████────────██████████─██████──██████████─██████████████─
         ───────────────────────────────────────────────────────────────────────────────

Qu'est-ce-qu'IRC:
-----------------

IRC (Internet Relay Chat) est un protocole de communication en temps réel utilisé principalement pour la discussion en groupe sur les réseaux. 
Il permet également l'échange de fichiers et la réalisation de transferts de fichiers entre utilisateurs. Il est souvent utilisé pour des discussions en groupe, mais peut également être utilisé pour des discussions en tête-à-tête.

Comment fonctionne IRC ?:
-----------------------

IRC fonctionne sur un modèle client-serveur, où les utilisateurs se connectent à un serveur IRC et rejoignent des canaux de discussion

    Qu'est-ce-qu'un modele client-serveur ?
    ---------------------------------------

    Un modèle client-serveur est un modèle de communication informatique qui permet à plusieurs programmes informatiques (clients) d'accéder aux ressources ou aux services fournis par un autre programme informatique (serveur).
    Dans ce modèle, le serveur est un programme qui fournit un service ou une ressource à d'autres programmes informatiques, les clients. Le serveur fonctionne en permanence, à l'écoute de requêtes émises par les clients.
    Le client est un programme qui demande un service ou une ressource au serveur en envoyant une requête. Le client attend ensuite une réponse du serveur avant de poursuivre son traitement.
    Le serveur peut gérer simultanément plusieurs requêtes de clients en utilisant des threads ou des processus pour traiter chaque demande individuellement. Les clients peuvent également être situés sur des ordinateurs différents, communiquant avec le serveur via un réseau informatique.

Comment se presente le modele client-serveur d'IRC ?
----------------------------------------------------
Le serveur IRC agit comme un point central de communication, en gérant les connexions des clients et en assurant la distribution des messages entre les clients. Les clients IRC se connectent au serveur en utilisant un protocole standard, généralement TCP/IP, et s'authentifient en fournissant un nom d'utilisateur et un mot de passe.
Les canaux peuvent être publics ou privés, et sont identifiés par un nom qui commence souvent:
  - (#) pour les canaux publics
  - (&) pour les canaux privés.
Lorsqu'un utilisateur envoie un message dans un canal, le serveur le diffuse à tous les autres utilisateurs connectés au canal.


Pour implementer TCP/IP,  nous devons d'abord apprendre que sont les sockets ?
------------------------------------------------------------------------------
Une socket est le mécanisme que les systèmes d'exploitation les plus courants fournissent pour permettre aux programmes d'accéder au réseau. Il permet d'envoyer et de recevoir des messages entre des applications (processus non liés) sur différentes machines en réseau.
Le mécanisme des sockets a été créé pour être indépendant de tout type de réseau spécifique. Cependant, IP est de loin le réseau le plus dominant et l'utilisation la plus populaire des sockets.
L'utilisation des sockets se fait en quelques étapes :
   - Cree la socket
   - Identifier la socket
   - Sur le serveur, attendre une connexion entrante
   - Envoyer et recevoir des messages
   - Fermer le socket.

   1) cree la socket
        Nous devons faire un appel a l'appelle system socket.

         #### int server_fd = socket(domain, type, protocol);
         
        - domain/address family :   Domaine de communication dans lequel le socket doit être créé.

        - type:   le type de service. Celui-ci est sélectionné en fonction des           propriétés requises par l'application : SOCK_STREAM (service de circuit virtuel), SOCK_DGRAM (service de datagramme), SOCK_RAW (service IP direct). Vérifiez auprès de votre domain si un service particulier est disponible.

        - protocol:  indiquer un protocole spécifique à utiliser pour prendre en charge l'opération des sockets. Ceci est utile dans les cas où certaines familles peuvent avoir plus d'un protocole pour supporter un type de service donné. La valeur de retour est un descripteur de fichier (un petit nombre entier).

        Pour les sockets TCP/IP, nous voulons spécifier la famille d'adresses IP (AF_INET) et le service de circuit virtuel (SOCK_STREAM). Comme il n'y a qu'une seule forme de service de circuit virtuel, il n'y a pas de variations du protocole, donc le dernier argument, protocole, est zéro. Notre code pour créer un socket TCP ressemble à ceci :
        
        #####   #include <sys/socket.h>
        #####   if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        #####   {
        #####       perror(“cannot create socket”); 
        #####       return 0; 
        #####   }

    2) Identifier la socket 
        Lorsque nous parlons d'identifier une socket, nous parlons d'attribuer une adresse de transport à la socket (un numéro de port dans un réseau IP).

            Qu'est-ce-qu'une addresse de transport ?
                L'adresse de transport est définie dans une structure d'adresse de socket. Les sockets ayant été conçus pour fonctionner avec différents types d'interfaces de communication, l'interface est très générale. Au lieu d'accepter, par exemple, un numéro de port comme paramètre, elle prend une structure sockaddr dont le format réel est déterminé par la famille d'adresses (type de réseau) que vous utilisez. Par exemple, si vous utilisez des sockets de domaine UNIX, bind crée un fichier dans le système de fichiers.

        Dans les sockets, cette  opération s'appelle "binding an address" et l'appel système bind est utilisé pour cela:

        #### int bind(int socket, const struct sockaddr *address, socklen_t address_len);
        
        - socket: le socket que nous venons de creer avec l'appel system "socket"

        - struct sockaddr *address : La structure sockaddr est un conteneur générique qui permet simplement au système d'exploitation de lire les deux premiers octets qui identifient la famille d'adresses. La famille d'adresses détermine la variante de la structure sockaddr à utiliser, qui contient des éléments utiles pour ce type de communication spécifique. Pour les réseaux IP, nous utilisons la struct sockaddr_in, qui est définie dans l'en-tête netinet/in.h. Cette structure définit :

                #### struct sockaddr_in 
                #### { 
                ####     __uint8_t         sin_len; 
                ####     sa_family_t       sin_family; 
                ####     in_port_t         sin_port; 
                ####     struct in_addr    sin_addr; 
                ####     char              sin_zero[8]; 
                #### };

                Avant d'appeler bind, nous devons remplir cette structure. Les trois parties clés que nous devons définir sont :
                    - sin_family: La famille d'adresses que nous avons utilisée lorsque nous avons configuré le socket. Dans notre cas, c'est AF_INET.
                    
                    - sin_port:  Le numéro de port (l'adresse de transport). Vous pouvez attribuer explicitement une adresse de transport (port) ou laisser le système d'exploitation en attribuer une. Si vous êtes un client et que vous ne recevrez pas de connexions entrantes, vous laisserez généralement le système d'exploitation choisir n'importe quel numéro de port disponible en spécifiant le port 0. Si vous êtes un serveur, vous choisirez généralement un numéro spécifique puisque les clients auront besoin de connaître un numéro de port pour se connecter.

                    - sin_addr: L'adresse de ce socket. Il s'agit simplement de l'adresse IP de votre machine. Avec l'IP, votre machine aura une adresse IP pour chaque interface réseau. Par exemple, si votre machine dispose de connexions Wi-Fi et Ethernet, elle aura deux adresses, une pour chaque interface. La plupart du temps, nous ne nous soucions pas de spécifier une interface spécifique et pouvons laisser le système d'exploitation utiliser ce qu'il veut. L'adresse spéciale pour cela est 0.0.0.0, définie par la constante symbolique INADDR_ANY.
            
        - socklen_t address_len: Comme la structure d'adresse peut différer selon le type de transport utilisé, le troisième paramètre spécifie la longueur de cette structure. Il s'agit simplement de sizeof(struct sockaddr_in).

        le code pour bind une socket :
        
        ##### #include <sys/socket.h>
        ##### struct sockaddr_in address;
        ##### // 💡 Astuce pour connaitre les ports disponible sur sa machine:
        ##### // "netstat -an | grep LISTEN | awk '{print $4}' | awk -F ":" '{print $NF}' | sort -n | uniq"
        ##### const int PORT = 8080; 
        ##### memset((char *)&address, 0, sizeof(address));
        ##### address.sin_family = AF_INET; 
        ##### address.sin_addr.s_addr = htonl(INADDR_ANY);  // htonl convert a long int to a network representation 
        ##### address.sin_port = htons(PORT);               // htons convert a short int to network representation
        ##### 
        ##### if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
        ##### {
        #####   perror(“bind failed”);
        #####   return (0);
        ##### }
    
    3) Sur le serveur, attendre une connexion entrante
        Avant qu'un client puisse se connecter à un serveur, ce dernier doit disposer d'une socket prêt à accepter les connexions. L'appel système listen indique à une socket qu'il doit être capable d'accepter les connexions entrantes :

        #### #include <sys/socket.h> 
        #### int listen(int socket, int backlog);

            - socket: la socket que nous avons creer

            - backlog: définit le nombre maximum de connexions en attente qui peuvent être mises en file d'attente avant que les connexions ne soient refusées.

        💡La socket d'origine qui a été configurée pour l'écoute est utilisée uniquement pour accepter des connexions, et non pour échanger des données. 
        Par défaut, les opérations de socket sont synchrones, ou bloquantes, et accept bloquera jusqu'à ce qu'une connexion soit présente dans la file d'attente.

        L'appel système accept saisit la première demande de connexion dans la file d'attente des connexions en attente (configurée dans listen) et crée une nouvelle socket pour cette connexion.
        la syntax d'accept est :

        #### #include <sys/socket.h> 
        #### int accept(int socket, struct sockaddr *address, socklen_t *restrict address_len);

            - socket: c'est la socket qui a ete set pour accepter les connections avec listen

            - address : c'est la structure d'adresse qui est initialise avec l'adresse du client qui effectue la connexion. Cela nous permet d'examiner l'adresse et le numéro de port de la socket qui se connecte si nous le souhaitons.

            - address_len : la longueur de la structure d'adresse.

            le code pour listen et accept:
            
            #### #include <sys/socket.h>
            #### int pending_connection: 8;
            #### if (listen(server_fd, pending_connection) < 0)
            #### {
            ####    perror("Listen failed");
            ####    exit(EXIT_FAILLURE);        
            #### }
            #### if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))< 0 )
            #### {
            ####    perror("accept failed");
            ####    exit(EXIT_FAILLURE);        
            #### }
    
    4) Envoyer et recevoir des messages
        La communication est la partie facile. Les mêmes appels système de lecture et d'écriture qui fonctionnent sur les fichiers fonctionnent également sur les sockets.
        
        #### char buffer[1024] = {0};
        #### int valread = read( new_socket , buffer, 1024); 
        #### printf(“%s\n”,buffer );
        #### if(valread < 0)
        #### { 
        ####     printf("No bytes are there to read");
        #### }
        ####
        #### char *hello = "Hello from the server";
        #### write(new_socket , hello , strlen(hello));
    
    5) Fermer le socket.
        Lorsque nous avons terminé de communiquer, la chose la plus simple à faire est de fermer une socket avec l'appel système close - la même close que celle utilisée pour les fichiers.