/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "GoodEnough",              /* Team name */

    "Mingqi Hou",     /* First member full name */
    "mingqi.hou@mail.utoronto.ca",  /* First member email address */

    "Yuan Feng",                   /* Second member full name (leave blank if none) */
    "michaelyuan.feng@mail.utoronto.ca"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * ECE 454 Students: Write your rotate functions here:
 */ 

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{int j=0;
		int subBlockDim = 16;
		int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
		int iBlockIndex0 = 0, jBlockIndex0 = 0;
		int dimSub1dim = dim*(dim-1);
		int idimj, dstjBaseIdx, dstjBaseIdxPreComput1, dstjBaseIdxPreComput2;
		int idimPreCompute;
		
		while(iBlockIndex0 < dim){
			idimPreCompute = iBlockIndex0 * dim;
			dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
			while(jBlockIndex0 < dim){
				dstjBaseIdxPreComput2 = dstjBaseIdxPreComput1 - dim * jBlockIndex0;
				for (j = jBlockIndex0; j < jBlockBoundary; j++){
					idimj = idimPreCompute+j;
					dstjBaseIdx = dstjBaseIdxPreComput2;
						dst[dstjBaseIdx] = src[idimj];
						dst[dstjBaseIdx+1] = src[idimj += dim];
						dst[dstjBaseIdx+2] = src[idimj += dim];
						dst[dstjBaseIdx+3] = src[idimj += dim];
						dst[dstjBaseIdx+4] = src[idimj += dim];
						dst[dstjBaseIdx+5] = src[idimj += dim];
						dst[dstjBaseIdx+6] = src[idimj += dim];
						dst[dstjBaseIdx+7] = src[idimj += dim];
						dst[dstjBaseIdx+8] = src[idimj += dim];
						dst[dstjBaseIdx+9] = src[idimj += dim];
						dst[dstjBaseIdx+10] = src[idimj += dim];
						dst[dstjBaseIdx+11] = src[idimj += dim];
						dst[dstjBaseIdx+12] = src[idimj += dim];
						dst[dstjBaseIdx+13] = src[idimj += dim];
						dst[dstjBaseIdx+14] = src[idimj += dim];
						dst[dstjBaseIdx+15] = src[idimj += dim];
					dstjBaseIdxPreComput2 -= dim;
				}
				jBlockIndex0 += subBlockDim;
				jBlockBoundary += subBlockDim;
			}
			iBlockIndex0 += subBlockDim;
			iBlockBoundary += subBlockDim;
			jBlockIndex0 = 0;
			jBlockBoundary = subBlockDim;
		}
}

char rotate_one_descr[] = "Unroll 4 - 32";
void attempt_one(int dim, pixel *src, pixel *dst) 
{
	int i=0, j=0;
    int subBlockDim = 32;
    int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
    int iBlockIndex0 = 0, jBlockIndex0 = 0;
    int dimSub1dim = dim*(dim-1);
    int idimj, dstjBaseIdx, dstjBaseIdxPreComput2;
    int idimPreCompute;
	int iBlockBoundaryUnroll;
	
    while(iBlockIndex0 < dim){
      idimPreCompute = iBlockIndex0 * dim;
	  iBlockBoundaryUnroll = iBlockBoundary -4;
	 // dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
      while(jBlockIndex0 < dim){
		dstjBaseIdxPreComput2 = dimSub1dim - dim * jBlockIndex0;
        for (j = jBlockIndex0; j < jBlockBoundary; j++){
          idimj = idimPreCompute+j;
		  dstjBaseIdx = dstjBaseIdxPreComput2;
          for (i = iBlockIndex0; i <= iBlockBoundaryUnroll; i+=4){ 
            dst[dstjBaseIdx+i] = src[idimj];
			dst[dstjBaseIdx+i+1] = src[idimj += dim];
			dst[dstjBaseIdx+i+2] = src[idimj += dim];
			dst[dstjBaseIdx+i+3] = src[idimj += dim];
            idimj += dim;
          }
          dstjBaseIdxPreComput2 -= dim;
        }
        jBlockIndex0 += subBlockDim;
        jBlockBoundary += subBlockDim;
      }
      iBlockIndex0 += subBlockDim;
      iBlockBoundary += subBlockDim;
      jBlockIndex0 = 0;
      jBlockBoundary = subBlockDim;
	
	}
}

char rotate_two_descr[] = "Unroll 4 - 16";
void attempt_two(int dim, pixel *src, pixel *dst) 
{
    int i=0, j=0;
    int subBlockDim = 32;
    int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
    int iBlockIndex0 = 0, jBlockIndex0 = 0;
    int dimSub1dim = dim*(dim-1);
    int idimj, dstjBaseIdx, dstjBaseIdxPreComput2;
    int idimPreCompute;
	int iBlockBoundaryUnroll;
	
    while(iBlockIndex0 < dim){
      idimPreCompute = iBlockIndex0 * dim;
	  iBlockBoundaryUnroll = iBlockBoundary -4;
	 // dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
      while(jBlockIndex0 < dim){
		dstjBaseIdxPreComput2 = dimSub1dim - dim * jBlockIndex0;
        for (j = jBlockIndex0; j < jBlockBoundary; j++){
          idimj = idimPreCompute+j;
		  dstjBaseIdx = dstjBaseIdxPreComput2;
          for (i = iBlockIndex0; i <= iBlockBoundaryUnroll; i+=4){ 
            dst[dstjBaseIdx+i] = src[idimj];
			dst[dstjBaseIdx+i+1] = src[idimj += dim];
			dst[dstjBaseIdx+i+2] = src[idimj += dim];
			dst[dstjBaseIdx+i+3] = src[idimj += dim];
            idimj += dim;
          }
          dstjBaseIdxPreComput2 -= dim;
        }
        jBlockIndex0 += subBlockDim;
        jBlockBoundary += subBlockDim;
      }
      iBlockIndex0 += subBlockDim;
      iBlockBoundary += subBlockDim;
      jBlockIndex0 = 0;
      jBlockBoundary = subBlockDim;
	}
}

char rotate_three_descr[] = "unroll 8 - 32";
void attempt_three(int dim, pixel *src, pixel *dst) 
{
    int i=0, j=0;
    int subBlockDim = 32;
    int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
    int iBlockIndex0 = 0, jBlockIndex0 = 0;
    int dimSub1dim = dim*(dim-1);
    int idimj, dstjBaseIdx, dstjBaseIdxPreComput2;
    int idimPreCompute;
	int iBlockBoundaryUnroll;
	
    while(iBlockIndex0 < dim){
      idimPreCompute = iBlockIndex0 * dim;
	  iBlockBoundaryUnroll = iBlockBoundary -8;
	 // dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
      while(jBlockIndex0 < dim){
		dstjBaseIdxPreComput2 = dimSub1dim - dim * jBlockIndex0;
        for (j = jBlockIndex0; j < jBlockBoundary; j++){
          idimj = idimPreCompute+j;
		  dstjBaseIdx = dstjBaseIdxPreComput2;
          for (i = iBlockIndex0; i <= iBlockBoundaryUnroll; i+=8){ 
            dst[dstjBaseIdx+i] = src[idimj];
			dst[dstjBaseIdx+i+1] = src[idimj += dim];
			dst[dstjBaseIdx+i+2] = src[idimj += dim];
			dst[dstjBaseIdx+i+3] = src[idimj += dim];
			dst[dstjBaseIdx+i+4] = src[idimj += dim];
			dst[dstjBaseIdx+i+5] = src[idimj += dim];
			dst[dstjBaseIdx+i+6] = src[idimj += dim];
			dst[dstjBaseIdx+i+7] = src[idimj += dim];

            idimj += dim;
          }
          dstjBaseIdxPreComput2 -= dim;
        }
        jBlockIndex0 += subBlockDim;
        jBlockBoundary += subBlockDim;
      }
      iBlockIndex0 += subBlockDim;
      iBlockBoundary += subBlockDim;
      jBlockIndex0 = 0;
      jBlockBoundary = subBlockDim;
	}
}

char rotate_four_descr[] = "unroll 8 - 16";
void attempt_four(int dim, pixel *src, pixel *dst) 
{
    int i=0, j=0;
    int subBlockDim = 16;
    int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
    int iBlockIndex0 = 0, jBlockIndex0 = 0;
    int dimSub1dim = dim*(dim-1);
    int idimj, dstjBaseIdx, dstjBaseIdxPreComput2;
    int idimPreCompute;
	int iBlockBoundaryUnroll;
	
    while(iBlockIndex0 < dim){
      idimPreCompute = iBlockIndex0 * dim;
	  iBlockBoundaryUnroll = iBlockBoundary -8;
	 // dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
      while(jBlockIndex0 < dim){
		dstjBaseIdxPreComput2 = dimSub1dim - dim * jBlockIndex0;
        for (j = jBlockIndex0; j < jBlockBoundary; j++){
          idimj = idimPreCompute+j;
		  dstjBaseIdx = dstjBaseIdxPreComput2;
          for (i = iBlockIndex0; i <= iBlockBoundaryUnroll; i+=8){ 
            dst[dstjBaseIdx+i] = src[idimj];
			dst[dstjBaseIdx+i+1] = src[idimj += dim];
			dst[dstjBaseIdx+i+2] = src[idimj += dim];
			dst[dstjBaseIdx+i+3] = src[idimj += dim];
			dst[dstjBaseIdx+i+4] = src[idimj += dim];
			dst[dstjBaseIdx+i+5] = src[idimj += dim];
			dst[dstjBaseIdx+i+6] = src[idimj += dim];
			dst[dstjBaseIdx+i+7] = src[idimj += dim];

            idimj += dim;
          }
          dstjBaseIdxPreComput2 -= dim;
        }
        jBlockIndex0 += subBlockDim;
        jBlockBoundary += subBlockDim;
      }
      iBlockIndex0 += subBlockDim;
      iBlockBoundary += subBlockDim;
      jBlockIndex0 = 0;
      jBlockBoundary = subBlockDim;
	}
}

char rotate_five_descr[] = "base line - 16k";
void attempt_five(int dim, pixel *src, pixel *dst) 
{
    int i=0, j=0;
    int subBlockDim = 16;
    int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
    int iBlockIndex0 = 0, jBlockIndex0 = 0;
    int dimSub1dim = dim*(dim-1);
    int idim, dstjBaseIdx, jdim;
    int idimPreCompute;
    while(iBlockIndex0 < dim){

      idimPreCompute = iBlockIndex0 * dim;
      while(jBlockIndex0 < dim){


        jdim = dim * jBlockIndex0;



        for (j = jBlockIndex0; j < jBlockBoundary; j++){
          dstjBaseIdx = dimSub1dim -jdim;
          idim = idimPreCompute;
          for (i = iBlockIndex0; i < iBlockBoundary; i++){            
            dst[dstjBaseIdx+i] = src[idim+j];
            idim += dim;
          }
          jdim += dim;
        }

        jBlockIndex0 += subBlockDim;
        jBlockBoundary += subBlockDim;
      }
      iBlockIndex0 += subBlockDim;
      iBlockBoundary += subBlockDim;
      jBlockIndex0 = 0;
      jBlockBoundary = subBlockDim;
    }
}

char rotate_six_descr[] = "base line - 32k";
void attempt_six(int dim, pixel *src, pixel *dst) 
{
     int i=0, j=0;
    int subBlockDim = 32;
    int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
    int iBlockIndex0 = 0, jBlockIndex0 = 0;
    int dimSub1dim = dim*(dim-1);
    int idim, dstjBaseIdx, jdim;
    int idimPreCompute;
    while(iBlockIndex0 < dim){

      idimPreCompute = iBlockIndex0 * dim;
      while(jBlockIndex0 < dim){


        jdim = dim * jBlockIndex0;



        for (j = jBlockIndex0; j < jBlockBoundary; j++){
          dstjBaseIdx = dimSub1dim -jdim;
          idim = idimPreCompute;
          for (i = iBlockIndex0; i < iBlockBoundary; i++){            
            dst[dstjBaseIdx+i] = src[idim+j];
            idim += dim;
          }
          jdim += dim;
        }

        jBlockIndex0 += subBlockDim;
        jBlockBoundary += subBlockDim;
      }
      iBlockIndex0 += subBlockDim;
      iBlockBoundary += subBlockDim;
      jBlockIndex0 = 0;
      jBlockBoundary = subBlockDim;
    }
}


char rotate_seven_descr[] = "Unroll 32 - 32";
void attempt_seven(int dim, pixel *src, pixel *dst) 
{
		int j=0;
		int subBlockDim = 32;
		int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
		int iBlockIndex0 = 0, jBlockIndex0 = 0;
		int dimSub1dim = dim*(dim-1);
		int idimj, dstjBaseIdx, dstjBaseIdxPreComput1, dstjBaseIdxPreComput2;
		int idimPreCompute;
		
		while(iBlockIndex0 < dim){
			idimPreCompute = iBlockIndex0 * dim;
			dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
			while(jBlockIndex0 < dim){
				dstjBaseIdxPreComput2 = dstjBaseIdxPreComput1 - dim * jBlockIndex0;
				for (j = jBlockIndex0; j < jBlockBoundary; j++){
					idimj = idimPreCompute+j;
					dstjBaseIdx = dstjBaseIdxPreComput2;
						dst[dstjBaseIdx] = src[idimj];
						dst[dstjBaseIdx+1] = src[idimj += dim];
						dst[dstjBaseIdx+2] = src[idimj += dim];
						dst[dstjBaseIdx+3] = src[idimj += dim];
						dst[dstjBaseIdx+4] = src[idimj += dim];
						dst[dstjBaseIdx+5] = src[idimj += dim];
						dst[dstjBaseIdx+6] = src[idimj += dim];
						dst[dstjBaseIdx+7] = src[idimj += dim];
						dst[dstjBaseIdx+8] = src[idimj += dim];
						dst[dstjBaseIdx+9] = src[idimj += dim];
						dst[dstjBaseIdx+10] = src[idimj += dim];
						dst[dstjBaseIdx+11] = src[idimj += dim];
						dst[dstjBaseIdx+12] = src[idimj += dim];
						dst[dstjBaseIdx+13] = src[idimj += dim];
						dst[dstjBaseIdx+14] = src[idimj += dim];
						dst[dstjBaseIdx+15] = src[idimj += dim];
						dst[dstjBaseIdx+16] = src[idimj += dim];
						dst[dstjBaseIdx+17] = src[idimj += dim];
						dst[dstjBaseIdx+18] = src[idimj += dim];
						dst[dstjBaseIdx+19] = src[idimj += dim];
						dst[dstjBaseIdx+20] = src[idimj += dim];
						dst[dstjBaseIdx+21] = src[idimj += dim];
						dst[dstjBaseIdx+22] = src[idimj += dim];
						dst[dstjBaseIdx+23] = src[idimj += dim];
						dst[dstjBaseIdx+24] = src[idimj += dim];
						dst[dstjBaseIdx+25] = src[idimj += dim];
						dst[dstjBaseIdx+26] = src[idimj += dim];
						dst[dstjBaseIdx+27] = src[idimj += dim];
						dst[dstjBaseIdx+28] = src[idimj += dim];
						dst[dstjBaseIdx+29] = src[idimj += dim];
						dst[dstjBaseIdx+30] = src[idimj += dim];
						dst[dstjBaseIdx+31] = src[idimj += dim];
					dstjBaseIdxPreComput2 -= dim;
				}
				jBlockIndex0 += subBlockDim;
				jBlockBoundary += subBlockDim;
			}
			iBlockIndex0 += subBlockDim;
			iBlockBoundary += subBlockDim;
			jBlockIndex0 = 0;
			jBlockBoundary = subBlockDim;
		}
}

char rotate_eight_descr[] = "unroll 16 -16";
void attempt_eight(int dim, pixel *src, pixel *dst) 
{int j=0;
		int subBlockDim = 16;
		int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
		int iBlockIndex0 = 0, jBlockIndex0 = 0;
		int dimSub1dim = dim*(dim-1);
		int idimj, dstjBaseIdx, dstjBaseIdxPreComput1, dstjBaseIdxPreComput2;
		int idimPreCompute;
		
		while(iBlockIndex0 < dim){
			idimPreCompute = iBlockIndex0 * dim;
			dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
			while(jBlockIndex0 < dim){
				dstjBaseIdxPreComput2 = dstjBaseIdxPreComput1 - dim * jBlockIndex0;
				for (j = jBlockIndex0; j < jBlockBoundary; j++){
					idimj = idimPreCompute+j;
					dstjBaseIdx = dstjBaseIdxPreComput2;
						dst[dstjBaseIdx] = src[idimj];
						dst[dstjBaseIdx+1] = src[idimj += dim];
						dst[dstjBaseIdx+2] = src[idimj += dim];
						dst[dstjBaseIdx+3] = src[idimj += dim];
						dst[dstjBaseIdx+4] = src[idimj += dim];
						dst[dstjBaseIdx+5] = src[idimj += dim];
						dst[dstjBaseIdx+6] = src[idimj += dim];
						dst[dstjBaseIdx+7] = src[idimj += dim];
						dst[dstjBaseIdx+8] = src[idimj += dim];
						dst[dstjBaseIdx+9] = src[idimj += dim];
						dst[dstjBaseIdx+10] = src[idimj += dim];
						dst[dstjBaseIdx+11] = src[idimj += dim];
						dst[dstjBaseIdx+12] = src[idimj += dim];
						dst[dstjBaseIdx+13] = src[idimj += dim];
						dst[dstjBaseIdx+14] = src[idimj += dim];
						dst[dstjBaseIdx+15] = src[idimj += dim];
					dstjBaseIdxPreComput2 -= dim;
				}
				jBlockIndex0 += subBlockDim;
				jBlockBoundary += subBlockDim;
			}
			iBlockIndex0 += subBlockDim;
			iBlockBoundary += subBlockDim;
			jBlockIndex0 = 0;
			jBlockBoundary = subBlockDim;
		}
}

char rotate_nine_descr[] = "unroll 8 combo";
void attempt_nine(int dim, pixel *src, pixel *dst) 
{
	if(dim <4097){
		int j=0;
		int subBlockDim = 32;
		int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
		int iBlockIndex0 = 0, jBlockIndex0 = 0;
		int dimSub1dim = dim*(dim-1);
		int idimj, dstjBaseIdx, dstjBaseIdxPreComput1, dstjBaseIdxPreComput2;
		int idimPreCompute;
		
		while(iBlockIndex0 < dim){
			idimPreCompute = iBlockIndex0 * dim;
			dstjBaseIdxPreComput1 = dimSub1dim + iBlockIndex0;
			while(jBlockIndex0 < dim){
				dstjBaseIdxPreComput2 = dstjBaseIdxPreComput1 - dim * jBlockIndex0;
				for (j = jBlockIndex0; j < jBlockBoundary; j++){
					idimj = idimPreCompute+j;
					dstjBaseIdx = dstjBaseIdxPreComput2;
						dst[dstjBaseIdx] = src[idimj];
						dst[dstjBaseIdx+1] = src[idimj += dim];
						dst[dstjBaseIdx+2] = src[idimj += dim];
						dst[dstjBaseIdx+3] = src[idimj += dim];
						dst[dstjBaseIdx+4] = src[idimj += dim];
						dst[dstjBaseIdx+5] = src[idimj += dim];
						dst[dstjBaseIdx+6] = src[idimj += dim];
						dst[dstjBaseIdx+7] = src[idimj += dim];
						dst[dstjBaseIdx+8] = src[idimj += dim];
						dst[dstjBaseIdx+9] = src[idimj += dim];
						dst[dstjBaseIdx+10] = src[idimj += dim];
						dst[dstjBaseIdx+11] = src[idimj += dim];
						dst[dstjBaseIdx+12] = src[idimj += dim];
						dst[dstjBaseIdx+13] = src[idimj += dim];
						dst[dstjBaseIdx+14] = src[idimj += dim];
						dst[dstjBaseIdx+15] = src[idimj += dim];
						dst[dstjBaseIdx+16] = src[idimj += dim];
						dst[dstjBaseIdx+17] = src[idimj += dim];
						dst[dstjBaseIdx+18] = src[idimj += dim];
						dst[dstjBaseIdx+19] = src[idimj += dim];
						dst[dstjBaseIdx+20] = src[idimj += dim];
						dst[dstjBaseIdx+21] = src[idimj += dim];
						dst[dstjBaseIdx+22] = src[idimj += dim];
						dst[dstjBaseIdx+23] = src[idimj += dim];
						dst[dstjBaseIdx+24] = src[idimj += dim];
						dst[dstjBaseIdx+25] = src[idimj += dim];
						dst[dstjBaseIdx+26] = src[idimj += dim];
						dst[dstjBaseIdx+27] = src[idimj += dim];
						dst[dstjBaseIdx+28] = src[idimj += dim];
						dst[dstjBaseIdx+29] = src[idimj += dim];
						dst[dstjBaseIdx+30] = src[idimj += dim];
						dst[dstjBaseIdx+31] = src[idimj += dim];
					dstjBaseIdxPreComput2 -= dim;
				}
				jBlockIndex0 += subBlockDim;
				jBlockBoundary += subBlockDim;
			}
			iBlockIndex0 += subBlockDim;
			iBlockBoundary += subBlockDim;
			jBlockIndex0 = 0;
			jBlockBoundary = subBlockDim;
		}
	}else{
		int i=0, j=0;
		int subBlockDim = 16;
		int iBlockBoundary = subBlockDim, jBlockBoundary = subBlockDim;
		int iBlockIndex0 = 0, jBlockIndex0 = 0;
		int dimSub1dim = dim*(dim-1);
		int idim, dstjBaseIdx, jdim;
		int idimPreCompute;
		while(iBlockIndex0 < dim){
			idimPreCompute = iBlockIndex0 * dim;
			while(jBlockIndex0 < dim){
				jdim = dim * jBlockIndex0;
				for (j = jBlockIndex0; j < jBlockBoundary; j++){
					dstjBaseIdx = dimSub1dim -jdim;
					idim = idimPreCompute;
					for (i = iBlockIndex0; i < iBlockBoundary; i++){            
						dst[dstjBaseIdx+i] = src[idim+j];
						idim += dim;
					}
					jdim += dim;
				}		
				jBlockIndex0 += subBlockDim;
				jBlockBoundary += subBlockDim;
			}
			iBlockIndex0 += subBlockDim;
			iBlockBoundary += subBlockDim;
			jBlockIndex0 = 0;
			jBlockBoundary = subBlockDim;
		}
	}
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    //add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
   add_rotate_function(&attempt_one, rotate_one_descr);   

  add_rotate_function(&attempt_two, rotate_two_descr);   
   add_rotate_function(&attempt_three, rotate_three_descr);   
    add_rotate_function(&attempt_four, rotate_four_descr);   
    add_rotate_function(&attempt_five, rotate_five_descr);   
    add_rotate_function(&attempt_six, rotate_six_descr);   
   add_rotate_function(&attempt_seven, rotate_seven_descr);   
    add_rotate_function(&attempt_eight, rotate_eight_descr);   
  //add_rotate_function(&attempt_nine, rotate_nine_descr);   
    //add_rotate_function(&attempt_ten, rotate_ten_descr);   
    //add_rotate_function(&attempt_eleven, rotate_eleven_descr);   

    /* ... Register additional rotate functions here */
}

