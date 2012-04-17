/* SCILAB Computational function  */
/*     Copyright INRIA */
/*     Generated by Code_Generation toolbox of Scicos with scilab-4.1.2 */
/*     date : 4-Apr-2012 */

/* ---- Headers ---- */
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "machine.h"
#include "link.h"
#include "scicos.h"


/* prototype of 'cstblk4' (type 4) */
void cstblk4(scicos_block *, int );

/* prototype of 'dsslti4' (type 4) */
void dsslti4(scicos_block *, int );

/* prototype of 'summation' (type 4) */
void summation(scicos_block *, int );

/* Table of constant values */
static integer nrd_0 = 0;
static integer nrd_1 = 1;
static integer nrd_2 = 2;
static integer nrd_3 = 3;
static integer nrd_4 = 4;
static integer nrd_10 = 10;
static integer nrd_11 = 11;
static integer nrd_81 = 81;
static integer nrd_82 = 82;
static integer nrd_84 = 84;
static integer nrd_811 = 811;
static integer nrd_812 = 812;
static integer nrd_814 = 814;

/* Some general static variables */
static double zero=0;
static double w[1];
static int aaa=0, bbb=0;

/*---------------------------------------- Block Computational function */ 
int  SuperBlock(scicos_block *block, int flag)
{
  /* declaration of local variables for that block struct */
  double* z      = block->z;
  void **ozptr   = block->ozptr;
  void **oparptr = block->oparptr;
  int nopar      = block->nopar;
  double* x      = block->x;
  double* xd     = block->xd;
  double* res    = block->res;
  void** u       = block->inptr;
  void** y       = block->outptr;
  int nevprt     = block->nevprt;
  double* rpar   = block->rpar;
  int nrpar      = block->nrpar;
  int* ipar      = block->ipar;
  int nipar      = block->nipar;
  double* g      = block->g;
  int* mode      = block->mode;
  void **work    = block->work;

  double t     = get_scicos_time();
  int    phase = get_phase_simulation();

  int kf;
  int i;
  int* reentryflag;

  double *args[100];
  int local_flag;
  int nport;
  void **SuperBlock_block_outtbptr;
  scicos_block *block_SuperBlock;

  /*  work of blocks are catenated at the end of z */
  work = (void **)(z+1);

  /*  SuperBlock_block_outtbptr is catenated at the end of oz */
  SuperBlock_block_outtbptr = (void **)(ozptr+0);

  /* struct of all blocks are stored in work of that block struct */
  block_SuperBlock=(scicos_block*) *block->work;

  /* Copy inputs in the block outtb */
  memcpy(*(SuperBlock_block_outtbptr+3),*(u+0),1*sizeof(double));

  if (flag != 4 && flag != 6 && flag != 5){
    reentryflag=(int*) ((scicos_block *)(*block->work)+6);
    if (*reentryflag==0){
      *reentryflag=1;

      /* set blk struc. of 'cstblk4' (type 4 - blk nb 1) */
      block_SuperBlock[0].outptr[0] = SuperBlock_block_outtbptr[0];
      block_SuperBlock[0].z         = &(z[0]);
      block_SuperBlock[0].x         = &(zero);
      block_SuperBlock[0].xd        = w;
      block_SuperBlock[0].g         = &(zero);
      block_SuperBlock[0].rpar      = &(rpar[0]);
      block_SuperBlock[0].work      = (void **)(((double *)work)+0);

      /* set blk struc. of 'dsslti4' (type 4 - blk nb 2) */
      block_SuperBlock[1].inptr[0]  = SuperBlock_block_outtbptr[1];
      block_SuperBlock[1].outptr[0] = SuperBlock_block_outtbptr[2];
      block_SuperBlock[1].z         = &(z[0]);
      block_SuperBlock[1].x         = &(zero);
      block_SuperBlock[1].xd        = w;
      block_SuperBlock[1].g         = &(zero);
      block_SuperBlock[1].rpar      = &(rpar[1]);
      block_SuperBlock[1].work      = (void **)(((double *)work)+1);

      /* set blk struc. of 'summation' (type 4 - blk nb 3) */
      block_SuperBlock[2].inptr[0]  = SuperBlock_block_outtbptr[0];
      block_SuperBlock[2].inptr[1]  = SuperBlock_block_outtbptr[3];
      block_SuperBlock[2].outptr[0] = SuperBlock_block_outtbptr[1];
      block_SuperBlock[2].z         = &(z[1]);
      block_SuperBlock[2].x         = &(zero);
      block_SuperBlock[2].xd        = w;
      block_SuperBlock[2].g         = &(zero);
      block_SuperBlock[2].ipar      = &(ipar[0]);
      block_SuperBlock[2].work      = (void **)(((double *)work)+2);

      /* set blk struc. of 'actionneur1' (type 0 - blk nb 4) */
      block_SuperBlock[3].inptr[0]  = SuperBlock_block_outtbptr[2];
      block_SuperBlock[3].z         = &(z[1]);
      block_SuperBlock[3].x         = &(zero);
      block_SuperBlock[3].xd        = w;
      block_SuperBlock[3].g         = &(zero);
      block_SuperBlock[3].work      = (void **)(((double *)work)+3);

      /* set blk struc. of 'bidon' (type 0 - blk nb 5) */
      block_SuperBlock[4].z         = &(z[1]);
      block_SuperBlock[4].x         = &(zero);
      block_SuperBlock[4].xd        = w;
      block_SuperBlock[4].g         = &(zero);
      block_SuperBlock[4].work      = (void **)(((double *)work)+4);

      /* set blk struc. of 'capteur1' (type 0 - blk nb 6) */
      block_SuperBlock[5].outptr[0] = SuperBlock_block_outtbptr[3];
      block_SuperBlock[5].z         = &(z[1]);
      block_SuperBlock[5].x         = &(zero);
      block_SuperBlock[5].xd        = w;
      block_SuperBlock[5].g         = &(zero);
      block_SuperBlock[5].work      = (void **)(((double *)work)+5);
    }
  }

  if (flag == 1) { /* Output computation */
    switch (nevprt) {
      case 1 : /* Blocks activated on the event number 1 */
        /* Call of 'dsslti4' (type 4 - blk nb 2) */
        block_SuperBlock[1].nevprt = 1;
        local_flag = 1;
        dsslti4(&block_SuperBlock[1],local_flag);
        if(local_flag < 0) return(5 - local_flag);
    
        /* Call of 'summation' (type 4 - blk nb 3) */
        block_SuperBlock[2].nevprt = 2;
        local_flag = 1;
        summation(&block_SuperBlock[2],local_flag);
        if(local_flag < 0) return(5 - local_flag);
    
      break;

    }
  }
  else if (flag == 2) { /* Discrete state computation */
    switch (nevprt) {
      case 1 : /* Blocks activated on the event number 1 */
        /* Call of 'dsslti4' (type 4 - blk nb 2) */
        block_SuperBlock[1].nevprt = 1;
        local_flag = 2;
        dsslti4(&block_SuperBlock[1],local_flag);
        if(local_flag < 0) return(5 - local_flag);
    
      break;

    }
  }
  else if (flag == 4) { /* Initialization */
    if ((*block->work=scicos_malloc(sizeof(scicos_block)*6+sizeof(int)))== NULL) return 0;
    reentryflag=(int*) ((scicos_block *)(*block->work)+6);
    *reentryflag=0;
    block_SuperBlock=(scicos_block*) *block->work;

    /* set blk struc. of 'cstblk4' (type 4 - blk nb 1) */
    block_SuperBlock[0].type   = 4;
    block_SuperBlock[0].ztyp   = 0;
    block_SuperBlock[0].ng     = 0;
    block_SuperBlock[0].nx     = 1;
    block_SuperBlock[0].x      = &(zero);
    block_SuperBlock[0].xd     = w;
    block_SuperBlock[0].g      = &(zero);
    block_SuperBlock[0].nz     = 0;
    block_SuperBlock[0].noz    = 0;
    block_SuperBlock[0].nrpar  = 1;
    block_SuperBlock[0].nopar  = 0;
    block_SuperBlock[0].nipar  = 0;
    block_SuperBlock[0].nin    = 0;
    block_SuperBlock[0].nout   = 1;
    block_SuperBlock[0].nevout = 0;
    block_SuperBlock[0].nmode  = 0;
    if ((block_SuperBlock[0].evout  = calloc(block_SuperBlock[0].nevout,sizeof(double)))== NULL) return 0;
    if ((block_SuperBlock[0].insz   = malloc(3*sizeof(int)*block_SuperBlock[0].nin))== NULL) return 0;
    if ((block_SuperBlock[0].inptr  = malloc(sizeof(void *)*block_SuperBlock[0].nin))== NULL) return 0;
    if ((block_SuperBlock[0].outsz  = malloc(3*sizeof(int)*block_SuperBlock[0].nout))== NULL) return 0;
    if ((block_SuperBlock[0].outptr = malloc(sizeof(void*)*block_SuperBlock[0].nout))== NULL) return 0;
    block_SuperBlock[0].outptr[0] = SuperBlock_block_outtbptr[0];
    block_SuperBlock[0].outsz[0]  = 1;
    block_SuperBlock[0].outsz[1]  = 1;
    block_SuperBlock[0].outsz[2]  = SCSREAL_N;
    block_SuperBlock[0].z         = &(z[0]);
    block_SuperBlock[0].rpar      = &(rpar[0]);
    block_SuperBlock[0].work      = (void **)(((double *)work)+0);
    block_SuperBlock[0].nevprt    = nevprt;

    /* set blk struc. of 'dsslti4' (type 4 - blk nb 2) */
    block_SuperBlock[1].type   = 4;
    block_SuperBlock[1].ztyp   = 0;
    block_SuperBlock[1].ng     = 0;
    block_SuperBlock[1].nx     = 1;
    block_SuperBlock[1].x      = &(zero);
    block_SuperBlock[1].xd     = w;
    block_SuperBlock[1].g      = &(zero);
    block_SuperBlock[1].nz     = 1;
    block_SuperBlock[1].noz    = 0;
    block_SuperBlock[1].nrpar  = 4;
    block_SuperBlock[1].nopar  = 0;
    block_SuperBlock[1].nipar  = 0;
    block_SuperBlock[1].nin    = 1;
    block_SuperBlock[1].nout   = 1;
    block_SuperBlock[1].nevout = 0;
    block_SuperBlock[1].nmode  = 0;
    if ((block_SuperBlock[1].evout  = calloc(block_SuperBlock[1].nevout,sizeof(double)))== NULL) return 0;
    if ((block_SuperBlock[1].insz   = malloc(3*sizeof(int)*block_SuperBlock[1].nin))== NULL) return 0;
    if ((block_SuperBlock[1].inptr  = malloc(sizeof(void *)*block_SuperBlock[1].nin))== NULL) return 0;
    block_SuperBlock[1].inptr[0]  = SuperBlock_block_outtbptr[1];
    block_SuperBlock[1].insz[0]   = 1;
    block_SuperBlock[1].insz[1]   = 1;
    block_SuperBlock[1].insz[2]   = SCSREAL_N;
    if ((block_SuperBlock[1].outsz  = malloc(3*sizeof(int)*block_SuperBlock[1].nout))== NULL) return 0;
    if ((block_SuperBlock[1].outptr = malloc(sizeof(void*)*block_SuperBlock[1].nout))== NULL) return 0;
    block_SuperBlock[1].outptr[0] = SuperBlock_block_outtbptr[2];
    block_SuperBlock[1].outsz[0]  = 1;
    block_SuperBlock[1].outsz[1]  = 1;
    block_SuperBlock[1].outsz[2]  = SCSREAL_N;
    block_SuperBlock[1].z         = &(z[0]);
    block_SuperBlock[1].rpar      = &(rpar[1]);
    block_SuperBlock[1].work      = (void **)(((double *)work)+1);
    block_SuperBlock[1].nevprt    = nevprt;

    /* set blk struc. of 'summation' (type 4 - blk nb 3) */
    block_SuperBlock[2].type   = 4;
    block_SuperBlock[2].ztyp   = 0;
    block_SuperBlock[2].ng     = 0;
    block_SuperBlock[2].nx     = 1;
    block_SuperBlock[2].x      = &(zero);
    block_SuperBlock[2].xd     = w;
    block_SuperBlock[2].g      = &(zero);
    block_SuperBlock[2].nz     = 0;
    block_SuperBlock[2].noz    = 0;
    block_SuperBlock[2].nrpar  = 0;
    block_SuperBlock[2].nopar  = 0;
    block_SuperBlock[2].nipar  = 2;
    block_SuperBlock[2].nin    = 2;
    block_SuperBlock[2].nout   = 1;
    block_SuperBlock[2].nevout = 0;
    block_SuperBlock[2].nmode  = 0;
    if ((block_SuperBlock[2].evout  = calloc(block_SuperBlock[2].nevout,sizeof(double)))== NULL) return 0;
    if ((block_SuperBlock[2].insz   = malloc(3*sizeof(int)*block_SuperBlock[2].nin))== NULL) return 0;
    if ((block_SuperBlock[2].inptr  = malloc(sizeof(void *)*block_SuperBlock[2].nin))== NULL) return 0;
    block_SuperBlock[2].inptr[0]  = SuperBlock_block_outtbptr[0];
    block_SuperBlock[2].inptr[1]  = SuperBlock_block_outtbptr[3];
    block_SuperBlock[2].insz[0]   = 1;
    block_SuperBlock[2].insz[1]   = 1;
    block_SuperBlock[2].insz[2]   = 1;
    block_SuperBlock[2].insz[3]   = 1;
    block_SuperBlock[2].insz[4]   = SCSREAL_N;
    block_SuperBlock[2].insz[5]   = SCSREAL_N;
    if ((block_SuperBlock[2].outsz  = malloc(3*sizeof(int)*block_SuperBlock[2].nout))== NULL) return 0;
    if ((block_SuperBlock[2].outptr = malloc(sizeof(void*)*block_SuperBlock[2].nout))== NULL) return 0;
    block_SuperBlock[2].outptr[0] = SuperBlock_block_outtbptr[1];
    block_SuperBlock[2].outsz[0]  = 1;
    block_SuperBlock[2].outsz[1]  = 1;
    block_SuperBlock[2].outsz[2]  = SCSREAL_N;
    block_SuperBlock[2].z         = &(z[1]);
    block_SuperBlock[2].ipar      = &(ipar[0]);
    block_SuperBlock[2].work      = (void **)(((double *)work)+2);
    block_SuperBlock[2].nevprt    = nevprt;

    /* set blk struc. of 'actionneur1' (type 0 - blk nb 4) */
    block_SuperBlock[3].type   = 0;
    block_SuperBlock[3].ztyp   = 0;
    block_SuperBlock[3].ng     = 0;
    block_SuperBlock[3].nx     = 1;
    block_SuperBlock[3].x      = &(zero);
    block_SuperBlock[3].xd     = w;
    block_SuperBlock[3].g      = &(zero);
    block_SuperBlock[3].nz     = 0;
    block_SuperBlock[3].noz    = 0;
    block_SuperBlock[3].nrpar  = 0;
    block_SuperBlock[3].nopar  = 0;
    block_SuperBlock[3].nipar  = 1;
    block_SuperBlock[3].nin    = 1;
    block_SuperBlock[3].nout   = 0;
    block_SuperBlock[3].nevout = 0;
    block_SuperBlock[3].nmode  = 0;
    if ((block_SuperBlock[3].evout  = calloc(block_SuperBlock[3].nevout,sizeof(double)))== NULL) return 0;
    if ((block_SuperBlock[3].insz   = malloc(3*sizeof(int)*block_SuperBlock[3].nin))== NULL) return 0;
    if ((block_SuperBlock[3].inptr  = malloc(sizeof(void *)*block_SuperBlock[3].nin))== NULL) return 0;
    block_SuperBlock[3].inptr[0]  = SuperBlock_block_outtbptr[2];
    block_SuperBlock[3].insz[0]   = 1;
    block_SuperBlock[3].insz[1]   = 1;
    block_SuperBlock[3].insz[2]   = SCSREAL_N;
    if ((block_SuperBlock[3].outsz  = malloc(3*sizeof(int)*block_SuperBlock[3].nout))== NULL) return 0;
    if ((block_SuperBlock[3].outptr = malloc(sizeof(void*)*block_SuperBlock[3].nout))== NULL) return 0;
    block_SuperBlock[3].z         = &(z[1]);
    block_SuperBlock[3].work      = (void **)(((double *)work)+3);
    block_SuperBlock[3].nevprt    = nevprt;

    /* set blk struc. of 'bidon' (type 0 - blk nb 5) */
    block_SuperBlock[4].type   = 0;
    block_SuperBlock[4].ztyp   = 0;
    block_SuperBlock[4].ng     = 0;
    block_SuperBlock[4].nx     = 1;
    block_SuperBlock[4].x      = &(zero);
    block_SuperBlock[4].xd     = w;
    block_SuperBlock[4].g      = &(zero);
    block_SuperBlock[4].nz     = 0;
    block_SuperBlock[4].noz    = 0;
    block_SuperBlock[4].nrpar  = 0;
    block_SuperBlock[4].nopar  = 0;
    block_SuperBlock[4].nipar  = 1;
    block_SuperBlock[4].nin    = 0;
    block_SuperBlock[4].nout   = 0;
    block_SuperBlock[4].nevout = 1;
    block_SuperBlock[4].nmode  = 0;
    if ((block_SuperBlock[4].evout  = calloc(block_SuperBlock[4].nevout,sizeof(double)))== NULL) return 0;
    if ((block_SuperBlock[4].insz   = malloc(3*sizeof(int)*block_SuperBlock[4].nin))== NULL) return 0;
    if ((block_SuperBlock[4].inptr  = malloc(sizeof(void *)*block_SuperBlock[4].nin))== NULL) return 0;
    if ((block_SuperBlock[4].outsz  = malloc(3*sizeof(int)*block_SuperBlock[4].nout))== NULL) return 0;
    if ((block_SuperBlock[4].outptr = malloc(sizeof(void*)*block_SuperBlock[4].nout))== NULL) return 0;
    block_SuperBlock[4].z         = &(z[1]);
    block_SuperBlock[4].work      = (void **)(((double *)work)+4);
    block_SuperBlock[4].nevprt    = nevprt;

    /* set blk struc. of 'capteur1' (type 0 - blk nb 6) */
    block_SuperBlock[5].type   = 0;
    block_SuperBlock[5].ztyp   = 0;
    block_SuperBlock[5].ng     = 0;
    block_SuperBlock[5].nx     = 1;
    block_SuperBlock[5].x      = &(zero);
    block_SuperBlock[5].xd     = w;
    block_SuperBlock[5].g      = &(zero);
    block_SuperBlock[5].nz     = 0;
    block_SuperBlock[5].noz    = 0;
    block_SuperBlock[5].nrpar  = 0;
    block_SuperBlock[5].nopar  = 0;
    block_SuperBlock[5].nipar  = 1;
    block_SuperBlock[5].nin    = 0;
    block_SuperBlock[5].nout   = 1;
    block_SuperBlock[5].nevout = 0;
    block_SuperBlock[5].nmode  = 0;
    if ((block_SuperBlock[5].evout  = calloc(block_SuperBlock[5].nevout,sizeof(double)))== NULL) return 0;
    if ((block_SuperBlock[5].insz   = malloc(3*sizeof(int)*block_SuperBlock[5].nin))== NULL) return 0;
    if ((block_SuperBlock[5].inptr  = malloc(sizeof(void *)*block_SuperBlock[5].nin))== NULL) return 0;
    if ((block_SuperBlock[5].outsz  = malloc(3*sizeof(int)*block_SuperBlock[5].nout))== NULL) return 0;
    if ((block_SuperBlock[5].outptr = malloc(sizeof(void*)*block_SuperBlock[5].nout))== NULL) return 0;
    block_SuperBlock[5].outptr[0] = SuperBlock_block_outtbptr[3];
    block_SuperBlock[5].outsz[0]  = 1;
    block_SuperBlock[5].outsz[1]  = 1;
    block_SuperBlock[5].outsz[2]  = SCSREAL_N;
    block_SuperBlock[5].z         = &(z[1]);
    block_SuperBlock[5].work      = (void **)(((double *)work)+5);
    block_SuperBlock[5].nevprt    = nevprt;

    /* Call of 'cstblk4' (type 4 - blk nb 1) */
    block_SuperBlock[0].nevprt = 0;
    local_flag = 4;
    cstblk4(&block_SuperBlock[0],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'dsslti4' (type 4 - blk nb 2) */
    block_SuperBlock[1].nevprt = 0;
    local_flag = 4;
    dsslti4(&block_SuperBlock[1],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'summation' (type 4 - blk nb 3) */
    block_SuperBlock[2].nevprt = 0;
    local_flag = 4;
    summation(&block_SuperBlock[2],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'actionneur1' (type 0 - blk nb 4) */
    block_SuperBlock[3].nevprt = 0;
    local_flag = 4;
    nport = 1;
    SuperBlock_actuator(&local_flag, &nport, &block_SuperBlock[3].nevprt, \
                        &t, (SCSREAL_COP *)SuperBlock_block_outtbptr[2], \
                        &nrd_1, &nrd_1, &nrd_10,bbb);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'capteur1' (type 0 - blk nb 6) */
    block_SuperBlock[5].nevprt = 0;
    local_flag = 4;
    nport = 1;
    SuperBlock_sensor(&local_flag, &nport, &block_SuperBlock[0].nevprt, \
                      &t, (SCSREAL_COP *)SuperBlock_block_outtbptr[3], \
                      &nrd_1, &nrd_1, &nrd_10,aaa);
    if(local_flag < 0) return(5 - local_flag);

    /* initial blocks must be called with flag 1 */
    /* Call of 'cstblk4' (type 4 - blk nb 1) */
    block_SuperBlock[0].nevprt = 0;
    local_flag = 1;
    cstblk4(&block_SuperBlock[0],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'summation' (type 4 - blk nb 3) */
    block_SuperBlock[2].nevprt = 1;
    local_flag = 1;
    summation(&block_SuperBlock[2],local_flag);
    if(local_flag < 0) return(5 - local_flag);

  }
  else if (flag == 5) { /* Ending */
    block_SuperBlock=*block->work;

    /* set blk struc. of 'cstblk4' (type 4 - blk nb 1) */
    block_SuperBlock[0].outptr[0] = SuperBlock_block_outtbptr[0];
    block_SuperBlock[0].z=&(z[0]);
    block_SuperBlock[0].rpar=&(rpar[0]);
    block_SuperBlock[0].work=(void **)(((double *)work)+0);

    /* set blk struc. of 'dsslti4' (type 4 - blk nb 2) */
    block_SuperBlock[1].inptr[0]  = SuperBlock_block_outtbptr[1];
    block_SuperBlock[1].outptr[0] = SuperBlock_block_outtbptr[2];
    block_SuperBlock[1].z=&(z[0]);
    block_SuperBlock[1].rpar=&(rpar[1]);
    block_SuperBlock[1].work=(void **)(((double *)work)+1);

    /* set blk struc. of 'summation' (type 4 - blk nb 3) */
    block_SuperBlock[2].inptr[0]  = SuperBlock_block_outtbptr[0];
    block_SuperBlock[2].inptr[1]  = SuperBlock_block_outtbptr[3];
    block_SuperBlock[2].outptr[0] = SuperBlock_block_outtbptr[1];
    block_SuperBlock[2].z=&(z[1]);
    block_SuperBlock[2].ipar=&(ipar[0]);
    block_SuperBlock[2].work=(void **)(((double *)work)+2);

    /* set blk struc. of 'actionneur1' (type 0 - blk nb 4) */
    block_SuperBlock[3].inptr[0]  = SuperBlock_block_outtbptr[2];
    block_SuperBlock[3].z=&(z[1]);
    block_SuperBlock[3].work=(void **)(((double *)work)+3);

    /* set blk struc. of 'bidon' (type 0 - blk nb 5) */
    block_SuperBlock[4].z=&(z[1]);
    block_SuperBlock[4].work=(void **)(((double *)work)+4);

    /* set blk struc. of 'capteur1' (type 0 - blk nb 6) */
    block_SuperBlock[5].outptr[0] = SuperBlock_block_outtbptr[3];
    block_SuperBlock[5].z=&(z[1]);
    block_SuperBlock[5].work=(void **)(((double *)work)+5);

    /* Call of 'cstblk4' (type 4 - blk nb 1) */
    block_SuperBlock[0].nevprt = 0;
    local_flag = 5;
    cstblk4(&block_SuperBlock[0],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'dsslti4' (type 4 - blk nb 2) */
    block_SuperBlock[1].nevprt = 0;
    local_flag = 5;
    dsslti4(&block_SuperBlock[1],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'summation' (type 4 - blk nb 3) */
    block_SuperBlock[2].nevprt = 0;
    local_flag = 5;
    summation(&block_SuperBlock[2],local_flag);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'actionneur1' (type 0 - blk nb 4) */
    block_SuperBlock[3].nevprt = 0;
    local_flag = 5;
    nport = 1;
    SuperBlock_actuator(&local_flag, &nport, &block_SuperBlock[3].nevprt, \
                        &t, (SCSREAL_COP *)SuperBlock_block_outtbptr[2], \
                        &nrd_1, &nrd_1, &nrd_10,bbb);
    if(local_flag < 0) return(5 - local_flag);

    /* Call of 'capteur1' (type 0 - blk nb 6) */
    block_SuperBlock[5].nevprt = 0;
    local_flag = 5;
    nport = 1;
    SuperBlock_sensor(&local_flag, &nport, &block_SuperBlock[0].nevprt, \
                      &t, (SCSREAL_COP *)SuperBlock_block_outtbptr[3], \
                      &nrd_1, &nrd_1, &nrd_10,aaa);
    if(local_flag < 0) return(5 - local_flag);

    for (kf = 0; kf < 6; ++kf) {
      if (block_SuperBlock[kf].insz!=NULL) {
        free(block_SuperBlock[kf].insz);
      }else {
        break;
      }
      if (block_SuperBlock[kf].outsz!=NULL){
        free(block_SuperBlock[kf].outsz);
      }else {
        break;
      }
      if (block_SuperBlock[kf].nopar!=0){
        if (block_SuperBlock[kf].oparptr!=NULL){
          free(block_SuperBlock[kf].oparptr);
        }else {
          break;
        }
        if (block_SuperBlock[kf].oparsz!=NULL){
          free(block_SuperBlock[kf].oparsz);
        }else {
          break;
        }
        if (block_SuperBlock[kf].opartyp!=NULL){
          free(block_SuperBlock[kf].opartyp);
        }else {
          break;
        }
      }
      if (block_SuperBlock[kf].noz!=0){
        if (block_SuperBlock[kf].ozptr!=NULL){
          free(block_SuperBlock[kf].ozptr);
        }else {
          break;
        }
        if (block_SuperBlock[kf].ozsz!=NULL){
          free(block_SuperBlock[kf].ozsz);
        }else {
          break;
        }
        if (block_SuperBlock[kf].oztyp!=NULL){
          free(block_SuperBlock[kf].oztyp);
        }else {
          break;
        }
      }
      if (block_SuperBlock[kf].evout!=NULL){
        free(block_SuperBlock[kf].evout);
      }else {
        break;
      }
    }
    scicos_free(block_SuperBlock);
  }

  memcpy(*(y+0),*(SuperBlock_block_outtbptr+2),1*sizeof(double));
  return 0;

} /* SuperBlock */
