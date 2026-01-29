#include "simu_urgences.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Initialisation du service */
ServiceUrgences* init_service(int max_lits) {
    ServiceUrgences *service = (ServiceUrgences*)malloc(sizeof(ServiceUrgences));
    service->max_lits = max_lits;
    service->lits_occupes = 0;
    service->lits = (Patient**)malloc(max_lits * sizeof(Patient*));
    service->quantum_lit = (int*)malloc(max_lits * sizeof(int));
    for (int i = 0; i < max_lits; i++) {
        service->lits[i] = NULL;
        service->quantum_lit[i] = 0;
    }
    return service;
}

/* Libération du service */
void free_service(ServiceUrgences *service) {
    if (service) {
        free(service->lits);
        free(service->quantum_lit);
        free(service);
    }
}

/* Ajouter un patient à la salle d'attente */
void ajouter_patient_attente(ServiceUrgences *service, Patient *p,
                              Patient **file_attente, int *taille_file) {
    (void)service;
    
    p->etat = EN_ATTENTE;
    p->lit_assigne = -1;
    p->temps_en_lit = 0;
    file_attente[*taille_file] = p;
    (*taille_file)++;
}

/* Chercher le premier lit libre */
int trouver_lit_libre(ServiceUrgences *service) {
    for (int i = 0; i < service->max_lits; i++) {
        if (service->lits[i] == NULL) {
            return i;
        }
    }
    return -1;
}

/* Libérer un lit */
void liberer_lit(ServiceUrgences *service, int num_lit,
                  Patient **file_attente, int *taille_file) {
    (void)file_attente;
    (void)taille_file;
    
    if (num_lit >= 0 && num_lit < service->max_lits && service->lits[num_lit] != NULL) {
        service->lits[num_lit]->etat = TERMINE;
        service->lits[num_lit]->lit_assigne = -1;
        service->lits[num_lit] = NULL;
        service->lits_occupes--;
        service->quantum_lit[num_lit] = 0;
    }
}

/* ===== Sélection selon algorithmes ===== */

Patient* selectionner_patient_algo(Patient **file_attente, int taille_file, AlgoType algo) {
    if (taille_file == 0) return NULL;

    int idx = 0;

    if (algo == ALGO_FIFO || algo == ALGO_ROUND_ROBIN) {
        /* Premier de la file */
        idx = 0;
    }
    else if (algo == ALGO_SJF) {
        /* Plus court temps total */
        int min_temps = file_attente[0]->t_soin_total;
        for (int i = 1; i < taille_file; i++) {
            if (file_attente[i]->t_soin_total < min_temps) {
                min_temps = file_attente[i]->t_soin_total;
                idx = i;
            }
        }
    }
    else if (algo == ALGO_PRIORITE) {
        /* Plus haute priorité (plus petit numéro = plus grave) */
        int max_priorite = file_attente[0]->priorite;
        for (int i = 1; i < taille_file; i++) {
            if (file_attente[i]->priorite < max_priorite) {
                max_priorite = file_attente[i]->priorite;
                idx = i;
            }
        }
    }

    Patient *p = file_attente[idx];
    /* Retirer de la file */
    for (int i = idx; i < taille_file - 1; i++) {
        file_attente[i] = file_attente[i + 1];
    }
    return p;
}

/* Sélectionner un patient à préempter (pour préemption) */
Patient* selectionner_patient_preemption(ServiceUrgences *service, Patient *nouveau, AlgoType algo) {
    if (algo != ALGO_PRIORITE && algo != ALGO_ROUND_ROBIN) {
        return NULL; /* Pas de préemption pour FIFO/SJF */
    }

    if (algo == ALGO_PRIORITE) {
        /* Vérifier si nouveau est prioritaire */
        if (service->lits[0] == NULL) return NULL;
        
        /* Choisir le moins grave (plus grande priorité) parmi les occupants */
        int idx_victim = 0;
        int max_pri = service->lits[0]->priorite;
        for (int i = 1; i < service->max_lits; i++) {
            if (service->lits[i] != NULL && service->lits[i]->priorite > max_pri) {
                max_pri = service->lits[i]->priorite;
                idx_victim = i;
            }
        }
        
        /* Préempter seulement si le nouveau est plus grave */
        if (nouveau->priorite < service->lits[idx_victim]->priorite) {
            return service->lits[idx_victim];
        }
    }

    return NULL;
}

/* Allouer un patient à un lit */
void allouer_patient(ServiceUrgences *service, Patient *p, AlgoType algo, int quantum) {
    int lit = trouver_lit_libre(service);

    if (lit == -1) {
        /* Saturation - gérer selon algo */
        if (algo == ALGO_PRIORITE || algo == ALGO_ROUND_ROBIN) {
            /* Essayer préemption */
            Patient *victim = selectionner_patient_preemption(service, p, algo);
            if (victim != NULL) {
                /* Sauvegarder état et libérer le lit */
                int victim_lit = victim->lit_assigne;
                victim->etat = EN_ATTENTE;
                victim->lit_assigne = -1;
                service->lits[victim_lit] = NULL;
                service->lits_occupes--;
                lit = victim_lit;
            }
        }
        if (lit == -1) {
            /* Rien à faire, rester en attente */
            return;
        }
    }

    /* Assigner au lit */
    service->lits[lit] = p;
    p->etat = EN_SOIN;
    p->lit_assigne = lit;
    p->temps_en_lit = 0;
    service->lits_occupes++;
    if (algo == ALGO_ROUND_ROBIN) {
        service->quantum_lit[lit] = quantum;
    }
}

/* === SIMULATION === */

void simuler_un_cycle(ServiceUrgences *service, Patient **file_attente, int *taille_file,
                       Patient *tous_patients, int nb_patients,
                       int temps_courant, AlgoType algo, int quantum, Metriques *metriques) {

    /* 1. Arrivées de nouveaux patients */
    for (int i = 0; i < nb_patients; i++) {
        if (tous_patients[i].t_arrivee == temps_courant && tous_patients[i].etat == EN_ATTENTE) {
            ajouter_patient_attente(service, &tous_patients[i], file_attente, taille_file);
        }
    }

    /* 2. Allouer patients depuis la file d'attente */
    while (*taille_file > 0 && service->lits_occupes < service->max_lits) {
        Patient *p = selectionner_patient_algo(file_attente, *taille_file, algo);
        (*taille_file)--;
        allouer_patient(service, p, algo, quantum);
    }

    /* 3. Avancer le temps pour les patients en soin */
    for (int i = 0; i < service->max_lits; i++) {
        if (service->lits[i] != NULL && service->lits[i]->etat == EN_SOIN) {
            service->lits[i]->t_soin_effectue++;
            service->lits[i]->temps_en_lit++;

            /* Vérifier si terminé */
            if (service->lits[i]->t_soin_effectue >= service->lits[i]->t_soin_total) {
                liberer_lit(service, i, file_attente, taille_file);
            }
            /* Round Robin : préemption par quantum */
            else if (algo == ALGO_ROUND_ROBIN && service->quantum_lit[i] > 0) {
                service->quantum_lit[i]--;
                if (service->quantum_lit[i] == 0) {
                    /* Fin du quantum - réinsérer en file */
                    Patient *p_rr = service->lits[i];
                    p_rr->etat = EN_ATTENTE;
                    p_rr->lit_assigne = -1;
                    p_rr->temps_en_lit = 0;
                    service->lits[i] = NULL;
                    service->lits_occupes--;
                    file_attente[*taille_file] = p_rr;
                    (*taille_file)++;
                }
            }
        }
    }

    /* 4. Métriques */
    metriques->temps_total_occupation += service->lits_occupes;
}

/* Calcul des métriques */
void calculer_metriques(Metriques *m, int nb_patients, Patient *patients, int temps_total, int max_lits) {
    int somme_turnaround = 0;
    int nb_termines = 0;

    for (int i = 0; i < nb_patients; i++) {
        if (patients[i].etat == TERMINE) {
            nb_termines++;
            int temps_turnaround = patients[i].t_soin_effectue;
            somme_turnaround += temps_turnaround;
        }
    }

    m->patients_termines = nb_termines;
    m->taux_occupation = (temps_total > 0) ? (double)m->temps_total_occupation / (temps_total * max_lits) : 0;
    m->temps_turnaround_moyen = (nb_termines > 0) ? (double)somme_turnaround / nb_termines : 0;
    m->temps_attente_moyen = 0; /* À calculer avec timestamps d'attente */
}

/* Affichage des métriques */
void afficher_metriques(Metriques m, AlgoType algo) {
    const char *nom_algo[] = {"FIFO", "SJF", "PRIORITE", "ROUND_ROBIN"};
    printf("\n========== MÉTRIQUES - %s ==========\n", nom_algo[algo]);
    printf("Patients terminés: %d\n", m.patients_termines);
    printf("Taux d'occupation des lits: %.2f%%\n", m.taux_occupation * 100);
    printf("Temps turnaround moyen: %.2f\n", m.temps_turnaround_moyen);
    printf("Nombre d'évictions: %d\n", m.nb_evictions);
    printf("=====================================\n");
}
