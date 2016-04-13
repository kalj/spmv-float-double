/* -*- c-basic-offset:4; tab-width:4; indent-tabs-mode:nil -*-
 *
 * @(#)spmv.h
 * @author Karl Ljungkvist <k.ljungkvist@gmail.com>
 *
 */

#ifndef _SPMV_H
#define _SPMV_H

#define NUNROLL 8

template<typename vnum_type, typename mnum_type>
void mult(vnum_type *__restrict__ res, const vnum_type *__restrict__ v,
          const mnum_type *__restrict__ val, const unsigned int *__restrict__ ind,
          const unsigned int *__restrict__ ptr, const unsigned int N)
{
  typedef vnum_type comp_type;
#pragma omp parallel for schedule(static)
  for (unsigned int i=0; i<N; ++i) {
    comp_type tmp = 0;

    const unsigned int nrow=ptr[i+1]-ptr[i];
    unsigned int k;
    for(k = 0; k < nrow/NUNROLL; ++k) {

      for(unsigned int j = 0; j < NUNROLL; ++j) {
        const unsigned int idx=ptr[i]+k*NUNROLL+j;
        tmp +=  comp_type(val[idx]) * comp_type(v[ind[idx]]);
      }
    }

    // tail
    for(unsigned int j=0; j<nrow%NUNROLL; ++j) {
        const unsigned int idx=ptr[i]+k*NUNROLL+j;
      tmp +=  comp_type(val[idx]) * comp_type(v[ind[idx]]);
    }

    res[i] = tmp;
  }
}

#endif /* _SPMV_H */
