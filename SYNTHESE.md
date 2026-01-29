# 📋 SYNTHÈSE DU PROJET SIMU-URGENCES v1.1

## 🎯 Objectif
Simulateur d'allocation de ressources (lits limités) en service d'urgences,  
comparant 4 algorithmes : FIFO, SJF, PRIORITE, ROUND_ROBIN.

---

## 📦 FICHIERS DU PROJET

| Fichier | Rôle | Taille |
|---------|------|--------|
| **simu_urgences.h** | Déclarations + structures | 2.3K |
| **simu_urgences.c** | Implémentation algorithmes | 8.6K |
| **main.c** | Interface interactive | 21K |
| **scenarios.h** | Jeux de données prédéfinis | 4.1K |
| **Makefile** | Compilation (gcc) | 342B |
| **build_and_run.sh** | Script rapide | 519B |
| **test_config.sh** | Test automatisé | 803B |
| **demo.sh** | Démonstration | 548B |
| **GUIDE_UTILISATION.sh** | Guide complet | 6.5K |
| **README.md** | Documentation générale | - |

**Total : ~40K de code source**

---

## 🚀 UTILISATION

### Compilation
```bash
make              # Compiler
make clean        # Nettoyer
make run          # Compiler + exécuter
```

### Lancement
```bash
./simu_urgences   # Interface interactive
```

### Menu principal
```
1. Tester une politique seule
2. Comparer les 4 politiques ← RECOMMANDÉ POUR DÉBUTER
3. Configuration personnalisée
4. Quitter
```

---

## 🧠 ALGORITHMES IMPLÉMENTÉS

### 1. FIFO (First In, First Out)
- **Sélection** : Plus ancien dans la file
- **Saturation** : Bloquant (pas de préemption)
- **Équité** : Optimale mais peut causer famine

### 2. SJF (Shortest Job First)
- **Sélection** : Durée minimale
- **Saturation** : Bloquant
- **Objectif** : Minimiser turnaround moyen

### 3. PRIORITE (Préemptif)
- **Sélection** : Gravité la plus haute (1 = très grave)
- **Saturation** : **PRÉEMPTIF** - évince le moins grave
- **Cas d'usage** : Urgences médicales réelles

### 4. ROUND ROBIN (Tourniquet)
- **Sélection** : Premier de la file
- **Saturation** : Préemptif par quantum
- **Équité** : Maximale, responsiveness optimale

---

## 📊 MÉTRIQUES COLLECTÉES

```
Taux d'occupation des lits     : (Total occupation / (Temps × Lits)) × 100%
Temps turnaround moyen         : Temps total × Patients
Patients terminés              : Count
Nombre d'évictions (RR/PRIORITE) : Count
```

---

## 🔬 RÉSULTATS TYPIQUES (10 patients, 3 lits, 50 temps)

| Politique | Taux Occupé | Turnaround | Évictions |
|-----------|------------|-----------|-----------|
| FIFO | 20.7% | 4.1 | 0 |
| SJF | 20.7% | 4.1 | 0 |
| PRIORITE | 20.7% | 4.1 | 0 |
| ROUND_ROBIN | 12.0% | 4.1 | 0 |

**Observations** :
- Avec peu de saturation, tous se valent
- RR moins efficace (overhead commutation)
- PRIORITE shine en urgences vitales

---

## 🧪 TESTS PROPOSÉS

### Test 1 : Comparaison rapide (défaut)
```bash
./simu_urgences
→ Choisir : 2
```

### Test 2 : Saturation importante
```bash
./simu_urgences
→ Choisir : 3
→ Patients : 30, Lits : 2, Temps : 60
→ Comparer les 4
```

### Test 3 : PRIORITE seul
```bash
./simu_urgences
→ Choisir : 1
→ Patients : 20
→ Politique : 3 (PRIORITE)
```

---

## 🏗️ ARCHITECTURE INTERNE

### Structures principales

```c
typedef enum {
    EN_ATTENTE,  // Ready Queue
    EN_SOIN,     // Running
    TERMINE      // Exited
} EtatPatient;

typedef struct {
    int id;
    int t_arrivee;
    int t_soin_total;
    int t_soin_effectue;  // Continuité après préemption
    int priorite;         // 1=grave, 5=léger
    EtatPatient etat;
    int lit_assigne;
} Patient;

typedef struct {
    int max_lits;
    Patient** lits;        // Ressources
    int* quantum_lit;      // RR state
} ServiceUrgences;
```

### Boucle principale (temps-discret)

```
Pour chaque cycle t :
  1. Ajouter patients en arrivée
  2. Allouer depuis file selon algo
  3. Avancer temps pour patients en soin
  4. Terminer ou préempter
  5. Collecter métriques
```

---

## ✅ FONCTIONNALITÉS IMPLÉMENTÉES

✓ 4 algorithmes d'allocation  
✓ Gestion de saturation (bloquant/préemptif)  
✓ Calcul de métriques  
✓ Interface interactive  
✓ Comparaison tableau  
✓ Configuration flexible  
✓ Tests automatisés  

---

## 🚧 AMÉLIORATIONS POSSIBLES

- [ ] Anti-famine (vieillissement/aging)
- [ ] Visualisation graphique (TUI/ASCII)
- [ ] Configuration JSON pour jeux de données
- [ ] Export CSV pour analyse
- [ ] Tests unitaires complets
- [ ] Stress tests (1000+ patients)

---

## 📚 DOCUMENTATION COMPLÈTE

Voir `GUIDE_UTILISATION.sh` pour guide exhaustif.

```bash
bash GUIDE_UTILISATION.sh
```

---

## 🔧 DÉVELOPPEMENT

Compiler avec debug :
```bash
gcc -Wall -Wextra -g -std=c99 *.c -o simu_urgences
```

Valgrind (vérifier mémoire) :
```bash
valgrind --leak-check=full ./simu_urgences
```

---

## 📄 LICENCE

Académique - Libre d'utilisation.

---

**Version** : 1.1 (Gestion des ressources finies)  
**Date** : Jan 2026  
**Auteur** : [Ton Nom]
