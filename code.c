#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

//declaring pointers for Matrices
float mat1[10000][10000], mat2[10000][10000];
float singleThreadMat[10000][10000], multyThreadMat[10000][10000];

//declaring int variables for Matrix size
int row1, col1, row2,col2;

// read the matrices from the 2 files
void readMat() {

  FILE *inputMat1 = fopen("inputMat1.txt", "r");
  FILE *inputMat2 = fopen("inputMat2.txt", "r");

  if(inputMat1 == NULL || inputMat2 == NULL) {
      printf("Error opening file\n");
      exit(1);
  }
  // read from 1st file
  for(int i=0;i<row1;i++)
     for(int j=0;j<col1;j++)
         fscanf(inputMat1,"%f,",&mat1[i][j]);
  // read from 2nd file
  for(int i=0;i<row2;i++)
     for(int j=0;j<col2;j++)
         fscanf(inputMat2,"%f,",&mat2[i][j]);

  fclose(inputMat1);
  fclose(inputMat2);
}


//assign random Values to 2 Matrices
void randomMat() {

  for (int i=0; i<row1; i++)
    for (int j=0; j<col1; j++){
      if(rand() % 2==1)
        mat1[i][j] = (float)(rand() % 100)/RAND_MAX*10000;
      else
        mat1[i][j] = (rand() % 100);
    }
      

  for (int i=0; i<row2; i++)
    for (int j=0; j<col2; j++){
      if(rand() % 2==1)
        mat2[i][j] = (float)(rand() % 100)/RAND_MAX*10000;
      else
        mat2[i][j] = (rand() % 100);
    }
}

// display two input matrices
void displayMat() {

  printf("\n%s\n","------1st Matrix----- ");
  for (int i=0; i<row1; i++) {
    for (int j=0; j<col1; j++){
      if(mat1[i][j]-(int)mat1[i][j]>0)
        printf("\t%f ",mat1[i][j] );
      else
      printf("\t%0.f\t ",mat1[i][j] );
    }
    printf("\n");
  }

  printf("\n%s\n","------2nd Matrix----- ");
  for (int i=0; i<row2; i++) {
    for (int j=0; j<col2; j++){
      if(mat2[i][j]-(int)mat2[i][j]>0)
        printf("\t%f ",mat2[i][j] );
      else
      printf("\t%0.f\t ",mat2[i][j] );
    }
    printf("\n");
  }
}

// display two multiplied matrices
void displayOutputMat() {

  printf("\n%s\n","------Output Matrix----- ");
  for (int i=0; i<row1; i++) {
    for (int j=0; j<col2; j++){
      if(singleThreadMat[i][j]-(int)singleThreadMat[i][j]>0)
        printf("\t%f ",singleThreadMat[i][j] );
      else
      printf("\t%0.f\t ",singleThreadMat[i][j] );
    }
    printf("\n");
  }
}

// multiply 2 matrices
void multiplyMat() {

	for(int i = 0; i < row1; i++)
   	for(int j = 0; j < col2; j++)
	    for(int k=0; k<col1; k++)
		    singleThreadMat[i][j] += mat1[i][k] * mat2[k][j];
}

// multiply a row in a matrix
void * rowMultiplyer( void *arg ) {

  int threadID, row_start, row_end;
  double sum;

  threadID = *(int *)(arg);                   // get the thread ID
  row_start = threadID * (row2 / col1);
  row_end = (threadID+1) * (row2 / col1);

  for (int i = row_start; i < row_end; i++) { // hold row index of 'matrix1'
    for (int j = 0; j < col2; j++) {          // hold column index of 'matrix2'
      sum = 0;                                // hold value of a cell
      for (int k = 0; k < col1; k++)
	      sum += mat1[ i ][ k ] * mat2[ k ][ j ];
      multyThreadMat[ i ][ j ] = sum;
    }
  }
}

// multithreaded row wise multiplication 
void multiThreadMultiply() {

  pthread_t * threads;
  int num_threads = row1;
  threads = (pthread_t *) malloc( num_threads * sizeof(pthread_t) );
  for (int i = 0; i < num_threads; i++) {
    int *threadID;
    threadID = (int *) malloc( sizeof(int) );
    *threadID = i;
    pthread_create( &threads[i], NULL, rowMultiplyer, (void *)threadID );
  }

  for (int i = 0; i < num_threads; i++)
    pthread_join( threads[i], NULL );
}

void timeCal() {

  FILE *outputData;
  outputData = fopen("outputData.txt","a+");
  //  get the current time
  clock_t time;
  double exectime, timeDiff;
  time = clock();         // get start time
  multiplyMat();
  time = clock()-time;    // get time difference
  timeDiff = ((double)time)/CLOCKS_PER_SEC; // time difference in seconds

  displayOutputMat();

  printf("\n __________________________________________________________");
  printf("\n|         Single Thread Time      :  %.3lf s               |" ,timeDiff);

  fprintf(outputData,"\t%d\t\t ",row1*col2);
  fprintf(outputData,"\t%f\t\t ",timeDiff);

  time = clock();          // get start time
  multiThreadMultiply();
  time = clock()-time;     // get time difference
  timeDiff = ((double)time)/CLOCKS_PER_SEC; // time difference in seconds


  printf("\n|          MultiThread Time       :  %.3lf s               |" ,timeDiff);
  printf("\n|__________________________________________________________|");
  
  fprintf(outputData,"\t%f\t\t\n",timeDiff);

}

int main() {

  int select;
  srand((unsigned)time(NULL));

  printf("      Matrix  Multiplication Using POSIX Thread     \n" );
  printf("*****************************************************" );

  // get row and column size
  printf("\n 1st Matrix\n" );
  printf("\tRows      : " );
  scanf("%d",&row1);
  printf("\tColumns   : " );
  scanf("%d", &col1);

  printf("\n\n 2nd Matrix\n" );
  printf("\tRows      : " );
  scanf("%d",&row2);
  printf("\tColumns   : " );
  scanf("%d", &col2);

  // if this condition is true matrices are not multiplicable
  if(col1!=row2){
    printf("[%d][%d]  x [%d][%d] is not multiplicable \n",row1,col1,row2,col2 );
    return 0;
  }

  printf("\n\n[%d][%d]  x [%d][%d] is multiplicable  \n",row1,col1,row2,col2 );
  printf("\n\n-------------------------------------------------------" );
  printf("\nFill with the values in file  - 1");
  printf("\nFill with random values       - 2");

  FILE *outputData;
    outputData = fopen("outputData.txt","w");
    fprintf(outputData,"\tElements\t\t");
    fprintf(outputData,"Single Thread\t\t\t");
    fprintf(outputData,"Multi Thread\t\t\n");

  printf("\n\nSelect the Option : ");
  scanf("%d",&select);

  if(select==1) {
    printf("\n\tGet matrices from text file\n" );
    readMat();               // file read for two matices
    displayMat();            // display two matrices
    timeCal();                // calculate and display the time difference
  }
  else if(select==2) {
    printf("\n\tGenerate random matrices\n" );

    // for generate the size increesed matrix and get data to output file

    //for(int i = 0; i <= 1000; i += 10) {
      //row1=row2=col1=col2=i;
      //assign Random values to 2 randomMat
      randomMat();            // generate random matrix        
      displayMat();           // display two matrices
      timeCal();                // calculate and display the time difference
    //}
  }
  else
    printf("%s\n"," Invalid Selection ");


  return 0;

}
