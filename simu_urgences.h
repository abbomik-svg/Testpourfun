#ifndef SIMU_URGENCES_H
#define SIMU_URGENCES_H

#include <time.h>

/* États possibles d'un patient */
typedef enum {
    EN_ATTENTE,   /* Dans la Ready Queue */
    EN_SOIN,      /* Occupe un lit (Running) */
    TERMINE       /* A quitté le système */
} EtatPatient;

/* Structure Patient */
typedef struct {
    int id;
    int t_arrivee;
    int t_soin_total;
    int t_soin_effectue;  /* Pour reprendre après préemption */
    int priorite;         /* 1 = très grave, 5 = moins grave */
    EtatPatient etat;
    int lit_assigne;      /* Index du lit si EN_SOIN, -1 sinon */
    int temps_en_lit;     /* Temps passé dans le lit courant (RR) */
} Patient;

/* Structure ServiceUrgences */
typedef struct {
    int max_lits;
    int lits_occupes;
    Patient** lits;           /* Tableau de pointeurs vers patients en soin */
    int *quantum_lit;         /* Temps restant pour RR par lit */
} ServiceUrgences;

/* Types d'algorithmes */
typedef enum {
    ALGO_FIFO,
    ALGO_SJF,
    ALGO_PRIORITE,
    ALGO_ROUND_ROBIN
} AlgoType;

/* Métriques */
typedef struct {
    double taux_occupation;
    double temps_attente_moyen;
    double temps_turnaround_moyen;
    int nb_evictions;
    int patients_termines;
    int temps_total_occupation;
} Metriques;

/* Fonctions principales */
ServiceUrgences* init_service(int max_lits);
void free_service(ServiceUrgences *service);

void ajouter_patient_attente(ServiceUrgences *service, Patient *p, 
                              Patient **file_attente, int *taille_file);
void allouer_patient(ServiceUrgences *service, Patient *p, AlgoType algo, int quantum);
void liberer_lit(ServiceUrgences *service, int num_lit, 
                  Patient **file_attente, int *taille_file);

void simuler_un_cycle(ServiceUrgences *service, Patient **file_attente, int *taille_file,
                       Patient *tous_patients, int nb_patients, 
                       int temps_courant, AlgoType algo, int quantum, Metriques *metriques);

Patient* selectionner_patient_algo(Patient **file_attente, int taille_file, AlgoType algo);
Patient* selectionner_patient_preemption(ServiceUrgences *service, Patient *nouveau, AlgoType algo);

void calculer_metriques(Metriques *m, int nb_patients, Patient *patients, int temps_total, int max_lits);
void afficher_metriques(Metriques m, AlgoType algo);

#endif
