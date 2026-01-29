#!/bin/bash
# Script de démonstration - teste les 4 politiques en comparaison

echo "=========================================="
echo "  SIMU-URGENCES - Démo Comparative"
echo "=========================================="
echo ""

cd /workspaces/Testpourfun

# Test 1 : Comparaison rapide
echo "Test 1 : Comparaison des 4 politiques (10 patients, 3 lits, 50 temps)"
echo "2" | ./simu_urgences << EOF
2
EOF

echo ""
echo "=========================================="
echo "Démonstration terminée"
echo "=========================================="
