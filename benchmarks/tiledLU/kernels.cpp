/*
	these kernels are implementations
	of the plasma library
*/

#include "kernels.hpp"
#include <mkl_cblas.h>
#include <mkl_lapacke.h>
#include <mkl_lapack.h>
#include <math.h>
#include <cstring>
#include <iostream>

#define KERNEL_SUCCESS 0
#define coreblas_error(k, str) std::cout <<__func__<<": Parameter "<<k<<" "<<str<<std::endl
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

int core_dgessm 	( 	int  	M,
                			int  	N,
						          int  	K,
						          int  	IB,
						          int *  	IPIV,
						          double *  	L,
						          int  	LDL,
						          double *  	A,
						          int  	LDA
						      )
{
    static double zone = 1.0;
    static double mzone = -1.0;
    static int ione = 1;
    int i, sb;
    int tmp, tmp2;
    /* Check input arguments */
    if (M < 0) {
        coreblas_error(1, "Illegal value of M");
        return -1;
    }
    if (N < 0) {
        coreblas_error(2, "Illegal value of N");
        return -2;
    }
    if (K < 0) {
        coreblas_error(3, "Illegal value of K");
        return -3;
    }
    if (IB < 0) {
        coreblas_error(4, "Illegal value of IB");
        return -4;
    }
    if ((LDL < max(1,M)) && (M > 0)) {
        coreblas_error(7, "Illegal value of LDL");
        return -7;
    }
    if ((LDA < max(1,M)) && (M > 0)) {
        coreblas_error(9, "Illegal value of LDA");
        return -9;
    }
    /* Quick return */
    if ((M == 0) || (N == 0) || (K == 0) || (IB == 0))
        return KERNEL_SUCCESS;
    for(i = 0; i < K; i += IB) {
        sb = min(IB, K-i);
        /*
        * Apply interchanges to columns I*IB+1:IB*( I+1 )+1.
        */
        tmp = i+1;
        tmp2 = i+sb;
        //LAPACKE_dlaswp_work(LAPACK_COL_MAJOR, N, A, LDA, tmp, tmp2, IPIV, ione);
				dlaswp(&N, A, &LDA, &tmp, &tmp2, IPIV, &ione);
        /*
        * Compute block row of U.
        */
        cblas_dtrsm(
            CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit,
            sb, N, (zone),
            &L[LDL*i+i], LDL,
            &A[i], LDA );
        if (i+sb < M) {
            /*
            * Update trailing submatrix.
            */
            cblas_dgemm(
                CblasColMajor, CblasNoTrans, CblasNoTrans,
                M-(i+sb), N, sb,
                (mzone), &L[LDL*i+(i+sb)], LDL,
                &A[i], LDA,
                (zone), &A[i+sb], LDA );
        }
    }
    return KERNEL_SUCCESS;
}


int core_dssssm 	( 	int  	M1,
						          int  	N1,
						          int  	M2,
						          int  	N2,
						          int  	K,
						          int  	IB,
						          double *  	A1,
						          int  	LDA1,
						          double *  	A2,
						          int  	LDA2,
						          double *  	L1,
						          int  	LDL1,
						          double *  	L2,
						          int  	LDL2,
						          int *  	IPIV
						      )
{
    static double zone = 1.0;
    static double mzone =-1.0;
    int i, ii, sb;
    int im, ip;
    /* Check input arguments */
    if (M1 < 0) {
        coreblas_error(1, "Illegal value of M1");
        return -1;
    }
    if (N1 < 0) {
        coreblas_error(2, "Illegal value of N1");
        return -2;
    }
    if (M2 < 0) {
        coreblas_error(3, "Illegal value of M2");
        return -3;
    }
    if (N2 < 0) {
        coreblas_error(4, "Illegal value of N2");
        return -4;
    }
    if (K < 0) {
        coreblas_error(5, "Illegal value of K");
        return -5;
    }
    if (IB < 0) {
        coreblas_error(6, "Illegal value of IB");
        return -6;
    }
    if (LDA1 < max(1,M1)) {
        coreblas_error(8, "Illegal value of LDA1");
        return -8;
    }
    if (LDA2 < max(1,M2)) {
        coreblas_error(10, "Illegal value of LDA2");
        return -10;
    }
    if (LDL1 < max(1,IB)) {
        coreblas_error(12, "Illegal value of LDL1");
        return -12;
    }
    if (LDL2 < max(1,M2)) {
        coreblas_error(14, "Illegal value of LDL2");
        return -14;
    }
    /* Quick return */
    if ((M1 == 0) || (N1 == 0) || (M2 == 0) || (N2 == 0) || (K == 0) || (IB == 0))
        return KERNEL_SUCCESS;
    ip = 0;
    for(ii = 0; ii < K; ii += IB) {
        sb = min(K-ii, IB);
        for(i = 0; i < sb; i++) {
            im = IPIV[ip]-1;
            if (im != (ii+i)) {
                im = im - M1;
                cblas_dswap(N1, &A1[ii+i], LDA1, &A2[im], LDA2);
            }
            ip = ip + 1;
        }
        cblas_dtrsm(
            CblasColMajor, CblasLeft, CblasLower,
            CblasNoTrans, CblasUnit,
            sb, N1, (zone),
            &L1[LDL1*ii], LDL1,
            &A1[ii], LDA1);
        cblas_dgemm(
            CblasColMajor, CblasNoTrans, CblasNoTrans,
            M2, N2, sb,
            (mzone), &L2[LDL2*ii], LDL2,
            &A1[ii], LDA1,
            (zone), A2, LDA2);
    }
    return KERNEL_SUCCESS;
}

int core_dtstrf 	( 	int  	M,
						          int  	N,
						          int  	IB,
						          int  	NB,
						          double *  	U,
						          int  	LDU,
						          double *  	A,
						          int  	LDA,
						          double *  	L,
						          int  	LDL,
						          int *  	IPIV,
						          double *  	WORK,
						          int  	LDWORK,
						          int *  	INFO
						      )
{
    static double zzero = 0.0;
    static double mzone =-1.0;
    double alpha;
    int i, j, ii, sb;
    int im, ip;
    /* Check input arguments */
    *INFO = 0;
    if (M < 0) {
        coreblas_error(1, "Illegal value of M");
        return -1;
    }
    if (N < 0) {
        coreblas_error(2, "Illegal value of N");
        return -2;
    }
    if (IB < 0) {
        coreblas_error(3, "Illegal value of IB");
        return -3;
    }
    if ((LDU < max(1,NB)) && (NB > 0)) {
        coreblas_error(6, "Illegal value of LDU");
        return -6;
    }
    if ((LDA < max(1,M)) && (M > 0)) {
        coreblas_error(8, "Illegal value of LDA");
        return -8;
    }
    if ((LDL < max(1,IB)) && (IB > 0)) {
        coreblas_error(10, "Illegal value of LDL");
        return -10;
    }
    /* Quick return */
    if ((M == 0) || (N == 0) || (IB == 0))
        return KERNEL_SUCCESS;
    /* Set L to 0 */
    memset(L, 0, LDL*N*sizeof(double));
    ip = 0;
    for (ii = 0; ii < N; ii += IB) {
        sb = min(N-ii, IB);
        for (i = 0; i < sb; i++) {
            im = cblas_idamax(M, &A[LDA*(ii+i)], 1);
            IPIV[ip] = ii+i+1;
            if (fabs(A[LDA*(ii+i)+im]) > fabs(U[LDU*(ii+i)+ii+i])) {
                /*
                * Swap behind.
                */
                cblas_dswap(i, &L[LDL*ii+i], LDL, &WORK[im], LDWORK );
                /*
                * Swap ahead.
                */
                cblas_dswap(sb-i, &U[LDU*(ii+i)+ii+i], LDU, &A[LDA*(ii+i)+im], LDA );
                /*
                * Set IPIV.
                */
                IPIV[ip] = NB + im + 1;
                for (j = 0; j < i; j++) {
                    A[LDA*(ii+j)+im] = zzero;
                }
            }
            if ((*INFO == 0) && (fabs(A[LDA*(ii+i)+im]) == zzero)
                    && (fabs(U[LDU*(ii+i)+ii+i]) == zzero)) {
                *INFO = ii+i+1;
            }
            alpha = ((double)1. / U[LDU*(ii+i)+ii+i]);
            cblas_dscal(M, (alpha), &A[LDA*(ii+i)], 1);
            cblas_dcopy(M, &A[LDA*(ii+i)], 1, &WORK[LDWORK*i], 1);
            cblas_dger(
                CblasColMajor, M, sb-i-1,
                (mzone), &A[LDA*(ii+i)], 1,
                &U[LDU*(ii+i+1)+ii+i], LDU,
                &A[LDA*(ii+i+1)], LDA );
            ip = ip+1;
        }
        /*
        * Apply the subpanel to the rest of the panel.
        */
        if(ii+i < N) {
            for(j = ii; j < ii+sb; j++) {
                if (IPIV[j] <= NB) {
                    IPIV[j] = IPIV[j] - ii;
                }
            }
            core_dssssm(
                NB, N-(ii+sb), M, N-(ii+sb), sb, sb,
                &U[LDU*(ii+sb)+ii], LDU,
                &A[LDA*(ii+sb)], LDA,
                &L[LDL*ii], LDL,
                WORK, LDWORK, &IPIV[ii]);
            for(j = ii; j < ii+sb; j++) {
                if (IPIV[j] <= NB) {
                    IPIV[j] = IPIV[j] + ii;
                }
            }
        }
    }
    return KERNEL_SUCCESS;
}

int core_dgetrf 	( 	int  	m,
						          int  	n,
						          double *  	A,
						          int  	lda,
						          int *  	IPIV,
						          int *  	info
						      )
{
    *info = LAPACKE_dgetrf(LAPACK_COL_MAJOR, m, n, A, lda, IPIV );
    return KERNEL_SUCCESS;
}

