

#include <iostream>
#include <iomanip>
//#include "../timer.hpp"
#include "matrix.hpp"
#include "kernels.hpp"
#include <mkl_lapacke.h>
#include <mkl_lapack.h>

#include "futures.hpp"

#define DEFAULT_NB 4
#define DEFAULT_IB 1
#define DEFAULT_SIZE 8

#define MAX(a, b) (a >= b):a?b
#define MIN(a, b) (a <= b):a?b

int SIZE;
int NB;
int IB;

using namespace std;

template<typename T>
void print_array(int M, int N, Matrix<T>& A) {
	std::cout << "----------" << std::endl;
	for(int i=0; i < M; i++) {
		for(int j=0; j < N; j++) {
			std::cout << std::setw(16) << A(i,j);
		}
		std::cout << std::endl;
	}
	std::cout << "----------" << std::endl;
}

template<typename T>
Matrix<Matrix<T>> format_array(int M, int N, int NB, Matrix<T>& A, int LDA) {
	int m = (M%NB==0) ? (M/NB) : ((M/NB)+1);
  int n = (N%NB==0) ? (N/NB) : ((N/NB)+1);
	int m_mod = M%NB;
	int n_mod = N%NB;
	Matrix<Matrix<T>> tiles(m, n);
	for(int i=0; i < m; i++) {
		for(int j=0; j < n; j++) {
			int block_m = NB;
			int block_n = NB;
			int ld = NB;
			if(m_mod != 0)
				if(i == m-1)
					block_m = m_mod;
			if(n_mod != 0)
				if(j == n-1)
					block_n = n_mod;
			tiles(i, j) = Matrix<T>(block_m, block_n);
			tiles(i, j).height = block_m;
			tiles(i, j).width = block_n;
			for(int ii=0; ii < block_m; ii++) {
				for(int jj=0; jj < block_n; jj++) {
					tiles(i, j)(ii, jj) = (&A(0,0))[(i+j*LDA)*NB+(ii+jj*LDA)];
				}
			}
		}
	}
	return tiles;
}

template<typename T>
void deformat_array(int M, int N, int NB, Matrix<T>& A, Matrix<Matrix<T>>& tiles, int LDA) {
	int m = (M%NB==0) ? (M/NB) : ((M/NB)+1);
  int n = (N%NB==0) ? (N/NB) : ((N/NB)+1);
	int m_mod = M%NB;
	int n_mod = N%NB;
	for(int i=0; i < m; i++) {
		for(int j=0; j < n; j++) {
			int block_m = NB;
			int block_n = NB;
			int ld = NB;
			if(m_mod != 0)
				if(i == m-1)
					block_m = m_mod;
			if(n_mod != 0)
				if(j == n-1)
					block_n = n_mod;	
			for(int ii=0; ii < block_m; ii++) {
				for(int jj=0; jj < block_n; jj++) {
					(&A(0,0))[(i+j*LDA)*NB+(ii+jj*LDA)] = tiles(i, j)(ii, jj);
				}
			}
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
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & Akn;
  }
public:
	Matrix<double> Akn;
};

class dgessm_action {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & M;
			ar & N;
			ar & K;
			ar & IB;
      ar & IPIV;
      ar & L;
			ar & LDL;
			ar & A;
			ar & LDA;
  }
	int M;
	int  	N;
	int  	K;
	int  	IB;
	vector<int> IPIV;
	Matrix<double> L;
	int  	LDL;
	Matrix<double> A;
	int  	LDA;
public:
	dgessm_action() {};
	dgessm_action(int _M,
								int _N,
								int _K,
								int _IB,
								vector<int>& _IPIV,
								Matrix<double>& _L,
								int _LDL,
								Matrix<double>& _A,
								int _LDA)
	{
		M = _M;
		N = _N;
		K = _K;
		IB = _IB;
		IPIV = _IPIV;
		L = _L;
		LDL = _LDL;
		A = _A;
		LDA = _LDA;
	}

	dgessm_RV operator()()
	{
		core_dgessm(M, N, K, IB, &IPIV[0], &L(0,0), LDL, &A(0,0), LDA);
		dgessm_RV retVal;
		retVal.Akn = A;
		return retVal;	
	}
};

FUTURES_EXPORT_FUNCTOR((futures::async_function<dgessm_action>));

struct dtstrf_RV {
	Matrix<double> Akk;
	Matrix<double> Amk;
	Matrix<double> Lmk;
	vector<int> 	 IPIVmk;
	int info;
};

class dtstrf_action {
public:
	dtstrf_RV operator()(int  	M,
						        	int  	N,
						        	int  	IB,
						        	int  	NB,
								      Matrix<double> U,
								      int  	LDU,
								      Matrix<double> A,
								      int  	LDA,
								      Matrix<double> L,
								      int  	LDL,
								      vector<int>	IPIV	)
	{
		vector<double> work(M*N);
		int info;
		core_dtstrf(M, N, IB, NB, &U(0,0), LDU, &A(0,0), LDA,
		            &L(0,0), LDL, &IPIV[0], &work[0], M, &info);
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
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & Akn;
			ar & Amn;
  }
public:
	Matrix<double> Akn;
	Matrix<double> Amn;
};

class dssssm_action {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & M1;
			ar & N1;
			ar & M2;
			ar & N2;
			ar & K;
			ar & IB;
			ar & A1;
			ar & LDA1;
			ar & A2;
			ar & LDA2;
      ar & L1;
			ar & LDL1;
      ar & L2;
			ar & LDL2;
      ar & IPIV;
  }
	int  M1;
	int  N1;
	int  M2;
	int  N2;
	int  K;
	int  IB;
	Matrix<double> A1;
	int  LDA1;
	Matrix<double> A2;
	int  LDA2;
	Matrix<double> L1;
	int  	LDL1;
	Matrix<double> L2;
	int  	LDL2;
	vector<int>	IPIV;
public:
	dssssm_action() {};
	dssssm_action(int  _M1,
								int  _N1,
								int  _M2,
								int  _N2,
								int  _K,
								int  _IB,
								Matrix<double>& _A1,
								int  _LDA1,
								Matrix<double>& _A2,
								int  _LDA2,
								Matrix<double>& _L1,
								int  	_LDL1,
								Matrix<double>& _L2,
								int  	_LDL2,
								vector<int>&	_IPIV) 
	{
		M1 = _M1;
		N1 = _N1;
		M2 = _M2;
		N2 = _N2;
		K = _K;
		IB = _IB;
		A1 = _A1;
		LDA1 = _LDA1;
		A2 = _A2;
		LDA2 = _LDA2;
		L1 = _L1;
		LDL1 = _LDL1;
		L2 = _L2;
		LDL2 = _LDL2;
		IPIV = _IPIV;	
	}
	~dssssm_action() {
/*
		cout << "deleting dssssm_action" << endl;
		A1.~Matrix<double>();
		cout << "deleted A1" << endl;
		A2.~Matrix<double>();
		cout << "deleted A2" << endl;
		L1.~Matrix<double>();
		cout << "deleted L1" << endl;
		IPIV.~vector<int>();
		cout << "deleted all" << endl;
*/
	};

	dssssm_RV operator()()
	{
		core_dssssm(M1, N1, M2, N2, K, IB, 
								&A1(0,0), LDA1, &A2(0,0), LDA2,
		            &L1(0,0), LDL1, &L2(0,0), LDL2, &IPIV[0]);
		dssssm_RV retVal;
		retVal.Akn = A1;
		retVal.Amn = A2;
		return retVal;
	}
};

FUTURES_EXPORT_FUNCTOR((futures::async_function<dssssm_action>));

int dgetrf(int M, int N, Matrix<Matrix<double>>& A, int LDA, 
					Matrix<Matrix<double>>& L, Matrix<vector<int>>& IPIV) {

		int TILES = (M%NB==0) ? (M/NB) : ((M/NB)+1);
		int LDIPIV = LDA;
 		Matrix<futures::future<dgessm_RV>> dgessm_tiles(TILES,TILES);
		Matrix<futures::future<dssssm_RV>> dssssm_tiles(TILES,TILES);
    for(int k=0; k < TILES; k++) {
        int info;		
				if(k != 0) { 
					//unless we are in the first iteration, we need to get updated block
					//from dssssm
					//copy_tile(m, n, dssssm_tiles[k+k*TILES].get().Amn, &A[k*m+k*n*LDA], LDA);
					A(k,k) = dssssm_tiles(k,k).get().Amn;
					/*we also need to update the dgessm tile of the previous iteration, as it 
						is altered by the application of dssssm on the last row of the submatrix
						NOTE: this is not needed by the dgetrf step, but we need it to update the array
						for the final result
					*/				
					A(k-1, k) = dssssm_tiles(TILES-1, k).get().Akn;
				}
#if 0
				cout << "dgetrf input:" << endl;
				cout << "A"<<k<<k<<":" << endl;
				print_array(m, n, &A[k+k*TILES][0], m);
				cout << "IPIV"<<k<<k<<":" << endl;
				cout << "IPIV.size="<<IPIV.size()<<endl;
				cout << "indexing IPIV["<<k+k*TILES<<"]"<<endl;
				print_array(m, 1, &IPIV[k+k*TILES][0], m);
#endif
        core_dgetrf(A(k,k).height, A(k,k).width, &A(k,k)(0,0), 
                    A(k,k).height, &IPIV(k,k)[0], &info);
#if 0
				cout << "dgetrf output:" << endl;
				cout << "A"<<k<<k<<":" << endl;
				print_array(m, n, &A[k+k*TILES][0], m);
				cout << "IPIV"<<k<<k<<":" << endl;
				print_array(m, 1, &IPIV[k+k*TILES][0], m);
#endif				
        if(info != 0)
            std::cout << "dgetrf failed with " << info << "." << std::endl;
        //second step
        for(int nn = k+1; nn < TILES; nn++) {
						if(k != 0) {
							//unless we are in the first row of the matrix, we need to get
							//the updated values from dssssm
							A(k,nn) = dssssm_tiles(k,nn).get().Amn;
							/*
								we also need to update the dgessm tile of the previous iteration, as it 
								is altered by the application of dssssm on the last row of the submatrix
								NOTE: this is not needed by the dgetrf step, but we need it to update the array
								for the final result
							*/
							A(k-1, nn) = dssssm_tiles(TILES-1, nn).get().Akn;	
						}
#if 0						
						cout << "dgessm input:" << endl;
						cout << "A"<<k<<k<<":" << endl;
						print_array(m, n, &A[k+k*TILES][0], m);
						cout << "A"<<k<<nn<<":" << endl;
						print_array(m, n, &A[k+nn*TILES][0], m);
						cout << "IPIV"<<k<<k<<":" << endl;
						print_array(m, 1, &IPIV[k+k*TILES][0], m);
#endif						
						dgessm_action dgessm(	A(k,nn).height, A(k,nn).width, A(k,nn).height, A(k,nn).height, 
																	IPIV(k,k), A(k,k), A(k,k).width,
																	A(k,nn), A(k,nn).height);
						dgessm_tiles(k,nn) = futures::async2(A(k,nn).height*A(k,nn).width, dgessm);
#if 0						
						cout << "dgessm output:" << endl;
						cout << "A"<<k<<nn<<":" << endl;
						print_array(m, n, &A[k+nn*TILES][0], m);
#endif						
				}
        for(int mm = k+1; mm < TILES; mm++) {
						int info;
						if(k != 0) {
							//unless we are in the first column of the matrix, we need to get
							//the updated values from dssssm
							A(mm,k) = dssssm_tiles(mm,k).get().Amn;
						}
						vector<double> work(A(mm,k).height*A(mm,k).width);
#if 0						
						cout << "dtstrf input:" << endl;
						cout << "A"<<k<<k<<":" << endl;
						print_array(m, n, &A[k+k*TILES][0], m);
						cout << "A"<<mm<<k<<":" << endl;
						print_array(m, n, &A[mm+k*TILES][0], m);
						cout << "L"<<mm<<k<<":" << endl;
						print_array(m, n, &L[mm+k*TILES][0], m);
						cout << "IPIV"<<mm<<k<<":" << endl;
						print_array(m, 1, &IPIV[mm+k*TILES][0], m);
#endif						
						core_dtstrf(A(mm,k).height, A(mm,k).width, A(mm,k).height, A(mm,k).height, 
												&A(k,k)(0,0), A(k,k).height, &A(mm,k)(0,0), A(mm,k).height, 
												&L(mm,k)(0,0), L(mm,k).height, &IPIV(mm,k)[0], 
												&work[0], A(mm,k).height, &info);
#if 0						
						cout << "dtstrf output:" << endl;
						cout << "A"<<k<<k<<":" << endl;
						print_array(m, n, &A[k+k*TILES][0], m);
						cout << "A"<<mm<<k<<":" << endl;
						print_array(m, n, &A[mm+k*TILES][0], m);
						cout << "L"<<mm<<k<<":" << endl;
						print_array(m, n, &L[mm+k*TILES][0], m);
						cout << "IPIV"<<mm<<k<<":" << endl;
						print_array(m, 1, &IPIV[mm+k*TILES][0], m);
#endif						
           	if(info != 0)
                std::cout << "dtstrf failed with " << info << "." << std::endl;

            for(int nn=k+1; nn < TILES; nn++) {
#if 0								
								cout << "dssssm input:" << endl;
								cout << "A"<<k<<nn<<":" << endl;
								print_array(m, n, &A[k+nn*TILES][0], m);
								cout << "A"<<mm<<nn<<":" << endl;
								print_array(m, n, &A[mm+nn*TILES][0], m);
								cout << "L"<<mm<<k<<":" << endl;
								print_array(m, n, &L[mm+k*TILES][0], m);
								cout << "A"<<mm<<k<<":" << endl;
								print_array(m, n, &A[mm+k*TILES][0], m);
								cout << "IPIV"<<mm<<k<<":" << endl;
								print_array(m, 1, &IPIV[mm+k*TILES][0], m);
#endif								
								if(mm == k+1) {
									//if we are on the top of the column get dgessm result
									A(k,nn) = dgessm_tiles(k,nn).get().Akn;
								}
								else { 
									//else get the value of dssssm applied on the above tile
									A(k,nn) = dssssm_tiles((mm-1),nn).get().Akn;
								}		
								if(k != 0) {
									//unless we are in the first iteration, we need to get
									//the updated values from dssssm from previous iteration
									A(mm,nn) = dssssm_tiles(mm,nn).get().Amn;
								}	
								dssssm_action dssssm(	A(k,nn).height, A(k,nn).width, A(mm,nn).height, A(mm,nn).width,
																			A(k,nn).height, A(mm,k).height, 
																			A(k,nn), A(k,nn).height, A(mm,nn), A(mm,nn).height,
																			L(mm,k), L(mm,k).height, A(mm,k), A(mm,k).height, 
																			IPIV(mm,k));
								dssssm_tiles(mm,nn) = futures::async2(NB*NB*2, dssssm);
								//if(mm == TILES-1)
									//if we reach bottom, update tile of gdessm step
									//A(k,nn) = dssssm_tiles(mm,nn).get().Akn;
#if 0								
								cout << "dssssm output:" << endl;
								cout << "A"<<k<<nn<<":" << endl;
								print_array(m, n, &A[k+nn*TILES][0], m);
								cout << "A"<<mm<<nn<<":" << endl;
								print_array(m, n, &A[mm+nn*TILES][0], m);
#endif						
            }
        }
    }
    return 0;
};

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */

Matrix<vector<int>> init_ipiv(int M, int N, int NB) {
  int MT = (M%NB==0) ? (M/NB) : ((M/NB)+1);
	int NT = (N%NB==0) ? (N/NB) : ((N/NB)+1);
  Matrix<vector<int>> ipiv(MT, NT);
	for(int i=0; i < MT; i++) {
		for(int j=0; j < NT; j++) {
			ipiv(i,j) = vector<int>(NB);
		}
	}
	return ipiv;
};

int main(int argc, char* argv[]) {

		SIZE = DEFAULT_SIZE;
		IB = NB = DEFAULT_NB;
		bool print_result = false;
		char c;

		while ((c = getopt(argc, argv, "n:b:v")) != -1)
		switch (c)	{
			case 'n':
				SIZE = atoi(optarg); 
			case 'b':
				IB = NB = atoi(optarg);	 
			 	break;
			case 'v':
				print_result = true;
				break;
		 	default:
				break;		
		}

		futures::Futures_Initialize(argc, argv);

    int N     = SIZE;
    int LDA   = N;
    int info;
    int LDAxN = LDA*N;

		Matrix<double> A(LDA, N);
		Matrix<double> L(LDA, N);
    /* Initialize A Matrix */
    dlarnv(&IONE, ISEED, &LDAxN, &A(0,0));
		/* Format arrays in tiles*/
		
		int tiles = N/NB;
		int m = N/NB;
		int n = N/NB;
		int m_mod = N%NB;
		if(m_mod != 0) {
			m++;
			n++;
			IB = m_mod; //just use the smaller size of elements in a block						
		}
		//print_array(N, N, A);
		Matrix<Matrix<double>> tiled_A = format_array(N, N, NB, A, LDA);
		Matrix<Matrix<double>> tiled_L = format_array(N, N, NB, L, LDA);
		Matrix<vector<int>> IPIV = init_ipiv(N, N, NB);
		//run LU factorization

		REGISTER_TIMER("total time");
		START_TIMER("total time");
    info = dgetrf(N, N, tiled_A, LDA, tiled_L, IPIV);
		STOP_TIMER("total time");
		cout << "LU completed!" << endl;
		if(print_result) {
			deformat_array(N, N, NB, A, tiled_A, LDA);
			print_array(N, N, A);
		}
		PRINT_TIMER("total time");
		futures::Futures_Finalize();

    return 0;
}

