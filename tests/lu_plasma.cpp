

#include <iostream>
#include <iomanip>

#include "timer.hpp"
#include "matrix.hpp"
#include "kernels.hpp"
#include <mkl_lapacke.h>
#include <mkl_lapack.h>
#include <plasma.h>

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8
#define DEFAULT_CORES 1

int SIZE;
int NB;
int CORES;

using namespace std;

template<typename T>
void print_array(int M, int N, T *A, int LDA) {
	std::cout << "----------" << std::endl;
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			//std::cout << std::setw(10) << "["<<i+j*LDA<<"]="<<A[i+j*LDA];
			std::cout << std::setw(16) << A[i+j*LDA];
		}
		std::cout << std::endl;
	}
	std::cout << "----------" << std::endl;
}

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */

int main(int argc, char* argv[]) {

		SIZE = DEFAULT_SIZE;
		NB = DEFAULT_NB;
		CORES = DEFAULT_CORES;
		char c;

		while ((c = getopt(argc, argv, "n:b:a:")) != -1)
		switch (c)	{
			case 'n':
				SIZE = atoi(optarg); 
			case 'b':
				NB = atoi(optarg);	 
			 	break;
			case 'a':
				CORES = atoi(optarg);
				break;
		 	default:
				break;		
		}

    int N     = SIZE;
    int LDA   = N;
    int info;
    int LDAxN = LDA*N;

    double *A = (double *)malloc(LDA*N*(sizeof*A));
    PLASMA_desc *L;
    int *IPIV;
		

    //Plasma Initialize
    PLASMA_Init(CORES);
    printf("-- PLASMA is initialized to run on %d cores. \n",CORES);
    /* Initialize A Matrix */
    dlarnv(&IONE, ISEED, &LDAxN, A);
    // Allocate L and IPIV
    info = PLASMA_Alloc_Workspace_dgetrf_incpiv(N, N, &L, &IPIV);
   	//LU factorization of the matrix A
		start_timer();
    info = PLASMA_dgetrf_incpiv(N, N, A, LDA, L, IPIV);
		stop_timer();
		free(A); free(IPIV); free(L);
		PLASMA_Finalize();

		cout << "LU completed!" << endl;
		print_timer();		

    return 0;
}

