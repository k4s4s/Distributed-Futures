#if 1
#include <iostream>
#include <iomanip>

#include "matrix.hpp"
#include "kernels.hpp"

#include <mkl_lapack.h>
#include <mkl_lapacke.h>

#include <future>

#include "timer.hpp"


#define DEFAULT_NB 4
#define DEFAULT_SIZE 8

int SIZE;
int NB;

using namespace std;

typedef shared_future<void>  TileFuture;

struct dgetrf_f
{ 
   dgetrf_f(Matrix<double> & A_, 
		int m_,	 
		int n_, 
		int k_, 		
		int LDA_,
		std::vector<int> & IPIV_,
		TileFuture & dep_
		)
  :A(A_),m(m_),n(n_),k(k_),LDA(LDA_),IPIV(IPIV_),dep(dep_)
  {}
     
  void operator()()
  {
     dep.get(); 
      
     //std::cout<<hpx::get_worker_thread_num()<<std::endl;
     core_dgetrf(m, n, &A(k*m,k*n), LDA, &IPIV[k*m+k*LDA], &info);
  }
  
  Matrix<double> & A;
  int m,n,k,info,LDA;  
  std::vector<int> & IPIV;
  TileFuture & dep;
};

struct dtstrf_f
{
   dtstrf_f(    Matrix<double> & A_, 
		Matrix<double> & L_, 
		int m_,	 
		int n_, 
		int k_,
		int mm_, 		
		int LDA_,		
		std::vector<int> & IPIV_,
		TileFuture & dep1_,
		TileFuture & dep2_
		)
  :A(A_),L(L_),m(m_),n(n_),k(k_),mm(mm_),LDA(LDA_),
   IPIV(IPIV_),work(m_*n_),dep1(dep1_),dep2(dep2_)
  {}
   
  
  void operator()()
  {    
     dep1.get();
     dep2.get();
     
     //std::cout<<hpx::get_worker_thread_num()<<std::endl;
      
     core_dtstrf(m, n, m, n, &A(k*m,k*n), LDA, &A(mm*m,k*n), LDA,
	      &L(mm*m,k*n), n, &IPIV[mm*m+k*LDA], &work[0], n, &info);
  }
  
  Matrix<double> & A, & L;
  int m,n,k,mm,info,LDA;  
  std::vector<int> & IPIV;
  std::vector<double> work;
  TileFuture & dep1, & dep2;
};

struct dgessm_f
{ 
   dgessm_f(    Matrix<double> & A_, 
		Matrix<double> & L_, 
		int m_,	 
		int n_, 
		int k_,
		int nn_, 		
		int LDA_,
		std::vector<int> & IPIV_,
		TileFuture & dep1_,
		TileFuture & dep2_
		)
  :A(A_),L(L_),m(m_),n(n_),k(k_),nn(nn_),LDA(LDA_),
   IPIV(IPIV_),dep1(dep1_),dep2(dep2_)
  {}
   
  
  void operator()()
  {    
    dep1.get();
    dep2.get();
      
    //std::cout<<hpx::get_worker_thread_num()<<std::endl;
    
    core_dgessm(m, n, m, n, &IPIV[k*m+k*LDA], &A(k*m,k*n), LDA,
		&A(k*m,nn*n), LDA);
  }
  
  Matrix<double> & A, & L;
  int m,n,k,nn,LDA;  
  std::vector<int> & IPIV;  
  TileFuture & dep1,& dep2;
};


struct dssssm_f
{ 
   dssssm_f(    Matrix<double> & A_, 
		Matrix<double> & L_, 
		int m_,	 
		int n_, 
		int k_,
		int mm_,
		int nn_, 		
		int LDA_,
		std::vector<int> & IPIV_,
		TileFuture & dep1_,
		TileFuture & dep2_,
		TileFuture & dep3_
		)
  :A(A_),L(L_),m(m_),n(n_),k(k_),mm(mm_),nn(nn_),LDA(LDA_),IPIV(IPIV_),
   dep1(dep1_),dep2(dep2_),dep3(dep3_)
  {}
   
  
  void operator()()
  {    
    dep1.get();	    
    dep2.get();
    dep3.get();
      
    //std::cout<<hpx::get_worker_thread_num()<<std::endl;
    
    core_dssssm(m, n, m, n, m, m, &A(k*m,nn*n), LDA, &A(mm*m,nn*n), LDA,
		    &L(mm*m,k*n), m, &A(mm*m,k*n), LDA, &IPIV[mm*m+k*LDA]);
  }
  
  Matrix<double> & A, & L;
  int m,n,k,mm,nn,LDA;  
  std::vector<int> & IPIV;
  TileFuture & dep1, & dep2, & dep3;
};

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

int dgetrf(int M, int N, Matrix<double>& A, int LDA, Matrix<double>& L, std::vector<int>& IPIV) { 

    int TILES = M/NB;
    int m = M/TILES;
    int n = N/TILES;

    int src(0), dst(1);
		    
    std::vector< Matrix<TileFuture> > Tiles;
    //Tiles.reserve(M+1);
		Matrix<TileFuture> mat(TILES+1,TILES+1);
		vector<promise<void>> init((TILES+1)*(TILES+1));
		for(int i=0; i < TILES+1; i++) {
			for(int j=0; j < TILES+1; j++) {
				mat(i, j) = init[i+j*(TILES+1)].get_future();
				init[i+j*(TILES+1)].set_value();
			}
		}
    Tiles.push_back(mat);

    for(int k=0; k <TILES; k++)      
    Tiles.push_back(Matrix<TileFuture>(TILES-k,TILES-k));

    for(int k=0; k < TILES; k++) {	

    //step 1
		Tiles[dst](0,0) = async(launch::async, dgetrf_f(A,m,n,k,LDA,IPIV,
																					Tiles[src](1,1)));
	
        //step 2	
        for(int mm = k+1; mm < TILES; mm++) {

		      Tiles[dst](mm-k,0) = async(launch::async, dtstrf_f(A,L,m,n,k,mm,LDA,IPIV,
		                                          			Tiles[dst](mm-k-1,0),
		                                          			Tiles[src](mm-k+1,1)));
        }

        //step 3
        for(int nn = k+1; nn < TILES; nn++) {

			    Tiles[dst](0,nn-k) = async(launch::async, dgessm_f(A,L,m,n,k,nn,LDA,IPIV,
                                              			Tiles[dst](0,0),
                                              			Tiles[src](1,nn-k+1)));
        }

        //step 4
        for(int nn = k+1; nn < TILES; nn++) {	   
                for(int mm=k+1; mm < TILES; mm++) {
                  
                    Tiles[dst](mm-k,nn-k) = async(launch::async, dssssm_f(A,L,m,n,k,mm,nn,LDA,IPIV,
                                                           			Tiles[dst](mm-k,0),
                                                           			Tiles[dst](mm-k-1,nn-k),
                                                           			Tiles[src](mm-k+1,nn-k+1)));  
                }
        }

        src ++;
        dst ++;
      }
    
    Tiles[src](0,0).get();
    
    return 0;
};

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */

int main(int argc, char* argv[]){

		SIZE = DEFAULT_SIZE;
		NB = DEFAULT_NB;
		bool print_result = false;
		char c;

		while ((c = getopt(argc, argv, "n:b:v")) != -1)
		switch (c)	{
			case 'n':
				SIZE = atoi(optarg); 
			case 'b':
				NB = atoi(optarg);	 
			 	break;
			case 'v':
				print_result = true;
				break;
		 	default:
				break;		
		}

    int N     = SIZE;
    int LDA   = N;
    int info;
    int LDAxN = LDA*N;

    Matrix<double> A(LDA,N);
    Matrix<double> L(LDA,N);
    std::vector<int> IPIV(N*LDA*DEFAULT_NB);

    /* Initialize A Matrix */
    dlarnv(&IONE, ISEED, &LDAxN, &A(0,0));

    //print_matrix(N, N, &A(0,0), N);

    std::cout << "Running LU factorization" << std::endl;
		start_timer();
    info = dgetrf(N, N, A, LDA, L, IPIV);
    stop_timer();
		if(print_result)
    	print_matrix(N, N, &A(0,0), N);

    /* Solve the problem */
    std::cout << "LU completed!" <<std::endl;
		print_timer();

}
#endif
/***** OLD LU IMPLEMENTATION *****/
#if 0

#include <iostream>
#include <iomanip>

#include "timer.hpp"
#include "matrix.hpp"
#include "kernels.hpp"
#include <mkl_lapacke.h>
#include <mkl_lapack.h>

#include <future>

#define LU_DEBUG 0

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

template<typename T>
vector<vector<T>> format_array(int M, int N, int m, int n, T* A, int LDA) {
vector<vector<T>> tiles(M*N);
for(int i=0; i < M; i++) {
for(int j=0; j < N; j++) {
tiles[i+j*M] = vector<T>(n*m);
for(int ii=0; ii < m; ii++) {
for(int jj=0; jj < n; jj++) {
tiles[i+j*M][ii+jj*m] = A[(i+j*LDA)*m+(ii+jj*LDA)];
}
}
}
}
return tiles;
}

template<typename T>
void deformat_array(int M, int N, int m, int n, vector<T>& A, vector<vector<T>>& tiles, int LDA) {
for(int i=0; i < M; i++) {
for(int j=0; j < N; j++) {
for(int ii=0; ii < m; ii++) {
for(int jj=0; jj < n; jj++) {
A[((i+j*LDA))*m+(ii+jj*LDA)] = tiles[i+j*M][ii+jj*m];
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
vector<double> Akn;
};

class dgessm_action {
public:
dgessm_RV operator()(int M,
int N,
int K,
int IB,
vector<int> IPIV,
vector<double> L,
int LDL,
vector<double> A,
int LDA )
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
vector<int> IPIVmk;
int info;
};

class dtstrf_action {
public:
dtstrf_RV operator()(int M,
int N,
int IB,
int NB,
vector<double> U,
int LDU,
vector<double> A,
int LDA,
vector<double> L,
int LDL,
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
dssssm_RV operator()(int M1,
int N1,
int M2,
int N2,
int K,
int IB,
vector<double> A1,
int LDA1,
vector<double> A2,
int LDA2,
vector<double> L1,
int LDL1,
vector<double> L2,
int LDL2,
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

int dgetrf(int M, int N, vector<vector<double>>& A, int LDA,
vector<vector<double>>& L, vector<vector<int>>& IPIV) {

int TILES = M/NB;
int m = M/TILES;
int n = N/TILES;
int LDIPIV = NB*m;
  vector<future<dgessm_RV>> dgessm_tiles(TILES*TILES);
vector<shared_future<dssssm_RV>> dssssm_tiles(TILES*TILES);
    for(int k=0; k < TILES; k++) {
        int info;

if(k != 0) {
//unless we are in the firts iteration, we need to get updated block
//from dssssm
//copy_tile(m, n, dssssm_tiles[k+k*TILES].get().Amn, &A[k*m+k*n*LDA], LDA);
A[k+k*TILES] = dssssm_tiles[k+k*TILES].get().Amn;
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
        core_dgetrf(m, n, &A[k+k*TILES][0],
                    m, &IPIV[k+k*TILES][0], &info);
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
A[k+nn*TILES] = dssssm_tiles[k+nn*TILES].get().Amn;
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
dgessm_action dgessm;
dgessm_tiles[k+nn*TILES] = async(launch::async, dgessm, m, n, m, n, IPIV[k+k*TILES],
A[k+k*TILES], m, A[k+nn*TILES], m);
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
A[mm+k*TILES] = dssssm_tiles[mm+k*TILES].get().Amn;
}
vector<double> work(m*n);
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
core_dtstrf(m, n, m, n, &A[k+k*TILES][0], m, &A[mm+k*TILES][0], m,
&L[mm+k*TILES][0], m, &IPIV[mm+k*TILES][0],
&work[0], m, &info);
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
A[k+nn*TILES] = dgessm_tiles[k+nn*TILES].get().Akn;
}
else {
//else get the value of dssssm applied on the above tile
A[k+nn*TILES] = dssssm_tiles[(mm-1)+nn*TILES].get().Akn;
}	
if(k != 0) {
//unless we are in the first iteration, we need to get
//the updated values from dssssm from previous iteration
A[mm+nn*TILES] = dssssm_tiles[mm+nn*TILES].get().Amn;
}	
dssssm_action dssssm;
dssssm_tiles[mm+nn*TILES] = async(launch::async, dssssm, m, n, m, n, m, m,
A[k+nn*TILES], m, A[mm+nn*TILES], m,
L[mm+k*TILES], m, A[mm+k*TILES], m,
IPIV[mm+k*TILES]);
if(mm == TILES-1)
//if we reach bottom, update tile of gdessm step
A[k+nn*TILES] = dssssm_tiles[mm+nn*TILES].get().Akn;
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
int ISEED[4] = {0,0,0,1}; /* initial seed for dlarnv() */

vector<vector<int>> init_ipiv(int M, int N, int NB) {
int NT = (N%NB==0) ? (N/NB) : ((N/NB)+1);
  int MT = (M%NB==0) ? (M/NB) : ((M/NB)+1);
  vector<vector<int>> ipiv(NT*MT);
for(int i=0; i < MT; i++) {
for(int j=0; j < NT; j++) {
ipiv[i+j*MT] = vector<int>(NB);
}
}
return ipiv;
};

int main(int argc, char* argv[]) {

SIZE = DEFAULT_SIZE;
NB = DEFAULT_NB;
bool print_result = false;
char c;

while ((c = getopt(argc, argv, "n:b:v")) != -1)
switch (c)	{
case 'n':
SIZE = atoi(optarg);
case 'b':
NB = atoi(optarg);	
break;
case 'v':
print_result = true;
break;
default:
break;	
}

    int N = SIZE;
    int LDA = N;
    int info;
    int LDAxN = LDA*N;

vector<double> A(LDA*N);
vector<double> L(LDA*N);
    /* Initialize A Matrix */
    dlarnv(&IONE, ISEED, &LDAxN, &A[0]);
/* Format arrays in tiles*/
int tiles = N/NB;
int m = N/NB;
int n = N/NB;
vector<vector<double>> tiled_A = format_array(m, n, NB, NB, &A[0], LDA);
vector<vector<double>> tiled_L = format_array(m, n, NB, NB, &L[0], LDA);
vector<vector<int>> IPIV = init_ipiv(N, N, NB);
//print_array(N, N, &A[0], N);
//run LU factorization
start_timer();
    info = dgetrf(N, N, tiled_A, LDA, tiled_L, IPIV);
stop_timer();
cout << "LU completed!" << endl;
if(print_result) {
deformat_array(N/NB, N/NB, NB, NB, A, tiled_A, LDA);
print_array(N, N, &A[0], LDA);
}
print_timer();
    return 0;
}
#endif
