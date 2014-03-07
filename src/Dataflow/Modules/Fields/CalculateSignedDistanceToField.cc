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

#include <Dataflow/Network/Module.h>
#include <Core/Datatypes/Field.h>
#include <Core/Datatypes/Matrix.h>
#include <Dataflow/Network/Ports/FieldPort.h>
#include <Dataflow/Network/Ports/MatrixPort.h>

#include <Core/Algorithms/Fields/DistanceField/CalculateSignedDistanceField.h>

namespace ModelCreation {

using namespace SCIRun;

class CalculateSignedDistanceToField : public Module {
  public:
    CalculateSignedDistanceToField(GuiContext*);
    virtual ~CalculateSignedDistanceToField() {}
    virtual void execute();
    
  private:
    SCIRunAlgo::CalculateSignedDistanceFieldAlgo algo_;
};


DECLARE_MAKER(CalculateSignedDistanceToField)
CalculateSignedDistanceToField::CalculateSignedDistanceToField(GuiContext* ctx)
  : Module("CalculateSignedDistanceToField", ctx, Source, "ChangeFieldData", "SCIRun")
{
  algo_.set_progress_reporter(this);
}


void
CalculateSignedDistanceToField::execute()
{
  FieldHandle input, output, value;
  FieldHandle object;
 
  get_input_handle("Field",input,true);
  get_input_handle("ObjectField",object,true);
 
  bool value_connected = oport_connected("ValueField");
   
  if (inputs_changed_ || !oport_cached("SignedDistanceField") ||
      (!oport_cached("ValueField") && value_connected))
  {
    update_state(Executing);
    
    if (value_connected)
    {
      if(!(algo_.run(input,object,output,value))) return;
   
      send_output_handle("SignedDistanceField", output); 
      send_output_handle("ValueField", value); 
    }
    else
    {
      if(!(algo_.run(input,object,output))) return;
   
      send_output_handle("SignedDistanceField", output);     
    }
  }
}


} // End namespace ModelCreation


