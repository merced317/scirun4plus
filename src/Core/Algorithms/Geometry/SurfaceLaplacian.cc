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

//    File   : SurfaceLaplacian.cc
//    Author : Jeroen Stinstra, Brett Burton
//    Date   : Apr MST 2009

#include <Core/Datatypes/Mesh.h>
#include <Core/Datatypes/VMesh.h>
#include <Core/Containers/Array2.h>
#include <Core/Datatypes/Matrix.h>
#include <Core/Algorithms/Geometry/SurfaceLaplacian.h>
#include <Core/Datatypes/SparseRowMatrix.h>
#include <Core/Datatypes/MatrixTypeConverter.h>

namespace SCIRun {

using namespace SCIRun;

// function to compute laplacian matrix from neighbors
MatrixHandle surfaceLaplacian(VMesh *tsm) 
{
  tsm->synchronize(Mesh::NODE_NEIGHBORS_E);
  
  VMesh::size_type nnz = 0;
  VMesh::Node::size_type num_nodes = tsm->num_nodes();
  VMesh::Node::array_type nodes;
  std::vector<double> dist, odist;

  for (int idx=0; idx < num_nodes; idx++)
  {
    tsm->get_neighbors(nodes,idx);
    nnz += nodes.size() + 1;
  }

  SparseRowMatrix::Data matrix(num_nodes + 1, nnz);
  const SparseRowMatrix::Rows& row_buffer = matrix.rows();
  const SparseRowMatrix::Columns& col_buffer = matrix.columns();
  const SparseRowMatrix::Storage& values = matrix.data();
  index_type k = 0;
	row_buffer[0] = 0;
	
  for (VNodeIndex<SCIRun::index_type> idx = 0; idx < num_nodes; idx++)
  {
    double sum1 = 0;
    double sum2 = 0;
    Point p;

    dist.clear();
    odist.clear();
    tsm->get_center(p,idx);			// find point
    tsm->get_neighbors(nodes,idx);	// find neighbors
    col_buffer[k] = idx;			// col buffer given value of idx (lists index values for all neighbors)
    values[k] = static_cast<double>(nodes.size() + 1);

    for(size_type idx2 = 0; idx2 < nodes.size(); idx2++)
    {
      Point n;
      tsm->get_center(n,nodes[idx2]);
      double length = (p - n).length(); // p-n gives a vector

      dist.push_back(length); // hij
      odist.push_back(1.0/length);
      sum1 += length;
      sum2 += 1.0/length;
    }

    double avg_dist = sum1/nodes.size();  // mean(hij)
    double avg_dist_inv = 1.0/avg_dist;  // hi
    double avg_odist = sum2/nodes.size(); // ihi
    // put laplacian stuff here on down (diagonal is probably not negative...fix it)

    values[k] = -4.0 * avg_odist / avg_dist;  // diag values  
    k++;								  
    for(size_type idx2 = 0; idx2 < nodes.size(); idx2++)
    { 
      col_buffer[k] = nodes[idx2];

      double nodenum_inv = 1.0/nodes.size();
      values[k] = (4.0 * avg_dist_inv / nodes.size()) / (dist[idx2]); // non-diag values

      k++;
    }

    row_buffer[idx + 1] = k;
  }
	
  MatrixHandle surfLapl = new SparseRowMatrix(num_nodes, num_nodes, matrix, nnz, true);
  return surfLapl;
}

}
