#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "opt.h"

#define	Debug		0

/**************************************************
	Vector operations
**************************************************/

extern void vectorcopy(n, x, y)
int	n;
dbl	x[], y[];
{
	int	i;
	
	for (i=0; i<n; i++) {
		x[i] = y[i];
	}
}

extern void vectorfill(n, x, a)
int	n;
dbl	x[], a;
{
	int	i;
	
	for (i=0; i<n; i++) {
		x[i] = a;
	}
}

extern dbl vectorvector(n, x, y)	/* inner product xy */
int	n;
dbl	x[], y[];
{
	int	i;
	dbl	sum;
	
	sum = 0;
	for (i=0; i<n; i++) {
		sum += x[i] * y[i];
	}
	return(sum);
}

extern void scalarvector(n, y, k, x)	/* y = kx   k = scalar */
int	n;
dbl	y[], k, x[];
{
	int	i;
	
	for (i=0; i<n; i++) {
		y[i] = k * x[i];
	}
}

extern void vectoradd(n, x, y, z)	/* x = y + z */
int	n;
dbl	x[], y[], z[];
{
	int	i;
	
	for (i=0; i<n; i++) {
		x[i] = y[i] + z[i];
	}
}

extern void vectorsub(n, x, y, z)	/* x = y - z */
int	n;
dbl	x[], y[], z[];
{
	int	i;
	
	for (i=0; i<n; i++) {
		x[i] = y[i] - z[i];
	}
}

/**************************************************
	Matrix operations
**************************************************/

extern void matrixvector(m, n, y, a, x)	/* y = Ax   A = (m,n) matrix */
int	m, n;
dbl	y[], a[], x[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		y[i] = 0;
		for (j=0; j<n; j++) {
			y[i] += a[n*i+j] * x[j];
		}
	}
}

extern void matrixrowvector(m, n, y, a, i)	/* y = i-th row vector of A */
int	m, n, i;
dbl	y[], a[];
{
	int	j;
	
	for (j=0; j<n; j++) {
		y[j] = a[n*i+j];
	}
}

extern void matrixcolumnvector(m, n, x, a, j)	/* y = j-th column vector of A */
int	m, n, j;
dbl	x[], a[];
{
	int	i;
	
	for (i=0; i<m; i++) {
		x[i] = a[n*i+j];
	}
}

extern void matrixcopy(m, n, a, b)
int	m, n;
dbl	a[], b[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			a[n*i+j] = b[n*i+j];
		}
	}
}

extern void matrixunit(n, a)	/* A = I   (n,n) matrices */
int	n;
dbl	a[];
{
	int	i, j, m;
	
	m = n;
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			a[n*i+j] = 0;
		}
		a[n*i+i] = 1;
	}
}

extern void scalarmatrix(m, n, a, k, b)	/* A = kB   k = scalar */
int	m, n;
dbl	a[], k, b[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			a[n*i+j] = k * b[n*i+j];
		}
	}
}

extern void matrixadd(m, n, a, b, c)	/* A = B + C   (m,n) matrices */
int	m, n;
dbl	a[], b[], c[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			a[n*i+j] = b[n*i+j] + c[n*i+j];
		}
	}
}

extern void matrixsub(m, n, a, b, c)	/* A = B - C   (m,n) matrices */
int	m, n;
dbl	a[], b[], c[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			a[n*i+j] = b[n*i+j] - c[n*i+j];
		}
	}
}

/*
	matrix multiplication A = BC
		A=(m,n) B=(m,l) C=(l,n) matrices
*/

extern void matrixmatrix(m, n, l, a, b, c)	/* A = BC */
int	m, n, l;
dbl	a[], b[], c[];
{
	int	i, j, k;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			a[n*i+j] = 0;
			for (k=0; k<l; k++) {
				a[n*i+j] += b[l*i+k] * c[n*k+j];
			}
		}
	}
}

/*
	matrix transpose
		A=(m,n) B=(n,m) matrices
*/

extern void matrixtranspose(m, n, a, b)		/* A = B^{T} */
int	m, n;
dbl	a[], b[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
	  for (j=0; j<n; j++) {
	    a[n*i+j] = b[j*m+i];
	  }
	}
}

/*
	Basic operations of matrices
		A = (m,n) matrix
*/

extern void matrixrowscalar(m, n, a, i, p)	/* i-th row *= p */
int	m, n, i;
dbl	a[], p;
{
	int	j;
	
	for (j=0; j<n; j++) {
		a[i*n+j] *= p;
	}
}

extern void matrixrowexchange(m, n, a, i0, i1)	/* swap row i0 and row i1 */
int	m, n, i0, i1;
dbl	a[];
{
	int	j;
	dbl	tmp;
	
	for (j=0; j<n; j++) {
		tmp = a[i0*n+j];
		a[i0*n+j] = a[i1*n+j];
		a[i1*n+j] = tmp;
	}
}

extern void matrixrowadd(m, n, a, i0, p, i1)	/* row i0 += p * row i1 */
int	m, n, i0, i1;
dbl	a[], p;
{
	int	j;
	
	for (j=0; j<n; j++) {
		a[i0*n+j] += (p * a[i1*n+j]);
	}
}

extern void matrixrowaddtwo(ma, mb, n, a, b, ia, p, ib)
/* row ia of matrix A += p * row ib of matrix B */
/* A = (ma,n) matrix   B = (mb,n) matrix */
int	ma, mb, n, ia, ib;
dbl	a[], b[], p;
{
	int	j;
	
	for (j=0; j<n; j++) {
		a[ia*n+j] += (p * b[ib*n+j]);
	}
}

/*
	compare absolute values of a[i][j] where i \in [is,ie)
	input:	(m,n) matrix A
		j	column number
		is,ie	row numbers
	output:
		*pimax = row number corresponding to the maximum absolute value
		*pmax = the maximum value
*/
extern void matrixsearchcolumnmaxabs(m, n, a, j, is, ie, pimax, pmax)
int	m, n, j, is, ie;
dbl	a[];
int	*pimax;
dbl	*pmax;
{
	int	i;
	dbl	value;
	
	*pimax = is;
	*pmax = fabs(a[is*n+j]);
	for (i=is+1; i<ie; i++) {
		if ((value = fabs(a[i*n+j])) > *pmax) {
			*pimax = i;
			*pmax = value;
		}
	}
}

/*
	inverse of matrix
	input:	A = (n,n) matrix
		eps = small value
	output:	Ainv = (n,n) matrix
*/

extern void matrixinverse(n, ainv, a, eps)	/* Ainv = a^{-1} */
int	n;
dbl	a[], ainv[], eps;
{
	int	i, j, m, imax;
	dbl	max, div, mul;
	
	m = n;
	matrixunit(n, ainv);
#if Debug
	printf("--- marix inverse ---\n");
	matrixfprint(stdout, m, n, a);
	printf("\n");
	matrixfprint(stdout, m, n, ainv);
#endif
	
	/*	forward elimination	*/
	for (j=0; j<m; j++) {
		matrixsearchcolumnmaxabs(m,n,a,j,j,m,&imax,&max);
#if Debug > 1
		fprintf(stdout, "imax=%d max=%lf\n", imax, max);
#endif
		if (max <= eps) {
		  fprintf(stderr, "matrix inverse: singular\n");
		  matrixfprint(stderr, m, n, ainv);
		  exit(1);
		  break;
		}
		if (j != imax) {
			matrixrowexchange(m, n, a,    j, imax);
			matrixrowexchange(m, n, ainv, j, imax);
		}
#if Debug > 1
		printf("exchange %d and %d\n", j, imax);
		matrixfprint(stdout, m, n, a);
		printf("\n");
		matrixfprint(stdout, m, n, ainv);
#endif
		div = 1/a[j*n+j];
		matrixrowscalar(m, n, a,    j, div);
		matrixrowscalar(m, n, ainv, j, div);
#if Debug > 1
		printf("scalar %lf\n", div);
		matrixfprint(stdout, m, n, a);
		printf("\n");
		matrixfprint(stdout, m, n, ainv);
#endif

		for (i=j+1; i<m; i++) {
			mul = -a[i*n+j];
			matrixrowadd(m, n, a,    i, mul, j);
			matrixrowadd(m, n, ainv, i, mul, j);
#if Debug > 1
			printf("add\n");
			matrixfprint(stdout, m, n, a);
			printf("\n");
			matrixfprint(stdout, m, n, ainv);
#endif
		}
#if Debug
		printf("forward process\n");
		matrixfprint(stdout, m, n, a);
		printf("\n");
		matrixfprint(stdout, m, n, ainv);
#endif
	}
	
	for (j=n-1; j>=0; j--) {
		for (i=j-1; i>=0; i--) {
			mul = -a[i*n+j];
			matrixrowadd(m, n, a,    i, mul, j);
			matrixrowadd(m, n, ainv, i, mul, j);
		}
#if Debug
		printf("backward process\n");
		matrixfprint(stdout, m, n, a);
		printf("\n");
		matrixfprint(stdout, m, n, ainv);
#endif
	}
	
#if Debug
	printf("answer\n");
	matrixfprint(stdout, m, n, a);
	printf("\n");
	matrixfprint(stdout, m, n, ainv);
	printf("\n");
#endif
}

/*	Print & Scan		*/

static char	*format = " %lf ";

extern void vectorfprint(fd, n, x)
FILE	*fd;
int	n;
dbl	x[];
{
	int	i;
	
	for (i=0; i<n; i++) {
		fprintf(fd, format, x[i]);
	}
	fputc('\n', fd);
}

extern void vectorfscan(fd, n, x)
FILE	*fd;
int	n;
dbl	x[];
{
	int	i;
	
	for (i=0; i<n; i++) {
		fscanf(fd, format, x+i);
	}
}

extern void matrixfprint(fd, m, n, a)
FILE	*fd;
int	m, n;
dbl	a[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			fprintf(fd, format, a[n*i+j]);
		}
		fputc('\n', fd);
	}
}

extern void matrixfscan(fd, m, n, a)
FILE	*fd;
int	m, n;
dbl	a[];
{
	int	i, j;
	
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			fscanf(fd, format, a+n*i+j);
		}
	}
}

extern void matrixprintformat(fmt)
char	*fmt;
{
	format = fmt;
}

extern void matrixallocfscan(fd, m, n, pa)
FILE	*fd;
int	m, n;
dbl	**pa;
{
	int	i, j;
	dbl	*a;
	
	*pa = a = allc(dbl, m*n);
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			fscanf(fd, format, a++);
		}
	}
}
