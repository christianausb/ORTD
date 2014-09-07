#ifndef _ORTD_IO_H
#define _ORTD_IO_H 1


#include "libdyn.h"
#include "libdyn_cpp.h"

class ortd_io_internal {
public:
    
  ortd_io_internal();
  
  void PutString(char *s);
  
  
};

class ortd_io {
public:
   static void PutString(libdyn *sim, char *s);
   static void PutString(libdyn_nested2 *sim, char *s);
   static void PutString(dynlib_simulation_t *sim, char *s);
   static void PutString(dynlib_block_t *block, char *s);
   
//    void ortd_io::printf(dynlib_simulation_t* sim, char* format, ...);

};



#endif  