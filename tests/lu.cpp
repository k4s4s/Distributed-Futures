

#include <iostream>
#include <iomanip>

#include "matrix.hpp"
#include "kernels.hpp"
#include <cblas.h>
#include "lapacke.h"

//#include <future>
//#include <plasma.h>

//#include <futures.hpp>

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8

using namespace std;

void print_matrix(int M, int N, double *A, int LDA) {
	std::cout << "----------" << std::endl;
	for(int j=0; j < N; j++) {
		for(int i=0; i < M; i++) {
			std::cout << std::setw(16) << A[i*LDA+j];
		}
		std::cout << std::endl;
	}
	std::cout << "----------" << std::endl;
}

int dgetrf(int M, int N, vector<double>& A, int LDA, vector<double>& L, vector<int>& IPIV) { 
    int NB = DEFAULT_NB;

		int TILES = M/NB;
		int m = M/TILES;
		int n = N/TILES;

		//std::cout << "TILES:" << TILES << std::endl;
    for(int k=0; k < TILES; k++) {
				//print_matrix(M, N, A, M);
        int info;
				//std::cout << "dgetrf on tile A"<<k<<k<< std::endl;
				//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
        core_dgetrf(m, n, &A[k*m+k*n*LDA], 
                    LDA, &IPIV[k*m+k*n*LDA], &info);
				//std::cout << "dgetrf result" << std::endl;
				//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
        if(info != 0)
            std::cout << "dgetrf failed with " << info << "." << std::endl;
        //second step

        for(int mm = k+1; mm < TILES; mm++) {
            //dgessm dgessm_call;
            //dgessm_call(bsz, bsz, bsz, bsz, IPIV(k,k), A(k,k), max(1, bsz), A(k,m), max(1, bsz));
						double* work = (double*)malloc(m*n*sizeof(double));
						//std::cout << "dtstrf on tiles A"<<k<<k<<" and A"<<mm<<k << std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[mm*m+k*n*LDA], LDA);
            core_dtstrf(m, n, m, n, &A[k*m+k*n*LDA], LDA, &A[mm*m+k*n*LDA], LDA,
                        &L[mm*m+k*n*LDA], n, &IPIV[mm*m+k*n*LDA], work, n, &info);
						//std::cout << "dtstrf result" << std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[mm*m+k*n*LDA], LDA);
						free(work);
            if(info != 0)
                std::cout << "dtstrf failed with " << info << "." << std::endl;
        }
        for(int nn = k+1; nn < TILES; nn++) {
						//std::cout << "dgessm on tiles A"<<k<<k<<" and A"<<k<<nn<< std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
            core_dgessm(m, n, m, n, &IPIV[k*m+k*n*LDA], &A[k*m,k*n*LDA], LDA,
                        &A[k*m+nn*n*LDA], LDA);
						//std::cout << "dgessm result" << std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[k*m+nn*n*LDA], LDA);
            for(int mm=k+1; mm < TILES; mm++) {
                core_dssssm(m, n, m, n, m, m, &A[k*m+nn*n*LDA], LDA, &A[mm*m+nn*n*LDA], LDA,
                            &L[mm*m+k*n*LDA], m, &A[mm*m+k*n*LDA], LDA, &IPIV[mm*m+k*n*LDA]);
            }
        }
    }
    return 0;
};

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */

void init_ipiv(int M, int N, std::vector<int>& IPIV) {
	int NB = DEFAULT_NB;
  int NT = (N%NB==0) ? (N/NB) : ((N/NB)+1);
  int MT = (M%NB==0) ? (M/NB) : ((M/NB)+1);
  int size = (size_t)MT*NT*NB*sizeof(int);
	IPIV = std::vector<int>(size);
};

int main(int argc, char* argv[]) {

    int cores = 1;
    int N     = 8;
    int LDA   = 8;
    int NRHS  = 4;
    int LDB   = 8;
    int info;
    int LDAxN = LDA*N;
    int LDBxNRHS = LDB*NRHS;
/*
    double *_A = (double *)malloc(LDA*N*(sizeof*_A));
    PLASMA_desc *_L;
    int *_IPIV;
*/
		//Matrix<double> A(LDA, N);
		//Matrix<double> L(LDA, N);
		std::vector<double> A(LDA*N);
		std::vector<double> L(LDA*N);
		std::vector<int> IPIV;
	 	init_ipiv(N, N, IPIV);
/*
    //Plasma Initialize
    PLASMA_Init(cores);
    printf("-- PLASMA is initialized to run on %d cores. \n",cores);
*/
    /* Initialize A Matrix */
    LAPACKE_dlarnv_work(IONE, ISEED, LDAxN, &A[0]);
/*
    for (int i = 0; i < N; i++)
        for (int  j = 0; j < N; j++)
            _A[LDA*j+i] = A[i+LDA*j];
*/
		print_matrix(N, N, &A[0], N);
/*
    // Allocate L and IPIV
    info = PLASMA_Alloc_Workspace_dgetrf_incpiv(N, N, &_L, &_IPIV);
   	//LU factorization of the matrix A
    info = PLASMA_dgetrf_incpiv(N, N, _A, LDA, _L, _IPIV);
		print_matrix(N, N, _A, N);
		//print_matrix(N, N, (double*)_L->mat, N);
		//print_matrix(N, N, (double*)_IPIV, N);	
		free(_A); free(_IPIV); free(_L);
		PLASMA_Finalize();
*/
		cout << "Running LU factorization" << endl;
    info = dgetrf(N, N, A, LDA, L, IPIV);
		print_matrix(N, N, &A[0], N);
		//print_matrix(N, N, (double*)&_L->mat, N);
		//print_matrix(N, N, (double*)&_IPIV, N);
		//delete IPIV;
    /* Solve the problem */
		cout << "LU completed!" << endl;

    return 0;
}

