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

#include "libdyn_scicos_macros.h"
#include "irpar.h"
#include "math.h"
}

#include "libdyn_cpp.h"





extern "C" {
    int libdyn_module_nested_siminit(struct dynlib_simulation_t *sim, int bid_ofs);
}

template <class compute_instance> class background_computation {
public:

    // the class compute_instance should provide a method int computer()
    background_computation(compute_instance *ci);
    ~background_computation();

    bool start_computation();

    void join_computation();

    bool get_finished();
    void set_finished(bool f);

    void loop();

private:

    compute_instance * ci;

    int signal;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    pthread_t thread;

    bool finished;
    pthread_mutex_t finished_mutex;

    pthread_mutex_t comp_active_mutex;


    static void * start_thread(void *obj);
    void signal_thread(int sig);
};



template <class compute_instance> void * background_computation<compute_instance>::start_thread(void *obj)
{
    background_computation<compute_instance> *ci;

    ci = (background_computation<compute_instance> *) obj;
    ci->loop();
//     reinterpret_cast<compute_instance *>(obj)->loop();
}

template <class compute_instance> void background_computation<compute_instance>::loop()
{
    signal = 0;

    for (;;) {

        pthread_mutex_lock(&mutex);


        while (signal == 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        int sig = signal;
        signal = 0;

        set_finished(false);



        pthread_mutex_unlock(&mutex);


        // do something based on sig
        if (sig == 1) {

            pthread_mutex_lock(&comp_active_mutex);

            // Run the function computer() of the given class, which type is parametrised by this template
            ci->computer();

            pthread_mutex_unlock(&comp_active_mutex);


// 	     computation finished
            set_finished(true);
        }

        if (sig == -1)
            return;

    }
}

template <class compute_instance> void background_computation<compute_instance>::signal_thread(int sig)
{
    pthread_mutex_lock(&mutex);
    signal = sig;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond); // Notify reader thread

}

template <class compute_instance> bool background_computation<compute_instance>::start_computation()
{

    bool finished_cpy = get_finished();

    //   only if computation is not running
    if (finished_cpy == true) {
//         printf("trig\n");
        signal_thread(1); // start the computation
    }

}

template <class compute_instance> void background_computation<compute_instance>::join_computation()
{
//    TODO: kill computation

    // wait until computation is complete
    signal_thread(-1);

    pthread_join(thread, NULL);
}

template <class compute_instance> bool background_computation<compute_instance>::get_finished()
{

    pthread_mutex_lock(&finished_mutex);
    bool finished_cpy = finished;
    pthread_mutex_unlock(&finished_mutex);

//      printf("get finished %d\n", finished_cpy ? 1 : 0);

    return finished_cpy;
}

template <class compute_instance> void background_computation<compute_instance>::set_finished(bool f)
{
    pthread_mutex_lock(&finished_mutex);
    finished = f;
//     printf("set finished to %d\n", f ? 1 : 0);
    pthread_mutex_unlock(&finished_mutex);
}


template <class compute_instance> background_computation<compute_instance>::background_computation(compute_instance *ci)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_mutex_init(&finished_mutex, NULL);
    pthread_mutex_init(&comp_active_mutex, NULL);


    this->ci = ci; // the callback class

    signal = 0; // ???

    set_finished(true); // initially the computation is finished



    int rc = pthread_create(&thread, NULL, &background_computation<compute_instance>::start_thread, (void *) this);
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }


}


template <class compute_instance> background_computation<compute_instance>::~background_computation()
// background_computation::~background_computation()
{
    /*    signal = -1;
        pthread_cond_signal(&cond); // Notify reader thread
        pthread_join(thread, NULL);*/

    join_computation();

    printf("background computer: joinined\n");


    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    pthread_mutex_destroy(&finished_mutex);
    pthread_mutex_destroy(&comp_active_mutex);

}





/*
class ortd_asychronous_computation_thread {
  public:


  private:


};*/


template <class callback_class> class ortd_asychronous_computation {
private:

    libdyn_nested * simnest;
    libdyn *sim;

    callback_class *cb;

    background_computation< ortd_asychronous_computation > *computer_mgr;
public:
    // simnest: a ready to use set-up schematic
    ortd_asychronous_computation(callback_class *cb, libdyn_nested * simnest);
    ortd_asychronous_computation(callback_class *cb, libdyn * simnest);

    ~ortd_asychronous_computation();

    int computeNSteps(int N);
    bool computation_finished();
    void join_computation();


    int computer();
};


template <class callback_class> ortd_asychronous_computation<callback_class>::ortd_asychronous_computation(callback_class *cb, libdyn_nested * simnest)
{
    this->cb = cb;
    this->simnest = simnest;
    this->sim = simnest->current_sim;

    computer_mgr = new background_computation< ortd_asychronous_computation > (this);

}

template <class callback_class> ortd_asychronous_computation<callback_class>::~ortd_asychronous_computation()
{
    delete computer_mgr;
}


template <class callback_class> int ortd_asychronous_computation<callback_class>::computeNSteps(int N)
{
    // trigger computation

    computer_mgr->start_computation();

}

template <class callback_class> bool ortd_asychronous_computation<callback_class>::computation_finished()
{
    return computer_mgr->get_finished();
}

template <class callback_class> int ortd_asychronous_computation<callback_class>::computer()
{

//     printf("running computer\n");
//     sleep(8);

    // Simulate

    // do
    sim->simulation_step(0);
    sim->simulation_step(1);
    // while (some abort criterion set by the schematic sim)


    // call the callback to copy the results
    cb->async_copy_output_callback();

//     printf("finished\n");
}










class compu_func_nested_class {
public:
    compu_func_nested_class(struct dynlib_block_t *block);
    void destruct();
    void io(int update_states);
    void io_sync(int update_states);
    void io_async(int update_states);

    void async_copy_output_callback();

    int init();
private:
    struct dynlib_block_t *block;

    void lock_output();
    void unlock_output();
    pthread_mutex_t output_mutex;

    // Used when asyn computation is desired
    bool async_comp;
    ortd_asychronous_computation<compu_func_nested_class> * async_comp_mgr;

    libdyn_nested * simnest;
    libdyn_master * master;
    irpar *param;

    int Nin;
    int Nout;
    int *insizes, *outsizes;
    int *intypes, *outtypes;

    int Nsimulations, dfeed, asynchron_simsteps;

    int error;

};

compu_func_nested_class::compu_func_nested_class(dynlib_block_t* block)
{
    this->block = block;
}

int compu_func_nested_class::init()
{
    int simCreateCount;
    int i;



    double *rpar = libdyn_get_rpar_ptr(block);
    int *ipar = libdyn_get_ipar_ptr(block);

    struct irpar_ivec_t insizes_irp, outsizes_irp, intypes_irp, outtypes_irp, param;


    int error = 0; // FIXME Länge prüfen
    if ( irpar_get_ivec(&insizes_irp, ipar, rpar, 10) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&outsizes_irp, ipar, rpar, 11) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&intypes_irp, ipar, rpar, 12) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&outtypes_irp, ipar, rpar, 13) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&param, ipar, rpar, 20) < 0 ) error = 1 ;

    if (error == 1) {
        printf("nested: could not get parameter from irpar set\n");
        return -1;
    }

    this->insizes = insizes_irp.v;
    this->outsizes = outsizes_irp.v;
    this->intypes = intypes_irp.v;
    this->outtypes = outtypes_irp.v;


// 	int dfeed = param.v[1];

    Nin = insizes_irp.n;
    Nout = outsizes_irp.n;

    Nsimulations = param.v[0];
    dfeed = param.v[1];
    asynchron_simsteps = param.v[2];

    /*



      */


    bool use_buffered_input = false;  // in- and out port values are not buffered (default)
    if (asynchron_simsteps > 0) {
        use_buffered_input == true;  // with async computation buffered inputs have to be used
    }

    // create a new container for multiple simulations
    simnest = new libdyn_nested(Nin, insizes, intypes, Nout, outsizes, outtypes, use_buffered_input);
    simnest->allocate_slots(Nsimulations);

    //
    // set pointers to the input ports of this block
    //
    for (i = 0; i < Nin; ++i) {
        double *in_p = (double*) libdyn_get_input_ptr(block, i);
        simnest->cfg_inptr(i, (void*) in_p);
    }


    // If there is a libdyn master : use it
    master = (libdyn_master *) block->sim->master;
    if (master == NULL) {  // no master available
//       fprintf(stderr, "WARNING: libdyn: parameter block requires a libdyn master\n");
    }

    simnest->set_master(master);

    // load all schematics
    for (simCreateCount = 0; simCreateCount < Nsimulations; ++simCreateCount) {
        int shematic_id = 900 + simCreateCount;

        printf("libdyn_nested: loading shematic id %d\n", shematic_id);
        if (simnest->add_simulation(ipar, rpar, shematic_id) < 0) {
            goto destruct_simulations;  // An error
        }

//         printf("added schematic\n");
    }


    //
    // start async computation manager if desired
    //

    if (asynchron_simsteps <= 0) {
        this->async_comp = false;
    } else {
        // Initialise async computation manager
        printf("nested: Using async\n");

        this->async_comp = true;
        this->async_comp_mgr = new ortd_asychronous_computation<compu_func_nested_class>(this, simnest);

        pthread_mutex_init(&this->output_mutex, NULL);
    }

//     printf("nested was set-up\n");

    return 0;

destruct_simulations:
    printf("nested: Destructing all simulations\n");

    simnest->destruct(); // all simulations are destructed
    delete simnest;

    return -1;

}

void compu_func_nested_class::io_sync(int update_states)
{
    int i,j;



    // map scicos events to libdyn events
    // convert scicos events to libdyn events (acutally there is no conversion)
    int eventmask = __libdyn_event_get_block_events(block);
    simnest->event_trigger_mask(eventmask);

    if (update_states == 1) {
        //
        // update states
        //

        simnest->simulation_step(1);

        //        printf("neszed sup\n");
    } else {
        //
        // calc outputs
        //


        simnest->simulation_step(0);

        for (i=0; i< Nout ; ++i) {
            double *out_p = (double*) libdyn_get_output_ptr(block, i);
            simnest->copy_outport_vec(i, out_p);

//       printf("nested outp %f\n", out_p[0]);
        }

        // Some control signal input analysis
        double *switch_inp = (double*) libdyn_get_input_ptr(block, Nin+0);
        double *reset_inp = (double*) libdyn_get_input_ptr(block, Nin+1);

        if (*reset_inp > 0) {
            //printf("reset states\n");
            simnest->reset_blocks();
        }

        int nSim = *switch_inp;
//     printf("switch sig=%f reset=%f sw=%d\n", *switch_inp, *reset_inp, nSim);
        simnest->set_current_simulation(nSim);

    }


}

void compu_func_nested_class::io_async(int update_states)
{
    int i;

    // map scicos events to libdyn events
    // convert scicos events to libdyn events (acutally there is no conversion)
    int eventmask = __libdyn_event_get_block_events(block);
    simnest->event_trigger_mask(eventmask);

//     printf("io_async up=%d\n", update_states);

    if (update_states == 1) {
        //
        // update states
        //
        double *comptrigger_inp = (double*) libdyn_get_input_ptr(block, Nin+1);
//         printf("starting comp = %f\n", *comptrigger_inp);

        if (*comptrigger_inp > 0) {
            this->async_comp_mgr->computeNSteps(1);
        }


    } else {
        //
        // set comp_finieshed output to 1 if result is available
        //

        double *comp_finished = (double*) libdyn_get_output_ptr(block, Nout);

        // If the background computation is finished the the output to 1
        // nonblocking check for a result
        if (this->async_comp_mgr->computation_finished()) {
            *comp_finished = 1;
        } else {
            *comp_finished = 0;
        }


    }


}

void compu_func_nested_class::async_copy_output_callback()
{
// async copy of outputs
    int i;

    lock_output();


    Nout;
    for (i=0; i< Nout ; ++i) {
        double *out_p = (double*) libdyn_get_output_ptr(block, i);

        simnest->copy_outport_vec(i, out_p);


        //       printf("nested outp %f\n", out_p[0]);
    }


    unlock_output();

}



void compu_func_nested_class::io(int update_states)
{
    if (async_comp == false) {
        io_sync(update_states);
    } else {
        io_async(update_states);
    }

}

void compu_func_nested_class::destruct()
{

    if (this->async_comp) {
        delete async_comp_mgr;
        pthread_mutex_destroy(&this->output_mutex);
    }

    simnest->destruct();
    delete simnest;



}

void compu_func_nested_class::lock_output()
{
    pthread_mutex_lock(&this->output_mutex);
}

void compu_func_nested_class::unlock_output()
{
    pthread_mutex_unlock(&this->output_mutex);
}



int compu_func_nested(int flag, struct dynlib_block_t *block)
{

//     printf("comp_func nested: flag==%d\n", flag);

    double *in;
    double *rpar = libdyn_get_rpar_ptr(block);
    int *ipar = libdyn_get_ipar_ptr(block);

    int Nin = ipar[0];
    int Nout = ipar[1];


    switch (flag) {
    case COMPF_FLAG_CALCOUTPUTS:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_nested_class *worker = (compu_func_nested_class *) libdyn_get_work_ptr(block);

        worker->io(0);

    }
    return 0;
    break;
    case COMPF_FLAG_UPDATESTATES:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_nested_class *worker = (compu_func_nested_class *) libdyn_get_work_ptr(block);

        worker->io(1);

    }
    return 0;
    break;
    case COMPF_FLAG_CONFIGURE:  // configure. NOTE: do not reserve memory or open devices. Do this while init instead!
    {
//         int irpar_get_ivec(struct irpar_ivec_t *ret, int *ipar, double *rpar, int id);

        struct irpar_ivec_t insizes, outsizes, intypes, outtypes, param;

        irpar_get_ivec(&insizes, ipar, rpar, 10);
        irpar_get_ivec(&outsizes, ipar, rpar, 11);
        irpar_get_ivec(&intypes, ipar, rpar, 12);
        irpar_get_ivec(&outtypes, ipar, rpar, 13);
        irpar_get_ivec(&param, ipar, rpar, 20);

        int dfeed = param.v[1];

        int Ninports = insizes.n;
        int Noutports = outsizes.n;

        int synchron_simsteps = param.v[2];

        int Noutextra = 0;

        if (synchron_simsteps > 0) {
            ;  // with async computation
            Noutextra = 1;
        }


        int i;
        libdyn_config_block(block, BLOCKTYPE_DYNAMIC, Noutports+Noutextra, Ninports + 2, (void *) 0, 0);

        for (i = 0; i < Ninports; ++i) {
            libdyn_config_block_input(block, i, insizes.v[i], DATATYPE_FLOAT);
        }

        libdyn_config_block_input(block, Ninports, 1, DATATYPE_FLOAT); // switch signal input
        libdyn_config_block_input(block, Ninports+1, 1, DATATYPE_FLOAT); // reset signal input

        for (i = 0; i < Noutports; ++i) {
            libdyn_config_block_output(block, i, outsizes.v[i], DATATYPE_FLOAT, dfeed);
        }

        if (Noutextra == 1)
            libdyn_config_block_output(block, Noutports, 1, DATATYPE_FLOAT, dfeed); // computation ready output. (when using async comp)


    }
    return 0;
    break;
    case COMPF_FLAG_INIT:  // init
    {
        compu_func_nested_class *worker = new compu_func_nested_class(block);
        libdyn_set_work_ptr(block, (void*) worker);

        int ret = worker->init();
        if (ret < 0)
            return -1;
    }
    return 0;
    break;
    case COMPF_FLAG_DESTUCTOR: // destroy instance
    {
        compu_func_nested_class *worker = (compu_func_nested_class *) libdyn_get_work_ptr(block);

        worker->destruct();
        delete worker;

    }
    return 0;
    break;
    case COMPF_FLAG_PRINTINFO:
        printf("I'm a nested block\n");
        return 0;
        break;

    }
}




/*
  A state machine using nested simulations
*/

class compu_func_statemachine_class {
public:
    compu_func_statemachine_class(struct dynlib_block_t *block);
    void destruct();
    void io(int update_states);
    void io_sync(int update_states);
    void reset();

    int init();
private:
    struct dynlib_block_t *block;

//     void lock_output();
//     void unlock_output();
//     pthread_mutex_t output_mutex;


    libdyn_nested * simnest;
    libdyn_master * master;
    irpar *param;

    int Ndatain;
    int Ndataout;
    int *insizes, *outsizes;
    int *intypes, *outtypes;

    int Nsimulations;
    int inittial_state;

    double active_state; // the currently active state / schematic // FIXME: move this to int


    int error;

    int Nglobal_states; // size of memory space for state storage of the nested simulation
    void *global_states_buffer;
    void *global_states_buffer_initial; // pointer to initial values (used in case of a reset)
};

compu_func_statemachine_class::compu_func_statemachine_class(dynlib_block_t* block)
{
    this->block = block;
    this->global_states_buffer = NULL;
}

int compu_func_statemachine_class::init()
{
    int simCreateCount;
    int i;

    double *rpar = libdyn_get_rpar_ptr(block);
    int *ipar = libdyn_get_ipar_ptr(block);

    struct irpar_ivec_t insizes_irp, outsizes_irp, intypes_irp, outtypes_irp, param;


    int error = 0; // FIXME Länge prüfen
    if ( irpar_get_ivec(&insizes_irp, ipar, rpar, 10) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&outsizes_irp, ipar, rpar, 11) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&intypes_irp, ipar, rpar, 12) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&outtypes_irp, ipar, rpar, 13) < 0 ) error = 1 ;
    if ( irpar_get_ivec(&param, ipar, rpar, 20) < 0 ) error = 1 ;

    if (error == 1) {
        printf("nested: could not get parameter from irpar set\n");
        return -1;
    }

    this->insizes = insizes_irp.v;
    this->outsizes = outsizes_irp.v;
    this->intypes = intypes_irp.v;
    this->outtypes = outtypes_irp.v;


// 	int dfeed = param.v[1];

    Ndatain = insizes_irp.n - 1; // minus one because insizes also contain the size of global states which is an input to the neszed simulation only
    Ndataout = outsizes_irp.n - 2;

    Nsimulations = param.v[0];
    Nglobal_states = param.v[1];
    inittial_state = param.v[2];

//     printf("Nsim = %d, Nglobal_states = %d, init_state %d\n", Nsimulations, Nglobal_states, inittial_state);

    if (Nglobal_states != this->insizes[Ndatain+0]) { // assertion
//       printf("**** %d != %d \n", Nglobal_states, this->insizes[Ndatain+0]);
        return -1;
    }

    // fetch the initial x_global
    struct irpar_rvec_t param_x_global;

    if ( irpar_get_rvec(&param_x_global, ipar, rpar, 21) < 0 ) return -1;
    if (param_x_global.n  != Nglobal_states) return -1;

    this->global_states_buffer_initial = (void*) param_x_global.v;

    // Allocate memeory for the global states buffer
    this->global_states_buffer = (double*) malloc( sizeof(double) * Nglobal_states );

    // copy initial values for x_global
    memcpy(this->global_states_buffer, this->global_states_buffer_initial, sizeof(double) * Nglobal_states);

    //
    // set-up nested
    //
    bool use_buffered_input = false;  // in- and out port values are not buffered

    // create a new container for multiple simulations
    simnest = new libdyn_nested(Ndatain+1, insizes, intypes, Ndataout+2, outsizes, outtypes, use_buffered_input);
    simnest->allocate_slots(Nsimulations);

    //
    // set input pointers of the nest to the input ports of the outer block
    //
    for (i = 0; i < Ndatain; ++i) { // minus 1 because the last input is no input to the OUTER block but an internal thing for global_states_buffer
        double *in_p = (double*) libdyn_get_input_ptr(block, i);
        simnest->cfg_inptr(i, (void*) in_p);
    }

    // inputs to the nest are the states
    simnest->cfg_inptr(Ndatain, (void*) this->global_states_buffer); // The input of the nested to the global_states_buffer

    // If there is a libdyn master : use it
    master = (libdyn_master *) block->sim->master;
    if (master == NULL) {  // no master available
//       fprintf(stderr, "WARNING: libdyn: parameter block requires a libdyn master\n");
    }

    simnest->set_master(master);

    // load all schematics
    for (simCreateCount = 0; simCreateCount < Nsimulations; ++simCreateCount) {
        int shematic_id = 900 + simCreateCount;

        printf("libdyn_nested statemachine: loading shematic id %d\n", shematic_id);
        if (simnest->add_simulation(ipar, rpar, shematic_id) < 0) {
            goto destruct_simulations;  // An error
        }
    }

    // the initial state
    simnest->set_current_simulation(inittial_state-1);

    return 0;

destruct_simulations:
    printf("nested: Destructing all simulations\n");

    simnest->destruct(); // all simulations are destructed
    delete simnest;

    if (this->global_states_buffer != NULL)
        free(this->global_states_buffer);

    return -1;

}

void compu_func_statemachine_class::io_sync(int update_states)
{
    int i,j;

    // map scicos events to libdyn events
    // convert scicos events to libdyn events (acutally there is no conversion)
    int eventmask = __libdyn_event_get_block_events(block);
    simnest->event_trigger_mask(eventmask);

    if (update_states == 1) {
        //
        // update states
        //

        simnest->simulation_step(1);
// 	printf("statemachnie: copy global states outport of nested %d\n", Ndataout+1);
// 	printf("statemachnie: copy global active_state outport of nested %d\n", Ndataout+0);
        double tmp;
        simnest->copy_outport_vec(Ndataout+0, &tmp);
        this->active_state = round(tmp)-1;
// 	printf("active_state %f\n", this->active_state);

        int active = round(tmp)-1;
        if (active >= 0) {
            // copy the x_global states
            simnest->copy_outport_vec(Ndataout+1, this->global_states_buffer);

            // switch to the new state
            simnest->reset_blocks();
            simnest->set_current_simulation(active);
        }

    } else {
        //
        // calc outputs
        //


        simnest->simulation_step(0);

        for (i=0; i< Ndataout ; ++i) {
            double *out_p = (double*) libdyn_get_output_ptr(block, i);
            simnest->copy_outport_vec(i, out_p);
        }
    }
}

void compu_func_statemachine_class::reset()
{
    printf("nested: reset (NOT TESTED FOR NOW; contact the author)\n");
    // the initial state
    simnest->reset_blocks();
    simnest->set_current_simulation(inittial_state-1);

    // copy initial values for x_global
    memcpy(this->global_states_buffer, this->global_states_buffer_initial, sizeof(double) * Nglobal_states);
}





void compu_func_statemachine_class::io(int update_states)
{
    io_sync(update_states);
}

void compu_func_statemachine_class::destruct()
{


    simnest->destruct();
    delete simnest;

    if (this->global_states_buffer != NULL)
        free(this->global_states_buffer);



}



int compu_func_statemachine(int flag, struct dynlib_block_t *block)
{

//     printf("comp_func nested: flag==%d\n", flag);

    double *in;
    double *rpar = libdyn_get_rpar_ptr(block);
    int *ipar = libdyn_get_ipar_ptr(block);

    int Nin = ipar[0];
    int Nout = ipar[1];


    switch (flag) {
    case COMPF_FLAG_CALCOUTPUTS:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_statemachine_class *worker = (compu_func_statemachine_class *) libdyn_get_work_ptr(block);

        worker->io(0);

    }
    return 0;
    break;
    case COMPF_FLAG_UPDATESTATES:
    {
        in = (double *) libdyn_get_input_ptr(block,0);
        compu_func_statemachine_class *worker = (compu_func_statemachine_class *) libdyn_get_work_ptr(block);

        worker->io(1);

    }
    return 0;
    break;
    case COMPF_FLAG_RESETSTATES:
    {
        compu_func_statemachine_class *worker = (compu_func_statemachine_class *) libdyn_get_work_ptr(block);

        worker->reset();;
    }
    return 0;
    break;
    case COMPF_FLAG_CONFIGURE:  // configure. NOTE: do not reserve memory or open devices. Do this while init instead!
    {
//         int irpar_get_ivec(struct irpar_ivec_t *ret, int *ipar, double *rpar, int id);

        struct irpar_ivec_t insizes, outsizes, intypes, outtypes, param;

        irpar_get_ivec(&insizes, ipar, rpar, 10);
        irpar_get_ivec(&outsizes, ipar, rpar, 11);
        irpar_get_ivec(&intypes, ipar, rpar, 12);
        irpar_get_ivec(&outtypes, ipar, rpar, 13);
        irpar_get_ivec(&param, ipar, rpar, 20);

        int Nglobal_states = param.v[1];
        int dfeed = 1;

        // IO of the OUTER block
        int Ndatainports = insizes.n - 1; // minus one because insizes also contain the size of global states which is an input to the neszed simulation only
        int Ndataoutports = outsizes.n - 2;


//         printf("Ndatainports = %d, Ndataoutports = %d\n", Ndatainports, Ndataoutports);

        int i;
        libdyn_config_block(block, BLOCKTYPE_DYNAMIC, Ndataoutports+2, Ndatainports, (void *) 0, 0);

        for (i = 0; i < Ndatainports; ++i) {
            libdyn_config_block_input(block, i, insizes.v[i], DATATYPE_FLOAT);
        }

        for (i = 0; i < Ndataoutports; ++i) {
            libdyn_config_block_output(block, i, outsizes.v[i], DATATYPE_FLOAT, dfeed);
        }

        libdyn_config_block_output(block, Ndataoutports+0, 1, DATATYPE_FLOAT, dfeed); // output of the currently active state
        libdyn_config_block_output(block, Ndataoutports+1, Nglobal_states , DATATYPE_FLOAT, dfeed); // Output for the global states


    }
    return 0;
    break;
    case COMPF_FLAG_INIT:  // init
    {
        compu_func_statemachine_class *worker = new compu_func_statemachine_class(block);
        libdyn_set_work_ptr(block, (void*) worker);

        int ret = worker->init();
        if (ret < 0)
            return -1;
    }
    return 0;
    break;
    case COMPF_FLAG_DESTUCTOR: // destroy instance
    {
        compu_func_statemachine_class *worker = (compu_func_statemachine_class *) libdyn_get_work_ptr(block);

        worker->destruct();
        delete worker;

    }
    return 0;
    break;
    case COMPF_FLAG_PRINTINFO:
        printf("I'm a statemachine block\n");
        return 0;
        break;

    }
}









//#include "block_lookup.h"

int libdyn_module_nested_siminit(struct dynlib_simulation_t *sim, int bid_ofs)
{

    // Register my blocks to the given simulation

    int blockid = 15001;
    libdyn_compfnlist_add(sim->private_comp_func_list, blockid, LIBDYN_COMPFN_TYPE_LIBDYN, (void*) &compu_func_nested);
    libdyn_compfnlist_add(sim->private_comp_func_list, blockid+1, LIBDYN_COMPFN_TYPE_LIBDYN, (void*) &compu_func_statemachine);

    printf("libdyn module nested initialised\n");

}


//} // extern "C"
