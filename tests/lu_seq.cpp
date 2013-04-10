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
dgessm_tiles[k+nn*TILES] = async(dgessm, m, n, m, n, IPIV[k+k*TILES],
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
dssssm_tiles[mm+nn*TILES] = async(dssssm, m, n, m, n, m, m,
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
