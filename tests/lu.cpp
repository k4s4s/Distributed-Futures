

#include <iostream>
#include <iomanip>

#include <futures.hpp>
#include <ctime>
#include <boost/random.hpp> 

#include "matrix.hpp"
#include "kernels.hpp"
#include "lapacke.h"

#include <future>

#define NB 4
#define SIZE 8 


using namespace std;

boost::random::mt19937 gen; //random generator used for seeding other generators

Matrix<double> get_block(Matrix<double>& A, int row_tile, int col_tile, int bsz) {
	Matrix<double> subMatrix(bsz, bsz);
	int row_offset = bsz*row_tile;
	int col_offset = bsz*col_tile;
	for(int i=0; i < bsz; i++) {
		for(int j=0; j < bsz; j++) {
			subMatrix(i, j) = A(i+row_offset, j+col_offset);
		}
	}
	return subMatrix;
}

void reset_ipiv(vector<int>& ipiv, int size) {
	for(int i=0; i < size; i++)
		ipiv[i] = i+1;	
}

class dgessm {
public:
	dgessm() {};
	~dgessm() {};
	Matrix<double> operator()(int  M,
                						int  	N,
												    int  	K,
												    int  	IB,
												    vector<int> IPIV,
												    Matrix<double> L,
												    int  	LDL,
												    Matrix<double> A,
												    int  	LDA) {
		core_dgessm(K, N, K, IB, &(*IPIV.begin()), &L(0,0), LDL, &A(0,0), LDA);
		return A;
	}
};

FUTURES_SERIALIZE_CLASS(dgessm);
//typedef futures::async_function<dgessm_f, int, int, int, int, int*, double*, int, double*, int> dgessm_f_type;
FUTURES_EXPORT_FUNCTOR((futures::async_function<dgessm, int, int, int, int, 
																vector<int>, Matrix<double>, int, 
																Matrix<double>, int>));

void lu_facorazation(Matrix<Matrix<double> >& A, 
										Matrix<vector<int> >& IPIV,
										Matrix<Matrix<double>>& L,
										int bsz) {
	int TILES = SIZE/bsz;
	for(int k=0; k < TILES; k++) {
		int info;
		core_dgetrf(bsz, bsz, &A(k,k)(0,0),
				   			max(1, bsz), &(*IPIV(k, k).begin()), &info);
		if(info != 0)
			cout << "dgetrf failed with " << info << "." << endl;
		//second step
		for(int m = k+1; m < TILES; m++) {
			//dgessm dgessm_call;
			//dgessm_call(bsz, bsz, bsz, bsz, IPIV(k,k), A(k,k), max(1, bsz), A(k,m), max(1, bsz));
			core_dgessm(bsz, bsz, bsz, bsz, &(*IPIV(k, k).begin()), &A(k,k)(0,0), max(1, bsz), 
									&A(k,m)(0, 0), max(1, bsz));
		}
		for(int n = k+1; n < TILES; n++) {
			Matrix<double> Work(NB, NB); 
			core_dtstrf(bsz, bsz, bsz, bsz, &A(k,k)(0,0), max(1, bsz), &A(n,k)(0,0), max(1, bsz),
             			&L(n,k)(0,0), bsz, &(*IPIV(n, k).begin()), &Work(0,0), bsz, &info);
			if(info != 0)
				cout << "dtstrf failed with " << info << "." << endl;
			for(int m=k+1; m < TILES; m++) {
				core_dssssm(bsz, bsz, bsz, bsz, bsz, bsz, &A(k,m)(0,0), max(1, bsz), &A(n,m)(0,0), max(1, bsz), 
										&L(n,k)(0,0), bsz, &A(n,k)(0,0), max(1, bsz), &(*IPIV(n, k).begin())); 				
			}
 		}
	}
};

void lu_serial(Matrix<double>& A) {
		vector<int> ipiv (A.height);
		//set pivot values for IPIV
		for(int i=0; i < A.height; i++)
			ipiv[i] = i;
		int info;
		core_dgetrf(A.height, A.width, &A(0,0),
				   A.height, &(*ipiv.begin()), &info);
		if(info == 0)
			cout << "lu completed." << endl;		
		else
			cout << "lu failed with " << info << "." << endl;
}

void generate_matrix(Matrix<double>& A) {
	boost::random::uniform_real_distribution<> dist(0, 2);
	for(int i=0; i < A.height; i++) {
		for(int j=0; j < A.width; j++) {
			A(i, j) = dist(gen);		
		}
	}
}

void print_matrix(Matrix<double>& A) {
	cout << "----------" << endl;
	for(int i = 0; i < A.height; i++) {
		for(int j = 0; j < A.width; j++)
			cout << setw(16) << A(i,j);
		cout << endl;	
	}
	cout << "----------" << endl;
}

void print_matrix2(Matrix<Matrix<double>>& A) {
	cout << "----------" << endl;
	for(int i = 0; i < A.height; i++) {
		for(int ii=0; ii < NB; ii++) {
			for(int j = 0; j < A.width; j++) {
				for(int jj=0; jj < NB; jj++) {
					cout << setw(16) << A(i,j)(ii, jj);
				}
			}
			cout << endl;
		}
	}
	cout << "----------" << endl;	
}

void copy_matrix2(Matrix<Matrix<double>>& A, Matrix<double>& B) {
	for(int i = 0; i < A.height; i++) {
		for(int ii=0; ii < NB; ii++) {
			for(int j = 0; j < A.width; j++) {
				for(int jj=0; jj < NB; jj++) {
					B(i*NB+ii, j*NB+jj) = A(i, j)(ii, jj);
				}
			}
		}
	}	
}

int main(int argc, char* argv[]) {
	//Futures_Initialize(argc, argv);
	int size = SIZE/NB;
	Matrix<Matrix<double> > A(size, size);
	Matrix<Matrix<double> > L(size, size);
	Matrix<vector<int> > IPIV(size, size);  
	//fill matrix with random numbers	
	gen.seed(std::time(0));
	//init arrays
	for(int i=0; i < SIZE/NB; i++) {
		for(int j=0; j < SIZE/NB; j++) {
			IPIV(i,j) = vector<int>(NB);
			reset_ipiv(IPIV(i,j), NB);
			L(i, j) = Matrix<double>(NB, NB);
			A(i, j) = Matrix<double>(NB, NB);
			generate_matrix(A(i,j));
		}
	}
	print_matrix2(A);
	lu_facorazation(A, IPIV, L, NB);
	print_matrix2(A);

	//Futures_Finalize();
};

/*
double a[][16] = {
1, 17, 33, 49, 65, 81, 97, 113, 129, 145, 161, 177, 193, 209, 225, 241
2, 18, 34, 50, 66, 82, 98, 114, 130, 146, 162, 178, 194, 210, 226, 242
3, 19, 35, 51, 67, 83, 99, 115, 131, 147, 163, 179, 195, 211, 227, 243
4, 20, 36, 52, 68, 84, 100, 116, 132, 148, 164, 180, 196, 212, 228, 244
5, 21, 37, 53, 69, 85, 101, 117, 133, 149, 165, 181, 197, 213, 229, 245
6, 22, 38, 54, 70, 86, 102, 118, 134, 150, 166, 182, 198, 214, 230, 246
7, 23, 39, 55, 71, 87, 103, 119, 135, 151, 167, 183, 199, 215, 231, 247
8, 24, 40, 56, 72, 88, 104, 120, 136, 152, 168, 184, 200, 216, 232, 248
9, 25, 41, 57, 73, 89, 105, 121, 137, 153, 169, 185, 201, 217, 233, 249
10, 26, 42, 58, 74, 90, 106, 122, 138, 154, 170, 186, 202, 218, 234, 250
11, 27, 43, 59, 75, 91, 107, 123, 139, 155, 171, 187, 203, 219, 235, 251
12, 28, 44, 60, 76, 92, 108, 124, 140, 156, 172, 188, 204, 220, 236, 252
13, 29, 45, 61, 77, 93, 109, 125, 141, 157, 173, 189, 205, 221, 237, 253
14, 30, 46, 62, 78, 94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 254
15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255,
16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256 };
*/

/*
double a00[][8] = {
{1, 17, 33, 49, 65, 81, 97, 113},
{2, 18, 34, 50, 66, 82, 98, 114},
{3, 19, 35, 51, 67, 83, 99, 115},
{4, 20, 36, 52, 68, 84, 100, 116},
{5, 21, 37, 53, 69, 85, 101, 117},
{6, 22, 38, 54, 70, 86, 102, 118},
{7, 23, 39, 55, 71, 87, 103, 119},
{8, 24, 40, 56, 72, 88, 104, 120} };

double a10[][8] = {
{9, 25, 41, 57, 73, 89, 105, 121},
{10, 26, 42, 58, 74, 90, 106, 122},
{11, 27, 43, 59, 75, 91, 107, 123},
{12, 28, 44, 60, 76, 92, 108, 124},
{13, 29, 45, 61, 77, 93, 109, 125},
{14, 30, 46, 62, 78, 94, 110, 126},
{15, 31, 47, 63, 79, 95, 111, 127},
{16, 32, 48, 64, 80, 96, 112, 128} };

double a01[][8] = {
{129, 145, 161, 177, 193, 209, 225, 241},
{130, 146, 162, 178, 194, 210, 226, 242},
{131, 147, 163, 179, 195, 211, 227, 243},
{132, 148, 164, 180, 196, 212, 228, 244},
{133, 149, 165, 181, 197, 213, 229, 245},
{134, 150, 166, 182, 198, 214, 230, 246},
{135, 151, 167, 183, 199, 215, 231, 247},
{136, 152, 168, 184, 200, 216, 232, 248} };

double a11[][8] = {
{137, 153, 169, 185, 201, 217, 233, 249},
{138, 154, 170, 186, 202, 218, 234, 250},
{139, 155, 171, 187, 203, 219, 235, 251},
{140, 156, 172, 188, 204, 220, 236, 252},
{141, 157, 173, 189, 205, 221, 237, 253},
{142, 158, 174, 190, 206, 222, 238, 254},
{143, 159, 175, 191, 207, 223, 239, 255},
{144, 160, 176, 192, 208, 224, 240, 256} };

int p00[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
int p10[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
int p01[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
int p11[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

int main(int argc, char* argv[]) {

int n = 8;
int ib = 8; // does 'ib' affect the results ?
int k = n; // what's the 'K' parameter used in 'core_dgessm' and 'core_dssssm' ?
double work[8][8]; // what's 'work' (used in 'dssssm') ? and what's it's dimensions ?
int info;

dgetrf(n, n, &(a00[0][0]), n, p00, &info);
dgessm(n, n, k, ib, p00, &(a00[0][0]), n, &(a00[0][0]), n); // what's the K parameter ?
dtstrf(n, n, ib, n, &(a00[0][0]), n, &(a10[0][0]), n, &(a10[0][0]), n, p10, &(work[0][0]), 8, &info);
dssssm(n, n, n, n, k, ib, &(a01[0][0]), n, &(a11[0][0]), n, &(a10[0][0]), n, &(a10[0][0]), n, p10);
dgetrf(n, n, &(a11[0][0]), n, p11, &info);
}
*/
