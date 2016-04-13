#include <iostream>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <cstdlib>

#include "myio.h"
#include "spmv.h"

int main(int argc, char** argv)
{
  // typedef double mnum_type;
  typedef double vnum_type;
  typedef float mnum_type;
  // typedef float vnum_type;

  int N = 10;


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
    for(int j = 0; j < N; ++j) {
      rows_val[i].push_back(i*N+j);
      rows_ind[i].push_back(j);
    }

    nnz += rows_val[i].size();
  }
  // printf(" done!\n");

  printf("Matrix:\n");
  for(int i = 0; i < N; ++i) {
    printf("[");
    for(int j = 0; j < N; ++j) {
      printf("%4.2g",rows_val[i][j]);
    }
    printf("]\n");
  }



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
    vec[i] = 1.0;

  long vec_byte = N*sizeof(vnum_type);
  std::cout << "Size of (one) vector: "
            << vec_byte*1e-6
            << " MB." << std::endl;

  float datasize_mb = (vec_byte*2 + mat_byte)*1e-6;
  float gflop = nnz*2-N;


  // perform multiplications
  mult(res,vec,val,ind,ptr,N);

  printf("result: [");

  for(int j = 0; j < N; ++j) {
    printf("%5.3g",res[j]);
  }
  printf("]\n");


  delete[] vec;
  delete[] res;
  delete[] ptr;
  delete[] ind;
  delete[] val;

  return 0;
}
