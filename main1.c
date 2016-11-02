#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//initialise barrier object for syncronizing arrays
pthread_barrier_t   barrier; 
typedef struct t_data{
	//define a struct that contains all the data that needs to be handed over to a thread.
	int threadId;
	int arrayDim;
	double **threadArray;
	//int rowAmount;
} t_data;

void *threadFunction(void *threadData){
	
	t_data *data = (t_data *)threadData;
	double **relaxArray = data->threadArray;
	int i = data-> threadId;
	i=i+1; //use this to determine the row , and we never process the top row
	int size = data-> arrayDim;
	printf("Thread(%d) Created , I equals: %d \n ", data->threadId, i);
	int j;
	double *updatedArray = malloc(size*sizeof(double*));
	updatedArray[0] = relaxArray[i][0];
	updatedArray[size-1] = relaxArray[i][size-1];
	for(j=1;j<size-1;j=j+1 ){
				updatedArray[j] = (relaxArray[i][j+1] + relaxArray[i][j-1] + relaxArray[i+1][j] + relaxArray[i-1][j])/4;
				printf("%f is the average of %f %f %f %f" ,updatedArray[j],relaxArray[i][j+1],relaxArray[i][j-1],relaxArray[i+1][j],relaxArray[i-1][j]);
    }
	//return (void *) updatedArray;  
	pthread_barrier_wait(&barrier);
	pthread_exit(updatedArray);
}


int main( int argc, char** argv )
{
	//Initialise an array to be used by all threads
	double relaxArray[4][4] = {  
     {2, 3, 2, 1  } ,   //[0][1], [0][2] ..
     {8, 9, 12, 17} ,   
     {8, 3, 9, 1  } ,
     {12, 11, 2, 4}
     };
	 
	double prec = 0.2;
	//int testint= strtol("test:",NULL,0);
	//printf("Test: %d", testint);
	int arraySize = 4;
	if (strtol(argv[1], NULL, 0)>4){
		arraySize = strtol(argv[1], NULL, 0);
	}
	printf("arg: %d", arraySize);
	
	
	

	int threadCount;
	
	if(arraySize<18){
		threadCount = arraySize -2;
	}
	else{
		threadCount=16;
	}
	pthread_barrier_init (&barrier, NULL, threadCount);
    double **a = malloc(arraySize*sizeof(double*));
	if (a == NULL){
	}
	double *buf = malloc(arraySize*arraySize*sizeof(double));
	if (buf == NULL){
	}
	
	int i;
	int j;
	for (i = 0; i < arraySize; i++) {
		a[i] = buf + arraySize*i;
		for(j=0; j<arraySize;j++){
			a[i][j] =  rand() % 25;
			//printf("val %f \n",a[i][j]);
		}
	}
	printf("a[3][3] equals = %f \n", a[3][3]);
	pthread_t threadList[threadCount]; //create an array for the threads
	int threadIndex[threadCount];
	t_data *t_data_array = malloc(threadCount*sizeof(t_data));
	int t;
	int test;
	int doneCheck =1;
	int iterations = 0;
	
	
	//create a loop that iterates until numbers settle
	while(doneCheck){
		iterations= iterations+ 1;
		//int rowseach = arraySize / threadCount;
		//int remainder = arraySize % threadCount;
		//int rowStart = 0;
		for(t =0; t<threadCount; t=t+1){
			printf("Going to create thread number ( %d)", t);
			t_data_array[t].threadId = t;
			t_data_array[t].arrayDim = arraySize;
			t_data_array[t].threadArray = a;
			//if(){
			//	t_data_array[t].rowAmount = ramt;
			//}
			//else{
				
			//}
			
			//here I determine how much each thread will need to take care of
			//t_data toThread;
			//toThread.threadId = t;
			//toThread.arrayDim = arraySize;
			//toThread.threadArray = a;
			test = pthread_create( &threadList[t], NULL, threadFunction, &t_data_array[t]);
		}
		void *newArray;
		doneCheck =0; //assume the array matches until we see a line that doesnt.
		int l;
		//pthread_barrier_wait(&barrier);
		for (t = 0; t < threadCount;t=t+1) {
			pthread_join(threadList[t], &newArray);
			double *updateArraySegment = newArray;
			for(l=0; l< arraySize;l=l+1){
				double diff = fabs(a[t+1][l] - updateArraySegment[l]);
				//printf(" Diff: %f", diff );
				if(diff > prec){
					printf("found a bigger dif: %f %f \n", diff, prec);
					doneCheck = 1;
				}
			}
			//printf("Updated Array %f", newArray[1]);
			a[t+1]= updateArraySegment;
			//printf("Updated Array %f", newArray[1]);
		}
		printf("Array row 1: %f %f %f %f \n",a[0][0],a[0][1],a[0][2],a[0][3]  );
		printf("Array row 2: %f %f %f %f \n",a[1][0],a[1][1],a[1][2],a[1][3]  );
		printf("Array row 3: %f %f %f %f \n",a[2][0],a[2][1],a[2][2],a[2][3]  );
		printf("Array row 4: %f %f %f %f \n",a[3][0],a[3][1],a[3][2],a[3][3]  );
		
	}
	printf("ITERATIONS TAKEN: %d \n", iterations);
 }