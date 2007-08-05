#ifndef OPT_HEADER
#define OPT_HEADER


typedef	double	dbl;
typedef	enum status {success, failure, error} status;

//#define	allc(type,size)		((type *)malloc((size)*sizeof(type)))
//#define	alloc(type,size)	allc(type,size);

extern "C" void	vectorcopy(int, dbl *, dbl *);
extern "C" void	vectorfill(int, dbl *, dbl);
extern "C" dbl	vectorvector(int, dbl *, dbl *);
extern "C" void	scalarvector(int, dbl *, dbl, dbl *);
extern "C" void	vectoradd(int, dbl *, dbl *, dbl *);
extern "C" void	vectorsub(int, dbl *, dbl *, dbl *);

extern "C" void	matrixvector(int, int, dbl *, dbl *, dbl *);
extern "C" void	matrixrowvector(int, int, dbl *, dbl *, int);
extern "C" void	matrixcolumnvector(int, int, dbl *, dbl *, int);

extern "C" void	matrixcopy(int, int, dbl *, dbl *);
extern "C" void	matrixunit(int, dbl *);
extern "C" void	scalarmatrix(int, int, dbl *, dbl, dbl *);
extern "C" void	matrixadd(int, int, dbl *, dbl *, dbl *);
extern "C" void	matrixsub(int, int, dbl *, dbl *, dbl *);
extern "C" void	matrixmatrix(int, int, int, dbl *, dbl *, dbl *);
extern "C" void	matrixtranspose(int, int, dbl *, dbl *);

extern "C" void	matrixrowscalar(int, int, dbl *, int, dbl);
extern "C" void	matrixrowexchange(int, int, dbl *, int, int);
extern "C" void	matrixrowadd(int, int, dbl *, int, dbl, int);
extern "C" void	matrixrowaddtwo(int, int, int, dbl *, dbl *, int, dbl, int);
extern "C" void	matrixsearchcolumnmaxabs(int, int, dbl *, int, int, int,
					 int *, dbl *);
extern "C" void	matrixinverse(int, dbl *, dbl *, dbl);

extern "C" void	vectorfprint(FILE *, int, dbl *);
extern "C" void	vectorfscan(FILE *, int, dbl *);
extern "C" void	matrixfprint(FILE *, int, int, dbl *);
extern "C" void	matrixfscan(FILE *, int, int, dbl *);
extern "C" void	matrixprintformat(char *);
extern "C" int	nullspace(int, int, dbl *, dbl *, int *, dbl);
extern "C" int	svd(int, int, dbl *, dbl *, int *, dbl);
/*
extern status	IntervalReduction(dbl(), dbl, dbl, dbl *, int, dbl);

extern status	LPsimplex(int, int, dbl *, dbl *, dbl *, dbl *, dbl,
			  dbl *, dbl *);
extern status	LPsimplextableau(int, int, dbl *, dbl *, dbl *, dbl *,
				 dbl, int *);
extern void	LPsimplexsolution(int, int, dbl *, dbl *, int *,
				  dbl *, dbl *);
extern status	TwoPhasemethodtableau(int, int, dbl *, dbl *, dbl *, dbl *,
				      dbl *, dbl *, dbl, int *);
extern void	LPsimplexsolution(int, int, dbl *, dbl *, int *,
				  dbl *, dbl *);

extern status	LPfeasiblesolution(int, int, dbl *, dbl *, dbl, dbl *);
extern status	TwoPhasemethod(int, int, dbl *, dbl *, dbl *, dbl *, dbl,
			       dbl *, dbl *);


#define	MaxVars		256
#define	MaxConds	256

extern status	QuasiNewtonMethod(int, dbl(), dbl *(), dbl *, dbl *, int, dbl);
extern status	NelderMeadSimplexMethod(int, dbl (*f)(int,double *), dbl *, dbl, dbl *,
					int, dbl);
extern status	MultiplierMethod(int, dbl (), dbl *(),
				 int, dbl *(), dbl **(),
				 int, dbl *(), dbl **(),
				 dbl *, int, dbl);
extern status	MultiplierMethodSimplex(int, dbl (),
					int, dbl *(), int, dbl *(),
					dbl *, dbl, int, dbl);

extern dbl	TrapezoidIntegral(dbl(), dbl, dbl, int);
extern dbl	GaussIntegral(dbl(), dbl, dbl);
extern dbl	GaussIntegralBase(dbl());
extern dbl	GaussIntegralParams(dbl(), dbl, dbl, int, dbl *);

extern dbl	GaussIntegral2D(dbl(), dbl, dbl, dbl, dbl);
extern dbl	GaussIntegralBase2D(dbl());
extern dbl	GaussIntegral2DParams(dbl(), dbl, dbl, dbl, dbl, int, dbl *);
extern dbl	GaussIntegral2Dmarginal1(dbl(), dbl, dbl, dbl);
extern dbl	GaussIntegral2Dmarginal2(dbl(), dbl, dbl, dbl);
extern dbl	GaussIntegral2Dmarginal1Params(dbl(), dbl, dbl, dbl, int, dbl *);
extern dbl	GaussIntegral2Dmarginal2Params(dbl(), dbl, dbl, dbl, int, dbl *);
extern void	GaussIntegralInitialize(char *);

extern void	RungeKutta(FILE *, int, double *, void(), double *, double, int);
*/
#endif
