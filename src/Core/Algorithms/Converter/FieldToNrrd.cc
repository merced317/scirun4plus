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


#include <Core/Algorithms/Converter/FieldToNrrd.h>

#include <Core/Datatypes/Field.h>
#include <Core/Datatypes/FieldInformation.h>

#include <vector>

namespace SCIRunAlgo {

using namespace SCIRun;

class FieldToNrrdAlgoT {
public:
  bool FieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output);

  // Converters for node centered data
  template<class T>
  bool ScalarFieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output, int datatype);
  bool VectorFieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output);
  bool TensorFieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output);

};


// Templated converter for Scalar data so we can use every type supported by the Teem library

template<class T>
bool FieldToNrrdAlgoT::ScalarFieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output,int datatype)
{
  output = new NrrdData();
  output->nrrd_ = nrrdNew();
  
  if (output->nrrd_ == 0)
  {
    pr->error("FieldToNrrd: Could not create new Nrrd");
    return (false);      
  }

  // Get a pointer to make program more readable
  Nrrd* nrrd = output->nrrd_;
  
  int nrrddim = 0;
  int nrrdcenter = nrrdCenterNode;
  size_t dim[3];
  Transform tf;

  FieldInformation fi(input);

  if (fi.is_latvolmesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
    
    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);
    nrrddim = 3; 

    dim[0] = static_cast<size_t>(sz[0]);
    dim[1] = static_cast<size_t>(sz[1]);
    dim[2] = static_cast<size_t>(sz[2]);
    nrrdAlloc_nva(nrrd,datatype,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    field->get_values(reinterpret_cast<T*>(nrrd->data),mesh->num_nodes());
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_latvolmesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
    
    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 3;
    dim[0] = static_cast<size_t>(sz[0]);
    dim[1] = static_cast<size_t>(sz[1]);
    dim[2] = static_cast<size_t>(sz[2]);
    nrrdAlloc_nva(nrrd,datatype,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    field->get_values(reinterpret_cast<T*>(nrrd->data),mesh->num_elems());
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }

  if (fi.is_imagemesh() && fi.is_lineardata())
  {
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 2;
    dim[0] = static_cast<size_t>(sz[0]);
    dim[1] = static_cast<size_t>(sz[1]);
    nrrdAlloc_nva(nrrd,datatype,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    field->get_values(reinterpret_cast<T*>(nrrd->data),mesh->num_nodes());
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_imagemesh() && fi.is_constantdata())
  {
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 2;
    dim[0] = static_cast<size_t>(sz[0]);
    dim[1] = static_cast<size_t>(sz[1]);
    nrrdAlloc_nva(nrrd,datatype,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    field->get_values(reinterpret_cast<T*>(nrrd->data),mesh->num_elems());
   
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }

  if (fi.is_scanlinemesh() && fi.is_lineardata())
  {
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 1;
    dim[0] = static_cast<size_t>(sz[0]);
    nrrdAlloc_nva(nrrd,datatype,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    field->get_values(reinterpret_cast<T*>(nrrd->data),mesh->num_nodes());
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }
  

  if (fi.is_scanlinemesh() && fi.is_constantdata())
  {
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 1;
    dim[0] = static_cast<size_t>(sz[0]);
    nrrdAlloc_nva(nrrd,datatype,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    field->get_values(reinterpret_cast<T*>(nrrd->data),mesh->num_elems());
   
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }
  
  if (nrrddim)
  {
    // Set spacing information
    
    int centerdata[NRRD_DIM_MAX];
    for (int p=0;p<NRRD_DIM_MAX;p++)
    {
      centerdata[p] = nrrdcenter;
    }
    nrrdAxisInfoSet_nva(nrrd,nrrdAxisInfoCenter,centerdata); 

    int kind[NRRD_DIM_MAX];
    for (int p=0;p<NRRD_DIM_MAX;p++)
    {
      kind[p] = nrrdKindSpace;
    }
    nrrdAxisInfoSet_nva(nrrd,nrrdAxisInfoKind,kind);

    nrrd->spaceDim = 3;
    
    double Trans[16];
    tf.get(Trans);
    for (int p=0;p<3;p++)
    {
      nrrd->spaceOrigin[p] = Trans[3+4*p];
      for (int q=0;q<nrrddim;q++)
        nrrd->axis[q].spaceDirection[p] = Trans[q+4*p];
    }
    
    for (int p=0;p<3;p++)
      for (int q=0;q<3;q++)
      {
        if (p==q) nrrd->measurementFrame[p][q] = 1.0;
        else nrrd->measurementFrame[p][q] = 0.0;
      }
    
    nrrd->space = nrrdSpace3DRightHanded;

    return (true);
  }

  return (false);  
}



bool FieldToNrrdAlgoT::VectorFieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output)
{
  output = new NrrdData();
  output->nrrd_ = nrrdNew();
  
  if (output->nrrd_ == 0)
  {
    pr->error("FieldToNrrd: Could not create new Nrrd");
    return (false);      
  }

  // Get a pointer to make program more readable
  Nrrd* nrrd = output->nrrd_;
  
  int nrrddim = 0;
  int nrrdcenter = nrrdCenterNode;
  size_t dim[4];
  Transform tf;

  FieldInformation fi(input);

  if (fi.is_latvolmesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();

    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 4; dim[0] = 3;
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    dim[3] = static_cast<size_t>(sz[2]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Node::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
 
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Vector v;
      field->get_value(v,*it);
      data[k] = v.x(); k++;
      data[k] = v.y(); k++;
      data[k] = v.z(); k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_latvolmesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();

    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 4; dim[0] = 3;
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    dim[3] = static_cast<size_t>(sz[2]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Elem::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
 
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Vector v;
      field->get_value(v,*it);
      data[k] = v.x(); k++;
      data[k] = v.y(); k++;
      data[k] = v.z(); k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }

  if (fi.is_imagemesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();

    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 3; dim[0] = 3;
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Node::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
 
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Vector v;
      field->get_value(v,*it);
      data[k] = v.x(); k++;
      data[k] = v.y(); k++;
      data[k] = v.z(); k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_imagemesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();

    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 3; dim[0] = 3;
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Elem::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
 
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Vector v;
      field->get_value(v,*it);
      data[k] = v.x(); k++;
      data[k] = v.y(); k++;
      data[k] = v.z(); k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }

  if (fi.is_scanlinemesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();

    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 2; dim[0] = 3;
    dim[1] = static_cast<size_t>(sz[0]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Node::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
 
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Vector v;
      field->get_value(v,*it);
      data[k] = v.x(); k++;
      data[k] = v.y(); k++;
      data[k] = v.z(); k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_scanlinemesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();

    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 2; dim[0] = 3;
    dim[1] = static_cast<size_t>(sz[0]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Elem::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
 
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Vector v;
      field->get_value(v,*it);
      data[k] = v.x(); k++;
      data[k] = v.y(); k++;
      data[k] = v.z(); k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }

  if (nrrddim)
  {
    // Set spacing information
    
    int centerdata[NRRD_DIM_MAX];
    for (int p=0;p<NRRD_DIM_MAX;p++)
    {
      centerdata[p] = nrrdcenter;
    }
    nrrdAxisInfoSet_nva(nrrd,nrrdAxisInfoCenter,centerdata); 

    int kind[NRRD_DIM_MAX];
    for (int p=0;p<NRRD_DIM_MAX;p++)
    {
      if (p==0) kind[p] = nrrdKind3Vector;
      else kind[p] = nrrdKindSpace;
    }
    nrrdAxisInfoSet_nva(nrrd,nrrdAxisInfoKind,kind);

    nrrd->spaceDim = 3;
    
    double Trans[16];
    tf.get(Trans);
    nrrd->axis[0].spaceDirection[0] = 0.0;
    nrrd->axis[0].spaceDirection[1] = 0.0;
    nrrd->axis[0].spaceDirection[2] = 0.0;
    
    for (int p=0;p<3;p++)
    {
      nrrd->spaceOrigin[p] = Trans[3+4*p];
      for (int q=0;q<nrrddim;q++)
        nrrd->axis[q+1].spaceDirection[p] = Trans[q+4*p];
    }
    
    for (int p=0;p<3;p++)
      for (int q=0;q<3;q++)
      {
        if (p==q) nrrd->measurementFrame[p][q] = 1.0;
        else nrrd->measurementFrame[p][q] = 0.0;
      }
    
    nrrd->space = nrrdSpace3DRightHanded;

    return (true);
  }

  return (false);  
}


bool FieldToNrrdAlgoT::TensorFieldToNrrd(ProgressReporter* pr,FieldHandle input, NrrdDataHandle& output)
{
  output = new NrrdData();
  output->nrrd_ = nrrdNew();
  
  if (output->nrrd_ == 0)
  {
    pr->error("FieldToNrrd: Could not create new Nrrd");
    return (false);      
  }

  // Get a pointer to make program more readable
  Nrrd* nrrd = output->nrrd_;

  int nrrddim = 0;
  int nrrdcenter = nrrdCenterNode;
  size_t dim[4];
  Transform tf;

  FieldInformation fi(input);

  if (fi.is_latvolmesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 4; dim[0] = 6; 
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    dim[3] = static_cast<size_t>(sz[2]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Node::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
    
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Tensor t;
      field->get_value(t,*it);
      data[k] = t.mat_[0][0]; k++;
      data[k] = t.mat_[0][1]; k++;
      data[k] = t.mat_[0][2]; k++;
      data[k] = t.mat_[1][1]; k++;
      data[k] = t.mat_[1][2]; k++;
      data[k] = t.mat_[2][2]; k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_latvolmesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 4; dim[0] = 6; 
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    dim[3] = static_cast<size_t>(sz[2]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Elem::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
    
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Tensor t;
      field->get_value(t,*it);
      data[k] = t.mat_[0][0]; k++;
      data[k] = t.mat_[0][1]; k++;
      data[k] = t.mat_[0][2]; k++;
      data[k] = t.mat_[1][1]; k++;
      data[k] = t.mat_[1][2]; k++;
      data[k] = t.mat_[2][2]; k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }


  if (fi.is_imagemesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 3; dim[0] = 6; 
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Node::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
    
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Tensor t;
      field->get_value(t,*it);
      data[k] = t.mat_[0][0]; k++;
      data[k] = t.mat_[0][1]; k++;
      data[k] = t.mat_[0][2]; k++;
      data[k] = t.mat_[1][1]; k++;
      data[k] = t.mat_[1][2]; k++;
      data[k] = t.mat_[2][2]; k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_imagemesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 3; dim[0] = 6; 
    dim[1] = static_cast<size_t>(sz[0]);
    dim[2] = static_cast<size_t>(sz[1]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Elem::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
    
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Tensor t;
      field->get_value(t,*it);
      data[k] = t.mat_[0][0]; k++;
      data[k] = t.mat_[0][1]; k++;
      data[k] = t.mat_[0][2]; k++;
      data[k] = t.mat_[1][1]; k++;
      data[k] = t.mat_[1][2]; k++;
      data[k] = t.mat_[2][2]; k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }


  if (fi.is_scanlinemesh() && fi.is_lineardata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_dimensions(sz);

    nrrddim = 2; dim[0] = 6; 
    dim[1] = static_cast<size_t>(sz[0]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Node::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
    
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Tensor t;
      field->get_value(t,*it);
      data[k] = t.mat_[0][0]; k++;
      data[k] = t.mat_[0][1]; k++;
      data[k] = t.mat_[0][2]; k++;
      data[k] = t.mat_[1][1]; k++;
      data[k] = t.mat_[1][2]; k++;
      data[k] = t.mat_[2][2]; k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterNode;
    tf = mesh->get_transform();
  }

  if (fi.is_scanlinemesh() && fi.is_constantdata())
  {
    // LatVolField with data on nodes;
    VMesh* mesh = input->vmesh();
    VField* field = input->vfield();
        
    VMesh::dimension_type sz;
    mesh->get_elem_dimensions(sz);

    nrrddim = 3; dim[0] = 6; 
    dim[1] = static_cast<size_t>(sz[0]);
    nrrdAlloc_nva(nrrd,nrrdTypeDouble,nrrddim,dim);
  
    if (nrrd->data == 0)
    {
      pr->error("FieldToNrrd: Could not allocate enough space for new Nrrd");
      return (false);      
    }  

    VMesh::Elem::iterator it, it_end;
    mesh->begin(it);
    mesh->end(it_end);
    size_t k = 0;
    
    double* data = reinterpret_cast<double*>(nrrd->data);      
    while (it != it_end)
    {
      Tensor t;
      field->get_value(t,*it);
      data[k] = t.mat_[0][0]; k++;
      data[k] = t.mat_[0][1]; k++;
      data[k] = t.mat_[0][2]; k++;
      data[k] = t.mat_[1][1]; k++;
      data[k] = t.mat_[1][2]; k++;
      data[k] = t.mat_[2][2]; k++;
      ++it;
    }
    
    nrrdcenter = nrrdCenterCell;
    tf = mesh->get_transform();
  }

  if (nrrddim)
  {
    // Set spacing information
    
    int centerdata[NRRD_DIM_MAX];
    for (size_t p=0;p<NRRD_DIM_MAX;p++)
    {
      centerdata[p] = nrrdcenter;
    }
    nrrdAxisInfoSet_nva(nrrd,nrrdAxisInfoCenter,centerdata); 

    int kind[NRRD_DIM_MAX];
    for (size_t p=0;p<NRRD_DIM_MAX;p++)
    {
      if (p==0) kind[p] = nrrdKind3DSymMatrix;
      else kind[p] = nrrdKindSpace;
    }
    nrrdAxisInfoSet_nva(nrrd,nrrdAxisInfoKind,kind);

    nrrd->spaceDim = 3;
    
    double Trans[16];
    tf.get(Trans);
    nrrd->axis[0].spaceDirection[0] = 0.0;
    nrrd->axis[0].spaceDirection[1] = 0.0;
    nrrd->axis[0].spaceDirection[2] = 0.0;
    
    for (int p=0;p<3;p++)
    {
      nrrd->spaceOrigin[p] = Trans[3+4*p];
      for (int q=0;q<nrrddim;q++)
        nrrd->axis[q+1].spaceDirection[p] = Trans[q+4*p];
    }
    
    for (int p=0;p<3;p++)
      for (int q=0;q<3;q++)
      {
        if (p==q) nrrd->measurementFrame[p][q] = 1.0;
        else nrrd->measurementFrame[p][q] = 0.0;
      }
    
    nrrd->space = nrrdSpace3DRightHanded;

    return (true);
  }

  return (false);  
}

bool FieldToNrrdAlgo::FieldToNrrd(ProgressReporter *pr, FieldHandle input, NrrdDataHandle& output)
{

  if (input.get_rep() == 0)
  {
    pr->error("FieldToNrrd: No input Field");
    return (false);    
  } 

  FieldInformation fi(input);
  if (!(fi.is_scanline()||fi.is_image()||fi.is_latvol()))
  {
    pr->error("FieldToNrrd: The input field is not a ScanlineMesh, not an ImageMesh, nor a LatVolMesh.");
    pr->error("FieldToNrrd: Fields other than these three types cannot be converted. In order to store the field as a nrrd, one has to interpolate the field onto a regular grid.");
    return (false);
  }

  if (fi.is_nodata())
  {
    pr->error("FieldToNrrd: This Field cannot be converted as it does not contains any data");
    return (false);
  }

  if (fi.is_nonlineardata())
  {
    pr->error("FieldToNrrd: Non linear nrrdTypeDoubles are not supported by the Nrrd format.");
    return (false);
  }
  
  if (!fi.is_linearmesh())
  {
    pr->error("FieldToNrrd: Non linear nrrdTypeDoubles are not supported by the Nrrd format.");
    return (false);  
  }
  
  FieldToNrrdAlgoT algo;

  if (fi.is_scalar())
  {
    if (fi.is_double())               return(algo.ScalarFieldToNrrd<double>(pr,input,output,nrrdTypeDouble));
    if (fi.is_float())                return(algo.ScalarFieldToNrrd<float>(pr,input,output,nrrdTypeFloat));
    if (fi.is_char())                 return(algo.ScalarFieldToNrrd<char>(pr,input,output,nrrdTypeChar));
    if (fi.is_unsigned_char())        return(algo.ScalarFieldToNrrd<unsigned char>(pr,input,output,nrrdTypeUChar));
    if (fi.is_short())                return(algo.ScalarFieldToNrrd<short>(pr,input,output,nrrdTypeShort));
    if (fi.is_unsigned_short())       return(algo.ScalarFieldToNrrd<unsigned short>(pr,input,output,nrrdTypeUShort));
    if (fi.is_int())                  return(algo.ScalarFieldToNrrd<int>(pr,input,output,nrrdTypeInt));
    if (fi.is_unsigned_int())         return(algo.ScalarFieldToNrrd<unsigned int>(pr,input,output,nrrdTypeUInt));
    if (fi.is_longlong())            return(algo.ScalarFieldToNrrd<long long>(pr,input,output,nrrdTypeLLong));
    if (fi.is_unsigned_longlong())   return(algo.ScalarFieldToNrrd<unsigned long long>(pr,input,output,nrrdTypeULLong));
    pr->error("FieldToNrrd: The field type is not supported by nrrd format, hence we cannot convert it");
    return (false);
  }
  
  if (fi.is_vector())
  {
    return(algo.VectorFieldToNrrd(pr,input,output));
  }
  
  if (fi.is_tensor())
  {
    return(algo.TensorFieldToNrrd(pr,input,output));
  }
  
  pr->error("FieldToNrrd: Unknown Field type encountered, cannot convert Field into Nrrd");
  return (false);
}

} // end namespace SCIRunAlgo


