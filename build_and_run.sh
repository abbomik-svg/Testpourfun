#!/bin/bash
# Script de compilation et exécution pour Simu-Urgences

set -e

echo "=== Simu-Urgences v1.1 - Builder ==="
echo ""

# Compilation
echo "[1/3] Nettoyage..."
make clean > /dev/null 2>&1 || true

echo "[2/3] Compilation..."
make > /dev/null 2>&1

if [ ! -f simu_urgences ]; then
    echo "❌ Erreur : compilation échouée"
    exit 1
fi

echo "✓ Compilation réussie"
echo ""

# Exécution
echo "[3/3] Exécution de la simulation..."
echo ""
./simu_urgences

echo ""
echo "=== Simulation terminée ==="
