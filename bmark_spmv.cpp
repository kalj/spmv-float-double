#include <iostream>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <cstdlib>

#include "myio.h"

double timer(void) {
  struct timeval timeval_time;
  gettimeofday(&timeval_time,NULL);
  return (double)timeval_time.tv_sec + (double)timeval_time.tv_usec*1e-6;
}

template<typename vnum_type, typename mnum_type>
void mult(vnum_type *__restrict__ res, const vnum_type *__restrict__ v, const mnum_type *__restrict__ val,
          const unsigned int *__restrict__ ind, const unsigned int *__restrict__ ptr, const unsigned int N)
{
  typedef vnum_type comp_type;
#pragma omp parallel for schedule(static)
  for (unsigned int i=0; i<N; ++i) {
    comp_type tmp = 0;
    for(unsigned int j=ptr[i]; j<ptr[i+1]; ++j) {
      tmp +=  comp_type(val[j]) * comp_type(v[ind[j]]);
    }
    res[i] = tmp;
  }
}


int main(int argc, char** argv)
{
  // typedef double mnum_type;
  typedef double vnum_type;
  typedef float mnum_type;
  // typedef float vnum_type;

  int N = 1000000;
  int N_iter = 20;
  int nnzprow = 200;

  if(argc > 1) {
    sscanval(argv[1],N);
    if(argc > 2) {
      sscanval(argv[2],N_iter);
      if(argc > 3)
        sscanval(argv[3],nnzprow);
    }
  }

  std::cout << "Using ";
  if(sizeof(mnum_type) == 4) std::cout << "floats";
  else std::cout << "doubles";
  std::cout << " for matrix and ";
  if(sizeof(vnum_type) == 4) std::cout << "floats";
  else std::cout << "doubles";
  std::cout << " for vectors." << std::endl;

  printf("N: %d\n", N);

  // printf("Assembly...");
  std::vector<std::vector<mnum_type>> rows_val(N);
  std::vector<std::vector<unsigned int>> rows_ind(N);

  unsigned int nnz = 0;

  for(int i = 0; i < N; ++i) {
    rows_val[i].reserve(nnzprow);
    rows_ind[i].reserve(nnzprow);

    int jstart = i-(nnzprow/2);
    int jend = i+(nnzprow/2);
    if(jstart < 0) jstart = 0;
    if(jend > N) jend = N;

    for(int j = jstart; j < jend; ++j) {
      rows_val[i].push_back((i+j)/double(N));
      rows_ind[i].push_back(j);
    }

    nnz += rows_val[i].size();
  }
  // printf(" done!\n");

  std::cout << "Non-zeros: " << nnz << std::endl;

  // printf("Conversion to csr...");

  unsigned int *ptr = new unsigned int[N+1];
  unsigned int *ind = new unsigned int[nnz];
  mnum_type *val = new mnum_type[nnz];

  ptr[0] = 0;
  for(int i = 0; i < N; ++i) {

    unsigned int offset = ptr[i];
    for(unsigned int j = 0; j < rows_val[i].size(); ++j) {
      ind[offset+j] = rows_ind[i][j];
      val[offset+j] = rows_val[i][j];
    }

    ptr[i+1] = offset+rows_val[i].size();
  }
  // printf(" done!\n");

  // clear temporary storage
  std::vector<std::vector<mnum_type>>().swap(rows_val);
  std::vector<std::vector<unsigned int>>().swap(rows_ind);

  long mat_byte = sizeof(unsigned int)*(N+1) +
    sizeof(mnum_type)*nnz + sizeof(unsigned int)*nnz;

  std::cout << "Size of sparse matrix: "
            << mat_byte * 1e-6
            << " MB." << std::endl;


  //  initialize vectors
  vnum_type *vec = new vnum_type[N];
  vnum_type *res = new vnum_type[N];
  for(int i = 0; i < N; ++i)
    vec[i] = 1.3;

  long vec_byte = N*sizeof(vnum_type);
  std::cout << "Size of (one) vector: "
            << vec_byte*1e-6
            << " MB." << std::endl;

  float datasize_mb = (vec_byte*2 + mat_byte)*1e-6;
  float gflop = nnz*2-N;


  // perform multiplications
  float tmin = 1000000000;
  double t1;

  for(int i = 0; i < N_iter; ++i) {
    t1 = timer();
    mult(res,vec,val,ind,ptr,N);
    t1 = timer()-t1;
    tmin = tmin < t1 ? tmin : t1;
  }
  std::cout << "Wall time: " <<  tmin << " s" << std::endl;
  std::cout << "Bandwidth: " << datasize_mb*1e-3/tmin << " GB/s" << std::endl;
  std::cout << "Compute:   " << 1e-9*gflop/tmin  << " Gflop/s" << std::endl;

  delete[] vec;
  delete[] res;
  delete[] ptr;
  delete[] ind;
  delete[] val;

  return 0;
}
