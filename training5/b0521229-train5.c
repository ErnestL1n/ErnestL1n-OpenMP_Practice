//author:  B0521229  Ernest Lin 
//for homework5 A MOTIVATING EXAMPLE 
//A major issue in devising a parallel algorithm for a given problem is the way in which the computational load is divided between the multiple processors. 
//Problem: Prime number finding
//Please write a OpenMP program to find all prime numbers less than 10,000,000,000.
//List the total number of prime numbers.
//Calculate the speedup ratio for p=2, 4, 8, 16.


/*-----------------------------------And compare complexity to Train2 Mpi case-------------------------------------*/


//reference:Sieve of Eratosthenes algorithm
//Time complexity : O(n*log(log(n)))



#include<stdio.h>
#include<stdlib.h>
#include<omp.h>     //for OpenMP
#include<time.h>
#include<math.h>    //for sqrt
#include<string.h>  //for memset

#define N 10000000000
#define sqrtOfN 100000


int main(int argc,char *argv[]){
	
	int numberofthreads;
	
	//to store how many prime numbers<=N
	unsigned long long int primes=0;
	
	unsigned long long int i,j;

    double start=0.0,end;
	
	
	//for well load balancing,we schedule as "runtime type"
	//since we find primes,it is good to decide at runtime
	
	
	
	
	
	         /****************just modify number of threads you want to use here***************************/
			 omp_set_num_threads(1);
			 
			 
	
	
	
	
	//malloc a large array "ifprime[0..N]"
    //initialize all entries it as true(=1).
    //A value in prime[i] will finally be false if i is Not a prime, else true.
	char *ifprime=(char*)malloc(sizeof(char)*(N+1));
	
	
	//begin from 2
    memset(ifprime,'y',sizeof(ifprime[0])*(N+1));
	
    /*parallel directive=>create threads*/
	#pragma omp parallel 
	{
		numberofthreads=omp_get_num_threads();
		
	#pragma omp	master
	printf("\n\n--------------------There are %d threads created-----------------------\n",numberofthreads);
		
		
		
		
		start=omp_get_wtime();
		
	/*------GUIDED: Similar to DYNAMIC except chunk size decreases over time (better load balancing)------*/	
	#pragma omp for private(i,j) schedule(runtime)
	                    //atomic barrier
						/*using reduction to guarantee atomicity and increase performance*/				
	for(i=2;i<=sqrtOfN;i++)
	{
		// If prime[i] is not changed, then it is a prime 
		if(ifprime[i]=='y')
	    {
		// Update all multiples of p greater than or  
        // equal to the square of it 
        // numbers which are multiple of p and are 
        // less than p^2 are already been marked.  
            for (j=i*i; j<=N; j+=i) 
                ifprime[j] = 'n'; 
		}
	}	
	
	
	
	
	//atomic barrier
	#pragma omp for private(i) nowait reduction(+:primes) 
	for(i=2;i<=N;i++)
	{
		if(ifprime[i]=='y'){           
			primes++;
		}
	}
	
	
	
		
		
		
		end=omp_get_wtime();
		
		
	
	printf("                Thread %3d  took %8.6f unit times\n",omp_get_thread_num(),end-start);
	
	
	}



	
	printf("  There are %llu prime numbers within %llu.\n",primes,N);
    

    
	free(ifprime);
	

	return 0;
  	
}