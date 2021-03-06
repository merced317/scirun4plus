/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkShapedFloodFilledImageFunctionConditionalConstIterator.h,v $
  Language:  C++
  Date:      $Date: 2009-04-28 16:16:25 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShapedFloodFilledImageFunctionConditionalConstIterator_h
#define __itkShapedFloodFilledImageFunctionConditionalConstIterator_h

#include "itkShapedFloodFilledFunctionConditionalConstIterator.h"

namespace itk
{

/**
 * \class ShapedFloodFilledImageFunctionConditionalConstIterator
 * \brief Iterates over a flood-filled image function. 
 *
 * Contributed as a paper to the Insight Journal:
 *  http://hdl.handle.net/1926/1320
 *
 * \ingroup ImageIterators
 *
 */
template<class TImage, class TFunction>
class ITK_EXPORT ShapedFloodFilledImageFunctionConditionalConstIterator:
    public ShapedFloodFilledFunctionConditionalConstIterator<TImage, TFunction>
{
public:
  /** Standard class typedefs. */
  typedef ShapedFloodFilledImageFunctionConditionalConstIterator
          Self;
  typedef ShapedFloodFilledFunctionConditionalConstIterator<TImage, TFunction>
          Superclass;
  
  /** Type of function */
  typedef typename Superclass::FunctionType FunctionType;

  /** Type of vector used to store location info in the spatial function */
  typedef typename Superclass::FunctionInputType FunctionInputType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;

  /** Size typedef support. */
  typedef typename Superclass::SizeType SizeType;

  /** Region typedef support */
  typedef typename Superclass::RegionType RegionType;

  /** Image typedef support. */
  typedef typename Superclass::ImageType ImageType;

  /** Internal Pixel Type */
  typedef typename Superclass::InternalPixelType InternalPixelType;

  /** External Pixel Type */
  typedef typename Superclass::PixelType PixelType;

  /** Dimension of the image the iterator walks.  This constant is needed so
   * functions that are templated over image iterator type (as opposed to
   * being templated over pixel type and dimension) can have compile time
   * access to the dimension of the image that the iterator walks. */
  itkStaticConstMacro(NDimensions, unsigned int, Superclass::NDimensions);

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. This version of the constructor uses
   * an explicit seed pixel for the flood fill, the "startIndex" */
  ShapedFloodFilledImageFunctionConditionalConstIterator(
    const ImageType *imagePtr,
    FunctionType *fnPtr,
    IndexType startIndex): Superclass(imagePtr,
                                      fnPtr,
                                      startIndex) {};

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. This version of the constructor uses
   * an explicit list of seed pixels for the flood fill, the "startIndex" */
  ShapedFloodFilledImageFunctionConditionalConstIterator(
    const ImageType *imagePtr,
    FunctionType *fnPtr,
    std::vector<IndexType>& startIndex): Superclass(imagePtr,
                                                    fnPtr,
                                                    startIndex) {};

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. This version of the constructor
   * should be used when the seed pixel is unknown. */
  ShapedFloodFilledImageFunctionConditionalConstIterator(
    const ImageType *imagePtr,
    FunctionType *fnPtr): Superclass(imagePtr,
                                     fnPtr) {};
  /** Default Destructor. */
  virtual ~ShapedFloodFilledImageFunctionConditionalConstIterator() {};

  /** Compute whether the index of interest should be included in the flood */
  bool IsPixelIncluded(const IndexType & index) const;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkShapedFloodFilledImageFunctionConditionalConstIterator.txx"
#endif

#endif 
