/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
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


#ifndef DATAFLOW_NETWORK_MODULE_H
#define DATAFLOW_NETWORK_MODULE_H 1

#include <Dataflow/Network/Ports/Port.h>
#include <Dataflow/Network/Ports/SimplePort.h>
#include <Dataflow/Network/PortManager.h>

#include <Core/Util/Assert.h>
#include <Core/Util/Timer.h>
#include <Core/Util/Debug.h>
#include <Core/Util/ProgressReporter.h>

#include <Core/Util/StringUtil.h>
#include <Core/Containers/LockingHandle.h>

#include <Core/GeomInterface/Pickable.h>

#include <Core/Thread/RecursiveMutex.h>
#include <Core/Thread/Mailbox.h>
#include <Core/Thread/Time.h>
#include <Core/Thread/FutureValue.h>

#include <Dataflow/GuiInterface/GuiCallback.h>
#include <Dataflow/GuiInterface/TCLInterface.h>
#include <Dataflow/GuiInterface/GuiVar.h>

#include <sstream>
#include <iosfwd>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

#include <Dataflow/Network/share.h>

#ifdef _WIN32
#define DECLARE_MAKER(name) \
extern "C" __declspec(dllexport) Module* make_##name(GuiContext* ctx) \
{ \
  return new name(ctx); \
}
#else
#define DECLARE_MAKER(name) \
extern "C" Module* make_##name(GuiContext* ctx) \
{ \
  return new name(ctx); \
}
#endif

namespace SCIRun {
class IPort;
class OPort;
class BaseWidget;
class GuiContext;
class MessageBase;
class Module;
class ModuleHelper;
class Network;
class Scheduler;
class NetworkIO;

typedef IPort* (*iport_maker)(Module*, const std::string&);
typedef OPort* (*oport_maker)(Module*, const std::string&);

typedef Module* (*ModuleMaker)(GuiContext* ctx);

class SCISHARE Module : public ProgressReporter, public GuiCallback, public UsedWithLockingHandle<RecursiveMutex>
{
public:
  enum SchedClass {
    Sink,
    Source,
    Filter,
    Iterator,
    ViewerSpecial
  };
  
  Module(const std::string& name, GuiContext* ctx, SchedClass,
	 const std::string& cat="unknown", const std::string& pack="unknown", 
   const std::string& version="1.0");
  virtual ~Module();
  void kill_helper();

  Network* get_network() {
    return network_;
  }
  
  bool have_ui();
  void popup_ui();
  void delete_warn();
  virtual void prepare_cleanup();
  
  bool show_stats() { return show_stats_; }
  void set_show_stats(bool v) {show_stats_ = v;}

  //! ProgressReporter function
  virtual void error(const std::string&);
  virtual void warning(const std::string&);
  virtual void remark(const std::string&);
  virtual void add_raw_message(const std::string&);
  virtual void report_start(const std::string& tag);
  virtual void report_end();

  
  // This one isn't as thread safe as the other ProgressReporter functions.
  // Use add_raw_message or one of the others instead if possible.
  virtual std::ostream &msg_stream() { return msg_stream_; }
  virtual void msg_stream_flush();

  //! This one should go when we have redone the PowerApps
  virtual bool in_power_app();

  //! Execution time progress.
  //! Percent is number between 0.0-1.0
  virtual void          update_progress(double percent);
  virtual void          update_progress(int current, int max);

  std::string get_modulename()   { return module_name_; };
  std::string get_categoryname() { return category_name_; };
  std::string get_packagename()  { return package_name_; };

  std::string get_old_modulename()   { return old_module_name_; };
  std::string get_old_categoryname() { return old_category_name_; };
  std::string get_old_packagename()  { return old_package_name_; };
  
  bool oport_cached(const int item);
  bool oport_connected(const int item);
  
  bool oport_cached(const std::string &name);
  bool oport_connected(const std::string &name);

  bool oport_supports_cache_flag(int p);
  bool get_oport_cache_flag(int p);
  void set_oport_cache_flag(int p, bool val);

  //! Used by ModuleHelper
  void set_pid(int pid);
  virtual void do_execute();
  virtual void do_synchronize();
  virtual void execute() = 0;

  void request_multisend(OPort*);
  Mailbox<MessageBase*> mailbox_;
  virtual void set_context(Network* network);
  virtual void delete_thread();

  // Called just after the variables are set when reading a network.
  virtual void post_read();

  //! Callbacks
  
  virtual void connection(Port::ConnectionState, int, bool);
  virtual void widget_moved(bool last, BaseWidget *widget);

  void get_position(int& x, int& y);
  std::string get_id() const { return id_; }
  //! CollabVis code end

  void want_to_execute();

  // Get handles to ports, thread safety issues with
  // network editing and execution at the same time
  
  template<class DP>
  bool get_iport_handle(const std::string &name, DP& handle);

  template<class DP>
  bool get_iport_handles(const std::string &name, std::vector<DP>& handles);
  
  template<class DP>
  bool get_oport_handle(const std::string &name, DP& handle);
  
  template<class DP>
  bool get_iport_handle(int portnum, DP& handle);
  
  template<class DP>
  bool get_oport_handle(int portnum, DP& handle);

  void lock_iports() { iports_.lock(); }
  void unlock_iports() { iports_.unlock(); }

  void lock_oports() { oports_.lock(); }
  void unlock_oports() { oports_.unlock(); }

  //--------------------------------------------------------------------
  // They are still here for compatibility of old modules
  //
  // Use get_input_handle(), get_dynamic_input_handles(), send_output_handle
  // instead. These have been made thread safe
  //--------------------------------------------------------------------

private:
      friend class NetworkIO;

      port_range_type get_iports(const std::string &name);
      IPort* get_iport(const std::string &name);
      OPort* get_oport(const std::string &name);
      OPort* get_oport(int idx);
      IPort* get_iport(int idx);
      int num_input_ports();
      int num_output_ports();
      port_range_type get_input_ports(const std::string &name);
      IPort* get_input_port(const std::string &name);
      OPort* get_output_port(const std::string &name);
      OPort* get_output_port(int idx);
      IPort* get_input_port(int idx);
  //--------------------------------------------------------------------
  
protected:

  friend class ModuleHelper;
  friend class NetworkEditor;
  friend class PackageDB;
  friend class Network;
  friend class OPort;
  friend class IPort;
  friend class Scheduler;

  enum State {
    NeedData,
    JustStarted,
    Executing,
    Completed
  };
  enum MsgState {  
    Remark,
    Warning,
    Error,
    Reset
  };
  void update_state(State);
  void update_msg_state(MsgState);  
  virtual void tcl_command(GuiArgs&, void*);
  virtual void presave() {}
  GuiContext* get_ctx() { return ctx_; }
  void set_stack_size(unsigned long long stack_size);
  void reset_vars();

  void set_modulename(const std::string& name)   { module_name_ = name; }
  void set_categoryname(const std::string& name) { category_name_ = name; }
  void set_packagename(const std::string& name)  { package_name_ = name; }
  void set_old_modulename(const std::string& name)   { old_module_name_ = name; }
  void set_old_categoryname(const std::string& name) { old_category_name_ = name; }
  void set_old_packagename(const std::string& name)  { old_package_name_ = name; }
  void set_old_version(const std::string& name)      { old_version_ = name; }

public:
  //! Get the handle for a single port.
  template<class DH>
  bool get_input_handle(const std::string& name, DH& handle, bool required = true);
  template<class DH>
  bool get_input_handle(int num, DH& handle, bool required = true);

  //! Get the handles for dynamic ports.
  template<class DH>
  bool get_dynamic_input_handles(const std::string& name, std::vector< DH > &handles,
				 bool required = true);

  //! cache_in_module should be true if the module is doing it's own
  //! caching of the handle.  Otherwise that will be handled by the
  //! port.
  template<class DH>
  bool send_output_handle(const std::string& name, DH& handle,
			  bool cache_in_module = false,
			  bool send_intermediate = false);

  template<class DH>
  bool send_output_handle(int numport, DH& handle,
			  bool cache_in_module = false,
			  bool send_intermediate = false);


  //! Specialization for geometry ports.
  bool send_output_handle(const std::string& port_name,
			  GeomHandle& handle,
			  const std::string& obj_name);

  bool send_output_handle(const std::string& port_name,
			  GeomHandle& handle,
			  const char *obj_name)
  {
    return send_output_handle(port_name, handle, std::string(obj_name));
  }

  bool send_output_handle(const std::string& port_name,
			  std::vector<GeomHandle>& handle,
			  std::vector<std::string>& obj_name);

  protected:

    std::string            module_name_;
    std::string            package_name_;
    std::string            category_name_;
    std::string            version_;

    std::string            old_module_name_;
    std::string            old_package_name_;
    std::string            old_category_name_;
    std::string            old_version_;

    Scheduler             *sched_;
    bool                   lastportdynamic_;
    int                    pid_;
    bool                   have_own_dispatch_;
    std::string            id_;
    bool                   abort_flag_;
    Time::SysClock         timer_;
    std::ostringstream     msg_stream_;
    bool                   need_execute_;
    SchedClass             sched_class_;

    //! Used by the execute module;
    //! True if either the gui vars or data has changed.
    bool                   inputs_changed_;   
    //! Error during execution not related to checks.
    bool                   execute_error_;    

    double                 get_time()
    { return Time::secondsPerTick()*(Time::currentTicks()-timer_); }

    //! Version control
    std::string get_version() { return (version_); }
    std::string get_old_version() { return (old_version_); }

    void set_version(const std::string& ver) { version_ = ver; }

  private:
  
    void remove_iport(int);
    void add_iport(IPort*);
    void add_oport(OPort*);

    GuiContext*            ctx_;
    State                  state_;
    MsgState               msg_state_;  
                   
    PortManager<OPortHandle,oport_maker>  oports_;
    PortManager<IPortHandle,iport_maker>  iports_;
    iport_maker               dynamic_port_maker_;
    std::string               lastportname_;
    int                       first_dynamic_port_;
    unsigned long long        stacksize_;
    ModuleHelper              *helper_;
    Thread                    *helper_thread_;
    Network                   *network_;
    bool                      show_stats_;
    GuiString                 log_string_;

    //! for module version control, this one is by default 1.0
    
    //! For reporting progress
    std::list<std::string>    tags_;
    
    //! cache the number of dynamic ports at previous execute
    unsigned int              old_num_dynamic_ports_;

    Module(const Module&);
    Module& operator=(const Module&);
};


//! Used to get handles with error checking.
template<class DH>
bool
Module::get_input_handle(const std::string& name,
			 DH& handle,
			 bool required)
{
//  update_state(NeedData);

  bool return_state = false;

  SimpleIPortHandle dataport;

  handle = 0;

  //! We always require the port to be there.
  if (!(get_iport_handle(name,dataport)))
  {
    throw "Incorrect data type sent to input port '" + name +
      "' (dynamic_cast failed).";
  }
 
  //! Get the handle and check for data.
  else if (dataport->get(handle) && handle.get_rep())
  {
    //! See if the data has changed. Note only change the boolean if
    //! it is false this way it can be cascaded with other handle gets.
    if( inputs_changed_ == false ) {
      inputs_changed_ = dataport->changed();
    }
    //! we have a valid handle, return true.
    return_state = true;
  }

  else if( required )
  {
    //! The first input on the port was required to have a valid
    //! handle and data so report an error.
    throw "No handle or representation for input port '" +
           name + "'.";
  }
  else
  {
    //! See if the data has changed. Note only change the boolean if
    //! it is false this way it can be cascaded with other handle gets.
    if( inputs_changed_ == false ) 
    {
      inputs_changed_ = dataport->changed();
    }  
  }


  return return_state;
}


//! Used to get handles with error checking.
template<class DH>
bool
Module::get_input_handle(int num,
			 DH& handle,
			 bool required)
{
//  update_state(NeedData);

  bool return_state = false;

  SimpleIPortHandle dataport;
  handle = 0;

  //! We always require the port to be there.
  if (!(get_iport_handle(num,dataport)))
  {
    std::ostringstream oss;
    oss << "port " << num;
    throw "Incorrect data type sent to input port '" + oss.str() +
      "' (dynamic_cast failed).";
  }
 
  //! Get the handle and check for data.
  else if (dataport->get(handle) && handle.get_rep())
  {
    //! See if the data has changed. Note only change the boolean if
    //! it is false this way it can be cascaded with other handle gets.
    if( inputs_changed_ == false ) {
      inputs_changed_ = dataport->changed();
    }
    //! we have a valid handle, return true.
    return_state = true;
  }

  else if( required )
  {
    std::string name = dataport->get_portname();
    //! The first input on the port was required to have a valid
    //! handle and data so report an error.
    throw "No handle or representation for input port '" +
           name + "'.";
  }
  else
  {
    //! See if the data has changed. Note only change the boolean if
    //! it is false this way it can be cascaded with other handle gets.
    if( inputs_changed_ == false ) {
      inputs_changed_ = dataport->changed();
    }  
  }

  return return_state;
}




//! Used to get handles with error checking.
//! If valid handles are set in the vector, return true.
//! Only put valid handles in the vector.
template<class DH>
bool
Module::get_dynamic_input_handles(const std::string& name,
				  std::vector<DH> &handles,
				  bool data_required)
{
  bool return_state = false;

//  update_state(NeedData);

  unsigned int nPorts = 0;
  handles.clear();

  std::vector<SimpleIPortHandle> dataports;
  
  if (!(get_iport_handles(name,dataports)))
  {
    throw "Unable to initialize dynamic input port '" + name + "'.";
  }
  else
  {
    for (size_t p=0; p < dataports.size(); p++)
    {
      //! Get the handle and check for data.
      DH handle;

      if (dataports[p]->get(handle) && handle.get_rep())
      {
        handles.push_back(handle);
  
        //! See if the data has changed. Note only change the boolean if
        //! it is false this way it can be cascaded with other handle gets.
        if( inputs_changed_ == false ) 
        {
          inputs_changed_ = dataports[p]->changed();
        }
        return_state = true;
      } 
      else 
      {
        if( inputs_changed_ == false ) 
        {
          inputs_changed_ = dataports[p]->changed();
        }
        handles.push_back(0);
      }
      ++nPorts;
    }
  }

  if (old_num_dynamic_ports_ != nPorts)
  {
    inputs_changed_ = true;
    old_num_dynamic_ports_ = nPorts;
  }
  
  //! The last port is always empty so remove it.
  handles.pop_back();

  if (return_state == false) 
  {
    //! At least one port was required to have a valid ! handle and
    //data so report an error.
    if( data_required ) {
      throw "No handle or representation for dynamic input port #" +
      to_string(nPorts) + " ' " +name + "'.";
    }
    //! If we have no valid handles, make sure iteration over 
    //! the set of handles is empty.
    handles.clear();
  }

  return return_state;
}


//! Used to send handles with error checking.
template<class DH>
bool
Module::send_output_handle(const std::string& name, DH& handle,
			   bool cache, bool send_intermediate)
{
  //! Don't send on empty, assume cached version is more valid instead.
  //! Dunno if false return value is correct.  We don't check returns
  //! on this one.
  if (!handle.get_rep()) return false;

  SimpleOPortHandle dataport;

  //! We always require the port to be there.
  if (!(get_oport_handle(name,dataport)))
  {
    throw "Incorrect data type sent to output port '" + name +
      "' (dynamic_cast failed).";
  }

  if( send_intermediate )
    dataport->send_intermediate( handle );
  else
    dataport->send_and_dereference( handle, !cache );

  return true;
}


template<class DH>
bool
Module::send_output_handle(int portnum, DH& handle,
			   bool cache, bool send_intermediate)
{
  //! Don't send on empty, assume cached version is more valid instead.
  //! Dunno if false return value is correct.  We don't check returns
  //! on this one.
  if (!handle.get_rep()) return false;

  SimpleOPortHandle dataport;

  //! We always require the port to be there.
  if (!(get_oport_handle(portnum,dataport)))
  {
    std::ostringstream oss;
    oss << portnum;
    throw "Incorrect data type sent to output port number '" + oss.str() +
      "' (dynamic_cast failed).";
    return false;
  }

  if( send_intermediate )
    dataport->send_intermediate( handle );
  else
    dataport->send_and_dereference( handle, !cache );

  return true;
}


template<class DP>
bool Module::get_iport_handle(const std::string &name, DP& handle)
{
  std::vector<IPortHandle> iports;
  iports = iports_.get_port_range(name);
  if (iports.size() == 0)
  {
    // TODO: improve error message
    throw "Unable to initialize iport '" + name + "'.";  
  }

  IPort* ptr = iports[0].get_rep();
  typename DP::pointer_type port_ptr = dynamic_cast<typename DP::pointer_type>(ptr);

  handle = port_ptr;
  
  return (handle.get_rep()!=0);
}


template<class DP>
bool Module::get_iport_handles(const std::string &name, std::vector<DP>& handles)
{
  std::vector<IPortHandle> iports;
  iports = iports_.get_port_range(name);
  if (iports.size() == 0)
  {
    throw "Unable to initialize iport '" + name + "'.";  
  }
  
  DP handle;
  for (size_t p=0; p<iports.size();p++)
  {
    handle = dynamic_cast<typename DP::pointer_type>(iports[p].get_rep());
    if (handle.get_rep()) handles.push_back(handle);
  }

  return (handles.size()!=0);
}

template<class DP>
bool Module::get_oport_handle(const std::string &name, DP& handle)
{

  std::vector<OPortHandle> oports;
  oports = oports_.get_port_range(name);
  if (oports.size() == 0)
  {
    throw "Unable to initialize oport '" + name + "'.";  
  }
  OPort* ptr = oports[0].get_rep();
  typename DP::pointer_type port_ptr = dynamic_cast<typename DP::pointer_type>(ptr);

  handle = port_ptr;
  return (handle.get_rep()!=0);
}

template<class DP>
bool Module::get_iport_handle(int item, DP& handle)
{

  IPortHandle h = iports_[item];
  handle = dynamic_cast<typename DP::pointer_type>(h.get_rep());


  return (handle.get_rep()!=0);
}

template<class DP>
bool Module::get_oport_handle(int item, DP& handle)
{

  OPortHandle h = oports_[item];
  OPort* ptr = h.get_rep();
  typename DP::pointer_type port_ptr = dynamic_cast<typename DP::pointer_type>(ptr);
  handle = port_ptr;

  return (handle.get_rep()!=0);
}


typedef LockingHandle<Module> ModuleHandle;

}

#endif
