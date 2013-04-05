#include <iostream>
#include <iomanip>

#include "matrix.hpp"
#include "kernels.hpp"

#include <mkl_lapack.h>
#include <mkl_lapacke.h>

#include <boost/thread/future.hpp>

#include "timer.hpp"


#define DEFAULT_NB 4
#define DEFAULT_SIZE 8

int SIZE;
int NB;

using namespace std;
using namespace boost;

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
		Tiles[dst](0,0) = async(dgetrf_f(A,m,n,k,LDA,IPIV,
																					Tiles[src](1,1)));
	
        //step 2	
        for(int mm = k+1; mm < TILES; mm++) {

		      Tiles[dst](mm-k,0) = async(dtstrf_f(A,L,m,n,k,mm,LDA,IPIV,
		                                          			Tiles[dst](mm-k-1,0),
		                                          			Tiles[src](mm-k+1,1)));
        }

        //step 3
        for(int nn = k+1; nn < TILES; nn++) {

			    Tiles[dst](0,nn-k) = async(dgessm_f(A,L,m,n,k,nn,LDA,IPIV,
                                              			Tiles[dst](0,0),
                                              			Tiles[src](1,nn-k+1)));
        }

        //step 4
        for(int nn = k+1; nn < TILES; nn++) {	   
                for(int mm=k+1; mm < TILES; mm++) {
                  
                    Tiles[dst](mm-k,nn-k) = async(dssssm_f(A,L,m,n,k,mm,nn,LDA,IPIV,
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
