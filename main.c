#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simu_urgences.h"

#define MAX_PATIENTS 100
#define MAX_FILE_ATTENTE 200

/* Jeu de données de test - paramétrizable */
Patient* creer_jeu_patients_custom(int nb_patients, int *nb_p_out) {
    Patient *patients = (Patient*)malloc(nb_patients * sizeof(Patient));
    
    /* Distribution aléatoire mais déterministe basée sur ID */
    for (int i = 0; i < nb_patients; i++) {
        patients[i].id = i;
        patients[i].t_arrivee = (i * 2) / 3;              /* Étaler les arrivées */
        patients[i].t_soin_total = 2 + (i % 6);           /* 2-7 unités */
        patients[i].t_soin_effectue = 0;
        patients[i].priorite = 1 + (i % 5);               /* 1-5 (1 = très grave) */
        patients[i].etat = EN_ATTENTE;
        patients[i].lit_assigne = -1;
        patients[i].temps_en_lit = 0;
    }
    
    *nb_p_out = nb_patients;
    return patients;
}

/* Jeu de données par défaut - 10 patients */
Patient* creer_jeu_patients_test(int *nb_patients) {
    Patient *patients = (Patient*)malloc(MAX_PATIENTS * sizeof(Patient));

    /* Scénario : 10 patients, capacité 3 lits */
    int idx = 0;

    /* Patient 0 : arrive à t=0, durée 5, priorité 3 (moins grave) */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 0;
    patients[idx].t_soin_total = 5;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 3;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 1 : arrive à t=1, durée 3, priorité 2 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 1;
    patients[idx].t_soin_total = 3;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 2;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 2 : arrive à t=2, durée 4, priorité 1 (très grave) */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 2;
    patients[idx].t_soin_total = 4;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 1;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 3 : arrive à t=3, durée 2, priorité 4 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 3;
    patients[idx].t_soin_total = 2;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 4;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 4 : arrive à t=4, durée 6, priorité 3 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 4;
    patients[idx].t_soin_total = 6;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 3;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 5 : arrive à t=5, durée 3, priorité 2 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 5;
    patients[idx].t_soin_total = 3;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 2;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 6 : arrive à t=6, durée 4, priorité 1 (très grave) */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 6;
    patients[idx].t_soin_total = 4;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 1;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 7 : arrive à t=7, durée 2, priorité 5 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 7;
    patients[idx].t_soin_total = 2;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 5;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 8 : arrive à t=8, durée 5, priorité 3 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 8;
    patients[idx].t_soin_total = 5;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 3;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    /* Patient 9 : arrive à t=10, durée 3, priorité 2 */
    patients[idx].id = idx;
    patients[idx].t_arrivee = 10;
    patients[idx].t_soin_total = 3;
    patients[idx].t_soin_effectue = 0;
    patients[idx].priorite = 2;
    patients[idx].etat = EN_ATTENTE;
    patients[idx].lit_assigne = -1;
    patients[idx].temps_en_lit = 0;
    idx++;

    *nb_patients = idx;
    return patients;
}

/* Afficher état actuel de la simulation */
void afficher_etat_simulation(ServiceUrgences *service, Patient **file_attente, int taille_file, int temps) {
    printf("\n--- Temps %d ---\n", temps);
    printf("Lits occupés: %d/%d\n", service->lits_occupes, service->max_lits);
    for (int i = 0; i < service->max_lits; i++) {
        if (service->lits[i] != NULL) {
            printf("  Lit %d: Patient %d (gravité %d, %d/%d temps)\n",
                   i, service->lits[i]->id, service->lits[i]->priorite,
                   service->lits[i]->t_soin_effectue, service->lits[i]->t_soin_total);
        }
    }
    printf("File d'attente: %d patients\n", taille_file);
    for (int i = 0; i < taille_file && i < 5; i++) {
        printf("  - Patient %d (gravité %d, durée %d)\n",
               file_attente[i]->id, file_attente[i]->priorite, file_attente[i]->t_soin_total);
    }
}

/* Main : exécuter simulation pour chaque algorithme */
int main() {
    printf("========== SIMU-URGENCES v1.1 ==========\n");
    printf("Simulation d'allocation de ressources en service d'urgences\n\n");

    int choix_menu = 0;
    int nb_patients = 0;
    int max_lits = 3;
    int max_temps = 50;
    int quantum_rr = 2;
    
    /* Menu principal */
    printf("╔════════════════════════════════════════╗\n");
    printf("║         MENU PRINCIPAL                 ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("1. Tester une politique seule\n");
    printf("2. Comparer les 4 politiques\n");
    printf("3. Configuration personnalisée\n");
    printf("4. Quitter\n");
    printf("\nChoisissez (1-4): ");
    fflush(stdout);
    scanf("%d", &choix_menu);

    if (choix_menu == 4) {
        printf("Au revoir!\n");
        return 0;
    }

    /* Configuration commune */
    if (choix_menu == 1) {
        printf("\n--- Configuration ---\n");
        printf("Nombre de patients (1-%d, défaut 10): ", MAX_PATIENTS);
        fflush(stdout);
        int tmp;
        if (scanf("%d", &tmp) == 1 && tmp > 0 && tmp <= MAX_PATIENTS) {
            nb_patients = tmp;
        } else {
            nb_patients = 10;
        }
    } else if (choix_menu == 2) {
        nb_patients = 10;  /* Par défaut pour comparaison */
    } else if (choix_menu == 3) {
        printf("\n--- Configuration Personnalisée ---\n");
        printf("Nombre de patients (1-%d, défaut 10): ", MAX_PATIENTS);
        fflush(stdout);
        int tmp;
        if (scanf("%d", &tmp) == 1 && tmp > 0 && tmp <= MAX_PATIENTS) {
            nb_patients = tmp;
        } else {
            nb_patients = 10;
        }

        printf("Nombre de lits (1-10, défaut 3): ");
        fflush(stdout);
        if (scanf("%d", &tmp) == 1 && tmp > 0 && tmp <= 10) {
            max_lits = tmp;
        } else {
            max_lits = 3;
        }

        printf("Temps de simulation max (1-100, défaut 50): ");
        fflush(stdout);
        if (scanf("%d", &tmp) == 1 && tmp > 0 && tmp <= 100) {
            max_temps = tmp;
        } else {
            max_temps = 50;
        }

        printf("Quantum pour Round Robin (1-5, défaut 2): ");
        fflush(stdout);
        if (scanf("%d", &tmp) == 1 && tmp > 0 && tmp <= 5) {
            quantum_rr = tmp;
        } else {
            quantum_rr = 2;
        }
    }

    Patient *patients = creer_jeu_patients_custom(nb_patients, &nb_patients);

    if (choix_menu == 1) {
        /* === TEST D'UNE POLITIQUE SEULE === */
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║       CHOIX DE LA POLITIQUE            ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("1. FIFO (First In, First Out)\n");
        printf("2. SJF (Shortest Job First)\n");
        printf("3. PRIORITE (Préemptif par urgence)\n");
        printf("4. ROUND_ROBIN (Tourniquet)\n");
        printf("\nChoisissez (1-4): ");
        fflush(stdout);
        int choix_algo;
        scanf("%d", &choix_algo);

        AlgoType algos[] = {ALGO_FIFO, ALGO_SJF, ALGO_PRIORITE, ALGO_ROUND_ROBIN};
        const char *nom_algos[] = {"FIFO", "SJF", "PRIORITE", "ROUND_ROBIN"};
        
        if (choix_algo < 1 || choix_algo > 4) {
            printf("Choix invalide!\n");
            free(patients);
            return 1;
        }

        AlgoType algo = algos[choix_algo - 1];
        const char *nom_algo = nom_algos[choix_algo - 1];

        /* Réinitialiser patients */
        for (int i = 0; i < nb_patients; i++) {
            patients[i].etat = EN_ATTENTE;
            patients[i].t_soin_effectue = 0;
            patients[i].lit_assigne = -1;
            patients[i].temps_en_lit = 0;
        }

        printf("\n");
        printf("╔════════════════════════════════════════╗\n");
        printf("║ SIMULATION - %s\n", nom_algo);
        printf("║ Patients: %d | Lits: %d | Temps max: %d\n", nb_patients, max_lits, max_temps);
        printf("╚════════════════════════════════════════╝\n\n");

        /* Initialiser service */
        ServiceUrgences *service = init_service(max_lits);
        Patient **file_attente = (Patient**)malloc(MAX_FILE_ATTENTE * sizeof(Patient*));
        int taille_file = 0;
        Metriques metriques = {0, 0, 0, 0, 0, 0};

        /* Simulation */
        for (int t = 0; t <= max_temps; t++) {
            simuler_un_cycle(service, file_attente, &taille_file, patients, nb_patients,
                             t, algo, quantum_rr, &metriques);

            /* Afficher snapshot tous les 10 cycles (option verbose) */
            if (t % 10 == 0) {
                printf("t=%2d: Lits %d/%d | File %d patients\n", 
                       t, service->lits_occupes, service->max_lits, taille_file);
            }
        }

        /* Calculer et afficher métriques */
        calculer_metriques(&metriques, nb_patients, patients, max_temps, max_lits);
        
        printf("\n");
        afficher_metriques(metriques, algo);

        /* Afficher détails patients terminés */
        int nb_termine = 0;
        for (int i = 0; i < nb_patients; i++) {
            if (patients[i].etat == TERMINE) nb_termine++;
        }
        printf("Détail: %d/%d patients terminés\n", nb_termine, nb_patients);

        /* Libération */
        free(file_attente);
        free_service(service);

    } else if (choix_menu == 2) {
        /* === COMPARAISON DES 4 POLITIQUES === */
        printf("\n");
        printf("╔════════════════════════════════════════╗\n");
        printf("║ COMPARAISON DES 4 POLITIQUES\n");
        printf("║ Patients: %d | Lits: %d | Temps max: %d\n", nb_patients, max_lits, max_temps);
        printf("╚════════════════════════════════════════╝\n\n");

        AlgoType algos[] = {ALGO_FIFO, ALGO_SJF, ALGO_PRIORITE, ALGO_ROUND_ROBIN};
        const char *nom_algos[] = {"FIFO", "SJF", "PRIORITE", "ROUND_ROBIN"};
        Metriques resultats[4];
        int nb_algos = 4;

        for (int algo_idx = 0; algo_idx < nb_algos; algo_idx++) {
            /* Réinitialiser patients */
            for (int i = 0; i < nb_patients; i++) {
                patients[i].etat = EN_ATTENTE;
                patients[i].t_soin_effectue = 0;
                patients[i].lit_assigne = -1;
                patients[i].temps_en_lit = 0;
            }

            /* Initialiser service */
            ServiceUrgences *service = init_service(max_lits);
            Patient **file_attente = (Patient**)malloc(MAX_FILE_ATTENTE * sizeof(Patient*));
            int taille_file = 0;
            Metriques metriques = {0, 0, 0, 0, 0, 0};

            /* Simulation */
            for (int t = 0; t <= max_temps; t++) {
                simuler_un_cycle(service, file_attente, &taille_file, patients, nb_patients,
                                 t, algos[algo_idx], quantum_rr, &metriques);
            }

            /* Calculer métriques */
            calculer_metriques(&metriques, nb_patients, patients, max_temps, max_lits);
            resultats[algo_idx] = metriques;

            printf("[%d/%d] %s... OK (taux: %.1f%% | turnaround: %.1f)\n", 
                   algo_idx + 1, nb_algos, nom_algos[algo_idx],
                   metriques.taux_occupation * 100, metriques.temps_turnaround_moyen);

            /* Libération */
            free(file_attente);
            free_service(service);
        }

        /* Tableau comparatif */
        printf("\n╔════════════════════════════════════════════════════════════════╗\n");
        printf("║                    RÉSULTATS COMPARATIFS                        ║\n");
        printf("╠════════════════════════════════════════════════════════════════╣\n");
        printf("║ Politique    │ Patients │ Taux Occup. │ Turnaround │ Évictions ║\n");
        printf("╠════════════════════════════════════════════════════════════════╣\n");

        for (int i = 0; i < nb_algos; i++) {
            printf("║ %-12s │    %d/%d   │   %.2f%%    │   %.2f     │    %d     ║\n",
                   nom_algos[i],
                   resultats[i].patients_termines, nb_patients,
                   resultats[i].taux_occupation * 100,
                   resultats[i].temps_turnaround_moyen,
                   resultats[i].nb_evictions);
        }
        printf("╚════════════════════════════════════════════════════════════════╝\n");

    } else if (choix_menu == 3) {
        /* === CONFIGURATION PERSONNALISÉE === */
        free(patients);
        patients = creer_jeu_patients_custom(nb_patients, &nb_patients);

        printf("\n");
        printf("╔════════════════════════════════════════╗\n");
        printf("║       CHOIX DE LA POLITIQUE            ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("1. FIFO\n");
        printf("2. SJF\n");
        printf("3. PRIORITE\n");
        printf("4. ROUND_ROBIN\n");
        printf("5. Comparer les 4\n");
        printf("\nChoisissez (1-5): ");
        int choix_algo;
        scanf("%d", &choix_algo);

        AlgoType algos[] = {ALGO_FIFO, ALGO_SJF, ALGO_PRIORITE, ALGO_ROUND_ROBIN};
        const char *nom_algos[] = {"FIFO", "SJF", "PRIORITE", "ROUND_ROBIN"};

        if (choix_algo >= 1 && choix_algo <= 4) {
            /* Test d'une seule politique */
            AlgoType algo = algos[choix_algo - 1];
            const char *nom_algo = nom_algos[choix_algo - 1];

            /* Réinitialiser patients */
            for (int i = 0; i < nb_patients; i++) {
                patients[i].etat = EN_ATTENTE;
                patients[i].t_soin_effectue = 0;
                patients[i].lit_assigne = -1;
                patients[i].temps_en_lit = 0;
            }

            printf("\n");
            printf("╔════════════════════════════════════════╗\n");
            printf("║ SIMULATION - %s\n", nom_algo);
            printf("║ Config: %d patients | %d lits | %d temps\n", nb_patients, max_lits, max_temps);
            printf("╚════════════════════════════════════════╝\n\n");

            ServiceUrgences *service = init_service(max_lits);
            Patient **file_attente = (Patient**)malloc(MAX_FILE_ATTENTE * sizeof(Patient*));
            int taille_file = 0;
            Metriques metriques = {0, 0, 0, 0, 0, 0};

            for (int t = 0; t <= max_temps; t++) {
                simuler_un_cycle(service, file_attente, &taille_file, patients, nb_patients,
                                 t, algo, quantum_rr, &metriques);
                if (t % (max_temps / 5 + 1) == 0) {
                    printf("t=%d: %d/%d lits | File: %d\n", t, service->lits_occupes, max_lits, taille_file);
                }
            }

            calculer_metriques(&metriques, nb_patients, patients, max_temps, max_lits);
            afficher_metriques(metriques, algo);

            free(file_attente);
            free_service(service);

        } else if (choix_algo == 5) {
            /* Comparaison des 4 */
            printf("\nComparaison des 4 politiques...\n\n");

            Metriques resultats[4];

            for (int algo_idx = 0; algo_idx < 4; algo_idx++) {
                /* Réinitialiser patients */
                for (int i = 0; i < nb_patients; i++) {
                    patients[i].etat = EN_ATTENTE;
                    patients[i].t_soin_effectue = 0;
                    patients[i].lit_assigne = -1;
                    patients[i].temps_en_lit = 0;
                }

                ServiceUrgences *service = init_service(max_lits);
                Patient **file_attente = (Patient**)malloc(MAX_FILE_ATTENTE * sizeof(Patient*));
                int taille_file = 0;
                Metriques metriques = {0, 0, 0, 0, 0, 0};

                for (int t = 0; t <= max_temps; t++) {
                    simuler_un_cycle(service, file_attente, &taille_file, patients, nb_patients,
                                     t, algos[algo_idx], quantum_rr, &metriques);
                }

                calculer_metriques(&metriques, nb_patients, patients, max_temps, max_lits);
                resultats[algo_idx] = metriques;

                printf("[OK] %s\n", nom_algos[algo_idx]);

                free(file_attente);
                free_service(service);
            }

            printf("\n╔════════════════════════════════════════════════════════════════╗\n");
            printf("║                    RÉSULTATS COMPARATIFS                        ║\n");
            printf("╠════════════════════════════════════════════════════════════════╣\n");
            printf("║ Politique    │ Patients │ Taux Occup. │ Turnaround │ Évictions ║\n");
            printf("╠════════════════════════════════════════════════════════════════╣\n");

            for (int i = 0; i < 4; i++) {
                printf("║ %-12s │    %d/%d   │   %.2f%%    │   %.2f     │    %d     ║\n",
                       nom_algos[i],
                       resultats[i].patients_termines, nb_patients,
                       resultats[i].taux_occupation * 100,
                       resultats[i].temps_turnaround_moyen,
                       resultats[i].nb_evictions);
            }
            printf("╚════════════════════════════════════════════════════════════════╝\n");
        }
    }

    free(patients);
    printf("\n========== FIN SIMULATION ==========\n");

    return 0;
}
