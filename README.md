##Temps Réel en multi-coeurs: problème de contention mémoire

Louisa Bessad: louisa.bessad@etu.upmc.fr

Roberto Medina: roberto.medina@etu.upmc.fr

### Description

La description du projet peut être lue [ici](http://www-master.ufr-info-p6.jussieu.fr/2013/Temps-reel-en-multi-coeurs).
Le projet a pour but l'analyse de l'impact de la mémoire partagée sur les processeurs multicoeurs. Le processeur en question doit éxécuter une tâche temps-réel et d'autres tâches considérées comme attaquantes.

Pour arriver à étudier ce phénomène on utilise la [librairie PAPI](http://icl.cs.utk.edu/papi/).

### Prérequis

Pour avoir une meilleure isolation du processus temps réels, on utilise le paramètre <code>isolcpus = 1</code> pour le noyau Linux.
Le wrapper va faire que la tâche temps réel s'éxécute sur le deuxième coeur du processeur.

### Compilation et éxécution

#### Compilation du projet

Pour compiler le wrapper:<br>
<code>make</code>

Pour compiler la série de tests:<br>
<code>make tests</code>

#### Lancer le wrapper avec des arguments

Le wrapper nécessite des droits de super utilisateur pour pouvoir attacher le set d'événements à un seul coeur.

<code>sudo ./bin/papi_wrapper bin/rt_task arg1</code>

Pour avoir de l'aide sur le wrapper:

<code>./bin/papi_wrapper -h </code>




