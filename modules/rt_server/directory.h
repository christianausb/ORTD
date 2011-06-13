#include <pthread.h>
#include <stdio.h>
#include <map>
#include <string>

#include <pthread.h>



class directory_leaf;

class directory_entry {
  public:
    directory_entry();
    void set(const char* name, void* userptr);

    typedef struct {
      const char *name; bool allocated_name;
      void *userptr; // either pointer to data or parameter_directory *dir
      void *belonges_to_class; // the class instance that will be called
      int type;
      int shortcut_id;
    } direntry;

    
    direntry content;
    
    void destruct();
};

class directory_leaf {
  public:
    directory_leaf();
    
    void set_name(const char *name);
    
    double * get_double_value_ptr(char *parname); // FIXME: REMOVE?
    bool add_entry(char *name, void *belonges_to_class, void* userptr);


    //  if ret = 1 then  *object IS "direntry *"   , *object is "parameter_directory *dir"
    
    directory_entry::direntry *access2(char* path);
    
    // same as above but checks wheter the file is add_entry'ed with belonges_to_class.
    // if not returns NULL
    directory_entry::direntry *access2(char* path, void *belonges_to_class);
    directory_entry * access1(char* path);
    

    // list of entries
    typedef std::map<std::string, directory_entry *> entry_map_t;
    entry_map_t entries;
    
    // collapse all subdirs
    void destruct();
  private:
    directory_leaf * upper_level;
    
    //parameter_manager * pmgr;    
    
    // list of contents
    class directory_entry only_one_entry;
    
    
    // call access
    directory_entry * get_entry(char *name);
};




// FIXME Add mutex for this class
// which is the *only* user interface!

class directory_tree {
  public:
    directory_tree();
    void destruct();
    
    bool add_entry(char *name, void *belonges_to_class, void* userptr);
    directory_entry::direntry* access(char* path, void* belonges_to_class);
    
    // list the pwd directory
    void begin_list();
    directory_entry::direntry* get_list_next();
    void end_list();
    
private:
    // list
    directory_leaf::entry_map_t::iterator list_iterator;
    pthread_mutex_t list_process_mutex;
    
    
    directory_leaf * root;
    directory_leaf * pwd; // current directory
};