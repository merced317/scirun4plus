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

#include <Core/Algorithms/FiniteElements/Mapping/BuildNodeLink.h>

#include <Core/Datatypes/FieldInformation.h>
#include <Core/Datatypes/Matrix.h>
#include <Core/Datatypes/MatrixOperations.h>
#include <Core/Datatypes/ColumnMatrix.h>
#include <Core/Datatypes/SparseRowMatrix.h>

#include <Core/Math/MiscMath.h>

#include <float.h>

namespace SCIRunAlgo {

using namespace SCIRun;

bool 
BuildNodeLinkAlgo::
run(MatrixHandle NodeDomain, 
    MatrixHandle& NodeLink)
{
  algo_start("BuildNodeLink");

  if (NodeDomain.get_rep() == 0)
  {
    error("No matrix on input");
    algo_end(); return (false);
  }

  if (NodeDomain->ncols() != 1)
  {
    error("NodeDomain needs to be Column Vector");
    algo_end(); return (false);    
  }


  ColumnMatrix* dmat = NodeDomain->column();
  MatrixHandle dmat_handle = dmat;

  size_type num_values = dmat->nrows();
  
  double min2 = -DBL_MAX;
  double min = DBL_MAX;
  double* data = dmat->get_data_pointer();
  
  bool found_domain = false;
  
  SparseRowMatrix::Builder nodeLinkData;
  const SparseRowMatrix::Rows& rr = nodeLinkData.allocate_rows(num_values+1);
  const SparseRowMatrix::Columns& cc = nodeLinkData.allocate_columns(num_values);
  size_type nnz = 0;
  
  if (!rr || !cc)
  {
    error("Could not allocate memory for sparse matrix");
    algo_end(); return (false);
  } 
  
  for (index_type j = 0; j<num_values; j++) cc[j] = -1;
  
  do
  {
    found_domain = false;
    min = DBL_MAX;
    
    for (index_type j = 0; j<num_values; j++)
    {
      double val = data[j];
      if (IsFinite(val))
      {
        if (val < min && val > min2 ) 
        {
          found_domain = true;
          min = val;
        }
      }
    }
    
    if (found_domain)
    {
      min2 = min;
      index_type idx = -1;
      for (index_type j = 0; j<num_values; j++)
      {
        double val = data[j];    
        if (val == min2)
        {
          if (idx < 0) idx = j;
          else { cc[j] = idx; nnz++; }
        }
      }
    }
  }
  while (found_domain);

  const SparseRowMatrix::Storage& aa = nodeLinkData.allocate_data(nnz);

  if (!aa)
  {
    error("Could not allocate memory for sparse matrix");
    algo_end(); return (false);
  } 

  
  index_type k = 0;
  rr[0] = 0;
  for (index_type j = 0; j<num_values; j++)
  {
    if (cc[j] > -1 ) 
    {
      cc[k] = cc[j];
      aa[k] = 1.0;
      k++;
    }
    rr[j+1] = k;
  }
  
  NodeLink = new SparseRowMatrix(num_values,num_values,nodeLinkData.build(),nnz);
  MatrixHandle TNodeLink = NodeLink->make_transpose();
  NodeLink = NodeLink + TNodeLink;

  algo_end(); return (true);
}

} // end namespace
