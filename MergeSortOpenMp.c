//for Final_Project MergeSort
//Please write a parallel program to sort 1000000000 numbers (value range: 1-100), randomly generated in master. 
//a OpenMP program with 2, 4, 8, 16 processors

#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<limits.h>
#include <string.h>

//N must be large enough to show parallelism
#define N 1000000
#define range 100


/********************************just modify number of threads you want to use here*******************************************/
#define threads 1


//for qsort
int compare(const void*,const void*);

void merge(int[],int,int[]);

void mergesort_serial(int[],int,int[]);

void mergesort_parallel_omp(int[],int,int[],int);


int main(int argc,char *argv[]){
	
 	
	
 int *array=(int*)malloc(N*sizeof(int));
 int *temp =(int*)malloc(N*sizeof(int));
 
 int i;
 
 double start=0.0,end; 
 
 
 
 //generate N numbers which range is : 1-100,put into array
 for(i=0;i<N;i++)array[i]=(rand()%100)+1;
 
 printf("\n\n--------------------There are %d threads created--------------------\n\n",threads);

 start=omp_get_wtime();
 
     //?????
	 omp_set_nested(1);
	 
	 mergesort_parallel_omp(array,N,temp,threads);
 
 end=omp_get_wtime();
 
 
 printf("Total execution time of mergesort_parallel_omp is %f \n",end-start);

 char sorted = 'Y';
 for(i = 0; i < N - 2; i++){
		if(array[i] > array[i+1]){
				sorted = 'N';
				break;
			}			
		}
		if(sorted == 'Y'){
			printf("\nSORTING is CORRECT\n");
		}else{
			printf("\nSORTING is NOT CORRECT\n");
		}
		printf("\n--------------------------------------------------------------------\n");
		
 printf("\n");
 
 
 free(array);
 return 0;
}


void merge(int a[], int size, int temp[]) {
    int left = 0;
    int right = size/2;
    int tempindex = 0;
    while (left < size/2 && right < size) {
        if (a[left] < a[right]) {
            temp[tempindex] = a[left];
            left++;
        } else {
            temp[tempindex] = a[right];
            right++;
        }
        tempindex++;
    }
    while (left < size/2) {
        temp[tempindex] = a[left];
        left++;
        tempindex++;
    }
    while (right < size) {
        temp[tempindex] = a[right];
        right++;
        tempindex++;
    }
    // Copy sorted temp array into main array
    memcpy(a, temp, size*sizeof(int));
}




void mergesort_parallel_omp(int a[], int size, int temp[], int thread) {
    if ( thread == 1) {
    	mergesort_serial(a, size, temp);
    } else if (thread > 1) {
       #pragma omp parallel sections num_threads(threads)
       {
		        
			#pragma omp section
		    { 
			mergesort_parallel_omp(a, size/2, temp, thread/2);
			}
			#pragma omp section
		    { 
			mergesort_parallel_omp(a + size/2, size - size/2, temp + size/2, thread - thread/2);
			}
				
       }

       merge(a, size, temp); 
	   
    } else {
       printf("Error!!", thread); 
       return;
    }
}
void mergesort_serial(int a[], int size, int temp[]) {
    // Switch to qsort for small arrays
     if (size < 50) {
       qsort(a,size,sizeof(int), compare);
       return;
    }    
    mergesort_serial(a, size/2, temp);
    mergesort_serial(a + size/2, size - size/2, temp);
    merge(a, size, temp);
}


int compare(const void *num1, const void *num2){
	int a1=*(int*)num1;
	int a2=*(int*)num2;
	if(a1<a2)return -1;
	else if(a1==a2)return 0;
	else return 1;
}



