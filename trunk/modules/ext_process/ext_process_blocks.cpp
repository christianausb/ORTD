/*
    Copyright (C) 2010, 2011  Christian Klauer

    This file is part of OpenRTDynamics, the Real Time Dynamic Toolbox

    OpenRTDynamics is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenRTDynamics is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with OpenRTDynamics.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <malloc.h>

extern "C" {
#include "libdyn.h"
#include "libdyn_scicos_macros.h"
#include "irpar.h"
}



#include "ortd_fork.h"




class compu_func_runproc_class {
public:
    compu_func_runproc_class(struct dynlib_block_t *block);
    void destruct();
    void io(int update_states);
    int init();
    void postinit();
private:
   struct dynlib_block_t *block;
   
   ortd_fork *process;
   int WhenToStart;

};

compu_func_runproc_class::compu_func_runproc_class(dynlib_block_t* block)
{
    this->block = block;
}

int compu_func_runproc_class::init()
{
    double *rpar = libdyn_get_rpar_ptr(block);
    int *ipar = libdyn_get_ipar_ptr(block);

    WhenToStart = ipar[4];
    int lenpathstr = ipar[5];
    char *pathstr;  

    irpar_getstr(&pathstr, ipar, 6, lenpathstr);

    fprintf(stderr, "ortd_fork execpath = %s\n", pathstr);
    process = new ortd_fork(pathstr, false);
    
    free(pathstr);
    
/*    if (WhenToStart == 0) {
//       printf("Starting\n");
      process->init();
    }*/
    
    // Return -1 to indicate an error, so the simulation will be destructed
  
    return 0;
}

void compu_func_runproc_class::postinit()
{
    if (WhenToStart == 0) {
//       printf("Starting\n");
      process->init();
    }

}



void compu_func_runproc_class::io(int update_states)
{
    if (update_states==0) {
        double *output = (double*) libdyn_get_output_ptr(block, 0);
	
	*output = 1;
    }
}

void compu_func_runproc_class::destruct()
{
  process->terminate(1);  // Send "HUP" (Hangup signal)
  
    delete process;
}


// This is the main C-Callback function, which forwards requests to the C++-Class above
int compu_func_runproc(int flag, struct dynlib_block_t *block)
{

//     printf("comp_func template: flag==%d\n", flag);

    double *in;
    double *rpar = libdyn_get_rpar_ptr(block);
    int *ipar = libdyn_get_ipar_ptr(block);

    int Nin = 0;
    int Nout = 1;


    switch (flag) {
    case COMPF_FLAG_CALCOUTPUTS:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_runproc_class *worker = (compu_func_runproc_class *) libdyn_get_work_ptr(block);

        worker->io(0);

    }
    return 0;
    break;
    case COMPF_FLAG_UPDATESTATES:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_runproc_class *worker = (compu_func_runproc_class *) libdyn_get_work_ptr(block);

        worker->io(1);

    }
    return 0;
    break;
    case COMPF_FLAG_CONFIGURE:  // configure. NOTE: do not reserve memory or open devices. Do this while init instead!
    {
        int i;
        libdyn_config_block(block, BLOCKTYPE_STATIC, Nout, Nin, (void *) 0, 0);
        
        //libdyn_config_block_input(block, i, 1, DATATYPE_FLOAT);
        libdyn_config_block_output(block, 0, 1, DATATYPE_FLOAT, 1);


    }
    return 0;
    break;
    case COMPF_FLAG_INIT:  // init
    {
        compu_func_runproc_class *worker = new compu_func_runproc_class(block);
        libdyn_set_work_ptr(block, (void*) worker);

        int ret = worker->init();
        if (ret < 0)
            return -1;
    }
    return 0;
    break;
    case COMPF_FLAG_POSTINIT:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_runproc_class *worker = (compu_func_runproc_class *) libdyn_get_work_ptr(block);

        worker->postinit();      
    }
    return 0;
    break;    
    case COMPF_FLAG_DESTUCTOR: // destroy instance
    {
        compu_func_runproc_class *worker = (compu_func_runproc_class *) libdyn_get_work_ptr(block);

        worker->destruct();
	delete worker;

    }
    return 0;
    break;
    case COMPF_FLAG_PRINTINFO:
        printf("I'm a template block\n");
        return 0;
        break;

    }
}

// Export to C so the libdyn simulator finds this function
extern "C" {
    // ADJUST HERE: must match to the function name in the end of this file
    int libdyn_module_ext_process_siminit(struct dynlib_simulation_t *sim, int bid_ofs);
}

// CHANGE HERE: Adjust this function name to match the name of your module
int libdyn_module_ext_process_siminit(struct dynlib_simulation_t *sim, int bid_ofs)
{

    // Register my blocks to the given simulation

    int blockid =  15300;  // CHANGE HERE: choose a unique id for each block
    
    libdyn_compfnlist_add(sim->private_comp_func_list, blockid + 0, LIBDYN_COMPFN_TYPE_LIBDYN, (void*) &compu_func_runproc);

    printf("libdyn module ext_process initialised\n");

}


