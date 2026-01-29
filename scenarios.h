/*
 * Jeu de données détaillé pour Simu-Urgences
 * Peut être copié et adapté dans main.c
 */

#include "simu_urgences.h"

/**
 * Scénario 1 : Saturation progressive (conseillé pour débuter)
 * 5 patients, 2 lits
 * Expected: Files d'attente visibles dès t=2
 */
Patient* scenario_saturation_progressive(int *nb_patients) {
    Patient *patients = (Patient*)malloc(5 * sizeof(Patient));

    patients[0].id = 0;
    patients[0].t_arrivee = 0;
    patients[0].t_soin_total = 3;
    patients[0].priorite = 2;

    patients[1].id = 1;
    patients[1].t_arrivee = 1;
    patients[1].t_soin_total = 4;
    patients[1].priorite = 3;

    patients[2].id = 2;
    patients[2].t_arrivee = 2;
    patients[2].t_soin_total = 2;
    patients[2].priorite = 1;  /* Urgence vitale */

    patients[3].id = 3;
    patients[3].t_arrivee = 3;
    patients[3].t_soin_total = 3;
    patients[3].priorite = 4;

    patients[4].id = 4;
    patients[4].t_arrivee = 4;
    patients[4].t_soin_total = 2;
    patients[4].priorite = 2;

    for (int i = 0; i < 5; i++) {
        patients[i].t_soin_effectue = 0;
        patients[i].etat = EN_ATTENTE;
        patients[i].lit_assigne = -1;
        patients[i].temps_en_lit = 0;
    }

    *nb_patients = 5;
    return patients;
}

/**
 * Scénario 2 : Test de préemption
 * 3 gravités différentes, 1 lit
 * Expected: En mode Priorité, urgences vitales doivent être traitées en premier
 */
Patient* scenario_test_preemption(int *nb_patients) {
    Patient *patients = (Patient*)malloc(4 * sizeof(Patient));

    /* Longue tâche peu grave */
    patients[0].id = 0;
    patients[0].t_arrivee = 0;
    patients[0].t_soin_total = 10;
    patients[0].priorite = 5;

    /* Urgence vitale arrive pendant la tâche 0 */
    patients[1].id = 1;
    patients[1].t_arrivee = 3;
    patients[1].t_soin_total = 4;
    patients[1].priorite = 1;

    patients[2].id = 2;
    patients[2].t_arrivee = 5;
    patients[2].t_soin_total = 2;
    patients[2].priorite = 3;

    patients[3].id = 3;
    patients[3].t_arrivee = 8;
    patients[3].t_soin_total = 3;
    patients[3].priorite = 1;

    for (int i = 0; i < 4; i++) {
        patients[i].t_soin_effectue = 0;
        patients[i].etat = EN_ATTENTE;
        patients[i].lit_assigne = -1;
        patients[i].temps_en_lit = 0;
    }

    *nb_patients = 4;
    return patients;
}

/**
 * Scénario 3 : SJF vs FIFO
 * Durées très hétérogènes pour voir la différence
 * Expected: SJF doit avoir meilleur turnaround moyen
 */
Patient* scenario_sjf_comparison(int *nb_patients) {
    Patient *patients = (Patient*)malloc(5 * sizeof(Patient));

    patients[0].id = 0;
    patients[0].t_arrivee = 0;
    patients[0].t_soin_total = 15;
    patients[0].priorite = 3;

    patients[1].id = 1;
    patients[1].t_arrivee = 1;
    patients[1].t_soin_total = 2;
    patients[1].priorite = 3;

    patients[2].id = 2;
    patients[2].t_arrivee = 2;
    patients[2].t_soin_total = 3;
    patients[2].priorite = 3;

    patients[3].id = 3;
    patients[3].t_arrivee = 3;
    patients[3].t_soin_total = 1;
    patients[3].priorite = 3;

    patients[4].id = 4;
    patients[4].t_arrivee = 4;
    patients[4].t_soin_total = 5;
    patients[4].priorite = 3;

    for (int i = 0; i < 5; i++) {
        patients[i].t_soin_effectue = 0;
        patients[i].etat = EN_ATTENTE;
        patients[i].lit_assigne = -1;
        patients[i].temps_en_lit = 0;
    }

    *nb_patients = 5;
    return patients;
}

/**
 * Scénario 4 : Round Robin équité
 * Tâches courtes pour observer les changements de contexte
 */
Patient* scenario_round_robin_equity(int *nb_patients) {
    Patient *patients = (Patient*)malloc(6 * sizeof(Patient));

    for (int i = 0; i < 6; i++) {
        patients[i].id = i;
        patients[i].t_arrivee = i / 2;
        patients[i].t_soin_total = 4 + (i % 2);  /* 4 ou 5 */
        patients[i].priorite = 3;
        patients[i].t_soin_effectue = 0;
        patients[i].etat = EN_ATTENTE;
        patients[i].lit_assigne = -1;
        patients[i].temps_en_lit = 0;
    }

    *nb_patients = 6;
    return patients;
}
