/*
	these kernels are implementations
	of the plasma library
*/

#ifndef KERNELS_H
#define KERNELS_H

int core_dgessm 	( 	int  	M,
						          int  	N,
						          int  	K,
						          int  	IB,
						          int *  	IPIV,
						          double *  	L,
						          int  	LDL,
						          double *  	A,
						          int  	LDA
						      );

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
						      );

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
						      );

int core_dgetrf 	( 	int  	m,
						          int  	n,
						          double *  	A,
						          int  	lda,
						          int *  	IPIV,
						          int *  	info
						      );

#endif

