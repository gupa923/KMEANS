/*
 * k-Means clustering algorithm
 *
 * OpenMP version
 *
 * Parallel computing (Degree in Computer Engineering)
 * 2022/2023
 *
 * Version: 1.0
 *
 * (c) 2022 Diego García-Álvarez, Arturo Gonzalez-Escribano
 * Grupo Trasgo, Universidad de Valladolid (Spain)
 *
 * This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
 * https://creativecommons.org/licenses/by-sa/4.0/
 */



#include <omp.h>


/*
# ifdef _OPENMP
	int my_rank= omp_get_thread_num();
	int thread_count = omp_get_num_threads();
# else
	int my_rank =0;
	int thread_count =1;
# endif
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>


#define MAXLINE 2000
#define MAXCAD 200

//Macros
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/* 
Function showFileError: It displays the corresponding error during file reading.
*/
void showFileError(int error, char* filename)
{
	printf("Error\n");
	switch (error)
	{
		case -1:
			fprintf(stderr,"\tFile %s has too many columns.\n", filename);
			fprintf(stderr,"\tThe maximum number of columns has been exceeded. MAXLINE: %d.\n", MAXLINE);
			break;
		case -2:
			fprintf(stderr,"Error reading file: %s.\n", filename);
			break;
		case -3:
			fprintf(stderr,"Error writing file: %s.\n", filename);
			break;
	}
	fflush(stderr);	
}

/* 
Function readInput: It reads the file to determine the number of rows and columns.
*/
int readInput(char* filename, int *lines, int *samples)
{
    FILE *fp;
    char line[MAXLINE] = "";
    char *ptr;
    const char *delim = "\t";
    int contlines, contsamples = 0;
    
    contlines = 0;

    if ((fp=fopen(filename,"r"))!=NULL)
    {
        while(fgets(line, MAXLINE, fp)!= NULL) 
		{
			if (strchr(line, '\n') == NULL)
			{
				return -1;
			}
            contlines++;       
            ptr = strtok(line, delim);
            contsamples = 0;
            while(ptr != NULL)
            {
            	contsamples++;
				ptr = strtok(NULL, delim);
	    	}	    
        }
        fclose(fp);
        *lines = contlines;
        *samples = contsamples;  
        return 0;
    }
    else
	{
    	return -2;
	}
}

/* 
Function readInput2: It loads data from file.
*/
int readInput2(char* filename, float* data)
{
    FILE *fp;
    char line[MAXLINE] = "";
    char *ptr;
    const char *delim = "\t";
    int i = 0;
    
    if ((fp=fopen(filename,"rt"))!=NULL)
    {
        while(fgets(line, MAXLINE, fp)!= NULL)
        {         
            ptr = strtok(line, delim);
            while(ptr != NULL)
            {
            	data[i] = atof(ptr);
            	i++;
				ptr = strtok(NULL, delim);
	   		}
	    }
        fclose(fp);
        return 0;
    }
    else
	{
    	return -2; //No file found
	}
}

/* 
Function writeResult: It writes in the output file the cluster of each sample (point).
*/
int writeResult(int *classMap, int lines, const char* filename)
{	
    FILE *fp;
    
    if ((fp=fopen(filename,"wt"))!=NULL)
    {
        for(int i=0; i<lines; i++)
        {
        	fprintf(fp,"%d\n",classMap[i]);
        }
        fclose(fp);  
   
        return 0;
    }
    else
	{
    	return -3; //No file found
	}
}

/*

Function initCentroids: This function copies the values of the initial centroids, using their 
position in the input data structure as a reference map.
*/
void initCentroids(const float *data, float* centroids, int* centroidPos, int samples, int K)
{
	int i;
	int idx;
	for(i=0; i<K; i++)
	{
		idx = centroidPos[i];
		memcpy(&centroids[i*samples], &data[idx*samples], (samples*sizeof(float)));
	}
}

/*
Function euclideanDistance: Euclidean distance
This function could be modified
*/
float euclideanDistance(float *point, float *center, int samples)
{
	float dist=0.0;
	for(int i=0; i<samples; i++) 
	{
		dist+= (point[i]-center[i])*(point[i]-center[i]);
	}
	dist = sqrt(dist);
	return(dist);
}

/*
Function zeroFloatMatriz: Set matrix elements to 0
This function could be modified
*/
void zeroFloatMatriz(float *matrix, int rows, int columns)
{
	int ij;	
	#pragma omp parallel for num_threads(8)// schedule(dynamic,1)
	for (ij=0; ij<rows*columns; ij++)
		matrix[(ij/columns*columns)+(ij%columns)] = 0.0;
	/*
	int i,j;
	
		
	#pragma omp parallel for num_threads(4) collapse(2)
	for (i=0; i<rows; i++)
		for (j=0; j<columns; j++)
			matrix[i*columns+j] = 0.0;
	*/
	
	
}

/*
Function zeroIntArray: Set array elements to 0
This function could be modified
*/
void zeroIntArray(int *array, int size)
{
	int i;
	
	
	#pragma omp parallel for num_threads(8)
	for (i=0; i<size; i++)
		array[i] = 0;	
	
}



int main(int argc, char* argv[])
{

	//START CLOCK***************************************
	double start, end;
	start = omp_get_wtime();
	//**************************************************
	/*
	* PARAMETERS
	*
	* argv[1]: Input data file
	* argv[2]: Number of clusters
	* argv[3]: Maximum number of iterations of the method. Algorithm termination condition.
	* argv[4]: Minimum percentage of class changes. Algorithm termination condition.
	*          If between one iteration and the next, the percentage of class changes is less than
	*          this percentage, the algorithm stops.
	* argv[5]: Precision in the centroid distance after the update.
	*          It is an algorithm termination condition. If between one iteration of the algorithm 
	*          and the next, the maximum distance between centroids is less than this precision, the
	*          algorithm stops.
	* argv[6]: Output file. Class assigned to each point of the input file.
	* */


	if(argc !=  7)
	{
		fprintf(stderr,"EXECUTION ERROR K-MEANS: Parameters are not correct.\n");
		fprintf(stderr,"./KMEANS [Input Filename] [Number of clusters] [Number of iterations] [Number of changes] [Threshold] [Output data file]\n");
		fflush(stderr);
		exit(-1);
	}

	// Reading the input data
	// lines = number of points; samples = number of dimensions per point
	int lines = 0, samples= 0;  
	
	int error = readInput(argv[1], &lines, &samples);
	if(error != 0)
	{
		showFileError(error,argv[1]);
		exit(error);
	}
	
	float *data = (float*)calloc(lines*samples,sizeof(float));
	if (data == NULL)
	{
		fprintf(stderr,"Memory allocation error.\n");
		exit(-4);
	}
	error = readInput2(argv[1], data);
	if(error != 0)
	{
		showFileError(error,argv[1]);
		exit(error);
	}


	// Parameters
	int K=atoi(argv[2]); 
	int maxIterations=atoi(argv[3]);
	int minChanges= (int)(lines*atof(argv[4])/100.0);
	float maxThreshold=atof(argv[5]);

	int *centroidPos = (int*)calloc(K,sizeof(int));
	float *centroids = (float*)calloc(K*samples,sizeof(float));
	int *classMap = (int*)calloc(lines,sizeof(int));

    if (centroidPos == NULL || centroids == NULL || classMap == NULL)
	{
		fprintf(stderr,"Memory allocation error.\n");
		exit(-4);
	}
	// Initial centrodis
	srand(0);
	int i;
	for(i=0; i<K; i++) 
		centroidPos[i]=rand()%lines;
	
	// Loading the array of initial centroids with the data from the array data
	// The centroids are points stored in the data array.
	initCentroids(data, centroids, centroidPos, samples, K);


	printf("\n\tData file: %s \n\tPoints: %d\n\tDimensions: %d\n", argv[1], lines, samples);
	printf("\tNumber of clusters: %d\n", K);
	printf("\tMaximum number of iterations: %d\n", maxIterations);
	printf("\tMinimum number of changes: %d [%g%% of %d points]\n", minChanges, atof(argv[4]), lines);
	printf("\tMaximum centroid precision: %f\n", maxThreshold);
	
	//END CLOCK*****************************************
	end = omp_get_wtime();
	printf("\nMemory allocation: %f seconds\n", end - start);
	fflush(stdout);
	//**************************************************
	//START CLOCK***************************************
	start = omp_get_wtime();
	//**************************************************
	char *outputMsg = (char *)calloc(10000,sizeof(char));
	char line[100];

	int j;
	int class;
	float dist, minDist;
	int it=0;
	int changes = 0;
	float maxDist;

	//pointPerClass: number of points classified in each class
	//auxCentroids: mean of the points in each class
	int *pointsPerClass = (int *)malloc(K*sizeof(int));
	float *auxCentroids = (float*)malloc(K*samples*sizeof(float));
	float *distCentroids = (float*)malloc(K*sizeof(float)); 
	if (pointsPerClass == NULL || auxCentroids == NULL || distCentroids == NULL)
	{
		fprintf(stderr,"Memory allocation error.\n");
		exit(-4);
	}

/*
 *
 * START HERE: DO NOT CHANGE THE CODE ABOVE THIS POINT
 *
 */

	do{
		it++;
	
		//1. Calculate the distance from each point to the centroid
		//Assign each point to the nearest centroid.
		//#pragma omp parallel num_threads(8) 
		changes = 0;
		
		/*
		//OPZIONE 1 (non funziona)
		#pragma omp parallel for num_threads(8) private(dist) collapse(2) 
		for(i=0; i<lines; i++)
		{
			

			for(j=0; j<K; j++)
			{
				
				if (j==0)
				{
					class=1;
					minDist=FLT_MAX;
				}
			
				dist=euclideanDistance(&data[i*samples], &centroids[j*samples], samples);
				#pragma omp critical
				{
					if(dist < minDist)
					{
						
						minDist=dist;
						
						
						class=j+1;
					}	
					
				}
				if (j==0)
				{
					if(classMap[i]!=class)
					{
						#pragma omp atomic
						changes++;
					}
					classMap[i]=class;
				}
			
			
			}
		
			
		}
		*/
		
		/*
		//OPZIONE 2 (funziona ma va lento perche' non e' parallelizzato il primo for)
		//printf("%d\n", lines);
		for(i=0; i<lines; i++)
		{
			class=1;
			minDist=FLT_MAX;
			
			//#pragma omp for
			
			#pragma omp parallel for num_threads(8) private(dist) 
			for(j=0; j<K; j++)
			{
				
				//int my_rank = omp_get_thread_num();
				//int thread_count= omp_get_num_threads();
				//printf("Hello from thread %d of %d\n", my_rank, thread_count);
				dist=euclideanDistance(&data[i*samples], &centroids[j*samples], samples);
				#pragma omp critical
				{
					if(dist < minDist)
					{
						
						minDist=dist;
						
						
						class=j+1;
					}	
				}
				
				
			}
		
			if(classMap[i]!=class)
			{
				changes++;
			}
			classMap[i]=class;
		}
		*/
		///*
		//OPZIONE 3 (funziona ma e' da parallelizzare)
		
		int counter=0;
		

		//#pragma omp parallel for num_threads(8) private(dist) 
		for(int ij=0; ij<lines*K; ij++)
		{
			
			if ((ij%K)==0)
			{
				class=1;
				minDist=FLT_MAX;
			}
			//printf("%d\n", ij/K*samples);
			dist=euclideanDistance(&data[ij/K*samples], &centroids[(ij%K)*samples], samples);
			//#pragma omp critical
			//{
					
			if(dist < minDist)
			{
				
				minDist=dist;
				
				
				class=(ij%K)+1;
			}	
			//}
			
			
			if (counter <=K)
			{
				
				counter++;
			}
			
			//if (((ij%K)==0))
			if (counter==K)
			{
				counter=0;
				if(classMap[(ij/K)]!=class)
				{
					changes++;
				}
				classMap[(ij/K)]=class;
			}
			
		}
		
		
		//printf("\n%d\n", ij);
		//*/

		// 2. Recalculates the centroids: calculates the mean within each cluster

		//#pragma omp parallel num_threads(4)
		//{
			

			
		zeroIntArray(pointsPerClass,K);
		zeroFloatMatriz(auxCentroids,K,samples);
		//}


		//#pragma omp parallel num_threads(4)
		//{
			
		
		for(i=0; i<lines; i++) 
		{
			class=classMap[i];
			pointsPerClass[class-1] = pointsPerClass[class-1] +1;
			for(j=0; j<samples; j++){
				auxCentroids[(class-1)*samples+j] += data[i*samples+j];
			}
		}

		//int KSamples = K*samples;
		//int ij;
		
		int ij =0;
		#pragma omp parallel for num_threads(8) 
		for (ij=0;ij<K*samples;ij++)
		{
			
			//#pragma omp critical
			//{
				//printf("%d \n", ij);
			auxCentroids[(ij/samples*samples)+(ij%samples)] /= pointsPerClass[ij/samples];///samples];

			//}
			
		}
		
		
		
		
		
		//}
		
		/*
		#pragma omp parallel for num_threads(4) collapse(2)
		for(i=0; i<K; i++) 
		{
			for(j=0; j<samples; j++){
				auxCentroids[i*samples+j] /= pointsPerClass[i];
				//int my_rank = omp_get_thread_num();
				//int thread_count= omp_get_num_threads();
				//printf("Hello from thread %d of %d\n", my_rank, thread_count);
			}
		}
		*/
		
		/////////////////////////////////////////////////////////////////
		maxDist=FLT_MIN;
		//#pragma omp parallel for num_threads(4)
		for(i=0; i<K; i++){
			distCentroids[i]=euclideanDistance(&centroids[i*samples], &auxCentroids[i*samples], samples);
			if(distCentroids[i]>maxDist) {
				maxDist=distCentroids[i];
			}
			
		}
		
		memcpy(centroids, auxCentroids, (K*samples*sizeof(float)));
		
		sprintf(line,"\n[%d] Cluster changes: %d\tMax. centroid distance: %f", it, changes, maxDist);
		outputMsg = strcat(outputMsg,line);
		
		

		

	} while((changes>minChanges) && (it<maxIterations) && (maxDist>maxThreshold));

/*
 *
 * STOP HERE: DO NOT CHANGE THE CODE BELOW THIS POINT
 *
 */
	// Output and termination conditions
	printf("%s",outputMsg);	

	//END CLOCK*****************************************
	end = omp_get_wtime();
	printf("\nComputation: %f seconds", end - start);
	fflush(stdout);
	//**************************************************
	//START CLOCK***************************************
	start = omp_get_wtime();
	//**************************************************

	

	if (changes <= minChanges) {
		printf("\n\nTermination condition:\nMinimum number of changes reached: %d [%d]", changes, minChanges);
	}
	else if (it >= maxIterations) {
		printf("\n\nTermination condition:\nMaximum number of iterations reached: %d [%d]", it, maxIterations);
	}
	else {
		printf("\n\nTermination condition:\nCentroid update precision reached: %g [%g]", maxDist, maxThreshold);
	}	

	// Writing the classification of each point to the output file.
	error = writeResult(classMap, lines, argv[6]);
	if(error != 0)
	{
		showFileError(error, argv[6]);
		exit(error);
	}

	//Free memory
	free(data);
	free(classMap);
	free(centroidPos);
	free(centroids);
	free(distCentroids);
	free(pointsPerClass);
	free(auxCentroids);

	//END CLOCK*****************************************
	end = omp_get_wtime();
	printf("\n\nMemory deallocation: %f seconds\n", end - start);
	fflush(stdout);
	//***************************************************/
	return 0;
}
