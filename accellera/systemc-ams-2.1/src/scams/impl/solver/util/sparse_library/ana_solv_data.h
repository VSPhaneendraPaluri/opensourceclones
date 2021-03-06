/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.


   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

 *****************************************************************************/

/*****************************************************************************

 ana_solv_data.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 1715 $
 SVN last checkin  :  $Date: 2014-04-27 16:37:25 +0200 (Sun, 27 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: ana_solv_data.h 1715 2014-04-27 14:37:25Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef  ANA_SOLV_DATA_H
#define ANA_SOLV_DATA_H

#include "ma_typedef.h"

/****************************************/

typedef enum sca_algE
{
   EULER =0,
   TRAPEZ=1
}sca_algT;

/****************************************/

typedef struct sca_solv_dataS
{
      double h;					/* step size */
      double h_temp;  			/* temporary step size*/
      double h_diff;  			/* reciproke of the difference between h and h_temp */
      unsigned long size; 		/* number of equations */

      int variable_step_size;	/* = 0: step size constant, = 1: step size variable */

      double *r1;      			/*temporary vector*/
      double *r2;     			/*temporary vector*/
      double *xp;     			/*derivation of x-vector*/
      double *x_last; 			/*last state vector*/

      struct sparse *A;         /*square matrix A*/

      /*algorithm after first step*/
      sca_algT   algorithm;
      /*current active algorithm*/
      sca_algT   cur_algorithm;

      /***** data for euler *****/

      struct sparse *sZ_euler;	/* Z_euler = 1/h A + B */
      struct sparse *sW_euler;	/* W_euler = 1/h A */
      struct spcode *code_euler;	/* code for factorization/substitution Z_euler */

      /***** data for trapez *****/

      struct sparse *sZ_trapez;	/* Z_trapez = 2/h A + B */
      struct sparse *sW_trapez;	/* W_trapez = 2/h A */
      struct spcode *code_trapez;	/* code for factorization/substitution Z_trapez */

      long critical_row;		/* erroneous line in matrix Z which causes singularity */
      long critical_column;		/* erroneous column in matrix Z which causes singularity */

      /**** data for LR decomposition of matrix A ****/

      struct triangular *L;		/* left triangular matrix */
      struct triangular *R;		/* right triangular matrix */
      long rank_A;				/* rank of matrix A */

      /**** auxiliary field for the Woodbury cur_algorithm ****/

      double *aux_woodbury;		/* auxiliary vector for formula of Woodbury */
} sca_solv_data;

typedef struct sca_solv_checkpoint_dataS
{
	unsigned long size; 		/* number of equations */

	double* xp;
	double* x;

	sca_algT   algorithm;

}sca_solv_checkpoint_data;


/****************************************/



/****************************************/

#endif
