#include "f2c.h"

/* Subroutine */ int sstevx_(char *jobz, char *range, integer *n, real *d, 
	real *e, real *vl, real *vu, integer *il, integer *iu, real *abstol, 
	integer *m, real *w, real *z, integer *ldz, real *work, integer *
	iwork, integer *ifail, integer *info)
{
/*  -- LAPACK driver routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SSTEVX computes selected eigenvalues and, optionally, eigenvectors   
    of a real symmetric tridiagonal matrix A.  Eigenvalues and   
    eigenvectors can be selected by specifying either a range of values   
    or a range of indices for the desired eigenvalues.   

    Arguments   
    =========   

    JOBZ    (input) CHARACTER*1   
            = 'N':  Compute eigenvalues only;   
            = 'V':  Compute eigenvalues and eigenvectors.   

    RANGE   (input) CHARACTER*1   
            = 'A': all eigenvalues will be found.   
            = 'V': all eigenvalues in the half-open interval (VL,VU]   
                   will be found.   
            = 'I': the IL-th through IU-th eigenvalues will be found.   

    N       (input) INTEGER   
            The order of the matrix.  N >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the n diagonal elements of the tridiagonal matrix   
            A.   
            On exit, D may be multiplied by a constant factor chosen   
            to avoid over/underflow in computing the eigenvalues.   

    E       (input/output) REAL array, dimension (N)   
            On entry, the (n-1) subdiagonal elements of the tridiagonal   
            matrix A in elements 1 to N-1 of E; E(N) need not be set.   
            On exit, E may be multiplied by a constant factor chosen   
            to avoid over/underflow in computing the eigenvalues.   

    VL      (input) REAL   
    VU      (input) REAL   
            If RANGE='V', the lower and upper bounds of the interval to   
            be searched for eigenvalues. VL < VU.   
            Not referenced if RANGE = 'A' or 'I'.   

    IL      (input) INTEGER   
    IU      (input) INTEGER   
            If RANGE='I', the indices (in ascending order) of the   
            smallest and largest eigenvalues to be returned.   
            1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0.   
            Not referenced if RANGE = 'A' or 'V'.   

    ABSTOL  (input) REAL   
            The absolute error tolerance for the eigenvalues.   
            An approximate eigenvalue is accepted as converged   
            when it is determined to lie in an interval [a,b]   
            of width less than or equal to   

                    ABSTOL + EPS *   max( |a|,|b| ) ,   

            where EPS is the machine precision.  If ABSTOL is less   
            than or equal to zero, then  EPS*|T|  will be used in   
            its place, where |T| is the 1-norm of the tridiagonal   
            matrix.   

            Eigenvalues will be computed most accurately when ABSTOL is   
            set to twice the underflow threshold 2*SLAMCH('S'), not zero. 
  
            If this routine returns with INFO>0, indicating that some   
            eigenvectors did not converge, try setting ABSTOL to   
            2*SLAMCH('S').   

            See "Computing Small Singular Values of Bidiagonal Matrices   
            with Guaranteed High Relative Accuracy," by Demmel and   
            Kahan, LAPACK Working Note #3.   

    M       (output) INTEGER   
            The total number of eigenvalues found.  0 <= M <= N.   
            If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1.   

    W       (output) REAL array, dimension (N)   
            The first M elements contain the selected eigenvalues in   
            ascending order.   

    Z       (output) REAL array, dimension (LDZ, max(1,M) )   
            If JOBZ = 'V', then if INFO = 0, the first M columns of Z   
            contain the orthonormal eigenvectors of the matrix A   
            corresponding to the selected eigenvalues, with the i-th   
            column of Z holding the eigenvector associated with W(i).   
            If an eigenvector fails to converge (INFO > 0), then that   
            column of Z contains the latest approximation to the   
            eigenvector, and the index of the eigenvector is returned   
            in IFAIL.  If JOBZ = 'N', then Z is not referenced.   
            Note: the user must ensure that at least max(1,M) columns are 
  
            supplied in the array Z; if RANGE = 'V', the exact value of M 
  
            is not known in advance and an upper bound must be used.   

    LDZ     (input) INTEGER   
            The leading dimension of the array Z.  LDZ >= 1, and if   
            JOBZ = 'V', LDZ >= max(1,N).   

    WORK    (workspace) REAL array, dimension (5*N)   

    IWORK   (workspace) INTEGER array, dimension (5*N)   

    IFAIL   (output) INTEGER array, dimension (N)   
            If JOBZ = 'V', then if INFO = 0, the first M elements of   
            IFAIL are zero.  If INFO > 0, then IFAIL contains the   
            indices of the eigenvectors that failed to converge.   
            If JOBZ = 'N', then IFAIL is not referenced.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  if INFO = i, then i eigenvectors failed to converge.   
                  Their indices are stored in array IFAIL.   

    ===================================================================== 
  


       Test the input parameters.   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static integer c__1 = 1;
    
    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    real r__1, r__2;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static integer imax;
    static real rmin, rmax, tnrm;
    static integer itmp1, i, j;
    static real sigma;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    static char order[1];
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), sswap_(integer *, real *, integer *, real *, integer *
	    );
    static logical wantz;
    static integer jj;
    static logical alleig, indeig;
    static integer iscale, indibl;
    static logical valeig;
    extern doublereal slamch_(char *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static real bignum;
    static integer indisp, indiwo, indwrk;
    extern doublereal slanst_(char *, integer *, real *, real *);
    extern /* Subroutine */ int sstein_(integer *, real *, real *, integer *, 
	    real *, integer *, integer *, real *, integer *, real *, integer *
	    , integer *, integer *), ssterf_(integer *, real *, real *, 
	    integer *);
    static integer nsplit;
    extern /* Subroutine */ int sstebz_(char *, char *, integer *, real *, 
	    real *, integer *, integer *, real *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, real *, integer *, 
	    integer *);
    static real smlnum;
    extern /* Subroutine */ int ssteqr_(char *, integer *, real *, real *, 
	    real *, integer *, real *, integer *);
    static real eps, vll, vuu, tmp1;



#define D(I) d[(I)-1]
#define E(I) e[(I)-1]
#define W(I) w[(I)-1]
#define WORK(I) work[(I)-1]
#define IWORK(I) iwork[(I)-1]
#define IFAIL(I) ifail[(I)-1]

#define Z(I,J) z[(I)-1 + ((J)-1)* ( *ldz)]

    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (valeig && *n > 0 && *vu <= *vl) {
	*info = -7;
    } else if (indeig && *il < 1) {
	*info = -8;
    } else if (indeig && (*iu < min(*n,*il) || *iu > *n)) {
	*info = -9;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -14;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSTEVX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (alleig || indeig) {
	    *m = 1;
	    W(1) = D(1);
	} else {
	    if (*vl < D(1) && *vu >= D(1)) {
		*m = 1;
		W(1) = D(1);
	    }
	}
	if (wantz) {
	    Z(1,1) = 1.f;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = slamch_("Safe minimum");
    eps = slamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1.f / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    r__1 = sqrt(bignum), r__2 = 1.f / sqrt(sqrt(safmin));
    rmax = dmin(r__1,r__2);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    if (valeig) {
	vll = *vl;
	vuu = *vu;
    }
    tnrm = slanst_("M", n, &D(1), &E(1));
    if (tnrm > 0.f && tnrm < rmin) {
	iscale = 1;
	sigma = rmin / tnrm;
    } else if (tnrm > rmax) {
	iscale = 1;
	sigma = rmax / tnrm;
    }
    if (iscale == 1) {
	sscal_(n, &sigma, &D(1), &c__1);
	i__1 = *n - 1;
	sscal_(&i__1, &sigma, &E(1), &c__1);
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }

/*     If all eigenvalues are desired and ABSTOL is less than zero, then 
  
       call SSTERF or SSTEQR.  If this fails for some eigenvalue, then   
       try SSTEBZ. */

    if ((alleig || indeig && *il == 1 && *iu == *n) && *abstol <= 0.f) {
	scopy_(n, &D(1), &c__1, &W(1), &c__1);
	i__1 = *n - 1;
	scopy_(&i__1, &E(1), &c__1, &WORK(1), &c__1);
	indwrk = *n + 1;
	if (! wantz) {
	    ssterf_(n, &W(1), &WORK(1), info);
	} else {
	    ssteqr_("I", n, &W(1), &WORK(1), &Z(1,1), ldz, &WORK(indwrk),
		     info);
	    if (*info == 0) {
		i__1 = *n;
		for (i = 1; i <= *n; ++i) {
		    IFAIL(i) = 0;
/* L10: */
		}
	    }
	}
	if (*info == 0) {
	    *m = *n;
	    goto L20;
	}
	*info = 0;
    }

/*     Otherwise, call SSTEBZ and, if eigenvectors are desired, SSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    indwrk = 1;
    indibl = 1;
    indisp = indibl + *n;
    indiwo = indisp + *n;
    sstebz_(range, order, n, &vll, &vuu, il, iu, abstol, &D(1), &E(1), m, &
	    nsplit, &W(1), &IWORK(indibl), &IWORK(indisp), &WORK(indwrk), &
	    IWORK(indiwo), info);

    if (wantz) {
	sstein_(n, &D(1), &E(1), m, &W(1), &IWORK(indibl), &IWORK(indisp), &Z(1,1), ldz, &WORK(indwrk), &IWORK(indiwo), &IFAIL(1), 
		info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

L20:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	r__1 = 1.f / sigma;
	sscal_(&imax, &r__1, &W(1), &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with   
       eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= *m-1; ++j) {
	    i = 0;
	    tmp1 = W(j);
	    i__2 = *m;
	    for (jj = j + 1; jj <= *m; ++jj) {
		if (W(jj) < tmp1) {
		    i = jj;
		    tmp1 = W(jj);
		}
/* L30: */
	    }

	    if (i != 0) {
		itmp1 = IWORK(indibl + i - 1);
		W(i) = W(j);
		IWORK(indibl + i - 1) = IWORK(indibl + j - 1);
		W(j) = tmp1;
		IWORK(indibl + j - 1) = itmp1;
		sswap_(n, &Z(1,i), &c__1, &Z(1,j), &
			c__1);
		if (*info != 0) {
		    itmp1 = IFAIL(i);
		    IFAIL(i) = IFAIL(j);
		    IFAIL(j) = itmp1;
		}
	    }
/* L40: */
	}
    }

    return 0;

/*     End of SSTEVX */

} /* sstevx_ */

