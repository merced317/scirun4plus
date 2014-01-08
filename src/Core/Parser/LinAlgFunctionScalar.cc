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

#include <Core/Parser/LinAlgFunctionCatalog.h>
#include <Core/Thread/Mutex.h>
#include <cmath>
#include <Core/Math/MiscMath.h>
#include <Core/Datatypes/DenseMatrix.h>
#include <Core/Datatypes/MatrixTypeConverter.h>

namespace LinAlgFunctions {

using namespace SCIRun;
  
//--------------------------------------------------------------------------
// Simple Scalar functions

bool isnan_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
  
  while (data != data_end)
  {
    if (IsNan(*data)) *data = 1.0; else *data = 0.0;
    data++;
  }
  
  return (true);
}


bool isfinite_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  

  while (data != data_end)
  {
    if (IsFinite(*data)) *data = 1.0; else *data = 0.0;
  }
  
  return (true);
}

bool isinfinite_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();
  
  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
  
  while (data != data_end)
  {
    if (IsInfinite(*data)) *data = 1.0; else *data = 0.0;
    data++;
  }
  
  return (true);
}


bool sign_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();
  
  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  

 
  while (data != data_end)
  {
   if (*data >= 0.0) *data = 1.0; else *data = 0.0;
   data++;
  }
  
  return (true);
}

bool inv_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = 1.0/(*data); data++;
  }
  
  return (true);
}

bool boolean_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    if(*data) *data = 1.0; else *data = 0.0; data++;
  }
  
  return (true);
}

bool abs_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    if(*data<0) *data = -(*data); data++;
  }
  
  return (true);
}

bool norm_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    if(*data<0) *data = -(*data); data++; 
  }
  
  return (true);
}

bool round_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = static_cast<double>(static_cast<int>(*data+0.5)); data++;
  }
  
  return (true);
}

bool floor_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = ::floor(*data); data++;
  }
  
  return (true);
}

bool ceil_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = ::ceil(*data); data++;
  }
  
  return (true);
}

bool exp_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = ::exp(*data); data++; 
  }
  
  return (true);
}

/*
bool pow_ss(SCIRun::LinAlgProgramCode& pc)
{ 
  double* data0 = pc.get_variable(0); 
  double* data1 = pc.get_variable(1); 
  double* data2 = pc.get_variable(2); 
  double* data0_end = data0 + pc.get_size();
  
  while (data0 != data0_end) 
  {
    *data0 = ::pow(*data0,*data2); data0++; data1++; data2++;
  }
  
  return (true);
}
*/

bool sqrt_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = ::sqrt(*data); data++;
  }
  
  return (true);
}

bool log_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = ::log(*data); data++;
  }
  
  return (true);
}

bool ln_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  while (data != data_end)
  {
    *data = ::log(*data); data++;
  }
  
  return (true);
}

bool log2_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  double s = 1.0/log(2.0);  
  while (data != data_end)  
  {
    *data = ::log(*data)*s; data++;
  }
  
  return (true);
}

bool log10_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size;  
    
  double s = 1.0/log(10.0);  
  while (data != data_end)  
  {
    *data = ::log(*data)*s; data++;
  } 
    
  return (true);
}

bool cbrt_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::pow(*data,1.0/3.0); data++;
  }
  
  return (true);
}

bool sin_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::sin(*data); data++;
  }
  
  return (true);
}

bool cos_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::cos(*data); data++;
  }
  
  return (true);
}

bool tan_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::tan(*data); data++;
  }
  
  return (true);
}

bool sinh_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::sinh(*data); data++;
  }
  
  return (true);
}

bool cosh_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::cosh(*data); data++;
  }
  
  return (true);
}

bool asin_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::asin(*data); data++;
  }
  
  return (true);
}

bool acos_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::acos(*data); data++;
  }
  
  return (true);
}

bool atan_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    *data = ::atan(*data); data++;
  }
  
  return (true);
}

bool asinh_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    double d = *data;
    *data = (d==0?0:(d>0?1:-1)) * ::log((d<0?-d:d) + ::sqrt(1+d*d)); data++;
  }
  
  return (true);
}

bool acosh_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  *data0 = (*data1)->clone();

  double* data = (*data0)->get_data_pointer();
  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end)  
  {
    double d = *data;
    *data = ::log(d + ::sqrt(d*d-1)); data++;
  }
  
  return (true);
}


bool or_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 

  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing an OR operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing an OR operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = (*data1)->dense();
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = (*data2)->dense();
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 || *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}


bool and_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 

  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing an AND operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing an AND operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 && *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}

bool eq_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 

  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing an EQUAL operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing an EQUAL operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 == *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}

bool neq_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 
  
  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing a NOT EQUAL operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing a NOT EQUAL operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 != *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}

bool le_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 
  
  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing a LESS OR EQUAL operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing a LESS OR EQUAL operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }
  
  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 <= *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}


bool ge_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 
  
  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing a GREATER OR EQUAL operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing a GREATER OR EQUAL operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 >= *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}

bool ls_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 
  
  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing a LESS THAN operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing a LESS THAN operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 < *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}

bool gt_ss(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  MatrixHandle* data2 = pc.get_handle(2); 
  
  if ((*data1).get_rep() == 0) return (false);
  if ((*data2).get_rep() == 0) return (false);

  if ((*data1)->nrows() != (*data2)->nrows())
  {
    err = "number of rows is not equal for doing a GREATER THAN operation.";
    return (false);
  }

  if ((*data1)->ncols() != (*data2)->ncols())
  {
    err = "number of columns is not equal for doing a GREATER THAN operation.";
    return (false);
  }

  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  MatrixHandle data2h = (*data2).get_rep();
  if (!(matrix_is::dense(data2h) || matrix_is::column(data2h)))
  {
    data2h = ((*data2)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();

  double* d1 = data1h->get_data_pointer();
  double* d2 = data2h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1 > *d2) *data = 1.0; else *data = 0.0; data++; d1++; d2++;
  }
  
  return (true);
}


bool not_s(SCIRun::LinAlgProgramCode& pc, std::string& err)
{ 
  err = "";
  MatrixHandle* data0 = pc.get_handle(0); 
  MatrixHandle* data1 = pc.get_handle(1); 
  
  if ((*data1).get_rep() == 0) return (false);
  
  // Convert none dense matrices to dense for this operation
  MatrixHandle data1h = (*data1).get_rep();
  if (!(matrix_is::dense(data1h) || matrix_is::column(data1h)))
  {
    data1h = ((*data1)->dense());
  }

  *data0 = data1h->clone();

  double* data = (*data0)->get_data_pointer();
  double* d1 = data1h->get_data_pointer();

  size_type size = (*data0)->get_data_size();
  double* data_end = data+size; 
  
  while (data != data_end) 
  {
    if (*d1) *data = 0.0; else *data = 1.0; data++; d1++;
  }
  
  return (true);
}



/*
bool max_ss(SCIRun::LinAlgProgramCode& pc)
{ 
  double* data0 = pc.get_variable(0); 
  double* data1 = pc.get_variable(1); 
  double* data2 = pc.get_variable(2); 
  double* data0_end = data0 + pc.get_size();
  
  while (data0 != data0_end) 
  {
    if (*data1 > *data2) *data0 = *data1; else *data0 = *data2; 
    data0++; data1++; data2++;
  }
  
  return (true);
}

bool median_sss(SCIRun::LinAlgProgramCode& pc)
{ 
  double* data0 = pc.get_variable(0); 
  double* data1 = pc.get_variable(1); 
  double* data2 = pc.get_variable(2); 
  double* data3 = pc.get_variable(3); 
  double* data0_end = data0 + pc.get_size();
  
  while (data0 != data0_end) 
  {
    if (*data1 > *data2) 
    {
      if (*data3 < *data2)
      {
        *data0 = *data2; 
      }
      else
      {
        if (*data1 < *data3)
          *data0 = *data1;
        else
          *data0 = *data3;
      }
    }
    else
    {
      if (*data3 < *data1)
      {
        *data0 = *data1; 
      }
      else
      {
        if (*data2 > *data3)
          *data0 = *data3;
        else
          *data0 = *data2;
      }    
    }
    data0++; data1++; data2++; data3++;
  }
  
  return (true);
}



bool min_ss(SCIRun::LinAlgProgramCode& pc)
{ 
  double* data0 = pc.get_variable(0); 
  double* data1 = pc.get_variable(1); 
  double* data2 = pc.get_variable(2); 
  double* data0_end = data0 + pc.get_size();
  
  while (data0 != data0_end) 
  {
    if (*data1 < *data2) *data0 = *data1; else *data0 = *data2; 
    data0++; data1++; data2++;
  }
  
  return (true);
}


SCIRun::Mutex RandomMutex("Mutex for rand() and random() functions");

bool random_value_(SCIRun::LinAlgProgramCode& pc)
{ 
  double* data0 = pc.get_variable(0); 
  double* data0_end = data0 + pc.get_size();
  
  // Random is not thread safe
  RandomMutex.lock();
  while (data0 != data0_end) 
  {
// Note: _WIN32 is always defined for applications for Win32 and Win64 (see http://msdn.microsoft.com/en-us/library/b0084kay.aspx).
#ifdef _WIN32
    // random() not available in Windows stdlib
    *data0 = static_cast<double>(rand())/static_cast<double>(RAND_MAX + 1);
#else
    *data0 = static_cast<double>(random())/static_cast<double>(0x7FFFFFFF);
#endif
    data0++;
  }
  RandomMutex.unlock();
  
  return (true);
}

*/

} // end namsespace 

namespace SCIRun {

void
InsertScalarLinAlgFunctionCatalog(LinAlgFunctionCatalogHandle& catalog)
{
  // Functions
  catalog->add_function(LinAlgFunctions::isnan_s,"isnan$S","S");
  catalog->add_function(LinAlgFunctions::isfinite_s,"isfinite$S","S");
  catalog->add_function(LinAlgFunctions::isinfinite_s,"isinfinite$S","S");
  catalog->add_function(LinAlgFunctions::isinfinite_s,"isinf$S","S");
  catalog->add_function(LinAlgFunctions::sign_s,"sign$S","S");

  catalog->add_function(LinAlgFunctions::inv_s,"inv$S","S");
  catalog->add_function(LinAlgFunctions::boolean_s,"boolean$S","S");
  catalog->add_function(LinAlgFunctions::abs_s,"abs$S","S");
  catalog->add_function(LinAlgFunctions::norm_s,"norm$S","S");
  catalog->add_function(LinAlgFunctions::round_s,"round$S","S");
  catalog->add_function(LinAlgFunctions::floor_s,"floor$S","S");
  catalog->add_function(LinAlgFunctions::ceil_s,"ceil$S","S");
  catalog->add_function(LinAlgFunctions::exp_s,"exp$S","S");
  catalog->add_function(LinAlgFunctions::sqrt_s,"sqrt$S","S");
  catalog->add_function(LinAlgFunctions::log_s,"log$S","S");
  catalog->add_function(LinAlgFunctions::ln_s,"ln$S","S");
  catalog->add_function(LinAlgFunctions::log2_s,"log2$S","S");
  catalog->add_function(LinAlgFunctions::log10_s,"log10$S","S");
  catalog->add_function(LinAlgFunctions::cbrt_s,"cbrt$S","S");
  catalog->add_function(LinAlgFunctions::sin_s,"sin$S","S");
  catalog->add_function(LinAlgFunctions::cos_s,"cos$S","S");
  catalog->add_function(LinAlgFunctions::tan_s,"tan$S","S");
  catalog->add_function(LinAlgFunctions::sinh_s,"sinh$S","S");
  catalog->add_function(LinAlgFunctions::cosh_s,"cosh$S","S");
  catalog->add_function(LinAlgFunctions::asin_s,"asin$S","S");
  catalog->add_function(LinAlgFunctions::acos_s,"acos$S","S");
  catalog->add_function(LinAlgFunctions::atan_s,"atan$S","S");
  catalog->add_function(LinAlgFunctions::asinh_s,"asinh$S","S");
  catalog->add_function(LinAlgFunctions::acosh_s,"acosh$S","S");

  catalog->add_sym_function(LinAlgFunctions::or_ss,"or$S:S","S");
  catalog->add_sym_function(LinAlgFunctions::and_ss,"and$S:S","S");
  catalog->add_sym_function(LinAlgFunctions::eq_ss,"eq$S:S","S");
  catalog->add_sym_function(LinAlgFunctions::neq_ss,"neq$S:S","S");
  catalog->add_function(LinAlgFunctions::ls_ss,"ls$S:S","S");
  catalog->add_function(LinAlgFunctions::le_ss,"le$S:S","S");
  catalog->add_function(LinAlgFunctions::gt_ss,"gt$S:S","S");
  catalog->add_function(LinAlgFunctions::ge_ss,"ge$S:S","S");
  catalog->add_function(LinAlgFunctions::not_s,"not$S","S");

}



} // end namespace
