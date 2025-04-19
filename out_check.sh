#!/bin/bash
# templete per fare i test per vedere se due file .inp sono uguali

# cambia il nome dei file per fare test su file diversi

# Confronta i due file riga per riga
diff omp_test_files/comp_seq100D2.inp out_files/mpi_100D2.inp > /dev/null
echo "test MPI 100D2"
# Verifica l'esito del comando diff
if [ $? -eq 0 ]; then
    echo "I file sono uguali"
else
    echo "I file sono diversi"
fi