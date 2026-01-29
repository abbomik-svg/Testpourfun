# Simu-Urgences v1.1 — Documentation Technique Complète

## Résumé Exécutif

**Simu-Urgences** est une application de simulation d'allocation de ressources en langage C, conçue pour comparer quatre algorithmes d'ordonnancement : FIFO, SJF, Priorité, et Round Robin. Elle modélise un service d'urgences avec un nombre limité de lits, générant des files d'attente et permettant la préemption selon l'algorithme choisi.

---

## 1. Architecture Générale

### Structures de Données Principales

#### `Patient` (simu_urgences.h)
```c
typedef struct {
    int id;                 // Identifiant unique
    int t_arrivee;          // Temps d'arrivée en simulation
    int t_soin_total;       // Durée totale de soin requise
    int t_soin_effectue;    // Durée déjà effectuée (sauvegarde après préemption)
    int priorite;           // 1 = très grave, 5 = moins grave
    EtatPatient etat;       // EN_ATTENTE, EN_SOIN, TERMINE
    int lit_assigne;        // Index du lit occupé (-1 si en attente)
    int temps_en_lit;       // Temps cumulé dans le lit courant (RR)
} Patient;
```

#### `ServiceUrgences`
```c
typedef struct {
    int max_lits;           // Nombre de lits disponibles
    int lits_occupes;       // Lits actuellement occupés
    Patient** lits;         // Tableau de pointeurs vers patients en soin
    int *quantum_lit;       // Temps restant pour RR par lit
} ServiceUrgences;
```

#### `Metriques`
```c
typedef struct {
    double taux_occupation;      // % d'utilisation des lits
    double temps_attente_moyen;  // Temps moyen avant admission
    double temps_turnaround_moyen; // Temps moyen arrivée→sortie
    int nb_evictions;            // Nombre de préemptions
    int patients_termines;       // Nombre de patients sortis
    int temps_total_occupation;  // Accumulation de lits occupés
} Metriques;
```

---

## 2. Algorithmes d'Ordonnancement

### A. FIFO (First In, First Out)

**Sélection** : Le patient le plus ancien dans la file d'attente.

**Saturation** : Bloquant — aucune préemption. Les nouveaux patients attendent qu'un lit se libère naturellement.

**Avantages** : Simple, justicier, pas de famine des patients en attente.

**Inconvénients** : Un patient court peut attendre derrière un patient très long.

**Complexité** : O(1) par allocation.

---

### B. SJF (Shortest Job First)

**Sélection** : Patient avec la durée de soin totale la plus courte.

**Saturation** : Bloquant.

**Avantages** : Réduit le temps d'attente moyen. Optimal pour temps de turnaround.

**Inconvénients** : Famine possible pour les longs patients (si arrivées continues). Require la connaissance préalable de la durée (problème en pratique).

**Complexité** : O(n) par allocation (parcours de la file).

---

### C. Priorité (Préemptif)

**Sélection** : Patient avec la priorité (gravité) la plus haute (numéro 1 = très grave).

**Saturation** : Préemptif.

**Mécanisme de Préemption** :
1. Un patient gravité 1 arrive, les 3 lits sont occupés par gravités 2-5.
2. Le système sélectionne le moins grave (plus haute priorité) en lit.
3. Ce patient est éjecté (état → EN_ATTENTE) et réinséré en file d'attente.
4. Son temps effectué est sauvegardé (reprendra sans recommencer depuis 0).

**Avantages** : Assure que les urgences vitales sont traitées rapidement. Adapté aux systèmes réels d'urgences.

**Inconvénients** : Peut causer une famine des patients peu graves (mitigation : vieillissement / aging).

**Complexité** : O(max_lits) pour sélection de victime.

---

### D. Round Robin (Tourniquet)

**Sélection** : Premier patient de la file.

**Saturation** : Préemptif par quantum temporel.

**Mécanisme** :
1. Chaque patient a un quantum de temps (ex : 2 unités).
2. Après `quantum` unités, le patient est forcé de quitter le lit (même si pas terminé).
3. Il retourne en fin de file d'attente pour faire place au suivant.

**Avantages** : Équité maximale. Aucun patient n'est indéfiniment bloqué.

**Inconvénients** : Surcharge de changement de contexte (overhead). Quantum doit être bien calibré.

**Complexité** : O(1) par allocation.

---

## 3. Logique de Simulation

### Cycle de Simulation

À chaque itération de temps :

```
1. Arrivées
   ├─ Pour chaque patient p : si p.t_arrivee == temps_courant
   │  └─ Ajouter p à salle d'attente (EN_ATTENTE)
   │
2. Allocations
   └─ Tant que (lits libres) ET (file non vide)
      ├─ Sélectionner patient selon algorithme
      ├─ Allouer au lit (EN_SOIN)
      └─ Initialiser quantum (RR)
   
3. Avancer le Temps
   ├─ Pour chaque lit occupé
   │  ├─ Incrémenter t_soin_effectue
   │  ├─ Incrémenter temps_en_lit
   │  ├─ Vérifier si terminé → libérer
   │  └─ (RR) Décrémenter quantum ; si 0 → réinsérer en file
   │
4. Métriques
   └─ Accumuler temps_total_occupation += lits_occupes
```

### Exemple Déroulé (FIFO, 3 lits)

```
t=0:
  Arriv : Patient 0 (dur=5, pri=3)
  File  : [0]
  Alloc : 0 → lit 0 [EN_SOIN]
  Lits  : [0, _, _]
  
t=1:
  Arriv : Patient 1 (dur=3, pri=2)
  File  : [1]
  Alloc : 1 → lit 1
  Avance: 0 : 1/5
  Lits  : [0:1/5, 1:1/3, _]
  
t=2:
  Arriv : Patient 2 (dur=4, pri=1)
  File  : [2]
  Alloc : 2 → lit 2
  Avance: 0:2/5, 1:2/3, 2:1/4
  Lits  : [0:2/5, 1:2/3, 2:1/4]
  
t=3:
  Arriv : Patient 3 (dur=2, pri=4)
  File  : [3]  ← Saturation (0 lits libres)
  Alloc : Aucun
  Avance: 0:3/5, 1:3/3, 2:2/4
  Lits  : [0:3/5, 1:TERMINE, 2:2/4]
  
  Alors : 1 → libérer
  Alloc : 3 → lit 1
  Lits  : [0:3/5, 3:1/2, 2:2/4]
```

---

## 4. Gestion de la Capacité

### État Disponible

Si `lits_occupes < max_lits` :
- Chercher le premier lit libre.
- Allouer le patient sélectionné.

### État Saturé

Si `lits_occupes == max_lits` :

**Comportement FIFO/SJF** (Bloquant) :
- Le nouveau patient reste en file.
- Attendre une libération naturelle.

**Comportement Priorité/RR** (Préemptif) :
- Comparer le nouveau avec les patients en lit.
- Si condition de préemption remplie (ex : nouveau gravité 1, patients gravités 3-5 en lit) :
  - Sélectionner le moins urgent (plus haute priorité numérique).
  - Éjecter : sauvegarder `t_soin_effectue`, mettre EN_ATTENTE, libérer le lit.
  - Allouer le nouveau au lit.

---

## 5. Fichiers du Projet

### Structure Arborescente

```
Simu-Urgences/
├── simu_urgences.h       # En-têtes, structures, énumérations
├── simu_urgences.c       # Implémentation des algorithmes
├── main.c                # Runner principal, jeu de test par défaut
├── scenarios.h           # Jeux de données alternatifs
├── Makefile              # Compilation
├── build_and_run.sh      # Script de compilation + exécution
├── README.md             # Documentation utilisateur
├── SCENARIOS.md          # Études de cas détaillées
├── TECH.md               # Cette documentation technique
└── (binaire) simu_urgences  # Exécutable généré
```

### Fichiers Clés

#### `simu_urgences.h` & `simu_urgences.c`

Implémente :
- `init_service()`, `free_service()` : Gestion du service.
- `ajouter_patient_attente()`, `liberer_lit()` : Gestion des patients.
- `selectionner_patient_algo()` : Sélection selon algorithme.
- `selectionner_patient_preemption()` : Sélection de victime en préemption.
- `allouer_patient()` : Allocation avec gestion de saturation.
- `simuler_un_cycle()` : Boucle principale de simulation.
- `calculer_metriques()`, `afficher_metriques()` : Collecte et affichage.

#### `main.c`

- `creer_jeu_patients_test()` : Jeu de 10 patients par défaut.
- Boucle sur 4 algorithmes, 30 unités temps, réinitialisation entre runs.
- Affichage de l'état tous les 5 temps.

#### `scenarios.h`

Jeux de données alternatifs pour tests spécifiques :
- `scenario_saturation_progressive()` : Saturation visible.
- `scenario_test_preemption()` : Urgence vitale vs patient peu grave.
- `scenario_sjf_comparison()` : Comparaison FIFO vs SJF.
- `scenario_round_robin_equity()` : Équité RR.

---

## 6. Compilation et Exécution

### Compilation Manuelle

```bash
gcc -Wall -Wextra -g -std=c99 -c main.c -o main.o
gcc -Wall -Wextra -g -std=c99 -c simu_urgences.c -o simu_urgences.o
gcc -Wall -Wextra -g -std=c99 -o simu_urgences main.o simu_urgences.o -lm
```

### Compilation via Makefile

```bash
make              # Compilation
make run          # Compilation + exécution
make clean        # Nettoyage
```

### Script de Compilation

```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

---

## 7. Résultats et Interprétation

### Métriques Affichées

Pour chaque algorithme :

```
========== MÉTRIQUES - FIFO ==========
Patients terminés: 10
Taux d'occupation des lits: 84.50%
Temps turnaround moyen: 35.40
Nombre d'évictions: 0
=====================================
```

**Interprétation** :

| Métrique | Signification |
|----------|--------------|
| **Patients terminés** | Nombre ayant complété leur soin dans la durée |
| **Taux d'occupation** | (Temps_total_occupation) / (Temps_total × max_lits) |
| **Turnaround moyen** | Durée moyenne depuis arrivée jusqu'à fin |
| **Nombre d'évictions** | Préemptions effectuées (0 pour FIFO/SJF) |

### Comparaison Entre Algorithmes

**Scénario saturation** :

| Algo | Taux Occ. | Turnaround | Justesse |
|------|-----------|-----------|---------|
| FIFO | ~70% | Dépend ordre | Haute |
| SJF  | ~75% | Bas (courts d'abord) | Moyenne |
| Priorité | ~72% | Bas (urgents d'abord) | Moyenne (famine bas-pri) |
| RR   | ~65% | Moyen (équité) | Haute |

---

## 8. Limitations et Améliorations Futures

### Limitations Actuelles

1. **Pas d'anti-famine** en Priorité : Patients bas-priorité peuvent être indéfiniment repoussés.
2. **Métriques basiques** : Pas de temps d'attente détaillé, pas de variance.
3. **Pas de I/O configuration** : Jeu de données codé en dur.
4. **Simulation déterministe** : Pas d'aléatoire pour tie-break ou arrivées stochastiques.
5. **Pas de visualisation** : Sortie console uniquement.

### Améliorations Proposées

```c
// Anti-famine (Aging)
void vieillir_patients_attente(Patient **file, int taille_file) {
    for (int i = 0; i < taille_file; i++) {
        if (file[i]->temps_attente > AGING_THRESHOLD) {
            file[i]->priorite--;  // Augmenter priorité (diminuer numéro)
            file[i]->temps_attente = 0;
        }
    }
}

// Chargement JSON
void charger_jeu_json(const char *fichier, Patient **patients, int *nb);

// Visualisation ASCII
void afficher_timeline_ascii(/* ... */);

// Export CSV/JSON
void exporter_metriques_csv(/* ... */);
```

---

## 9. Cas Limites Gérés

| Cas | Traitement |
|-----|-----------|
| Arrivées simultanées | Sélection par ordre d'itération (ID croissant en FIFO) |
| Saturation totale | Bloquant pour FIFO/SJF, préemption pour Priorité/RR |
| Préemption en fin de quantum | Patient marké TERMINE sans réinsertion |
| Division par zéro | Vérification `(temps_total > 0)` en calcul métriques |
| Pointeur NULL | Validation avant déréférencement |

---

## 10. Exemple d'Utilisation Avancée

### Modifier pour un jeu de données personnalisé

```c
// main.c

// À la place de :
// Patient *patients = creer_jeu_patients_test(&nb_patients);

// Utiliser :
Patient *patients = scenario_test_preemption(&nb_patients);

// Modifier paramètres
int max_lits = 1;         // Au lieu de 3
int max_temps = 25;       // Au lieu de 30
int quantum_rr = 3;       // Au lieu de 2
```

### Filtrer un algorithme

```c
// Dans main.c, boucle des algorithmes
AlgoType algos[] = {ALGO_PRIORITE};  // Tester que Priorité
int nb_algos = 1;
```

### Augmenter la verbosité

```c
// Dans simu_urgences.c::simuler_un_cycle()
// Ajouter logging détaillé
printf("[t=%d] Allouer Patient %d au Lit %d\n", t, p->id, lit);
```

---

## 11. Performance et Scalabilité

### Complexité Algorithmique

| Algo | Sélection | Préemption | Boucle Principale |
|------|-----------|-----------|-----------------|
| FIFO | O(1)      | N/A       | O(T × N × M)    |
| SJF  | O(N)      | N/A       | O(T × N² × M)   |
| Priorité | O(N)   | O(M)      | O(T × N² × M)   |
| RR   | O(1)      | O(M)      | O(T × N × M)    |

Où : T = temps total, N = patients, M = max_lits.

### Limitation Pratique

Actuellement :
- MAX_PATIENTS = 20 (peut augmenter).
- MAX_FILE_ATTENTE = 100.
- Max simulations = T × nb_algos.

Pour 10 patients × 4 algos × 30 temps : **< 1ms sur machine moderne**.

---

## 12. Références

- [Modern Operating Systems](https://www.os-book.com/), Tanenbaum — Scheduling algorithms.
- [Operating Systems Concepts](https://www.os-book.com/), Abraham Silberschatz — CPU Scheduling.
- [Priority Scheduling & Preemption](https://en.wikipedia.org/wiki/Category:Scheduling_(computing)) — Wikipedia.

---

**Fin de la documentation technique.**

*Auteur : [Ton Nom]*  
*Version : 1.1*  
*Date : Janvier 2026*
