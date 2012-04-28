/*****************************************************************************
  Copyright (c) 2011, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************
* Contents: Native high-level C interface to LAPACK function zgtsvx
* Author: Intel Corporation
* Generated November, 2011
*****************************************************************************/

#include "lapacke.h"
#include "lapacke_utils.h"

lapack_int LAPACKE_zgtsvx( int matrix_order, char fact, char trans,
                           lapack_int n, lapack_int nrhs,
                           const lapack_complex_double* dl,
                           const lapack_complex_double* d,
                           const lapack_complex_double* du,
                           lapack_complex_double* dlf,
                           lapack_complex_double* df,
                           lapack_complex_double* duf,
                           lapack_complex_double* du2, lapack_int* ipiv,
                           const lapack_complex_double* b, lapack_int ldb,
                           lapack_complex_double* x, lapack_int ldx,
                           double* rcond, double* ferr, double* berr )
{
    lapack_int info = 0;
    double* rwork = NULL;
    lapack_complex_double* work = NULL;
    if( matrix_order != LAPACK_COL_MAJOR && matrix_order != LAPACK_ROW_MAJOR ) {
        LAPACKE_xerbla( "LAPACKE_zgtsvx", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    /* Optionally check input matrices for NaNs */
    if( LAPACKE_zge_nancheck( matrix_order, n, nrhs, b, ldb ) ) {
        return -14;
    }
    if( LAPACKE_z_nancheck( n, d, 1 ) ) {
        return -7;
    }
    if( LAPACKE_lsame( fact, 'f' ) ) {
        if( LAPACKE_z_nancheck( n, df, 1 ) ) {
            return -10;
        }
    }
    if( LAPACKE_z_nancheck( n-1, dl, 1 ) ) {
        return -6;
    }
    if( LAPACKE_lsame( fact, 'f' ) ) {
        if( LAPACKE_z_nancheck( n-1, dlf, 1 ) ) {
            return -9;
        }
    }
    if( LAPACKE_z_nancheck( n-1, du, 1 ) ) {
        return -8;
    }
    if( LAPACKE_lsame( fact, 'f' ) ) {
        if( LAPACKE_z_nancheck( n-2, du2, 1 ) ) {
            return -12;
        }
    }
    if( LAPACKE_lsame( fact, 'f' ) ) {
        if( LAPACKE_z_nancheck( n-1, duf, 1 ) ) {
            return -11;
        }
    }
#endif
    /* Allocate memory for working array(s) */
    rwork = (double*)LAPACKE_malloc( sizeof(double) * MAX(1,n) );
    if( rwork == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_0;
    }
    work = (lapack_complex_double*)
        LAPACKE_malloc( sizeof(lapack_complex_double) * MAX(1,2*n) );
    if( work == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_1;
    }
    /* Call middle-level interface */
    info = LAPACKE_zgtsvx_work( matrix_order, fact, trans, n, nrhs, dl, d, du,
                                dlf, df, duf, du2, ipiv, b, ldb, x, ldx, rcond,
                                ferr, berr, work, rwork );
    /* Release memory and exit */
    LAPACKE_free( work );
exit_level_1:
    LAPACKE_free( rwork );
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        LAPACKE_xerbla( "LAPACKE_zgtsvx", info );
    }
    return info;
}
