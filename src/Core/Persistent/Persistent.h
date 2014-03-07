/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/



/*
 *  Persistent.h: Base class for persistent objects...
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   April 1994
 *
 */

#ifndef CORE_PERSISTENT_H
#define CORE_PERSISTENT_H 1

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

// for index and size types
#include <Core/Datatypes/Types.h>

#include <Core/Util/Assert.h>
#include <Core/Util/ProgressReporter.h>

#include <Core/Persistent/share.h>

namespace SCIRun {

class Persistent;
class Mutex;
class Piostream;

class SCISHARE PersistentTypeID {
public:
  PersistentTypeID();
  PersistentTypeID(const std::string& type, 
                     const std::string& parent,
                     Persistent* (*maker)(), 
                     Persistent* (*bc_maker1)() = 0, 
                     Persistent* (*bc_maker2)() = 0);
  std::string type;
  std::string parent;
  Persistent* (*maker)();
  Persistent* (*bc_maker1)();
  Persistent* (*bc_maker2)();    
};

typedef boost::shared_ptr<PersistentTypeID> PersistentTypeIDPtr;
typedef boost::shared_ptr<Piostream> PiostreamPtr;

SCISHARE PiostreamPtr auto_istream(const std::string& filename,
                                 ProgressReporter *pr = 0);
SCISHARE PiostreamPtr auto_ostream(const std::string& filename, const std::string& type,
                                 ProgressReporter *pr = 0);


//----------------------------------------------------------------------
class SCISHARE Piostream {
    
  public:
    typedef std::map<Persistent*, int>          MapPersistentInt;
    typedef std::map<int, Persistent*>          MapIntPersistent;

    enum Direction {
      Read,
      Write
    };

    enum Endian {
      Big,
      Little
    };

    static const int PERSISTENT_VERSION;
    void flag_error() { err = 1; }
    
  protected:
    Piostream(Direction, int, const std::string &, ProgressReporter *pr);

    Direction dir;
    int version_;
    bool err;
    int file_endian;
    
    MapPersistentInt* outpointers;
    MapIntPersistent* inpointers;
    
    int current_pointer_id;

    bool have_peekname_;
    std::string peekname_;

    ProgressReporter *reporter_;
    bool own_reporter_;
    bool backwards_compat_id_;
    bool disable_pointer_hashing_;
    virtual void emit_pointer(int& have_data, int& pointer_id);
  public:
    static bool readHeader(ProgressReporter *pr,
                           const std::string& filename, char* hdr,
                           const char* type, int& version, int& endian);
  private:
    virtual void reset_post_header() = 0;
  public:
    std::string file_name;

    virtual ~Piostream();

    virtual std::string peek_class();
    virtual int begin_class(const std::string& name, int current_version);
    virtual void end_class();
    virtual void begin_cheap_delim();
    virtual void end_cheap_delim();

    virtual void io(bool&);
    virtual void io(char&) = 0;
    virtual void io(signed char&) = 0;
    virtual void io(unsigned char&) = 0;
    virtual void io(short&) = 0;
    virtual void io(unsigned short&) = 0;
    virtual void io(int&) = 0;
    virtual void io(unsigned int&) = 0;
    virtual void io(long&) = 0;
    virtual void io(unsigned long&) = 0;
    virtual void io(long long&) = 0;
    virtual void io(unsigned long long&) = 0;
    virtual void io(double&) = 0;
    virtual void io(float&) = 0;
    virtual void io(std::string& str) = 0;
    virtual bool eof() { return false; }

    void io(Persistent*&, const PersistentTypeID&);

    bool reading() const { return dir == Read; }
    bool writing() const { return dir == Write; }
    bool error() const { return err; }

    int version() const { return version_; }
    bool backwards_compat_id() const { return backwards_compat_id_; }
    void set_backwards_compat_id(bool p) { backwards_compat_id_ = p; }
    virtual bool supports_block_io() { return false; } // deprecated, redundant.
    
    // Returns true if bkock_io was supported (even on error).
    virtual bool block_io(void*, size_t, size_t) { return false; }
    
    void disable_pointer_hashing() { disable_pointer_hashing_ = true; }

    SCISHARE friend PiostreamPtr auto_istream(const std::string& filename,
                                   ProgressReporter *pr);
    SCISHARE friend PiostreamPtr auto_ostream(const std::string& filename, 
                                   const std::string& type,
                                   ProgressReporter *pr);
};


//----------------------------------------------------------------------
class SCISHARE Persistent {

  public:
    virtual ~Persistent();
    virtual void io(Piostream&);

    //----------------------------------------------------------------------
    // Functions for managing the database of Persistent objects
    // Note all of these functions are static, to allow access from outside
    // the class

    static PersistentTypeIDPtr find_derived( const std::string& classname, 
                                        const std::string& basename );
    static bool is_base_of(const std::string& parent, const std::string& type);
  
    static void add_class(const std::string& type, 
                          const std::string& parent,
                          Persistent* (*maker)(),
                          Persistent* (*bc_maker1)() = 0,
                          Persistent* (*bc_maker2)() = 0);

    static void add_mesh_class(const std::string& type, 
                          Persistent* (*maker)(),
                          Persistent* (*bc_maker1)() = 0,
                          Persistent* (*bc_maker2)() = 0);                          

    static void add_field_class(const std::string& type, 
                          Persistent* (*maker)(),
                          Persistent* (*bc_maker1)() = 0,
                          Persistent* (*bc_maker2)() = 0);  
  private:
    // Mutex protecting the list, these are in the class so they will be 
    // initialized before any of the static functions can be called

    typedef std::map<std::string, PersistentTypeIDPtr>	MapStringPersistentID;
    static MapStringPersistentID* persistent_table_;  
    static Mutex* persistent_mutex_;
    
    static void initialize();
};

//----------------------------------------------------------------------
inline void Pio(Piostream& stream, bool& data) { stream.io(data); }
inline void Pio(Piostream& stream, char& data) { stream.io(data); }
inline void Pio(Piostream& stream, signed char& data) { stream.io(data); }
inline void Pio(Piostream& stream, unsigned char& data) { stream.io(data); }
inline void Pio(Piostream& stream, short& data) { stream.io(data); }
inline void Pio(Piostream& stream, unsigned short& data) { stream.io(data); }
inline void Pio(Piostream& stream, int& data) { stream.io(data); }
inline void Pio(Piostream& stream, unsigned int& data) { stream.io(data); }
inline void Pio(Piostream& stream, long& data) { stream.io(data); }
inline void Pio(Piostream& stream, unsigned long& data) { stream.io(data); }
inline void Pio(Piostream& stream, long long& data) { stream.io(data); }
inline void Pio(Piostream& stream, unsigned long long& data) { stream.io(data); }
inline void Pio(Piostream& stream, double& data) { stream.io(data); }
inline void Pio(Piostream& stream, float& data) { stream.io(data); }
inline void Pio(Piostream& stream, std::string& data) { stream.io(data); }
inline void Pio(Piostream& stream, Persistent& data) { data.io(stream); }

SCISHARE void Pio_index(Piostream& stream, index_type* data, size_type sz);

template<class T>
inline void Pio(Piostream& stream, T* data, size_type sz)
{
  if (!stream.block_io(data, sizeof(T), sz))
  {
    for (index_type i=0;i<sz;i++)
      stream.io(data[i]);
  }
}

inline void Pio_size(Piostream& stream, size_type& size)
{
  long long temp = static_cast<long long>(size);
  stream.io(temp);
  size = static_cast<size_type>(temp);
}

inline void Pio_index(Piostream& stream, index_type& index)
{
  long long temp = static_cast<long long>(index);
  stream.io(temp);
  index = static_cast<size_type>(temp);
}

} // End namespace SCIRun

#endif
