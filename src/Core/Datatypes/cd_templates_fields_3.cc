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

#include <Core/Persistent/PersistentSTL.h>
#include <Core/Geometry/Tensor.h>
#include <Core/Geometry/Vector.h>
#include <Core/Basis/NoData.h>
#include <Core/Basis/Constant.h>
#include <Core/Basis/TetQuadraticLgn.h>
#include <Core/Basis/HexTriquadraticLgn.h>
#include <Core/Basis/PrismLinearLgn.h>
#include <Core/Datatypes/PrismVolMesh.h>
#include <Core/Datatypes/TetVolMesh.h>
#include <Core/Datatypes/LatVolMesh.h>
#include <Core/Datatypes/GenericField.h>


using namespace SCIRun;
//NoData
typedef NoDataBasis<double>                  NDBasis;

//Constant
typedef ConstantBasis<Tensor>                CFDTensorBasis;
typedef ConstantBasis<Vector>                CFDVectorBasis;
typedef ConstantBasis<double>                CFDdoubleBasis;
typedef ConstantBasis<float>                 CFDfloatBasis;
typedef ConstantBasis<int>                   CFDintBasis;
typedef ConstantBasis<long long>             CFDlonglongBasis;
typedef ConstantBasis<short>                 CFDshortBasis;
typedef ConstantBasis<char>                  CFDcharBasis;
typedef ConstantBasis<unsigned int>          CFDuintBasis;
typedef ConstantBasis<unsigned short>        CFDushortBasis;
typedef ConstantBasis<unsigned char>         CFDucharBasis;
typedef ConstantBasis<unsigned long>         CFDulongBasis;

//Linear
typedef PrismLinearLgn<Tensor>                PFDTensorBasis;
typedef PrismLinearLgn<Vector>                PFDVectorBasis;
typedef PrismLinearLgn<double>                PFDdoubleBasis;
typedef PrismLinearLgn<float>                 PFDfloatBasis;
typedef PrismLinearLgn<int>                   PFDintBasis;
typedef PrismLinearLgn<long long>             PFDlonglongBasis;
typedef PrismLinearLgn<short>                 PFDshortBasis;
typedef PrismLinearLgn<char>                  PFDcharBasis;
typedef PrismLinearLgn<unsigned int>          PFDuintBasis;
typedef PrismLinearLgn<unsigned short>        PFDushortBasis;
typedef PrismLinearLgn<unsigned char>         PFDucharBasis;
typedef PrismLinearLgn<unsigned long>         PFDulongBasis;

typedef PrismVolMesh<PrismLinearLgn<Point> > PVMesh;
PersistentTypeID backwards_compat_PVM("PrismVolMesh", "Mesh",
				      PVMesh::maker, PVMesh::maker);
template class PrismVolMesh<PrismLinearLgn<Point> >;


//NoData
template class GenericField<PVMesh, NDBasis, std::vector<double> >;  

//Constant
template class GenericField<PVMesh, CFDTensorBasis, std::vector<Tensor> >;       
template class GenericField<PVMesh, CFDVectorBasis, std::vector<Vector> >;       
template class GenericField<PVMesh, CFDdoubleBasis, std::vector<double> >;       
template class GenericField<PVMesh, CFDfloatBasis,  std::vector<float> >;        
template class GenericField<PVMesh, CFDintBasis,    std::vector<int> >;
template class GenericField<PVMesh, CFDlonglongBasis,std::vector<long long> >;
template class GenericField<PVMesh, CFDshortBasis,  std::vector<short> >;        
template class GenericField<PVMesh, CFDcharBasis,   std::vector<char> >;         
template class GenericField<PVMesh, CFDuintBasis,   std::vector<unsigned int> >; 
template class GenericField<PVMesh, CFDushortBasis, std::vector<unsigned short> >;
template class GenericField<PVMesh, CFDucharBasis,  std::vector<unsigned char> >;
template class GenericField<PVMesh, CFDulongBasis,  std::vector<unsigned long> >;

//Linear
template class GenericField<PVMesh, PFDTensorBasis, std::vector<Tensor> >;       
template class GenericField<PVMesh, PFDVectorBasis, std::vector<Vector> >;       
template class GenericField<PVMesh, PFDdoubleBasis, std::vector<double> >;       
template class GenericField<PVMesh, PFDfloatBasis,  std::vector<float> >;        
template class GenericField<PVMesh, PFDintBasis,    std::vector<int> >;
template class GenericField<PVMesh, PFDlonglongBasis,std::vector<long long> >;
template class GenericField<PVMesh, PFDshortBasis,  std::vector<short> >;        
template class GenericField<PVMesh, PFDcharBasis,   std::vector<char> >;         
template class GenericField<PVMesh, PFDuintBasis,   std::vector<unsigned int> >; 
template class GenericField<PVMesh, PFDushortBasis, std::vector<unsigned short> >;
template class GenericField<PVMesh, PFDucharBasis,  std::vector<unsigned char> >;
template class GenericField<PVMesh, PFDulongBasis,  std::vector<unsigned long> >;

PersistentTypeID 
backwards_compat_PVFT("PrismVolField<Tensor>", "Field",
		      GenericField<PVMesh, PFDTensorBasis, 
		      std::vector<Tensor> >::maker, 
		      GenericField<PVMesh, CFDTensorBasis, 
		      std::vector<Tensor> >::maker);
PersistentTypeID 
backwards_compat_PVFV("PrismVolField<Vector>", "Field",
		      GenericField<PVMesh, PFDVectorBasis, 
		      std::vector<Vector> >::maker,
		      GenericField<PVMesh, CFDVectorBasis, 
		      std::vector<Vector> >::maker);
PersistentTypeID 
backwards_compat_PVFd("PrismVolField<double>", "Field",
		      GenericField<PVMesh, PFDdoubleBasis, 
		      std::vector<double> >::maker,
		      GenericField<PVMesh, CFDdoubleBasis, 
		      std::vector<double> >::maker,
		      GenericField<PVMesh, NDBasis, 
		      std::vector<double> >::maker);
PersistentTypeID 
backwards_compat_PVFf("PrismVolField<float>", "Field",
		      GenericField<PVMesh, PFDfloatBasis, 
		      std::vector<float> >::maker,
		      GenericField<PVMesh, CFDfloatBasis, 
		      std::vector<float> >::maker);
PersistentTypeID 
backwards_compat_PVFi("PrismVolField<int>", "Field",
		      GenericField<PVMesh, PFDintBasis, 
		      std::vector<int> >::maker,
		      GenericField<PVMesh, CFDintBasis, 
		      std::vector<int> >::maker);
PersistentTypeID 
backwards_compat_PVFs("PrismVolField<short>", "Field",
		      GenericField<PVMesh, PFDshortBasis, 
		      std::vector<short> >::maker,
		      GenericField<PVMesh, CFDshortBasis, 
		      std::vector<short> >::maker);
PersistentTypeID 
backwards_compat_PVFc("PrismVolField<char>", "Field",
		      GenericField<PVMesh, PFDcharBasis, 
		      std::vector<char> >::maker,
		      GenericField<PVMesh, CFDcharBasis, 
		      std::vector<char> >::maker);
PersistentTypeID 
backwards_compat_PVFui("PrismVolField<unsigned_int>", "Field",
		       GenericField<PVMesh, PFDuintBasis, 
		       std::vector<unsigned int> >::maker,
		       GenericField<PVMesh, CFDuintBasis, 
		       std::vector<unsigned int> >::maker);
PersistentTypeID 
backwards_compat_PVFus("PrismVolField<unsigned_short>", "Field",
		       GenericField<PVMesh, PFDushortBasis, 
		       std::vector<unsigned short> >::maker,
		       GenericField<PVMesh, CFDushortBasis, 
		       std::vector<unsigned short> >::maker);
PersistentTypeID 
backwards_compat_PVFuc("PrismVolField<unsigned_char>", "Field",
		       GenericField<PVMesh, PFDucharBasis, 
		       std::vector<unsigned char> >::maker,
		       GenericField<PVMesh, CFDucharBasis, 
		       std::vector<unsigned char> >::maker);
PersistentTypeID 
backwards_compat_PVFul("PrismVolField<unsigned_long>", "Field",
		       GenericField<PVMesh, PFDulongBasis, 
		       std::vector<unsigned long> >::maker,
		       GenericField<PVMesh, CFDulongBasis, 
		       std::vector<unsigned long> >::maker);

typedef TetLinearLgn<Tensor>                TFDTensorBasis;
typedef TetLinearLgn<Vector>                TFDVectorBasis;
typedef TetLinearLgn<double>                TFDdoubleBasis;
typedef TetLinearLgn<float>                 TFDfloatBasis;
typedef TetLinearLgn<int>                   TFDintBasis;
typedef TetLinearLgn<long long>             TFDlonglongBasis;
typedef TetLinearLgn<short>                 TFDshortBasis;
typedef TetLinearLgn<char>                  TFDcharBasis;
typedef TetLinearLgn<unsigned int>          TFDuintBasis;
typedef TetLinearLgn<unsigned short>        TFDushortBasis;
typedef TetLinearLgn<unsigned char>         TFDucharBasis;
typedef TetLinearLgn<unsigned long>         TFDulongBasis;

typedef TetVolMesh<TetLinearLgn<Point> > TVMesh;
PersistentTypeID backwards_compat_TVM("TetVolMesh", "Mesh",
				      TVMesh::maker, TVMesh::maker);
template class TetVolMesh<TetLinearLgn<Point> >;


//NoData
template class GenericField<TVMesh, NDBasis, std::vector<double> >;  

//Constant
template class GenericField<TVMesh, CFDTensorBasis, std::vector<Tensor> >;       
template class GenericField<TVMesh, CFDVectorBasis, std::vector<Vector> >;       
template class GenericField<TVMesh, CFDdoubleBasis, std::vector<double> >;       
template class GenericField<TVMesh, CFDfloatBasis,  std::vector<float> >;        
template class GenericField<TVMesh, CFDintBasis,    std::vector<int> >;
template class GenericField<TVMesh, CFDlonglongBasis,std::vector<long long> >;
template class GenericField<TVMesh, CFDshortBasis,  std::vector<short> >;        
template class GenericField<TVMesh, CFDcharBasis,   std::vector<char> >;         
template class GenericField<TVMesh, CFDuintBasis,   std::vector<unsigned int> >; 
template class GenericField<TVMesh, CFDushortBasis, std::vector<unsigned short> >;
template class GenericField<TVMesh, CFDucharBasis,  std::vector<unsigned char> >;
template class GenericField<TVMesh, CFDulongBasis,  std::vector<unsigned long> >;

//Linear
template class GenericField<TVMesh, TFDTensorBasis, std::vector<Tensor> >;       
template class GenericField<TVMesh, TFDVectorBasis, std::vector<Vector> >;       
template class GenericField<TVMesh, TFDdoubleBasis, std::vector<double> >;       
template class GenericField<TVMesh, TFDfloatBasis,  std::vector<float> >;        
template class GenericField<TVMesh, TFDintBasis,    std::vector<int> >;
template class GenericField<TVMesh, TFDlonglongBasis,std::vector<long long> >;
template class GenericField<TVMesh, TFDshortBasis,  std::vector<short> >;        
template class GenericField<TVMesh, TFDcharBasis,   std::vector<char> >;         
template class GenericField<TVMesh, TFDuintBasis,   std::vector<unsigned int> >; 
template class GenericField<TVMesh, TFDushortBasis, std::vector<unsigned short> >;
template class GenericField<TVMesh, TFDucharBasis,  std::vector<unsigned char> >;
template class GenericField<TVMesh, TFDulongBasis,  std::vector<unsigned long> >;

PersistentTypeID 
backwards_compat_TVFT("TetVolField<Tensor>", "Field",
		      GenericField<TVMesh, TFDTensorBasis, 
		      std::vector<Tensor> >::maker,
		      GenericField<TVMesh, CFDTensorBasis, 
		      std::vector<Tensor> >::maker);
PersistentTypeID 
backwards_compat_TVFV("TetVolField<Vector>", "Field",
		      GenericField<TVMesh, TFDVectorBasis, 
		      std::vector<Vector> >::maker,
		      GenericField<TVMesh, CFDVectorBasis, 
		      std::vector<Vector> >::maker);
PersistentTypeID 
backwards_compat_TVFd("TetVolField<double>", "Field",
		      GenericField<TVMesh, TFDdoubleBasis, 
		      std::vector<double> >::maker,
		      GenericField<TVMesh, CFDdoubleBasis, 
		      std::vector<double> >::maker,
		      GenericField<TVMesh, NDBasis, 
		      std::vector<double> >::maker);
PersistentTypeID 
backwards_compat_TVFf("TetVolField<float>", "Field",
		      GenericField<TVMesh, TFDfloatBasis, 
		      std::vector<float> >::maker,
		      GenericField<TVMesh, CFDfloatBasis, 
		      std::vector<float> >::maker);
PersistentTypeID 
backwards_compat_TVFi("TetVolField<int>", "Field",
		      GenericField<TVMesh, TFDintBasis, 
		      std::vector<int> >::maker,
		      GenericField<TVMesh, CFDintBasis, 
		      std::vector<int> >::maker);
PersistentTypeID 
backwards_compat_TVFs("TetVolField<short>", "Field",
		      GenericField<TVMesh, TFDshortBasis, 
		      std::vector<short> >::maker,
		      GenericField<TVMesh, CFDshortBasis, 
		      std::vector<short> >::maker);
PersistentTypeID 
backwards_compat_TVFc("TetVolField<char>", "Field",
		      GenericField<TVMesh, TFDcharBasis, 
		      std::vector<char> >::maker,
		      GenericField<TVMesh, CFDcharBasis, 
		      std::vector<char> >::maker);
PersistentTypeID 
backwards_compat_TVFui("TetVolField<unsigned_int>", "Field",
		       GenericField<TVMesh, TFDuintBasis, 
		       std::vector<unsigned int> >::maker,
		       GenericField<TVMesh, CFDuintBasis, 
		       std::vector<unsigned int> >::maker);
PersistentTypeID 
backwards_compat_TVFus("TetVolField<unsigned_short>", "Field",
		       GenericField<TVMesh, TFDushortBasis, 
		       std::vector<unsigned short> >::maker,
		       GenericField<TVMesh, CFDushortBasis, 
		       std::vector<unsigned short> >::maker);
PersistentTypeID 
backwards_compat_TVFuc("TetVolField<unsigned_char>", "Field",
		       GenericField<TVMesh, TFDucharBasis, 
		       std::vector<unsigned char> >::maker,
		       GenericField<TVMesh, CFDucharBasis, 
		       std::vector<unsigned char> >::maker);
PersistentTypeID 
backwards_compat_TVFul("TetVolField<unsigned_long>", "Field",
		       GenericField<TVMesh, TFDulongBasis, 
		       std::vector<unsigned long> >::maker,
		       GenericField<TVMesh, CFDulongBasis, 
		       std::vector<unsigned long> >::maker);


typedef TetQuadraticLgn<Tensor>                TQFDTensorBasis;
typedef TetQuadraticLgn<Vector>                TQFDVectorBasis;
typedef TetQuadraticLgn<double>                TQFDdoubleBasis;
typedef TetQuadraticLgn<float>                 TQFDfloatBasis;
typedef TetQuadraticLgn<int>                   TQFDintBasis;
typedef TetQuadraticLgn<long long>             TQFDlonglongBasis;
typedef TetQuadraticLgn<short>                 TQFDshortBasis;
typedef TetQuadraticLgn<char>                  TQFDcharBasis;
typedef TetQuadraticLgn<unsigned int>          TQFDuintBasis;
typedef TetQuadraticLgn<unsigned short>        TQFDushortBasis;
typedef TetQuadraticLgn<unsigned char>         TQFDucharBasis;
typedef TetQuadraticLgn<unsigned long>         TQFDulongBasis;

typedef TetVolMesh<TetQuadraticLgn<Point> > QTVMesh;
template class GenericField<QTVMesh, TQFDTensorBasis, std::vector<Tensor> >;       
template class GenericField<QTVMesh, TQFDVectorBasis, std::vector<Vector> >;       
template class GenericField<QTVMesh, TQFDdoubleBasis, std::vector<double> >;       
template class GenericField<QTVMesh, TQFDfloatBasis,  std::vector<float> >;   
template class GenericField<QTVMesh, TQFDintBasis,    std::vector<int> >;
template class GenericField<QTVMesh, TQFDlonglongBasis,std::vector<long long> >;
template class GenericField<QTVMesh, TQFDshortBasis,  std::vector<short> >;        
template class GenericField<QTVMesh, TQFDcharBasis,   std::vector<char> >;         
template class GenericField<QTVMesh, TQFDuintBasis,   std::vector<unsigned int> >; 
template class GenericField<QTVMesh, TQFDushortBasis, std::vector<unsigned short> >;
template class GenericField<QTVMesh, TQFDucharBasis,  std::vector<unsigned char> >;
template class GenericField<QTVMesh, TQFDulongBasis,  std::vector<unsigned long> >;
