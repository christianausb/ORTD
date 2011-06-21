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


//

extern "C" {
  #include "libdyn.h"  
}

#define REMOTE



#ifdef REMOTE
// #include "modules/rt_server/parameter_manager.h"
#include "parameter_manager.h"
#endif

// The different realtime envoronments
#define RTENV_UNDECIDED 0
#define RTENV_SIMULATION 1
#define RTENV_RTP 2
#define RTENV_RTAI 3


class irpar;
class libdyn_master;
class libdyn;
class libdyn_nested;

// Änderungen:
// - dynlib_simulation_t soll einen void *cpp_huelle - link zu libdyn_cpp Klasse erhalten
// - blöcke die den communication_server benutzen können über diesen link, über die masterklasse auf die Serverklasse zugreifen.
// - global_comp_func_list wird einmal vom master angelegt (alle module werden aufgerufen um die blöcke zu registrieren) und allen simulationen als pointer weitergereicht
// - libdyn_cpp Klassen können auch ohne master betrieben werden communication_server blöcke müssen diesen fall abfangen. (cpp_huelle==NULL) test
// - libdyn_cpp 
// - nested simulations

// NEU Class which is created once and <used for multiple simulations e.g. nested simulations


#define BUILD_COMMUNICATION_SEVER

class irpar {
  private:
    char fname_ipar[256];
    char fname_rpar[256];
  public:
    irpar();
    void destruct();
    
      int err; // variable for errors
  
      bool load_from_afile( char* fname_i, char* fname_r );
      bool load_from_afile( char* fname );
      
      int Nipar, Nrpar;
      int *ipar;
      double *rpar;

};

class libdyn_master {
  private:
    
    // Daten, die vererbt werden sollen
    struct lindyn_comp_func_list_head_t *global_comp_func_list;
    void *communication_server;
          // ... //
	  
	  #ifdef REMOTE
	  
    
	  #endif
	  
    // Die Verzeichnisstruktur aus communication_server hier herein bauen
    
  public:
    // return communication_server variable
    rt_server_threads_manager *get_communication_server();
    
    
    libdyn_master(int realtime_env, int remote_control_tcpport);
    libdyn_master();
    
    // gemeinsam genutzte systeme initialisieren
	  #ifdef REMOTE
	  rt_server_threads_manager * rts_mgr;

	  directory_tree * dtree;
	  parameter_manager * pmgr;

    int init_communication(int tcpport);
    void close_communication();
         // ... //
	  #endif
    
    // calls every module to register its blocks
    int init_blocklist();
    
    void destruct();
    
    // store the type of the realtime environment. rtp, rtai, simulation ...
    int realtime_environment;
    
    
};

class libdyn_nested {
  private:
    bool internal_init(int Nin, const int* insizes_, const int *intypes, int Nout, const int* outsizes_, const int *outtypes);
    bool allocate_inbuffers();
    void set_buffer_inptrs();
    
    void* InputBuffer;
    
    libdyn_master * ld_master;

  public:

    libdyn_nested(int Nin, const int* insizes_, const int *intypes, int Nout, const int* outsizes_, const int *outtypes);
    libdyn_nested(int Nin, const int* insizes_, const int *intypes, int Nout, const int* outsizes_, const int *outtypes, bool use_buffered_input);
    
    void destruct();
    
    void set_master(libdyn_master *master);
    
  /**
    * \brief Configure pointer to input port source variables
    * \param in number of input port
    * \param inptr array of a double variables that will be used as input vector
    */
    bool cfg_inptr(int in, double *inptr);    
    
    // uses a buffer for copies of the input data - necessary within scicos blocks or for a threaded nested simulation
    bool use_buffered_input;
    
    int add_simulation(irpar* param, int boxid);
    int add_simulation(int *ipar, double *rpar, int boxid);
    int add_simulation(struct dynlib_simulation_t *sim);
    

    bool set_current_simulation(struct dynlib_simulation_t *sim);
    bool reset_states_of_simulation(struct dynlib_simulation_t *sim);

    bool set_current_simulation(int nSim);
    bool reset_states_of_simulation(int nSim);
    
    // laods all schematics from an irpar container and call add_simulation for each
    bool load_simulations(int *ipar, double *rpar, int boxid);
    
    // length of one element depends on datatype
    void copy_outport_vec(int nPort, void *dest);
    
    // 
    void copy_inport_vec(int nPort, void *src);
    
    void event_trigger_mask(int mask);
    void simulation_step(int update_states);
    
    
    libdyn *current_sim;
    
    struct libdyn_io_config_t iocfg;
    
    

};

class libdyn {
private:
  libdyn_master *ld_master; // NEU pointer to libdyn_master; initial == NULL;
  
  struct dynlib_simulation_t *sim;
  struct libdyn_io_config_t iocfg;

  // NEU währen die Simulation sim läuft kann eine neue Simulation vorbereitet werden
  struct dynlib_simulation_t *sim_prepare;
  struct libdyn_io_config_t iocfg_prepare;

  int prepare_replacement_sim(int *ipar, double *rpar, int boxid);
  void switch_to_replacement_sim();
  
  // NEU liste eingebetter Simulationen. Diese kann sich durch laden einer neuen simulation ändern
  
  libdyn **nested_sim;
  
  // NEU Simulation der höheren Ebene, wenn keine =NULL
  
  libdyn *higher_level_sim;
  
  
  // Fehler ??
  int error;
  
public:
  // NEU set a new master
  void set_master(libdyn_master *ld_master);
  
 /**
  * \brief Set-up a new libdyn instance
  * \param Nin number of inputs
  * \param insizes_ an array of size Nin containing port sizes for the input ports
  * \param Nout number of outputs
  * \param outsizes_ an array of size Nout containing port sizes for the output ports
  */
  libdyn(int Nin, const int* insizes_, int Nout, const int* outsizes_);
  
  libdyn(libdyn_io_config_t * iocfg);
  
 /**
  * \brief Delete everything
  */
  void destruct();
  
 /**
  * \brief Configure pointer to input port source variables
  * \param in number of input port
  * \param inptr array of a double variables that will be used as input vector
  */
  bool cfg_inptr(int in, double *inptr);

 /**
  * \brief Set-up a simulation schematic based on integer and real vectors (irpar encoding; see irpar.c)
  * \brief this was encoded by a scilab script
  *
  * \brief irpar_load_from_afile is commonly used for loading these vector from files
  *
  * \param ipar array of integers containing irpar encoding
  * \param rpar array of doubles containing irpar encoding 
  * \param boxid irpar-ID of the schematic container
  *
  * \return if there is an error compiling the schematic a value less than 0 is returned
  */
  int irpar_setup(int *ipar, double *rpar, int boxid);
  
  
 /**
  * \brief Trigger events according to a given bitmask
  * \brief This should be called before "simulation_step".
  * \brief These events are routed to the blocks
  *
  * \param mask each bit stands for an event, which is one if the event should occur
  */
  void event_trigger_mask(int mask);

  /**
  * \brief One step in simulation
  * \brief if update_states == 1, then COMPF_FLAG_UPDATESTATES is send to all computational functions
  * \brief if update_states == 0, then COMPF_FLAG_CALCOUTPUTS is send to all computational functions in the correct sequence
  */
  void simulation_step(int update_states);

 /**
  * \brief Get a simulation output value (only for output ports of size one)
  * \param out Number of output port
  */
  double get_skalar_out(int out);

  /**
  * \brief get a pointer to an array of a simulation output port
  * \param out Number of output port
  */
  double * get_vec_out(int out);

  /**
  * \brief Add a user defined computational function (UNTESTED)
  * \param blockid the block id to be used (this has to be a unique one)
  * \param comp_fn pointer to the compuational function
  */
  bool add_libdyn_block(int blockid, void *comp_fn);
  
  /**
  * \brief Dump a list of all created blocks to stdout
  */  
  void dump_all_blocks();
};
