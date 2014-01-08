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
 *   Michael Callahan
 *   Department of Computer Science
 *   University of Utah
 *   May 2004
 *
 */

#include <Core/Thread/Mutex.h>
#include <Core/Util/StringUtil.h>
#include <Core/ImportExport/Nrrd/NrrdIEPlugin.h>

#include <map>



namespace SCIRun {

// Use Core/Util/share.h.
// Core_Util is the first library to be loaded at runtime.
#include <Core/Util/share.h>
extern SCISHARE Mutex nrrdIEPluginMutex;

static std::map<std::string, NrrdIEPlugin *> *matrix_plugin_table = 0;

//----------------------------------------------------------------------

NrrdIEPlugin::NrrdIEPlugin(const std::string& pname,
                           const std::string& fextension,
                           const std::string& fmagic,
                           NrrdDataHandle (*freader)(ProgressReporter *pr,
                                                 const char *filename),
                           bool (*fwriter)(ProgressReporter *pr,
                                           NrrdDataHandle f,
                                           const char *filename))
  : pluginname(pname),
    fileextension(fextension),
    filemagic(fmagic),
    fileReader_(freader),
    fileWriter_(fwriter)
{
  nrrdIEPluginMutex.lock();

  if (!matrix_plugin_table)
  {
    matrix_plugin_table = new std::map<std::string, NrrdIEPlugin *>();
  }

  std::string tmppname = pluginname;
  int counter = 2;
  for (;;)
  {
    std::map<std::string, NrrdIEPlugin *>::iterator loc = matrix_plugin_table->find(tmppname);
    if (loc == matrix_plugin_table->end())
    {
      if (tmppname != pluginname) { ((std::string)pluginname) = tmppname; }

      (*matrix_plugin_table)[pluginname] = this;
      break;
    }
    if (*(*loc).second == *this)
    {
      std::cerr << "WARNING: NrrdIEPlugin '" << tmppname << "' duplicated.\n";
      break;
    }

    std::cout << "WARNING: Multiple NrrdIEPlugins with '" << pluginname
	 << "' name.\n";
    tmppname = pluginname + "(" + to_string(counter) + ")";
    counter++;
  }

  nrrdIEPluginMutex.unlock();
}



NrrdIEPlugin::~NrrdIEPlugin()
{
  if (matrix_plugin_table == NULL)
  {
    std::cerr << "WARNING: NrrdIEPlugin.cc: ~NrrdIEPlugin(): matrix_plugin_table is NULL\n";
    std::cerr << "         For: " << pluginname << "\n";
    return;
  }

  nrrdIEPluginMutex.lock();

  std::map<std::string, NrrdIEPlugin *>::iterator iter = matrix_plugin_table->find(pluginname);
  if (iter == matrix_plugin_table->end())
  {
    std::cerr << "WARNING: NrrdIEPlugin " << pluginname << 
      " not found in database for removal.\n";
  }
  else
  {
    matrix_plugin_table->erase(iter);
  }

  if (matrix_plugin_table->size() == 0)
  {
    delete matrix_plugin_table;
    matrix_plugin_table = 0;
  }

  nrrdIEPluginMutex.unlock();
}


bool
NrrdIEPlugin::operator==(const NrrdIEPlugin &other) const
{
  return (pluginname == other.pluginname &&
	  fileextension == other.fileextension &&
	  filemagic == other.filemagic &&
	  fileReader_ == other.fileReader_ &&
	  fileWriter_ == other.fileWriter_);
}



void
NrrdIEPluginManager::get_importer_list(std::vector<std::string> &results)
{
  if (matrix_plugin_table == NULL) return;

  nrrdIEPluginMutex.lock();
  std::map<std::string, NrrdIEPlugin *>::const_iterator itr = matrix_plugin_table->begin();
  while (itr != matrix_plugin_table->end())
  {
    if ((*itr).second->fileReader_ != NULL)
    {
      results.push_back((*itr).first);
    }
    ++itr;
  }
  nrrdIEPluginMutex.unlock();
}


void
NrrdIEPluginManager::get_exporter_list(std::vector<std::string> &results)
{
  if (matrix_plugin_table == NULL) return;

  nrrdIEPluginMutex.lock();
  std::map<std::string, NrrdIEPlugin *>::const_iterator itr = matrix_plugin_table->begin();
  while (itr != matrix_plugin_table->end())
  {
    if ((*itr).second->fileWriter_ != NULL)
    {
      results.push_back((*itr).first);
    }
    ++itr;
  }
  nrrdIEPluginMutex.unlock();
}

 
NrrdIEPlugin *
NrrdIEPluginManager::get_plugin(const std::string &name)
{
  if (matrix_plugin_table == NULL) return NULL;

  // Should check for invalid name.
  std::map<std::string, NrrdIEPlugin *>::iterator loc = matrix_plugin_table->find(name);
  if (loc == matrix_plugin_table->end())
  {
    return NULL;
  }
  else
  {
    return (*loc).second;
  }
}


} // End namespace SCIRun


