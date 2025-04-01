Progetto universitario per il corso Programmazione di sistemi embeded e multicore

Dati del codice:
-data = matrice contenente i punti contenuti nel file input.

-centroidPos = array che contiene in numero della linea del cenroide iniziale nel file input.

-centroids = matrice dei centroidi (righe sono i centroidi e le colonne sono le coordinate).

-classMap = mappa ogni punto e il cluster a cui è associato.

-pointsPerClass = array che mantiene il numero di punti che appartengono ad ogni cluster.

-auxCentroids = matrice di appoggio usata nel ricalcolo dei centroidi.

-distCentroids = array che per ogni cluster mantiene la distanza tra il centroide vecchio e quello nuovo.

Come eseguire il codice sequenziale:

    ./KMEANS input_filename.inp num_cluster num_iterazioni percentuale_cambi(float) distanza_minima(float) output_filename.inp

    es: ./KMEANS test_files/input2D.inp 100 200 0.2 1.60 out_files/test4.inp
