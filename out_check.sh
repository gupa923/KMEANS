#!/bin/bash
# templete per fare i test per vedere se due file .inp sono uguali

# cambia il nome dei file per fare test su file diversi

# Confronta i due file riga per riga
diff out_files/mpi_test2.inp out_files/compare_w_mpi2.inp > /dev/null

# Verifica l'esito del comando diff
if [ $? -eq 0 ]; then
    echo "I file sono uguali"
else
    echo "I file sono diversi"
fi