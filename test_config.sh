#!/bin/bash
# Script de test automatisé - teste l'option configuration personnalisée

echo "=========================================="
echo "SIMU-URGENCES - Test Configuration"
echo "=========================================="
echo ""

cd /workspaces/Testpourfun

# Test option 3 avec configuration personnalisée
# Choix: 3 (config perso), puis 15 patients, 2 lits, 40 temps, quantum 3, algo 2 (SJF), comparaison
(
  echo "3"        # Option configuration personnalisée
  echo "15"       # 15 patients
  echo "2"        # 2 lits
  echo "40"       # 40 temps de simulation
  echo "3"        # quantum 3
  echo "5"        # Comparer les 4
) | timeout 15 ./simu_urgences

echo ""
echo "=========================================="
echo "Test terminé"
echo "=========================================="
