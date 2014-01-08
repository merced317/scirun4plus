//  
//  For more information, please see: http://software.sci.utah.edu
//  
//  The MIT License
//  
//  Copyright (c) 2009 Scientific Computing and Imaging Institute,
//  University of Utah.
//  
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//  
//    File   : NetworkIO.cc
//    Author : Martin Cole
//    Date   : Mon Feb  6 14:32:15 2006

// TODO: change string concatenation to string streams

#include <Dataflow/Network/NetworkIO.h>
#include <Dataflow/Network/Network.h>
#include <Dataflow/Network/NetworkEditor.h>
#include <Dataflow/Network/Module.h>
#include <Dataflow/GuiInterface/TCLInterface.h>
#include <Core/Util/Environment.h>
#include <Core/Util/Assert.h>
#include <Core/Util/StringUtil.h>
#include <Core/Util/Dir.h>
#include <sci_debug.h>

#include <libxml/catalog.h>

#include <iostream>
#include <sstream>

namespace SCIRun {
  
  inline 
  std::string
  NetworkIO::get_mod_id(const std::string& id)
  {
    id_map_t &mmap = netid_to_modid_.top();
    const std::string sn("Subnet"); 
    return (id == sn) ? sn : mmap[id];
  }
  
  
  std::string 
  NetworkIO::gui_push_subnet_ctx()
  {
    std::string cmmd = "set sn_ctx $Subnet(Loading)";
    std::string s = TCLInterface::eval(cmmd);
    return s;
  }
  
  
  void 
  NetworkIO::gui_pop_subnet_ctx(std::string ctx)
  {
    std::string cmmd = "set Subnet(Loading) " + ctx;
    TCLInterface::eval(cmmd);
    
    --sn_ctx_;
    netid_to_modid_.pop();
    netid_to_conid_.pop();
  }
  
  
  void 
  NetworkIO::gui_add_subnet_at_position(const std::string &mod_id, 
                                        const std::string &module, 
                                        const std::string& x, 
                                        const std::string &y)
  {
    ++sn_count_;
    // map the subnet to a local var before we push maps.
    id_map_t &mmap = netid_to_modid_.top();
    
    std::ostringstream snic;
    snic << "SubnetIcon" << sn_count_;
    mmap[mod_id] = snic.str();
    
    netid_to_modid_.push(id_map_t());
    netid_to_conid_.push(id_map_t());
    
    std::ostringstream cmmd;
    
    cmmd << "set Subnet(Loading) [makeSubnetEditorWindow " 
    << sn_ctx_ << " " << x  << " " << y << "]";
    TCLInterface::eval(cmmd.str());
    ++sn_ctx_;
    
    std::ostringstream cmmd1;
    cmmd1 << "set Subnet(Subnet" << sn_count_ << "_Name) \"" << module << "\"";
    TCLInterface::eval(cmmd1.str());
  }
  
  
  void 
  NetworkIO::gui_add_module_at_position(const std::string &mod_id, 
                                        const std::string &cpackage, 
                                        const std::string &ccategory, 
                                        const std::string &cmodule, 
                                        const std::string &cversion,
                                        const std::string& x, 
                                        const std::string &y)
  {
    // Create the module.
    std::string package;
    std::string category;
    std::string module;
    std::string version;
    
    ModuleHandle mod =
    net_->add_module_maybe_replace(cpackage, ccategory, cmodule, cversion,
                                   package, category, module, version);
    
    // TODO: Fix crash bug here when package is not available.
    // Invoke nice gui for not loading this network rather than crash to
    // command line.
    if (!mod.get_rep())
    {
      //    package = "SCIRun"; category = "Unknown"; module = "Unknown";
      //    mod = net_->add_module(package, 
      //							 category, 
      //							 module);
      net_->incr_network_error_code();
    }
    
    if (mod.get_rep())
    {
      // Now tell tcl about the module.
      std::string cmmd = "addModuleAtAbsolutePosition " + package + " " + 
      category + " " + module + " " + x + " " + y + " " + mod->get_id();
      std::string mid = TCLInterface::eval(cmmd);
      id_map_t &mmap = netid_to_modid_.top();
      mmap[mod_id] = mid;
    }
  }
  
  void 
  NetworkIO::gui_add_connection(const std::string &con_id,
                                const std::string &from_id, 
                                const std::string &from_port,
                                const std::string &to_id, 
                                const std::string &to_port0)
  {
    std::string to_port = to_port0;
    std::string from = get_mod_id(from_id);
    std::string to = get_mod_id(to_id);
    std::string arg = "1";
    if (from.find("Subnet") == std::string::npos && 
        to.find("Subnet") == std::string::npos) 
    {
      arg = "0";
      // create the connection.
      ModuleHandle omod = net_->get_module_by_id(from);
      ModuleHandle imod = net_->get_module_by_id(to);
      
      if (omod.get_rep() == 0 || imod.get_rep() == 0)
      {
        std::cerr << "Bad connection made, one or more modules not available.\n";
        return;
      }
      
      int owhich;
      from_string(from_port,owhich);
      int iwhich;
      from_string(to_port,iwhich);
      
      if( omod->get_oport(owhich) == 0 )
      {
        std::cerr << "Can not get " << omod->get_modulename()
        << " output port " << owhich
        << " that connects to  " << imod->get_modulename()
        << " input port " << iwhich
        << std::endl;
        
        return;
      }
      
      if( imod->get_iport(iwhich) == 0 )
      {
        std::cerr << "Can not get " << imod->get_modulename()
        << " input port " << iwhich
        << " that connects to  " << omod->get_modulename()
        << " output port " << owhich
        << std::endl;
        
        return;
      }
      
      std::string d0 = omod->get_oport(owhich)->get_typename();
      std::string d1 = imod->get_iport(iwhich)->get_typename();
      
      //---------------------
      // Compatibility with 3.0.2
      // Hack to fix results from another ugly hack that was present here for a while
      
      // In order to support an intermediate solution that allowed dynamic ports on
      // CalculateFieldDataCompiled and ClipFieldByFunction
      // For this purpose these two ports were swapped and now we swapped them back.
      // This code should correct for this
      
      // TODO: when intermediate networks are gone, we should remove this hack
      
      if (d0 != d1)
      {
        if ((iwhich == 1 && d0 == "SCIRun::Field")||(iwhich == 1 && d0 == "Field")) { iwhich--; to_port = to_string(iwhich); }
        if ((iwhich == 0 && d0 == "SCIRun::String")||(iwhich == 0 && d0 == "String")) { iwhich++; to_port = to_string(iwhich); }
        
        if( imod->get_iport(iwhich) == 0 )
        {
          std::cerr << "Can not get " << imod->get_modulename()
          << " input port " << iwhich
          << " that connects to  " << omod->get_modulename()
          << " output port " << owhich
          << std::endl;
          
          return;
        }
        d1 = imod->get_iport(iwhich)->get_typename();
      }
      
      //---------------------
      
      if (d0 != d1)
      {
        std::cerr << "Port type mismatch between output module "
        << omod->get_modulename()
        << " output port " << owhich << " with type " << d0
        << " that connects to input module " << iwhich
        << imod->get_modulename()
        << " input port " << iwhich << " with type " << d1
        << std::endl;
        
        return;
      }
      
      net_->connect(omod, owhich, imod, iwhich);
    }
    // Now tell tcl about the connection.
    // tell tcl about the connection, last argument tells it not to creat the 
    // connection on the C side, since we just did that above.
    std::string cmmd = "createConnection [list " + from + " " + from_port +
    " " + to + " " + to_port + "] 0 " + arg;
    
    std::string cid = TCLInterface::eval(cmmd);
    id_map_t &cmap = netid_to_conid_.top();
    cmap[con_id] = cid;
  }
  
  
  void 
  NetworkIO::gui_set_connection_disabled(const std::string &con_id)
  { 
    id_map_t &cmap = netid_to_conid_.top();
    std::string con = cmap[con_id];
    std::string cmmd = "set Disabled(" + con + ") {1}";
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_set_module_port_caching(const std::string &mid, const std::string &pid,
                                         const std::string &val)
  {
    std::string modid = get_mod_id(mid);
    std::string cmmd = "setPortCaching " + modid + " " + pid + " " + val;
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_call_module_callback(const std::string &id, const std::string &call)
  {
    std::string modid = get_mod_id(id);
    std::string cmmd = modid + " " + call;
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_set_modgui_variable(const std::string &mod_id, const std::string &var, 
                                     const std::string &val)
  {  
    std::string cmmd;
    std::string mod = get_mod_id(mod_id);
    
    // Some variables in tcl are quoted strings with spaces, so in that 
    // case insert the module identifying std::string after the first quote.
    size_t pos = var.find_first_of("\"");
    if (pos == std::string::npos) 
    {
      cmmd = "set " + mod + "-" + var +  " " + val;
    } 
    else 
    {
      std::string v = var;
      v.insert(++pos, mod + "-");
      cmmd = "set " + v +  " " + val;
    }
    TCLInterface::eval(cmmd);
  }
  
  void
  NetworkIO::gui_call_mod_post_read(const std::string &mod_id)
  {
    std::string modid = get_mod_id(mod_id);
    ModuleHandle mod = net_->get_module_by_id(modid);
    if (mod.get_rep()) { mod->post_read(); }
  }
  
  
  void 
  NetworkIO::gui_set_connection_route(const std::string &con_id, 
                                      const std::string &route)
  {  
    id_map_t &cmap = netid_to_conid_.top();
    std::string con = cmap[con_id];
    std::string cmmd = "set ConnectionRoutes(" + con + ") " + route;
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_set_module_note(const std::string &mod_id, const std::string &pos, 
                                 const std::string &col, const std::string &note)
  {  
    std::string mod = get_mod_id(mod_id);
    std::string cmmd = "set Notes(" + mod + ") " + note;
    TCLInterface::eval(cmmd);
    cmmd = "set Notes(" + mod + "-Position) " + pos;
    TCLInterface::eval(cmmd);
    cmmd = "set Notes(" + mod + "-Color) " + col;
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_set_connection_note(const std::string &con_id, const std::string &pos, 
                                     const std::string &col, const std::string &note)
  { 
    id_map_t &cmap = netid_to_conid_.top();
    std::string con = cmap[con_id];
    std::string cmmd = "set Notes(" + con + ") " + note;
    TCLInterface::eval(cmmd);
    cmmd = "set Notes(" + con + "-Position) " + pos;
    TCLInterface::eval(cmmd);
    cmmd = "set Notes(" + con + "-Color) " + col;
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_set_variable(const std::string &var, const std::string &val)
  {  
    std::string cmmd = "set " + var +  " " + val;
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::gui_open_module_gui(const std::string &mod_id)
  {
    std::string mod = get_mod_id(mod_id);
    std::string cmmd = mod + " initialize_ui";
    TCLInterface::eval(cmmd);
  }
  
  
  void 
  NetworkIO::process_environment(const xmlNodePtr enode)
  {
    xmlNodePtr node = enode->children;
    for (; node != 0; node = node->next) 
    {
      if (std::string(to_char_ptr(node->name)) == std::string("var")) 
      {
        xmlAttrPtr name_att = get_attribute_by_name(node, "name");
        xmlAttrPtr val_att = get_attribute_by_name(node, "val");
        env_subs_[std::string(to_char_ptr(name_att->children->content))] = 
        std::string(to_char_ptr(val_att->children->content));
      }
    }
  }
  
  
  void 
  NetworkIO::process_modules_pass1(const xmlNodePtr enode)
  {
    xmlNodePtr node = enode->children;
    for (; node != 0; node = node->next) 
    {
      if (std::string(to_char_ptr(node->name)) == std::string("module") ||
          std::string(to_char_ptr(node->name)) == std::string("subnet")) 
      {
        bool do_subnet = std::string(to_char_ptr(node->name)) == std::string("subnet");
        xmlNodePtr network_node = 0;
        
        std::string x,y;
        xmlAttrPtr id_att = get_attribute_by_name(node, "id");
        xmlAttrPtr package_att = get_attribute_by_name(node, "package");
        xmlAttrPtr category_att = get_attribute_by_name(node, "category");
        xmlAttrPtr name_att = get_attribute_by_name(node, "name");
        xmlAttrPtr version_att = get_attribute_by_name(node, "version");
        
        
        std::string mname = std::string(to_char_ptr(name_att->children->content));
        std::string mid = std::string(to_char_ptr(id_att->children->content));
        xmlNodePtr pnode = node->children;
        for (; pnode != 0; pnode = pnode->next) 
        {
          if (std::string(to_char_ptr(pnode->name)) == std::string("position")) 
          {
            xmlAttrPtr x_att = get_attribute_by_name(pnode, "x");
            xmlAttrPtr y_att = get_attribute_by_name(pnode, "y");
            x = std::string(to_char_ptr(x_att->children->content));
            y = std::string(to_char_ptr(y_att->children->content)); 
            
            if (do_subnet) 
            {
              std::string old_ctx = gui_push_subnet_ctx();
              gui_add_subnet_at_position(mid, mname, x, y);
              
              ASSERT(network_node != 0);
              process_network_node(network_node);
              gui_pop_subnet_ctx(old_ctx);
            } 
            else 
            {
              std::string package = std::string(to_char_ptr(package_att->children->content));
              std::string category = std::string(to_char_ptr(category_att->children->content));
              std::string version = "1.0";
              if (version_att != 0) version = std::string(to_char_ptr(version_att->children->content));
              
              gui_add_module_at_position(mid, package, category, 
                                         mname, version, x, y);
            }
          }
          else if (std::string(to_char_ptr(pnode->name)) == std::string("network")) 
          {
            network_node = pnode;
          } 	
          else if (std::string(to_char_ptr(pnode->name)) == std::string("note")) 
          {
            xmlAttrPtr pos_att = get_attribute_by_name(pnode, "position");
            xmlAttrPtr col_att = get_attribute_by_name(pnode, "color");
            std::string pos, col, note;
            if (pos_att)
              pos = std::string(to_char_ptr(pos_att->children->content));
            if (col_att)
              col = std::string(to_char_ptr(col_att->children->content));
            
            note = std::string(to_char_ptr(pnode->children->content));
            gui_set_module_note(mid, pos, col, note);
          }
          else if (std::string(to_char_ptr(pnode->name)) == std::string("port_caching")) 
          {
            xmlNodePtr pc_node = pnode->children;
            for (; pc_node != 0; pc_node = pc_node->next) 
            {
              if (std::string(to_char_ptr(pc_node->name)) == std::string("port")) 
              {
                xmlAttrPtr pid_att = get_attribute_by_name(pc_node, "id");
                xmlAttrPtr val_att = get_attribute_by_name(pc_node, "val");
                gui_set_module_port_caching(mid, 
                                            std::string(to_char_ptr(pid_att->children->content)),
                                            std::string(to_char_ptr(val_att->children->content)));
                
              }
            }
          }
        }
      }
    }
  }
  
  
  void 
  NetworkIO::process_modules_pass2(const xmlNodePtr enode)
  {
    xmlNodePtr node = enode->children;
    for (; node != 0; node = node->next) 
    {
      if (std::string(to_char_ptr(node->name)) == std::string("module")) 
      {
        std::string x,y;
        xmlAttrPtr id_att = get_attribute_by_name(node, "id");
        xmlAttrPtr visible_att = get_attribute_by_name(node, "gui_visible");
        
        xmlNodePtr pnode;
        
        pnode = node->children;
        for (; pnode != 0; pnode = pnode->next) 
        {	
          if (std::string(to_char_ptr(pnode->name)) == std::string("var")) 
          {
            xmlAttrPtr name_att = get_attribute_by_name(pnode, "name");
            xmlAttrPtr val_att = get_attribute_by_name(pnode, "val");
            xmlAttrPtr filename_att = get_attribute_by_name(pnode,"filename");
            xmlAttrPtr substitute_att= get_attribute_by_name(pnode,"substitute");
            
            std::string val = std::string(to_char_ptr(val_att->children->content));
            
            std::string filename = "no";
            if (filename_att != 0) filename = 
              std::string(to_char_ptr(filename_att->children->content));
            std::string substitute = "yes";
            if (substitute_att != 0) substitute = 
              std::string(to_char_ptr(substitute_att->children->content));             
            
            if (filename == "yes") 
            {
              if (substitute == "yes") val = process_substitute(val);
              val = process_filename(val); 
              if (val.size() > 0 && val[0] == '{')
                val = std::string("\"")+val.substr(1,val.size()-2)+std::string("\"");
            }
            else
            {
              if (substitute == "yes") val = process_substitute(val);
            }
            
            gui_set_modgui_variable(
                                    std::string(to_char_ptr(id_att->children->content)),
                                    std::string(to_char_ptr(name_att->children->content)),
                                    val);
          }
        }
        
        bool has_gui_callback = false;
        pnode = node->children;
        for (; pnode != 0; pnode = pnode->next) 
        {	
          if (std::string(to_char_ptr(pnode->name)) == std::string("gui_callback")) 
          {
            has_gui_callback = true;
            xmlNodePtr gc_node = pnode->children;
            for (; gc_node != 0; gc_node = gc_node->next) 
            {
              if (std::string(to_char_ptr(gc_node->name)) == std::string("callback")) 
              {
                std::string call = std::string(to_char_ptr(gc_node->children->content));
                gui_call_module_callback(
                                         std::string(to_char_ptr(id_att->children->content)),
                                         call);
                
              }
            }
          }
        }
        
        if (has_gui_callback)
        {
          pnode = node->children;
          for (; pnode != 0; pnode = pnode->next) 
          {	
            if (std::string(to_char_ptr(pnode->name)) == std::string("var")) 
            {
              xmlAttrPtr name_att = get_attribute_by_name(pnode, "name");
              xmlAttrPtr val_att = get_attribute_by_name(pnode, "val");
              xmlAttrPtr filename_att = get_attribute_by_name(pnode,"filename");
              xmlAttrPtr substitute_att= get_attribute_by_name(pnode,"substitute");
              
              std::string val = std::string(to_char_ptr(val_att->children->content));
              
              std::string filename = "no";
              if (filename_att != 0) filename = 
                std::string(to_char_ptr(filename_att->children->content));
              std::string substitute = "yes";
              if (substitute_att != 0) substitute = 
                std::string(to_char_ptr(substitute_att->children->content));             
              
              if (filename == "yes") 
              {
                if (substitute == "yes") val = process_substitute(val);
                val = process_filename(val); 
                if (val.size() > 0 && val[0] == '{')
                  val = std::string("\"")+val.substr(1,val.size()-2)+std::string("\"");
              }
              else
              {
                if (substitute == "yes") val = process_substitute(val);
              }
              
              gui_set_modgui_variable(
                                      std::string(to_char_ptr(id_att->children->content)),
                                      std::string(to_char_ptr(name_att->children->content)),
                                      val);
            }
          }
        }
        
        gui_call_mod_post_read(std::string(to_char_ptr(id_att->children->content)));
        
        if (visible_att && 
            std::string(to_char_ptr(visible_att->children->content)) == "yes")
        {
          gui_open_module_gui(std::string(to_char_ptr(id_att->children->content)));
        }
      }
    }
  }
  
  
  void 
  NetworkIO::process_connections(const xmlNodePtr enode)
  {
    xmlNodePtr node = enode->children;
    for (; node != 0; node = node->next) {
      if (std::string(to_char_ptr(node->name)) == std::string("connection")) {
        xmlAttrPtr id_att = get_attribute_by_name(node, "id");
        xmlAttrPtr from_att = get_attribute_by_name(node, "from");
        xmlAttrPtr fromport_att = get_attribute_by_name(node, "fromport");
        xmlAttrPtr to_att = get_attribute_by_name(node, "to");
        xmlAttrPtr toport_att = get_attribute_by_name(node, "toport");
        xmlAttrPtr dis_att = get_attribute_by_name(node, "disabled");
        
        std::string id = std::string(to_char_ptr(id_att->children->content));
        
        gui_add_connection(id,
                           std::string(to_char_ptr(from_att->children->content)),
                           std::string(to_char_ptr(fromport_att->children->content)),
                           std::string(to_char_ptr(to_att->children->content)),
                           std::string(to_char_ptr(toport_att->children->content)));
        
        if (dis_att && 
            std::string(to_char_ptr(dis_att->children->content)) == "yes") 
        {
          gui_set_connection_disabled(id);
        }
        
        
        xmlNodePtr cnode = node->children;
        for (; cnode != 0; cnode = cnode->next) 
        {	
          if (std::string(to_char_ptr(cnode->name)) == std::string("route")) 
          {
            gui_set_connection_route(id, 
                                     std::string(to_char_ptr(cnode->children->content)));
          } 
          else if (std::string(to_char_ptr(cnode->name)) == std::string("note")) 
          {
            xmlAttrPtr pos_att = get_attribute_by_name(cnode, "position");
            xmlAttrPtr col_att = get_attribute_by_name(cnode, "color");
            std::string pos, col, note;
            if (pos_att)
              pos = std::string(to_char_ptr(pos_att->children->content));
            if (col_att)
              col = std::string(to_char_ptr(col_att->children->content));
            
            note = std::string(to_char_ptr(cnode->children->content));
            gui_set_connection_note(id, pos, col, note);
          } 
        }
      }
    }
  }
  
  
  std::string
  NetworkIO::process_filename(const std::string &orig)
  {
    // This function reinterprets a filename
    
    // Copy the string and remove TCL brackets
    std::string filename = orig.substr(1,orig.size()-2);
    
    
    // Remove blanks and tabs from the input 
    // (Some could have editted the XML file manually and may have left spaces)
    
    if (filename.size() > 0)
    {
      if (filename[0] == '{')
      {
        filename = filename.substr(1,filename.size()-2);
      }
    }
    
    while (filename.size() > 0 && 
           ((filename[0] == ' ')||(filename[0] == '\t'))) {
      filename = filename.substr(1);
    }
    
    while (filename.size() > 0 && 
           ((filename[filename.size()-1] == ' ')||
            (filename[filename.size()-1] == '\t'))) {
      filename = filename.substr(1,filename.size()-1);
    }
    
    // Check whether filename is absolute:
    
    if ( filename.size() > 0 && filename[0] == '/') return (std::string("{")+filename+std::string("}")); // Unix absolute path
    if ( filename.size() > 1 && filename[1] == ':') return (std::string("{")+filename+std::string("}")); // Windows absolute path
    
    // If not substitute: 
    
    // Create a dynamic substitute called NETWORKDIR for relative path names
    FullFileName fnet_file(net_file_);
    std::string net_file = fnet_file.get_abs_filename();
    std::string net_path = fnet_file.get_abs_path();
    env_subs_[std::string("scisub_networkdir")] = std::string("SCIRUN_NETWORKDIR");
    sci_putenv("SCIRUN_NETWORKDIR",net_path);
    
    id_map_t::const_iterator iter = env_subs_.begin();
    while (iter != env_subs_.end()) 
    {
      const std::pair<const std::string, std::string> &kv = *iter++;
      const std::string &key = kv.first;
      
      id_map_t::size_type idx = filename.find(key);
      
      if (idx != std::string::npos) 
      {
        const std::string &env_var = kv.second;
        const char* env = sci_getenv(env_var);
        std::string subst = (env != 0)?env:"";
        
        if (env_var == std::string("SCIRUN_DATASET") && subst.size() == 0)
        {
          subst = std::string("sphere");
        }
        while (idx != std::string::npos) {
          filename = filename.replace(idx, key.size(), subst);
          idx = filename.find(key);
        }
      }
    }

    FullFileName ffn(filename);
    filename = ffn.get_abs_filename();
    
    return (std::string("{")+filename+std::string("}"));
  }
  
  
  std::string
  NetworkIO::process_substitute(const std::string &orig)
  {
    std::string src = orig;
    id_map_t::const_iterator iter = env_subs_.begin();
    while (iter != env_subs_.end()) 
    {
      const std::pair<const std::string, std::string> &kv = *iter++;
      const std::string &key = kv.first;
      
      id_map_t::size_type idx = src.find(key);
      
      if (idx != std::string::npos) {
        const std::string &env_var = kv.second;
        const char* env = sci_getenv(env_var);
        std::string subst = (env != 0)?env:"";
        
        if (env_var == std::string("SCIRUN_DATASET") && subst.size() == 0)
        {
          subst = std::string("sphere");
        }
        while (idx != std::string::npos) {
          src = src.replace(idx, key.size(), subst);
          idx = src.find(key);
        }
      }
    }
    
    return (src);
  }
  
  
  void
  NetworkIO::load_net(const std::string &net)
  {
    FullFileName netfile(net);
    net_file_ = netfile.get_abs_filename();
    sci_putenv("SCIRUN_NETFILE", net);
    load_network();
  }
  
  
  void
  NetworkIO::process_network_node(xmlNode* network_node)
  {
    // have to multi pass this document to workaround tcl timing issues.
    // PASS 1 - create the modules and connections
    xmlNode* node = network_node;
    for (; node != 0; node = node->next) {
      // skip all but the component node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        //! set attributes
        //xmlAttrPtr version_att = get_attribute_by_name(node, "version");
        xmlAttrPtr name_att = get_attribute_by_name(node, "name");
        gui_set_variable(std::string("name"), 
                         std::string(to_char_ptr(name_att->children->content)));
        xmlAttrPtr bbox_att = get_attribute_by_name(node, "bbox");
        gui_set_variable(std::string("bbox"), 
                         std::string(to_char_ptr(bbox_att->children->content)));
        xmlAttrPtr cd_att = get_attribute_by_name(node, "creationDate");
        gui_set_variable(std::string("creationDate"), 
                         std::string(to_char_ptr(cd_att->children->content)));
        xmlAttrPtr ct_att = get_attribute_by_name(node, "creationTime");
        gui_set_variable(std::string("creationTime"), 
                         std::string(to_char_ptr(ct_att->children->content)));
        xmlAttrPtr geom_att = get_attribute_by_name(node, "geometry");
        gui_set_variable(std::string("geometry"), 
                         std::string(to_char_ptr(geom_att->children->content)));
        
        
        xmlAttrPtr netversion_att = get_attribute_by_name(node, "netversion");
        if (netversion_att)
        {
          gui_set_variable(std::string("netversion"), 
                           std::string(to_char_ptr(netversion_att->children->content)));
        }
        else
        {
          gui_set_variable(std::string("netversion"), "0");      
        }
        
        xmlNode* enode = node->children;
        for (; enode != 0; enode = enode->next) {
          
          if (enode->type == XML_ELEMENT_NODE && 
              std::string(to_char_ptr(enode->name)) == std::string("environment")) 
          {
            process_environment(enode);
          } else if (enode->type == XML_ELEMENT_NODE && 
                     std::string(to_char_ptr(enode->name)) == std::string("modules")) 
          {
            process_modules_pass1(enode);
          } else if (enode->type == XML_ELEMENT_NODE && 
                     std::string(to_char_ptr(enode->name)) == std::string("connections")) 
          {
            process_connections(enode);
          } else if (enode->type == XML_ELEMENT_NODE && 
                     std::string(to_char_ptr(enode->name)) == std::string("note")) 
          {
            gui_set_variable(std::string("notes"), 
                             std::string(to_char_ptr(enode->children->content)));
          }
        }
      }
    }
    
    // PASS 2 -- call the callbacks and set the variables
    node = network_node;
    for (; node != 0; node = node->next) {
      // skip all but the component node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        xmlNode* enode = node->children;
        for (; enode != 0; enode = enode->next) {
          
          if (enode->type == XML_ELEMENT_NODE && 
              std::string(to_char_ptr(enode->name)) == std::string("modules")) 
          {
            process_modules_pass2(enode);
          }
        }
      }
    }
  }
  
  
  bool
  NetworkIO::load_network()
  {
    /*
     * this initializes the library and checks potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    
    // Reset some variables
    FullFileName netfile(net_file_);
    net_file_ = netfile.get_abs_filename();
    sci_putenv("SCIRUN_NETFILE", net_file_);
    sn_count_ = 0;
    sn_ctx_ = 0;	 
    
    LIBXML_TEST_VERSION;
    
    xmlParserCtxtPtr ctxt; /* the parser context */
    xmlDocPtr doc; /* the resulting document tree */
    
    std::string dtd_path( sci_getenv("SCIRUN_SRCDIR") );
    dtd_path += "/Dataflow/XML/network.dtd";
    xmlInitializeCatalog();
    xmlCatalogAdd(XMLUtil::char_to_xmlChar("public"),
                  XMLUtil::char_to_xmlChar("-//SCIRun/Network DTD"),
                  XMLUtil::char_to_xmlChar( dtd_path.c_str() ));
    
    /* create a parser context */
    ctxt = xmlNewParserCtxt();
    if (ctxt == 0) {
      std::cerr << "ComponentNode.cc: Failed to allocate parser context" 
      << std::endl;
      return false;
    }
    
    // parse the file, activating the DTD validation option (disabled - see below)
    // XML_PARSE_DTDVALID
    
    int flags = XML_PARSE_DTDATTR;
    // TODO: replacing the DTD URI in SCIRun network files is currently broken.
    // See bug 157 in the GForge CIBC project bug tracker.
    // Until this can be fixed, suppress the warning unless debugging.
#ifdef DEBUG
    flags |= XML_PARSE_PEDANTIC;
#else
    flags |= XML_PARSE_NOWARNING;
#endif
    doc = xmlCtxtReadFile(ctxt, net_file_.c_str(), 0, flags);
    /* check if parsing suceeded */
    if (doc == 0) {
      std::cerr << "ComponentNode.cc: Failed to parse " << net_file_ 
      << std::endl;
      return false;
    } else {
      /* check if validation suceeded */
      if (ctxt->valid == 0) {
        std::cerr << "ComponentNode.cc: Failed to validate " << net_file_ 
        << std::endl;
        return false;
      }
    }
    
    TCLInterface::eval("netedit dontschedule");
    
    // parse the doc at network node.
    process_network_node(doc->children);
    
    xmlFreeDoc(doc);
    /* free up the parser context */
    xmlFreeParserCtxt(ctxt);  
#ifndef _WIN32
    // there is a problem on windows when using Uintah 
    // which is either caused or exploited by this
    xmlCleanupParser();
#endif
    
    TCLInterface::eval("setGlobal NetworkChanged 0");
    TCLInterface::eval("set netedit_savefile {" + net_file_ + "}");
    TCLInterface::eval("netedit scheduleok");
    TCLInterface::eval("update_network_editor_title \"" + net_file_ + "\"");
    
    return true;
  }
  
  
  // push a new network root node.
  void 
  NetworkIO::push_subnet_scope(const std::string &id, const std::string &name)
  {
    // this is a child node of the network.
    xmlNode* mod_node = 0;
    xmlNode* net_node = 0;
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        net_node = node;
        xmlNode* mnode = node->children;
        for (; mnode != 0; mnode = mnode->next) {
          if (std::string(to_char_ptr(mnode->name)) == std::string("modules")) {
            mod_node = mnode;
            break;
          }
        }
      }
    }
    if (! mod_node) { 
      if (! net_node) { 
        std::cerr << "ERROR: could not find top level node." << std::endl;
        return;
      }
      mod_node = xmlNewChild(net_node, 0, BAD_CAST "modules", 0);
    }
    xmlNodePtr tmp = xmlNewChild(mod_node, 0, BAD_CAST "subnet", 0);
    xmlNewProp(tmp, BAD_CAST "id", BAD_CAST id.c_str());
    xmlNewProp(tmp, BAD_CAST "package", BAD_CAST "subnet");
    xmlNewProp(tmp, BAD_CAST "category", BAD_CAST "subnet");
    xmlNewProp(tmp, BAD_CAST "name", BAD_CAST name.c_str());
    xmlNewProp(tmp, BAD_CAST "version", BAD_CAST "1.0");
    
    xmlNodePtr sn_node = xmlNewChild(tmp, 0, BAD_CAST "network", 0);
    xmlNewProp(sn_node, BAD_CAST "version", BAD_CAST "contained");
    
    subnets_.push(sn_node);
  }
  
  
  void 
  NetworkIO::pop_subnet_scope()
  {
    subnets_.pop();
  }
  
  
  void 
  NetworkIO::start_net_doc(const std::string &fname, const std::string &vers, const std::string &netversion)
  {
    out_fname_ = fname;
    xmlNodePtr root_node = 0; /* node pointers */
    xmlDtdPtr dtd = 0;        /* DTD pointer */
    
    LIBXML_TEST_VERSION;
    
    /* 
     * Creates a new document, a node and set it as a root node
     */
    doc_ = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(0, BAD_CAST "network");
    subnets_.push(root_node);
    xmlDocSetRootElement(doc_, root_node);
    
    /*
     * Creates a DTD declaration.
     */
    std::string dtdstr = std::string("network.dtd");
    
    dtd = xmlCreateIntSubset(doc_, BAD_CAST "network", 
                             BAD_CAST "-//SCIRun/Network DTD", 
                             BAD_CAST dtdstr.c_str());
    
    /* 
     * xmlNewChild() creates a new node, which is "attached" as child node
     * of root_node node. 
     */
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST vers.c_str());
    xmlNewProp(root_node, BAD_CAST "netversion", BAD_CAST netversion.c_str());
    
    FullFileName netfile(fname);
    net_file_ = netfile.get_abs_filename();
    
    // Filter out backup names
    std::string filename;
    for (size_t k=0; k < fname.size(); k++) if (fname[k] != '#') filename += fname[k];
    sci_putenv("SCIRUN_NETFILE", filename);
  }
  
  
  void
  NetworkIO::write_net_doc()
  {
    // write the file
    xmlSaveFormatFileEnc(out_fname_.c_str(), doc_, "UTF-8", 1);
    
    // free the document
    xmlFreeDoc(doc_);
    doc_ = 0;
    out_fname_ = "";
    done_writing_ = true;
  }
  
  
  void 
  NetworkIO::add_net_var(const std::string &var, const std::string &val)
  {
    // add these as attributes of the network node.
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        break;
      }
    }
    if (! node) {
      std::cerr << "ERROR: could not find top level node." << std::endl;
      return;
    }
    xmlNewProp(node, BAD_CAST var.c_str(), BAD_CAST val.c_str());
  }
  
  
  void 
  NetworkIO::add_environment_sub(const std::string &var, const std::string &val)
  {
    // this is a child node of the network.
    xmlNode* env_node = 0;
    xmlNode* net_node = 0;
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        net_node = node;
        xmlNode* enode = node->children;
        for (; enode != 0; enode = enode->next) {
          if (std::string(to_char_ptr(enode->name)) == std::string("environment")) {
            env_node = enode;
            break;
          }
        }
      }
    }
    if (! env_node) { 
      if (! net_node) { 
        std::cerr << "ERROR: could not find top level node." << std::endl;
        return;
      }
      env_node = xmlNewChild(net_node, 0, BAD_CAST "environment", 0);
    }
    xmlNodePtr tmp = xmlNewChild(env_node, 0, BAD_CAST "var", 0);
    xmlNewProp(tmp, BAD_CAST "name", BAD_CAST var.c_str());
    xmlNewProp(tmp, BAD_CAST "val", BAD_CAST val.c_str());
  }
  
  
  void 
  NetworkIO::add_net_note(const std::string &val)
  {
    // this is a child node of the network, must come after 
    // environment node if it exists.
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        break;
      }
    }
    if (! node) { 
      std::cerr << "ERROR: could not find 'network' node." << std::endl;
      return;
    }
    
    xmlNewTextChild(node, 0, BAD_CAST "note", BAD_CAST val.c_str()); 
  }
  
  
  void 
  NetworkIO::add_module_node(const std::string &id, const std::string &pack, 
                             const std::string &cat, const std::string &mod, const std::string& ver)
  {
    // this is a child node of the network.
    xmlNode* mod_node = 0;
    xmlNode* net_node = 0;
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        net_node = node;
        xmlNode* mnode = node->children;
        for (; mnode != 0; mnode = mnode->next) {
          if (std::string(to_char_ptr(mnode->name)) == std::string("modules")) {
            mod_node = mnode;
            break;
          }
        }
      }
    }
    if (! mod_node) { 
      if (! net_node) { 
        std::cerr << "ERROR: could not find top level node." << std::endl;
        return;
      }
      mod_node = xmlNewChild(net_node, 0, BAD_CAST "modules", 0);
    }
    xmlNodePtr tmp = xmlNewChild(mod_node, 0, BAD_CAST "module", 0);
    xmlNewProp(tmp, BAD_CAST "id", BAD_CAST id.c_str());
    xmlNewProp(tmp, BAD_CAST "package", BAD_CAST pack.c_str());
    xmlNewProp(tmp, BAD_CAST "category", BAD_CAST cat.c_str());
    xmlNewProp(tmp, BAD_CAST "name", BAD_CAST mod.c_str());
    xmlNewProp(tmp, BAD_CAST "version", BAD_CAST ver.c_str());
  }
  
  
  xmlNode*
  NetworkIO::get_module_node(const std::string &id)
  {  
    xmlNode* mid_node = 0;
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        xmlNode* msnode = node->children;
        for (; msnode != 0; msnode = msnode->next) {
          
          if (std::string(to_char_ptr(msnode->name)) == std::string("modules")) {
            xmlNode* mnode = msnode->children;
            for (; mnode != 0; mnode = mnode->next) {
              
              if (std::string(to_char_ptr(mnode->name)) == std::string("module") ||
                  std::string(to_char_ptr(mnode->name)) == std::string("subnet")) 
              {
                xmlAttrPtr name_att = get_attribute_by_name(mnode, "id");
                std::string mid = std::string(to_char_ptr(name_att->children->content));
                if (mid == id) 
                {
                  mid_node = mnode;
                  break;
                }
              }
            }
          }
        }
      }
    }
    return mid_node;
  }
  
  
  void 
  NetworkIO::add_module_variable(const std::string &id, const std::string &var, 
                                 const std::string &val, bool filename, bool substitute, bool userelfilenames)
  {
    xmlNode* node = get_module_node(id);
    
    if (! node) 
    { 
      std::cerr << "ERROR: could not find module node with id (module variable): " << id << std::endl;
      return;
    }
    xmlNodePtr tmp = xmlNewChild(node, 0, BAD_CAST "var", 0);
    xmlNewProp(tmp, BAD_CAST "name", BAD_CAST var.c_str());
    
    std::string nval = val;
    if (filename && userelfilenames)
    {
      if ((nval.size() >0) &&  (nval[0] == '{'))
      {
        FullFileName relname(nval.substr(1, nval.size() - 2));
        nval = relname.get_rel_filename(out_fname_);
        nval = "{"+nval+"}";
      }
      else
      {
        FullFileName relname(nval);
        nval = relname.get_rel_filename(out_fname_);
      }
    }
    
    xmlNewProp(tmp, BAD_CAST "val", BAD_CAST nval.c_str());
    if (filename) xmlNewProp(tmp, BAD_CAST "filename", BAD_CAST "yes"); 
    
    if (substitute) xmlNewProp(tmp, BAD_CAST "substitute", BAD_CAST "yes"); 
    else xmlNewProp(tmp, BAD_CAST "substitute", BAD_CAST "no");
  }
  
  
  void 
  NetworkIO::set_module_gui_visible(const std::string &id)
  {
    xmlNode* node = get_module_node(id);
    
    if (! node) { 
      std::cerr << "ERROR: could not find module node with id (module gui visible): " << id << std::endl;
      return;
    }
    xmlNewProp(node, BAD_CAST "gui_visible", BAD_CAST "yes");
  }
  
  
  void 
  NetworkIO::add_module_gui_callback(const std::string &id, const std::string &call)
  {
    xmlNode* gc_node = 0;
    xmlNode* mod_node = get_module_node(id);
    if (! mod_node) { 
      std::cerr << "ERROR: could not find node for module id: " << id << std::endl;
      return;
    }
    xmlNode *node = mod_node->children;
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("gui_callback")) {
        gc_node = node;
        break;
      }
    }
    
    if (! gc_node) { 
      gc_node = xmlNewChild(mod_node, 0, BAD_CAST "gui_callback", 0);
    }
    xmlNewTextChild(gc_node, 0, BAD_CAST "callback", BAD_CAST call.c_str());
  }
  
  
  void 
  NetworkIO::add_module_position(const std::string &id, const std::string &x, 
                                 const std::string &y)
  {
    xmlNode* mid_node = get_module_node(id);
    
    if (! mid_node) { 
      std::cerr << "ERROR: could not find module node with id (add_module_position): " << id << std::endl;
      return;
    }
    xmlNodePtr tmp = xmlNewChild(mid_node, 0, BAD_CAST "position", 0);
    xmlNewProp(tmp, BAD_CAST "x", BAD_CAST x.c_str());
    xmlNewProp(tmp, BAD_CAST "y", BAD_CAST y.c_str());
    
  }
  
  
  void 
  NetworkIO::add_module_note(const std::string &id, const std::string &note)
  {
    xmlNode* mnode = get_module_node(id);
    
    if (! mnode) { 
      std::cerr << "ERROR: could not find module node with id (add_module_note): " << id << std::endl;
      return;
    }
    xmlNewTextChild(mnode, 0, BAD_CAST "note", BAD_CAST note.c_str());
  }
  
  
  void 
  NetworkIO::add_module_note_position(const std::string &id, const std::string &pos)
  {
    bool found = false;
    xmlNode* node = get_module_node(id);
    if (! node) { 
      std::cerr << "ERROR: could not find node for module id: " << id << std::endl;
      return;
    }
    node = node->children;
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("note")) {
        found = true;
        break;
      }
    }
    
    if (! found) { 
      std::cerr << "ERROR: could not find note node for module id: " << id << std::endl;
      return;
    }
    xmlNewProp(node, BAD_CAST "position", BAD_CAST pos.c_str());
  }
  
  
  void 
  NetworkIO::add_module_note_color(const std::string &id, const std::string &col)
  {
    bool found = false;
    xmlNode* node = get_module_node(id);
    if (! node) { 
      std::cerr << "ERROR: could not find node for module id: " << id << std::endl;
      return;
    }
    node = node->children;
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("note")) {
        found = true;
        break;
      }
    }
    
    if (! found) { 
      std::cerr << "ERROR: could not find note node for module id: " << id << std::endl;
      return;
    }
    xmlNewProp(node, BAD_CAST "color", BAD_CAST col.c_str());
  }
  
  
  void 
  NetworkIO::add_connection_node(const std::string &id, const std::string &fmod, 
                                 const std::string &fport, const std::string &tmod, 
                                 const std::string &tport)
  {
    // this is a child node of the network.
    xmlNode* con_node= 0;
    xmlNode* net_node= 0;
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        net_node = node;
        xmlNode* cnode = node->children;
        for (; cnode != 0; cnode = cnode->next) {
          if (std::string(to_char_ptr(cnode->name)) == std::string("connections")) {
            con_node = cnode;
            break;
          }
        }
      }
    }
    if (! con_node) { 
      if (! net_node) { 
        std::cerr << "ERROR: could not find top level node." << std::endl;
        return;
      }
      con_node = xmlNewChild(net_node, 0, BAD_CAST "connections", 0);
    }
    xmlNodePtr tmp = xmlNewChild(con_node, 0, BAD_CAST "connection", 0);
    xmlNewProp(tmp, BAD_CAST "id", BAD_CAST id.c_str());
    xmlNewProp(tmp, BAD_CAST "from", BAD_CAST fmod.c_str());
    xmlNewProp(tmp, BAD_CAST "fromport", BAD_CAST fport.c_str());
    xmlNewProp(tmp, BAD_CAST "to", BAD_CAST tmod.c_str());
    xmlNewProp(tmp, BAD_CAST "toport", BAD_CAST tport.c_str());
  }
  
  
  xmlNode*
  NetworkIO::get_connection_node(const std::string &id)
  {  
    xmlNode* cid_node = 0;
    xmlNode* node = subnets_.top();
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("network")) 
      {
        xmlNode* msnode = node->children;
        for (; msnode != 0; msnode = msnode->next) {
          
          if (std::string(to_char_ptr(msnode->name)) == std::string("connections")) {
            xmlNode* mnode = msnode->children;
            for (; mnode != 0; mnode = mnode->next) {
              
              if (std::string(to_char_ptr(mnode->name)) == std::string("connection")) {
                xmlAttrPtr name_att = get_attribute_by_name(mnode, "id");
                std::string cid = std::string(to_char_ptr(name_att->children->content));
                if (cid == id) {
                  cid_node = mnode;
                  break;
                }
              }
            }
          }
        }
      }
    }
    return cid_node;
  }
  
  
  void 
  NetworkIO::set_disabled_connection(const std::string &id)
  {
    xmlNode* cid_node = get_connection_node(id);
    
    if (! cid_node) { 
      std::cerr << "ERROR: could not find connection node with id: " << id << std::endl;
      return;
    }
    xmlNewProp(cid_node, BAD_CAST "disabled", BAD_CAST "yes");
  }
  
  
  void 
  NetworkIO::add_connection_route(const std::string &id, const std::string &route)
  {
    xmlNode* cid_node = get_connection_node(id);
    
    if (! cid_node) { 
      std::cerr << "ERROR: could not find connection node with id: " << id << std::endl;
      return;
    }
    
    xmlNewTextChild(cid_node, 0, BAD_CAST "route", BAD_CAST route.c_str());
  }
  
  
  void 
  NetworkIO::add_connection_note(const std::string &id, const std::string &note)
  {
    xmlNode* cid_node = get_connection_node(id);
    
    if (! cid_node) { 
      std::cerr << "ERROR: could not find connection node with id: " << id << std::endl;
      return;
    }
    
    xmlNewTextChild(cid_node, 0, BAD_CAST "note", BAD_CAST note.c_str());
  }
  
  
  void 
  NetworkIO::add_connection_note_position(const std::string &id, const std::string &pos)
  {
    bool found = false;
    xmlNode* node = get_connection_node(id);
    
    if (! node) { 
      std::cerr << "ERROR: could not find node for connection id: " << id << std::endl;
      return;
    }
    node = node->children;
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("note")) {
        found = true;
        break;
      }
    }
    
    if (! found) { 
      std::cerr << "ERROR: could not find note node for module id: " << id << std::endl;
      return;
    }
    xmlNewProp(node, BAD_CAST "position", BAD_CAST pos.c_str());
  }
  
  
  void 
  NetworkIO::add_connection_note_color(const std::string &id, const std::string &col)
  {
    bool found = false;
    xmlNode* node = get_connection_node(id);
    
    if (! node) { 
      std::cerr << "ERROR: could not find node for connection id: " << id << std::endl;
      return;
    }
    node = node->children;
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("note")) {
        found = true;
        break;
      }
    }
    
    if (! found) { 
      std::cerr << "ERROR: could not find note node for module id: " << id << std::endl;
      return;
    }
    xmlNewProp(node, BAD_CAST "color", BAD_CAST col.c_str());
  }
  
  
  void 
  NetworkIO::set_port_caching(const std::string &id, const std::string &port, 
                              const std::string &val)
  {
    xmlNode* mnode = get_module_node(id);
    xmlNode* pcnode = 0;
    if (! mnode) { 
      std::cerr << "ERROR: could not find module node with id (set_port_caching): " << id << std::endl;
      return;
    }
    
    xmlNode* node = mnode->children;
    for (; node != 0; node = node->next) {
      // skip all but the network node.
      if (node->type == XML_ELEMENT_NODE && 
          std::string(to_char_ptr(node->name)) == std::string("port_caching")) 
      {
        pcnode = node;
      }
    }
    
    if (! pcnode) { 
      pcnode = xmlNewChild(mnode, 0, BAD_CAST "port_caching", 0);
    }
    
    xmlNode *tmp;
    tmp = xmlNewChild(pcnode, 0, BAD_CAST "port", 0);
    xmlNewProp(tmp, BAD_CAST "id", BAD_CAST port.c_str());
    xmlNewProp(tmp, BAD_CAST "val", BAD_CAST val.c_str());
  }
  
} // end namespace SCIRun
