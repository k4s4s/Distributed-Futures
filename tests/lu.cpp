

#include <iostream>
#include <iomanip>

#include "matrix.hpp"
#include "kernels.hpp"
#include "lapacke.h"
//#include <plasma.h>

#include <future>
//#include <futures.hpp>

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8

int SIZE;
int NB;

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

template<typename T>
vector<T> get_tile(int M, int N, T *A, int LDA) {
	vector<T> tile(M*N);
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {	
			tile[i+j*M] = A[i+j*LDA];
		}
	}
	return tile;
}

template<typename T>
void copy_tile(int M, int N, vector<T> tile, T *A, int LDA) {
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			A[i+j*LDA] = tile[i+j*M];
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
								      vector<int>	IPIV	)
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

class dssssm_action {
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

		int TILES = M/NB;
		int m = M/TILES;
		int n = N/TILES;
		int LDIPIV = NB*m;
 		vector<future<dgessm_RV>> dgessm_tiles(TILES*TILES);
		vector<shared_future<dssssm_RV>> dssssm_tiles(TILES*TILES);
    for(int k=0; k < TILES; k++) {
        int info;
				/*
				cout << "dgetrf input:" << endl;
				cout << "A"<<k<<k<<":" << endl;
				print_array(m, n, &A[k*m+k*n*LDA], LDA);
				cout << "IPIV"<<k<<k<<":" << endl;
				print_array(m, 1, &IPIV[k*m+k*LDIPIV], LDIPIV);
				*/
				if(k != 0) { 
					//unless we are in the firts iteration, we need to get updated block
					//from dssssm
					copy_tile(m, n, dssssm_tiles[k+k*TILES].get().Amn, &A[k*m+k*n*LDA], LDA);
				}
        core_dgetrf(m, n, &A[k*m+k*n*LDA], 
                    LDA, &IPIV[k*m+k*LDIPIV], &info);
				/*
				cout << "dgetrf output:" << endl;
				cout << "A"<<k<<k<<":" << endl;
				print_array(m, n, &A[k*m+k*n*LDA], LDA);
				cout << "IPIV"<<k<<k<<":" << endl;
				print_array(m, 1, &IPIV[k*m+k*LDIPIV], LDIPIV);
				*/
        if(info != 0)
            std::cout << "dgetrf failed with " << info << "." << std::endl;
        //second step
        for(int nn = k+1; nn < TILES; nn++) {
						vector<double> Akk = get_tile(m, n, &A[k*m+k*n*LDA], LDA);
						vector<double> Akn;
						if(k != 0) {
							//unless we are in the first row of the matrix, we need to get
							//the updated values from dssssm
							Akn = dssssm_tiles[k+nn*TILES].get().Amn;
						}	
						else {
							Akn = get_tile(m, n, &A[k*m+nn*n*LDA], LDA);
						}	
						vector<int> IPIVkk = get_tile(m, 1, &IPIV[k*m+k*LDIPIV], LDA);
						/*
						cout << "dgessm input:" << endl;
						cout << "A"<<k<<k<<":" << endl;
						print_array(m, n, &Akk[0], m);
						cout << "A"<<k<<nn<<":" << endl;
						print_array(m, n, &Akn[0], m);
						cout << "IPIV"<<k<<k<<":" << endl;
						print_array(m, 1, &IPIVkk[0], m);
						*/
						dgessm_action dgessm;
						dgessm_tiles[k+nn*TILES] = async(dgessm, m, n, m, n, IPIVkk, Akk, m, Akn, m);
						/*
						cout << "dgessm output:" << endl;
						cout << "A"<<k<<nn<<":" << endl;
						print_array(m, n, &A[k*m+nn*n*LDA], m);
						*/
				}
        for(int mm = k+1; mm < TILES; mm++) {
						int info;
						if(k != 0) {
							//unless we are in the first column of the matrix, we need to get
							//the updated values from dssssm
							copy_tile(m, n, dssssm_tiles[mm+k*TILES].get().Amn, &A[mm*m+k*n*LDA], LDA);
						}
						vector<double> work(m*n);
						/*
						cout << "dtstrf input:" << endl;
						cout << "A"<<k<<k<<":" << endl;
						print_array(m, n, &Akk[0], m);
						cout << "A"<<mm<<k<<":" << endl;
						print_array(m, n, &Amk[0], m);
						cout << "L"<<mm<<k<<":" << endl;
						print_array(m, n, &Lmk[0], m);
						cout << "IPIV"<<mm<<k<<":" << endl;
						print_array(m, 1, &IPIVmk[0], m);
						*/
						core_dtstrf(m, n, m, n, &A[k*m+k*n*LDA], LDA, &A[mm*m+k*n*LDA], LDA, 
												&L[mm*m+k*n*LDA], LDA, &IPIV[mm*m+k*LDIPIV], 
												&work[0], m, &info);
						/*
						cout << "dtstrf output:" << endl;
						cout << "A"<<k<<k<<":" << endl;
						print_array(m, n, &A[k*m+k*n*LDA], LDA);
						cout << "A"<<mm<<k<<":" << endl;
						print_array(m, n, &A[mm*m+k*n*LDA], LDA);
						cout << "L"<<mm<<k<<":" << endl;
						print_array(m, n, &L[mm*m+k*n*LDA], LDA);
						cout << "IPIV"<<mm<<k<<":" << endl;
						print_array(m, 1, &IPIV[mm*m+k*LDIPIV], LDIPIV);
						*/
           	if(info != 0)
                std::cout << "dtstrf failed with " << info << "." << std::endl;

            for(int nn=k+1; nn < TILES; nn++) {
								/*
								cout << "dssssm input:" << endl;
								print_array(M, N, &A[0], LDA);
								cout << "A"<<k<<nn<<":" << endl;
								print_array(m, n, &A[k*m+nn*n*LDA], LDA);
								cout << "A"<<mm<<nn<<":" << endl;
								print_array(m, n, &A[mm*m+nn*n*LDA], LDA);
								cout << "L"<<mm<<k<<":" << endl;
								print_array(m, n, &L[mm*m+k*n*LDA], LDA);
								cout << "A"<<mm<<k<<":" << endl;
								print_array(m, n, &A[mm*m+k*n*LDA], LDA);
								cout << "IPIV"<<mm<<k<<":" << endl;
								print_array(m, 1, &IPIV[mm*m+k*LDIPIV], LDIPIV);
								*/
								vector<double> Akn;
								if(mm == k+1) {
									//if we are on the top of the column get dgessm result
									Akn = dgessm_tiles[k+nn*TILES].get().Akn;
								}
								else { 
									//else get the value of dssssm applied on the above tile
									Akn = dssssm_tiles[(mm-1)+nn*TILES].get().Akn;
								}
								vector<double> Amn;		
								if(k != 0) {
									//unless we are in the first iteration, we need to get
									//the updated values from dssssm from previous iteration
									Amn = dssssm_tiles[mm+nn*TILES].get().Amn;
								}	
								else {
									Amn = get_tile(m, n, &A[mm*m+nn*n*LDA], LDA);
								}	
								vector<double> Lmk = get_tile(m, n, &L[mm*m+k*n*LDA], LDA);
								vector<double> Amk = get_tile(m, n, &A[mm*m+k*n*LDA], LDA);
								vector<int> IPIVmk = get_tile(m, 1, &IPIV[mm*m+k*LDIPIV], LDIPIV);
								dssssm_action dssssm;
								dssssm_tiles[mm+nn*TILES] = async(dssssm, m, n, m, n, m, m, 
																									Akn, m, Amn, m,
																									Lmk, m, Amk, m, IPIVmk);
								if(mm == TILES-1) 
									//if we reach bottom, update tile of gdessm step
									copy_tile(m, n, dssssm_tiles[mm+nn*TILES].get().Akn, &A[k*m+nn*n*LDA], LDA);
								/*
								cout << "dssssm output:" << endl;
								cout << "A"<<k<<nn<<":" << endl;
								print_array(m, n, &A[k*m+nn*n*LDA], LDA);
								cout << "A"<<mm<<nn<<":" << endl;
								print_array(m, n, &A[mm*m+nn*n*LDA], LDA);
								*/
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

		SIZE = DEFAULT_SIZE;
		NB = DEFAULT_NB;
		char c;

		while ((c = getopt(argc, argv, "n:b:")) != -1)
		switch (c)	{
			case 'n':
				SIZE = atoi(optarg); 
			case 'b':
				NB = atoi(optarg);	 
			 	break;
		 	default:
				break;		
		}

    int cores = 1;
    int N     = SIZE;
    int LDA   = N;
    int info;
    int LDAxN = LDA*N;
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
		int tiles = N/DEFAULT_NB;
		int m = N/tiles;
		int n = N/tiles;
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

