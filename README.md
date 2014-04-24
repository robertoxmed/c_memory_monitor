##Temps Réel en multi-coeurs: problème de contention mémoire

Louisa Bessad: louisa.bessad@etu.upmc.fr

Roberto Medina: roberto.medina@etu.upmc.fr

### Description

La description du projet peut être lue [ici](http://www-master.ufr-info-p6.jussieu.fr/2013/Temps-reel-en-multi-coeurs).
Le projet a pour but l'analyse de l'impact de la mémoire partagée sur les processeurs multicoeurs. Le processeur en question doit éxécuter une tâche temps-réel et d'autres tâches considérées comme attaquantes.

Pour arriver à étudier ce phénomène on utilise la [librairie PAPI](http://icl.cs.utk.edu/papi/).<br>
C'est recommandé d'installer les librairies correspondantes avec <code>make install-libs</code> avec le tarball du projet PAPI.

### Prérequis

Pour avoir une meilleure isolation du processus temps réels et des attaquants, on utilise le paramètre <code>isolcpus = 1, 2, 3</code> pour le noyau Linux (passé à travers GRUB au démarrage).<br><br>
Le wrappeur et l'hyperviseur vont faire que la tâche temps réel s'éxécute sur le deuxième coeur du processeur.<br> L'hyperviseur s'assure que les tâches attanquantes vont sur les autres coeurs.<br><br>
Un coeurs doit être libre pour que l'OS puisse l'utiliser.<br>

### Compilation et éxécution

#### Compilation du projet

Pour compiler le projet:<br>
<code>git clone https://github.com/robertoxmed/psar.git</code><br>
<code>make</code>

#### Lancer le wrapper avec des arguments

Le wrapper nécessite des droits de super utilisateur pour pouvoir attacher le set d'événements à un seul coeur.

<code>sudo ./bin/papi_wrapper bin/rt_task nb_iterations</code>

Pour avoir de l'aide sur le wrapper:

<code>./bin/papi_wrapper -h </code>

#### Lancer le scheduler avec des arguments

Le scheduler, comme le wrappeur, nécessite des droits de super utilisateur pour pouvoir attacher le set d'événements à un seul coeur. Il se charge aussi de lancer les tâches attaquantes. Par défaut la tâche temp-réel va effectuer 2000000 d'itérations.

<code>sudo ./bin/papi_scheduler bin/rt_task nb_attaquants</code>

Pour avoir de l'aide sur le scheduler:

<code>./bin/papi_scheduler -h </code>

*Comportement du CPU:*

![Exemple d'éxécution](/doc/sar.png?raw=true "Exemple d'éxécution de l'hyperviseur")


*Schéma du Scheduler*

![Schema scheduler](/doc/papi_scheduler.png?raw=true "Schéma du scheduler")

