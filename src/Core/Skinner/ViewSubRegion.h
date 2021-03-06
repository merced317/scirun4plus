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
//    File   : ViewSubRegion.h
//    Author : McKay Davis
//    Date   : Sat Aug 12 12:37:40 2006


#ifndef SKINNER_VIEWSUBREGION_H
#define SKINNER_VIEWSUBREGION_H

#include <Core/Skinner/Parent.h>

namespace SCIRun {
namespace Skinner {


class ViewSubRegion : public Parent {
public:
  ViewSubRegion (Variables *variables);
  virtual ~ViewSubRegion();
  CatcherFunction_t                 process_event;

protected:
  CatcherFunction_t                 do_PointerEvent;

  Var<double>                       x_;
  Var<double>                       y_;
  Var<double>                       width_;
  Var<double>                       height_;
  Var<double>                       invisible_width_;
  Var<double>                       invisible_height_;
  RectRegion                        cache_region_;
};


}
}

#endif
