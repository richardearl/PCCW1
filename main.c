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
	int rowAmount;
} t_data;

typedef struct arrayOut{
	//needs to contain the array and indicate how many rows it accounts for.
	double *array;
	int rows;
}	arrayOut;

void *threadFunction(void *threadData){
	
	t_data *data = (t_data *)threadData;
	printf("Thread(%d) Created \n ", data->threadId);
	double **relaxArray = data->threadArray;
	int i = data-> threadId;
	i=i+1; //use this to determine the row , and we never process the top row
	int size = data-> arrayDim;
	//printf("Test0");
	int amountofrows =  data-> rowAmount;
	int j;
	//printf("Test0 amountofrows: %d",amountofrows);
	double **updatedArray = malloc(amountofrows*sizeof(double*));
	double *updatedBuf = malloc(amountofrows*size*sizeof(double));
	//printf("Test1");
	//this creates a 2d array with the ammount of rows the thread is taking on and the columns the same size as the main array
	int a;
	for (a = 0; a < amountofrows; a++) {
		updatedArray[a] = updatedBuf + amountofrows*a;
	}
	
	int r;
	//printf("Test2");
	//adding an extra loop to iterate through all of the rows a thread may have to compute.
	for(r=i;r<i+amountofrows; r=r+1){	
		//printf("Test2.3");
		//printf("index %d", r-i);
		updatedArray[r-i][0] = relaxArray[r][0];
		printf("borders: %f %f", relaxArray[r][0],relaxArray[r][size-1] );
		//printf("Test2.5");
		updatedArray[r-i][size-1] = relaxArray[r][size-1];
		//printf("Test3");
		for(j=1;j<size-1;j=j+1 ){
			updatedArray[r-i][j]=(relaxArray[r][j+1]+relaxArray[r][j-1]+relaxArray[r+1][j]+relaxArray[r-1][j])/4;
			printf("updatedArray[%d][%d] is %f",r-i, j, updatedArray[r-i][j]);
			printf("%f is the average of %f %f %f %f \n" ,updatedArray[r-i][j],relaxArray[r][j+1],relaxArray[r][j-1],relaxArray[r+1][j],relaxArray[r-1][j]);
    	}	
    }
	//return (void *) updatedArray; 
	//printf("Test4");
	arrayOut *outp;
	arrayOut out;
	//personPtr = (struct person*) malloc(n * sizeof(struct person));
	outp = (struct arrayOut*) malloc(sizeof(struct arrayOut));
	outp = &out;
	out.array = updatedBuf;
	out.rows = amountofrows;
	printf("Array to send back: %f, %f, %f, %f and rows: %d \n",out.array[0],out.array[1],out.array[2],out.array[3],out.rows);

	pthread_barrier_wait(&barrier);
	pthread_exit((void*) outp);
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
	//printf("arg: %d", arraySize);
	
	
	

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
	printf("Array row 1: %f %f %f %f \n",a[0][0],a[0][1],a[0][2],a[0][3]  );
	printf("Array row 2: %f %f %f %f \n",a[1][0],a[1][1],a[1][2],a[1][3]  );
	printf("Array row 3: %f %f %f %f \n",a[2][0],a[2][1],a[2][2],a[2][3]  );
	printf("Array row 4: %f %f %f %f \n",a[3][0],a[3][1],a[3][2],a[3][3]  );
	//printf("a[3][3] equals = %f \n", a[3][3]);
	pthread_t threadList[threadCount]; //create an array for the threads
	int threadIndex[threadCount];
	t_data *t_data_array = malloc(threadCount*sizeof(t_data));
	int t;
	int test;
	int doneCheck =1;
	int iterations = 0;
	int ramt = (arraySize-2)/threadCount;
	int remainder;
	int rownumber;
	//create a loop that iterates until numbers settle
	while(doneCheck){
		iterations= iterations+ 1;
		rownumber = 0;
		remainder = (arraySize -2) % threadCount;
		for(t =0; t<threadCount; t=t+1){
			printf("Going to create thread number ( %d)", t);
			//t_data_array[t].threadId = t; 
			t_data_array[t].threadId = rownumber; 
			printf("rownumber: %d, index: %d", rownumber,t);
			//this line casues seg fault for some reason
			t_data_array[t].arrayDim = arraySize;
			t_data_array[t].threadArray = a;
			if(remainder == 0 ){
				t_data_array[t].rowAmount = ramt;
				rownumber = rownumber + ramt;
			}
			else{
				t_data_array[t].rowAmount = ramt+1;
				rownumber = rownumber + ramt + 1;
				remainder=remainder-1;
			} 
			
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
			arrayOut *newArray;
			pthread_join(threadList[t], (void*)&newArray);
			double *updateArraySegment = newArray -> array;
			int howmanyrows = newArray -> rows;
			printf("Array: %f %f %f %f rows: %d \n", updateArraySegment[0], updateArraySegment[1], updateArraySegment[2], updateArraySegment[3], howmanyrows); 
			//split up the array into smaller arrays with col amt equal to actual array
			double **anew = malloc(howmanyrows*sizeof(double*));
			int n;
			for(n=0; n <howmanyrows; n=n+1){
				//a[i] = buf + arraySize*i;
				anew[n]=updateArraySegment + n*arraySize;
				for(l=0; l< arraySize;l=l+1){
					//double diff = fabs(a[t+1][l] - updateArraySegment[l]);
					double diff = fabs(a[t+1][l] - anew[n][l]);
					//printf(" Diff: %f", diff );
					if(diff > prec){
					printf("found a bigger dif: %f %f \n", diff, prec);
					doneCheck = 1;
					}
				}
				a[t+n+1]= updateArraySegment;
			}
			
			//printf("Updated Array %f", newArray[1]);
			//a[t+1]= updateArraySegment;
			//printf("Updated Array %f", newArray[1]);
		}
		printf("Array row 1: %f %f %f %f \n",a[0][0],a[0][1],a[0][2],a[0][3]  );
		printf("Array row 2: %f %f %f %f \n",a[1][0],a[1][1],a[1][2],a[1][3]  );
		printf("Array row 3: %f %f %f %f \n",a[2][0],a[2][1],a[2][2],a[2][3]  );
		printf("Array row 4: %f %f %f %f \n",a[3][0],a[3][1],a[3][2],a[3][3]  );
		
	}
	printf("ITERATIONS TAKEN: %d \n", iterations);
	printf("threadcount: %d arraysize: %d ramt: %d remainder: %d ", threadCount,arraySize, ramt, remainder);
 }