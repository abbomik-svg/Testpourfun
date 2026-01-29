# Études de Cas et Scenarios — Simu-Urgences v1.1

## 1. Scénario de Saturation Progressive

**Objectif** : Visualiser la formation d'une file d'attente et comparer les algorithmes.

**Configuration** :
- Capacité : 2 lits
- 5 patients avec arrivées étalées
- Durées : 2 à 4 unités
- Gravités mixtes

**Patients** :
| ID | Arrivée | Durée | Gravité | Description |
|----|---------|-------|---------|------------|
| 0  | t=0     | 3     | 2       | Patient standard |
| 1  | t=1     | 4     | 3       | Patient moins prioritaire |
| 2  | t=2     | 2     | 1       | **Urgence vitale** |
| 3  | t=3     | 3     | 4       | Patient peu grave |
| 4  | t=4     | 2     | 2       | Patient court |

**Résultats Attendus** :

- **FIFO** : Les patients sont traités dans l'ordre d'arrivée. Patient 2 (urgent) attend son tour.
- **SJF** : Patient 4 (durée 2) est traité en priorité → meilleur turnaround pour les courts.
- **Priorité** : Patient 2 coupe la file → admis immédiatement.
- **RR** : Alternance égale entre patients ; surcharge de contexte.

---

## 2. Scénario de Test de Préemption

**Objectif** : Démontrer le mécanisme de préemption en cas d'urgence vitale.

**Configuration** :
- Capacité : 1 lit
- 4 patients
- Cas extrême : patient long + peu grave, puis urgence vitale

**Patients** :
| ID | Arrivée | Durée | Gravité | Note |
|----|---------|-------|---------|------|
| 0  | t=0     | 10    | 5       | Patient "bobologie" long |
| 1  | t=3     | 4     | 1       | **Urgence vitale** |
| 2  | t=5     | 2     | 3       | Patient standard |
| 3  | t=8     | 3     | 1       | **Urgence vitale** |

**Scénario Déroulé** :

1. **t=0** : Patient 0 occupe le lit (gravité 5, durée 10).
2. **t=3** : Patient 1 arrive (gravité 1 = urgent).
   - **FIFO/SJF** : Patient 1 attend en file.
   - **Priorité** : Patient 0 est éjecté, Patient 1 admis (sauvegarde l'état de Patient 0 : 3/10 effectués).
3. **t=7** : Patient 1 termine.
   - **Priorité** : Patient 0 reprend depuis 3/10 (pas depuis le début).

**Résultats Attendus** :

| Algorithme | Résultat |
|------------|---------|
| **FIFO** | Patient 1 attend t=10, puis est admis à t=10 |
| **SJF** | Dépend des durées en file |
| **Priorité** | Patient 1 admis à t=3 (préemption) |
| **RR** | Patient 0 remet tous les 2 temps ; Patient 1 s'insère |

---

## 3. Scénario SJF vs FIFO

**Objectif** : Comparer les performances en présence de tâches très hétérogènes.

**Configuration** :
- Capacité : 2 lits
- 5 patients
- Durées : 1, 2, 3, 5, 15 (très hétérogène)

**Patients** :
| ID | Arrivée | Durée | Gravité |
|----|---------|-------|---------|
| 0  | t=0     | 15    | 3       | Tâche très longue |
| 1  | t=1     | 2     | 3       | Court |
| 2  | t=2     | 3     | 3       | Moyen |
| 3  | t=3     | 1     | 3       | Très court |
| 4  | t=4     | 5     | 3       | Moyen-long |

**Chronologie FIFO** :
- t=0-15 : Patient 0 aux lits 0
- t=1-3  : Patient 1 aux lit 1
- t=3-6  : Patient 2 aux lit 1
- t=4    : Patient 3 rejoint Patient 0
- ...

**Chronologie SJF** :
- t=0-15 : Patient 0 aux lits 0
- t=1-2  : Patient 3 (dur=1) aux lit 1 ← choix du plus court
- t=2-5  : Patient 1 (dur=2) aux lit 1
- ...

**Métrique Turnaround Moyen** :
- **FIFO** : Plus élevé pour les patients courts (ils attendent les longs).
- **SJF** : Réduit pour les patients courts ; mais Patient 0 reste long de toute façon.

---

## 4. Scénario Round Robin — Équité

**Objectif** : Vérifier que RR distribue équitablement le temps de traitement.

**Configuration** :
- Capacité : 1 lit
- 6 patients
- Durées similaires (4-5)
- Quantum : 2 unités

**Patients** :
| ID | Arrivée | Durée | Quantum Attendu |
|----|---------|-------|-----------------|
| 0  | t=0     | 4     | 2 → remet à t=2, puis 2 → termine à t=4 |
| 1  | t=0     | 5     | t=2-4, t=6-8, t=10 |
| ...| ...     | ...   | ... |

**Résultat Attendu** :
Tous les patients effectuent un peu de travail chacun → plus équitable que FIFO/SJF, mais coûte plus cher en contexte.

---

## 5. Cas Limite : Famine (Starvation)

**Objectif** : Montrer un patient de basse priorité jamais traité en mode Priorité sans anti-famine.

**Configuration** :
- Capacité : 1 lit
- Arrivée continue de patients gravité 1 (urgents)
- 1 patient gravité 5 (peu grave)

**Scénario** :
```
t=0   : Patient A (gravité 5) — peu grave
t=1   : Patient B (gravité 1) — urgent
t=2   : Patient C (gravité 1) — urgent
t=3   : Patient D (gravité 1) — urgent
...
```

**Résultat Attendu (sans anti-famine)** :
Patient A reste indéfiniment en file d'attente, car les urgents le dépassent toujours.

**Solution Proposée** :
Implémenter un mécanisme de "vieillissement" (aging) où la priorité d'un patient en attente augmente avec le temps.

---

## 6. Cas Limite : Arrivées Simultanées

**Configuration** :
```
t=2 : 3 patients arrivent simultanément
      - Capacité : 1 lit
      - Qui est admis en premier ?
```

**Règle Implémentée** :
Par défaut, d'après `selectionner_patient_algo()` :
- **FIFO** : Premier dans le tableau (par ID croissant).
- **SJF** : Celui avec la durée la plus courte.
- **Priorité** : Celui avec la gravité la plus haute.
- **RR** : Premier dans le tableau.

---

## 7. Scénario de Stress Test

**Configuration** :
- Capacité : 3 lits
- 50 patients
- Arrivées rapides (rafales)
- Durées : 1-10

**Objectif** : Tester la stabilité et les performances du simulateur.

---

## Recommandations d'Utilisation

### Pour débuter
1. Commencer par le **Scénario 1** (saturation progressive).
2. Observer la formation de la file d'attente.
3. Comparer FIFO vs SJF.

### Pour tester la préemption
1. **Scénario 2** (test de préemption).
2. Vérifier que les urgences vitales sont admises immédiatement en Priorité.
3. Vérifier la sauvegarde de l'état lors de l'éjection.

### Pour l'équité
1. **Scénario 4** (RR équité).
2. Observer que tous les patients avancent à peu près au même rythme.

### Pour les cas limites
1. **Scénario 5** (famine).
2. **Scénario 6** (arrivées simultanées).

---

## Notes d'Implémentation

- Tous les scénarios sont définis dans `scenarios.h`.
- Pour changer de scénario, modifier `main.c` et appeler le scénario souhaité au lieu de `creer_jeu_patients_test()`.
- Les résultats dépendent du `MAX_LITS` et du `quantum_rr` choisis.

