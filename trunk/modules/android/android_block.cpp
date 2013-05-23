/*
    Copyright (C) 2010, 2011, 2012, 2013  Christian Klauer

    This file is part of OpenRTDynamics, the Real-Time Dynamics Framework

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
#include <stdio.h>

extern "C" {
#include "libdyn.h"
#include "libdyn_scicos_macros.h"
}
#include "libdyn_cpp.h"




// only compile this code if the platform is android
#ifdef __ORTD_TARGET_ANDROID


// android headers
#include <jni.h>
#include <errno.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/looper.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))




#define LOOPER_ID 1







class SynchronisingAndroidSensorsBlock {
public:
    SynchronisingAndroidSensorsBlock(struct dynlib_block_t *block) {
        this->block = block;    // no nothing more here. The real initialisation take place in init()
        
//         printf("construct()\n");
    }
    ~SynchronisingAndroidSensorsBlock()
    {
        // free your allocated memory, ...
    }

    //
    // define states or other variables
    //

    int SAMP_PER_SEC;
    bool ExitLoop;

    ASensorManager* sensorManager;
    ALooper* looper;
    ASensorRef accelerometerSensor;
    ASensorEventQueue* queue;

    double ax, ay, az;
    

    //
    // initialise your block
    //

    int init() {
//       printf("init()\n");
      
        int *Uipar;
        double *Urpar;

        // Get the irpar parameters Uipar, Urpar
        libdyn_AutoConfigureBlock_GetUirpar(block, &Uipar, &Urpar);


	SAMP_PER_SEC = Uipar[0];


        // set the initial states
        resetStates();

        // register the callback function to the simulator that shall trigger the simulation while running in a loop
        libdyn_simulation_setSyncCallback(block->sim, &syncCallback_ , this);
        libdyn_simulation_setSyncCallbackDestructor(block->sim, &syncCallbackDestructor_ , this);

        ExitLoop = false;

        // Return -1 to indicate an error, so the simulation will be destructed
        return 0;
    }


    inline void updateStates()
    {
        double *in1 = (double *) libdyn_get_input_ptr(block,0); // the first input port
        double *in2 = (double *) libdyn_get_input_ptr(block,1); // the 2nd input port
        double *output = (double*) libdyn_get_output_ptr(block, 0); // the first output port
    }


    inline void calcOutputs()
    {
        double *in1 = (double *) libdyn_get_input_ptr(block,0); // the first input port
        double *in2 = (double *) libdyn_get_input_ptr(block,1); // the 2nd input port
        double *output = (double*) libdyn_get_output_ptr(block, 0); // the first output port

        // Put out the sensor value
        output[0] = ax;
        output[1] = ay;
        output[2] = az;
        //   printf("output calc\n");
    }


    inline void resetStates()
    {
        
    }



    int SyncCallback( struct dynlib_simulation_t * sim )
    {
        /*
         *		***	MAIN FUNCTION	***
         *
         * This function is called before any of the output or state-update flags
         * are called.
         * If 0 is returned, the simulation will continue to run
         * If 1 is returned, the simulation will pause and has to be re-triggered externally.
         * e.g. by the trigger_computation input of the async nested_block.
        */

//         printf("Threaded simulation started execution\n");


        // set-up sensors
        sensorManager = ASensorManager_getInstance();
        looper = ALooper_forThread();
        if(looper == NULL)
            looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

        accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,ASENSOR_TYPE_ACCELEROMETER);
        fprintf(stderr, "accelerometerSensor: %s, vendor: %s\n", ASensor_getName(accelerometerSensor), ASensor_getVendor(accelerometerSensor));
        queue = ASensorManager_createEventQueue(sensorManager, looper, LOOPER_ID, NULL, NULL);

        ASensorEventQueue_enableSensor(queue, accelerometerSensor);
        ASensorEventQueue_setEventRate(queue, accelerometerSensor, (1000L/SAMP_PER_SEC)*1000);

    int ident;//identifier
    int events;


        // This is the main loop of the new simulation
        {
            //
            // Do something e.g. wait for a new value from a sensors
            //
            while (  (ident=ALooper_pollAll(-1, NULL, &events, NULL) >= 0)  && !ExitLoop ) {
                // If a sensor has data, process it now.
                if (ident == LOOPER_ID) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(queue, &event, 1) > 0) {
//                     printf("accelerometer X = %f y = %f z=%f \n", event.acceleration.x, event.acceleration.y, event.acceleration.z);

                        ax = event.acceleration.x;
                        ay = event.acceleration.y;
                        az = event.acceleration.z;

                        // run one step of the ortd simulator
                        // Use C-functions to simulation one timestep
                        libdyn_event_trigger_mask(sim, 1);
                        libdyn_simulation_step(sim, 0);
                        libdyn_simulation_step(sim, 1);

                    }
                }


            }
        }

        fprintf(stderr, "Android Sensors main loop exited\n");

        return 1; // 1 - this shall not be executed again, directly after returning from this function!
    }

    int SyncDestructorCallback( struct dynlib_simulation_t * sim )
    {
        // Trigger termination of the the main loop
        ExitLoop = true;
    }


    void printInfo() {
        fprintf(stderr, "I'm a AndroidSensors block\n");
    }

    // uncommonly used flags
    void PrepareReset() {}
    void HigherLevelResetStates() {}
    void PostInit() {}
    
    // The Computational function that is called by the simulator
    // and that distributes the execution to the various functions
    // in this C++ - Class, including: init(), io(), resetStates() and the destructor
    static int CompFn(int flag, struct dynlib_block_t *block) {
//       printf("CompFn called %d\n", flag);
        return LibdynCompFnTempate<SynchronisingAndroidSensorsBlock>( flag, block ); // this expands a template for a C-comp fn
    }
    static int syncCallback_(struct dynlib_simulation_t * sim) {
        void * obj = sim->sync_callback.userdat;
        SynchronisingAndroidSensorsBlock *p = (SynchronisingAndroidSensorsBlock *) obj;
        return p->SyncCallback(sim);
    }
    static int syncCallbackDestructor_(struct dynlib_simulation_t * sim) {
        void * obj = sim->sync_callback.userdatDestructor;
        SynchronisingAndroidSensorsBlock *p = (SynchronisingAndroidSensorsBlock *) obj;
        return p->SyncDestructorCallback(sim);
    }

    // The data for this block managed by the simulator
    struct dynlib_block_t *block;
};











#endif // Android



// Export to C so the libdyn simulator finds this function
extern "C" {
    // ADJUST HERE: must match to the function name in the end of this file
    int libdyn_module_android_siminit(struct dynlib_simulation_t *sim, int bid_ofs);

//// Compile only on Android /////
  #ifdef __ORTD_TARGET_ANDROID
//////////////////////////////////

    
    int compu_func_AndroidSensor(int flag, struct dynlib_block_t *block);

//// Compile only on Android /////
  #endif
//////////////////////////////////
    
}

// CHANGE HERE: Adjust this function name to match the name of your module
int libdyn_module_android_siminit(struct dynlib_simulation_t *sim, int bid_ofs)
{
//// Compile only on Android /////
  #ifdef __ORTD_TARGET_ANDROID
//////////////////////////////////

    // Register my blocks to the given simulation

    int blockid = 15500;  // CHANGE HERE: choose a unique id for each block
//     libdyn_compfnlist_add(sim->private_comp_func_list, blockid+0, LIBDYN_COMPFN_TYPE_LIBDYN, (void*) &compu_func_AndroidSensor);
    libdyn_compfnlist_add(sim->private_comp_func_list, blockid+0, LIBDYN_COMPFN_TYPE_LIBDYN, (void*) &SynchronisingAndroidSensorsBlock::CompFn  );
        
    
//     libdyn_compfnlist_add(sim->private_comp_func_list, blockid+1, LIBDYN_COMPFN_TYPE_LIBDYN, (void*) &compu_func_AndroidAudio);

    
    printf("libdyn module android initialised\n");
    
//// Compile only on Android /////
  #endif
//////////////////////////////////


}


