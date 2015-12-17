/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    blashelper.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME blashelper
// .SECTION Description


#ifdef NO_BLAS
 void DAXPY(const int& N, const double alpha, const double *X, const int incX, double *Y, const int incY){
  int i,cptX,cptY;
  cptX = 0;
  cptY = 0;
  for(i = 0; i < N; ++i){
    Y[cptY] += alpha * X[cptX];
    cptX += incX;
    cptY += incY;
  };
 };
 void SAXPY(const int N, const float alpha, const float *X, const int incX, float *Y, const int incY){
  int i,cptX,cptY;
  cptX = 0;
  cptY = 0;
  for(i = 0; i < N; ++i){
    Y[cptY] += alpha * X[cptX];
    cptX += incX;
    cptY += incY;
  };
 };

#else
 #ifdef __APPLE__
  #include   <Accelerate/Accelerate.h>
  #define DAXPY cblas_daxpy
  #define SAXPY cblas_saxpy
 #else
  #ifdef USE_CBLAS
   #define DAXPY cblas_daxpy
   #define SAXPY cblas_saxpy
   extern "C" {
    void DAXPY(const int& N, const double alpha, const double *X,
               const int incX, double *Y, const int incY);
    void SAXPY(const int N, const float alpha, const float *X,
               const int incX, float *Y, const int incY);
   }
  #else
   extern "C" {
    int daxpy_(const int *, double *, double *, int *, double *, int *);
    int saxpy_(int *,  float *,  float *, int *,  float *, int *);
   }
   //
   inline void  DAXPY(const int& N,  double alpha,  double *X,  int incX, double *Y,  int incY) {
     daxpy_(&N, &alpha, X, &incX, Y, &incY);
   };
   //
   inline void SAXPY( int& N,  float alpha,  float *X,  int incX, float *Y,  int incY) {
      saxpy_(&N, &alpha, X, &incX, Y, &incY);
   };
  #endif // USE_CBLAS
 #endif // __APPLE__
#endif // NO_BLAS
//
