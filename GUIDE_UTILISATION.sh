#!/bin/bash
# Guide d'utilisation Simu-Urgences v1.1

cat << 'EOF'

╔════════════════════════════════════════════════════════════════════╗
║               SIMU-URGENCES v1.1                                   ║
║     Simulateur d'Allocation de Ressources en Urgences              ║
╚════════════════════════════════════════════════════════════════════╝

📋 ARBORESCENCE DES FICHIERS
════════════════════════════════════════════════════════════════════

simu_urgences.h          : Déclarations (structures, énumérations, API)
simu_urgences.c          : Implémentation (algorithmes, simulation)
main.c                   : Interface interactive avec menu
scenarios.h              : Jeux de données prédéfinis (optionnel)
Makefile                 : Compilation (gcc + make)
build_and_run.sh         : Script rapide de compilation et exécution
test_config.sh           : Test automatisé (15 patients, 2 lits)
demo.sh                  : Démonstration (comparaison 4 politiques)

═══════════════════════════════════════════════════════════════════════

🚀 DÉMARRAGE RAPIDE
═══════════════════════════════════════════════════════════════════════

1) Compiler :
   $ make

2) Exécuter interactivement :
   $ ./simu_urgences

   Ou avec script :
   $ bash build_and_run.sh

═══════════════════════════════════════════════════════════════════════

📊 MENU PRINCIPAL - OPTIONS
═══════════════════════════════════════════════════════════════════════

Option 1 : Tester une politique seule
   • Choisissez le nombre de patients
   • Sélectionnez 1 des 4 politiques
   • Visualisez les métriques en détail
   
   Exemple : Tester SJF avec 20 patients

Option 2 : Comparer les 4 politiques
   • Exécute FIFO, SJF, PRIORITE, ROUND_ROBIN en séquence
   • Tableau comparatif avec :
     - Taux d'occupation des lits
     - Temps turnaround moyen
     - Nombre d'évictions
   • Configuration fixe : 10 patients, 3 lits, 50 temps

Option 3 : Configuration personnalisée
   • Définissez le nombre de patients (1-100)
   • Nombre de lits (1-10)
   • Temps de simulation (1-100)
   • Quantum pour Round Robin (1-5)
   • Puis choisissez : une politique OU comparaison des 4

Option 4 : Quitter

═══════════════════════════════════════════════════════════════════════

🎯 ALGORITHMES D'ALLOCATION
═══════════════════════════════════════════════════════════════════════

1. FIFO (First In, First Out)
   • Plus ancien dans la file → premier servi
   • Saturation : BLOQUANT (pas de préemption)
   • Cas d'usage : Système simple, équitable

2. SJF (Shortest Job First)
   • Patient avec durée minimale → prioritaire
   • Saturation : BLOQUANT
   • Cas d'usage : Minimiser turnaround moyen

3. PRIORITE (Préemptif)
   • Gravité la plus haute (1 = très grave) → prioritaire
   • Saturation : PRÉEMPTIF
   • Action : Patient urgent évince le moins grave
   • Cas d'usage : Urgences médicales

4. ROUND ROBIN (Tourniquet)
   • Premier de la file, quantum = temps max par lit
   • Saturation : PRÉEMPTIF par quantum
   • Action : Fin du quantum → retour en fin de file
   • Cas d'usage : Équité, responsiveness

═══════════════════════════════════════════════════════════════════════

📈 MÉTRIQUES COLLECTÉES
═══════════════════════════════════════════════════════════════════════

Taux d'occupation des lits :
  (Temps total lits occupés) / (Temps total × MAX_LITS) × 100%
  Mesure l'efficacité de l'utilisation des ressources

Temps turnaround moyen :
  Temps total × nombre de patients traités
  Mesure la réactivité du système

Patients terminés :
  Nombre de patients ayant complété leur soin

Nombre d'évictions :
  Préemptions effectuées (RR, PRIORITE uniquement)

═══════════════════════════════════════════════════════════════════════

🔬 EXEMPLES D'UTILISATION
═══════════════════════════════════════════════════════════════════════

Exemple 1 : Comparer rapidement
  $ ./simu_urgences
  Choisissez : 2
  (Lancera comparaison avec config défaut)

Exemple 2 : Test personnalisé - Saturation
  $ ./simu_urgences
  Choisissez : 3
  Patients : 30
  Lits : 2
  Temps : 60
  Quantum : 2
  Politique : 5 (Comparer)
  (Scénario de saturation importante)

Exemple 3 : Test PRIORITE seul (urgences vitales)
  $ ./simu_urgences
  Choisissez : 1
  Patients : 15
  Politique : 3 (PRIORITE)
  (Visualiser comment les urgences graves sont traitées)

═══════════════════════════════════════════════════════════════════════

🧪 SCRIPTS DE TEST AUTOMATISÉS
═══════════════════════════════════════════════════════════════════════

test_config.sh
  Teste avec : 15 patients, 2 lits, 40 temps, comparaison
  $ bash test_config.sh

demo.sh
  Démonstration simple (prépare pour future amélioration)
  $ bash demo.sh

═══════════════════════════════════════════════════════════════════════

🛠️ COMPILATION MANUELLE
═══════════════════════════════════════════════════════════════════════

Nettoyer :
  $ make clean

Compiler :
  $ make

Compiler + Exécuter :
  $ make run

═══════════════════════════════════════════════════════════════════════

📝 STRUCTURE INTERNE (Développeurs)
═══════════════════════════════════════════════════════════════════════

Patient :
  id, t_arrivee, t_soin_total, t_soin_effectue
  priorite (1=grave, 5=léger), etat, lit_assigne

ServiceUrgences :
  max_lits, lits_occupes
  lits[] : tableau de pointeurs patients
  quantum_lit[] : quantum restant par lit (RR)

Simulation temps-discret :
  1. Arrivées de nouveaux patients
  2. Allocation depuis la file selon algorithme
  3. Avancer temps pour patients en soin
  4. Terminer ou préempter selon politique
  5. Collecter métriques

═══════════════════════════════════════════════════════════════════════

💡 CONSEILS D'UTILISATION
═══════════════════════════════════════════════════════════════════════

• Commencez par l'option 2 (comparaison rapide)
• Testez une seule politique à la fois pour détails
• Augmentez les patients pour voir saturation
• Comparez taux d'occupation : indicateur clé
• Turnaround moyen : qualité du service
• Évictions : charge de commutation

═══════════════════════════════════════════════════════════════════════

🔗 RESSOURCES
═══════════════════════════════════════════════════════════════════════

PRD v1.1 : PRD_Simu_Urgences.pdf (fourni)
Code source : GitHub ou archive locale
Licence : Académique - Libre d'utilisation

═══════════════════════════════════════════════════════════════════════

Questions ? Améliorations ? Consultez le README.md ou le code source.

EOF
