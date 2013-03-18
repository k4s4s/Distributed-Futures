

#include <iostream>
#include <iomanip>

#include "matrix.hpp"
#include "kernels.hpp"
#include "lapacke.h"
//#include <plasma.h>

#include <future>
//#include <futures.hpp>

#define DEFAULT_NB 2
#define DEFAULT_SIZE 8

using namespace std;

void print_array(int M, int N, double *A, int LDA) {
	std::cout << "----------" << std::endl;
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			std::cout << std::setw(16) << A[i+j*LDA];
		}
		std::cout << std::endl;
	}
	std::cout << "----------" << std::endl;
}

template<typename T>
vector<T> get_tile(int M, int N, T *A, int LDA) {
	vector<T> tile(M*N);
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			 tile[i+j*N] = A[i+j*LDA];
		}
	}
	return tile;
}

template<typename T>
void copy_tile(int M, int N, vector<T> tile, T *A, int LDA) {
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			A[i+j*LDA] = tile[i+j*N];
		}
	}
}

void print_matrix(int M, int N, Matrix<double>& A) {
	std::cout << "----------" << std::endl;
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			std::cout << std::setw(16) << A(i, j);
		}
		std::cout << std::endl;
	}
	std::cout << "----------" << std::endl;
}

struct dgessm_RV {
	vector<double> Akn;
};

class dgessm_action {
public:
	dgessm_RV operator()(int M,
											int  	N,
											int  	K,
											int  	IB,
											vector<int> IPIV,
											vector<double> L,
											int  	LDL,
											vector<double> A,
											int  	LDA )
	{
		core_dgessm(M, N, K, IB, &IPIV[0], &L[0], LDL, &A[0], LDA);
		dgessm_RV retVal;
		retVal.Akn = A;
		return retVal;	
	}
};

struct dtstrf_RV {
	vector<double> Akk;
	vector<double> Amk;
	vector<double> Lmk;
	vector<int> 	 IPIVmk;
	int info;
};

class dtstrf_action {
public:
	dtstrf_RV operator()(int  	M,
						        	int  	N,
						        	int  	IB,
						        	int  	NB,
								      vector<double> U,
								      int  	LDU,
								      vector<double> A,
								      int  	LDA,
								      vector<double> L,
								      int  	LDL,
								      vector<int>	IPIV
								      /*vector<double> WORK,
								      int  	LDWORK,
								      /*int *  	INFO*/
											)
	{
		vector<double> work(M*N);
		int info;
		core_dtstrf(M, N, IB, NB, &U[0], LDU, &A[0], LDA,
		            &L[0], LDL, &IPIV[0], &work[0], M, &info);
		dtstrf_RV retVal;
		retVal.Akk = U;
		retVal.Amk = A;
		retVal.Lmk = L;
		retVal.IPIVmk = IPIV;
		retVal.info = info;
		return retVal;	
	}
};

struct dssssm_RV {
	vector<double> Akn;
	vector<double> Amn;
};

class dsssm_action {
public:
	dssssm_RV operator()(int  M1,
						          int  	N1,
						          int  	M2,
						          int  	N2,
						          int  	K,
						          int  	IB,
						          vector<double> A1,
						          int  	LDA1,
						          vector<double> A2,
						          int  	LDA2,
						          vector<double> L1,
						          int  	LDL1,
						          vector<double> L2,
						          int  	LDL2,
						          vector<int>	IPIV
						      )
	{
		core_dssssm(M1, N1, M2, N2, K, IB, &A1[0], LDA1, &A2[0], LDA2,
		            &L1[0], LDL1, &L2[0], LDL2, &IPIV[0]);
		dssssm_RV retVal;
		retVal.Akn = A1;
		retVal.Amn = A2;
		return retVal;
	}
};

int dgetrf(int M, int N, vector<double>& A, int LDA, vector<double>& L, vector<int>& IPIV) { 
    int NB = DEFAULT_NB;

		int TILES = M/NB;
		int m = M/TILES;
		int n = N/TILES;

		//std::cout << "TILES:" << TILES << std::endl;
		vector<future<dgessm_RV>> dgessm_tiles(TILES*TILES);
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
						//std::cout << "dtstrf on tiles A"<<k<<k<<" and A"<<mm<<k << std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[mm*m+k*n*LDA], LDA);
						vector<double> Akk = get_tile(m, n, &A[k*m+k*n*LDA], LDA);
						vector<double> Amk = get_tile(m, n, &A[mm*m+k*n*LDA], LDA);
						vector<double> Lmk = get_tile(m, n, &L[mm*m+k*n*LDA], LDA);
						vector<int> IPIVmk = get_tile(m, n, &IPIV[mm*m+k*n*LDA], LDA);
						dtstrf_action dtstrf;
						future<dtstrf_RV> fut = async(dtstrf, m, n, m, n, Akk, m, Amk, m, Lmk, n, IPIVmk);
            //dtstrf_RV res =  dtstrf(m, n, m, n, Akk, m, Amk, m, Lmk, n, IPIVmk);
						dtstrf_RV res = fut.get();
						copy_tile(m, n, res.Akk, &A[k*m+k*n*LDA], LDA);
						copy_tile(m, n, res.Amk, &A[mm*m+k*n*LDA], LDA);
						copy_tile(m, n, res.Lmk, &L[mm*m+k*n*LDA], LDA);
						copy_tile(m, n, res.IPIVmk, &IPIV[mm*m+k*n*LDA], LDA);
						//std::cout << "dtstrf result" << std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[mm*m+k*n*LDA], LDA);
           	if(res.info != 0)
                std::cout << "dtstrf failed with " << info << "." << std::endl;
        }
        for(int nn = k+1; nn < TILES; nn++) {
						//std::cout << "dgessm on tiles A"<<k<<k<<" and A"<<k<<nn<< std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						vector<double> Akk = get_tile(m, n, &A[k*m+k*n*LDA], LDA);
						vector<double> Akn = get_tile(m, n, &A[k*m+nn*n*LDA], LDA);
						vector<int> IPIVkk = get_tile(m, n, &IPIV[k*m+k*n*LDA], LDA);
						dgessm_action dgessm;
						//dgessm_RV res = dgessm(m, n, m, n, IPIVkk, Akk, m, Akn, m);
						dgessm_tiles[k+nn*TILES] = async(dgessm, m, n, m, n, IPIVkk, Akk, m, Akn, m);
						//copy_tile(m, n, dgessm_tiles[k+nn*TILES].get().Akn, &A[k*m+nn*n*LDA], LDA);
						//copy_tile(m, n, res.Akn, &A[k*m+nn*n*LDA], LDA);
						//std::cout << "dgessm result" << std::endl;
						//print_matrix(m, n, &A[k*m+k*n*LDA], LDA);
						//print_matrix(m, n, &A[k*m+nn*n*LDA], LDA);
					}
					for(int nn = k+1; nn < TILES; nn++) {
						vector<double> Akn = dgessm_tiles[k+nn*TILES].get().Akn;
						copy_tile(m, n, Akn, &A[k*m+nn*n*LDA], LDA);
						//vector<double> Akn = get_tile(m, n, &A[k*m+nn*n*LDA], LDA);
            for(int mm=k+1; mm < TILES; mm++) {
								dsssm_action dssssm;
								//vector<double> Akn = get_tile(m, n, &A[k*m+nn*n*LDA], LDA);
								vector<double> Amn = get_tile(m, n, &A[mm*m+nn*n*LDA], LDA);
								vector<double> Lmk = get_tile(m, n, &L[mm*m+k*n*LDA], LDA);
								vector<double> Amk = get_tile(m, n, &A[mm*m+k*n*LDA], LDA);
								vector<int> IPIVmk = get_tile(m, n, &IPIV[mm*m+k*n*LDA], LDA);
								future<dssssm_RV> fut = async(dssssm, m, n, m, n, m, m, Akn, m, Amn, m, Lmk, m, Amk, m, IPIVmk);
								//dssssm_RV res = dssssm(m, n, m, n, m, m, Akn, m, Amn, m, Lmk, m, Amk, m, IPIVmk);
								dssssm_RV res = fut.get();
								copy_tile(m, n, res.Akn, &A[k*m+nn*n*LDA], LDA);
								copy_tile(m, n, res.Amn, &A[mm*m+nn*n*LDA], LDA);
								Akn = get_tile(m, n, &A[k*m+nn*n*LDA], LDA); //FIXME:needs to get updated Akn at every step
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
		print_array(N, N, &A[0], N);
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
		print_array(N, N, &A[0], N);
		//print_matrix(N, N, (double*)&_L->mat, N);
		//print_matrix(N, N, (double*)&_IPIV, N);
		//delete IPIV;
    /* Solve the problem */
		cout << "LU completed!" << endl;

    return 0;
}

