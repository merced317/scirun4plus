/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageIORegion.h,v $
  Language:  C++
  Date:      $Date: 2009-07-12 10:52:54 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageIORegion_h
#define __itkImageIORegion_h

#include <algorithm>
#include "itkRegion.h"
#include "itkObjectFactory.h"
#include "itkImageRegion.h"
#include "itkIndex.h"
#include "itkSize.h"

#include <cstddef>

namespace itk
{

/** \class ImageIORegion
 * \brief An ImageIORegion represents a structured region of data.
 *
 * ImageIORegion is an class that represents some structured portion or
 * piece of an Image. The ImageIORegion is represented with an index and
 * a size in each of the n-dimensions of the image. (The index is the
 * corner of the image, the size is the lengths of the image in each of
 * the topological directions.)  ImageIORegion is not templated over
 * dimension, but uses dynamic arrays instead.
 *
 * The first pixel of an image always have a Zero index. Therefore the
 * index values of ImageIORegion may not directly correspond to those
 * of ImageRegion. When translation between the two is performed one
 * much consider the largest possible region who has a non-zero
 * starting index for the image.
 *
 * \sa Region
 * \sa ImageRegion
 * \sa Index
 * \sa Size
 * \sa MeshRegion
 */

class ITK_EXPORT ImageIORegion: public Region
{
public:
  /** Standard class typedefs. */
  typedef ImageIORegion Self;
  typedef Region        Superclass;

  /** these types correspond to those of itk::Size, itk::Offset and itk::Index */
  typedef size_t                  SizeValueType;
  typedef ptrdiff_t               IndexValueType;
  typedef ptrdiff_t               OffsetValueType;


  /** Index typedef support. An index is used to access pixel values. */
  typedef std::vector<IndexValueType>  IndexType;

  /** Size typedef support. A size is used to define region bounds. */
  typedef std::vector<SizeValueType>   SizeType;
    
  /** Region type taken from the superclass */
  typedef Superclass::RegionType       RegionType;

  /** Standard part of all itk objects. */
  itkTypeMacro(ImageIORegion, Region);

  /** Dimension of the image available at run time. */
  unsigned int GetImageDimension() const;

  /** Dimension of the region to be written. This differs from the
   * the image dimension and is calculated at run-time by examining
   * the size of the image in each coordinate direction. */
  unsigned int GetRegionDimension() const;
  
  /** Return the region type. Images are described with structured regions. */
  virtual RegionType GetRegionType() const;

  /** Constructor. ImageIORegion is a lightweight object that is not reference
   * counted, so the constructor is public. */
  ImageIORegion(unsigned int dimension);
  
  /** Constructor. ImageIORegion is a lightweight object that is not reference
   * counted, so the constructor is public.  Default dimension is 2. */
  ImageIORegion();
  
  /** Destructor. ImageIORegion is a lightweight object that is not reference
   * counted, so the destructor is public. */
  virtual ~ImageIORegion();

  /** Copy constructor. ImageIORegion is a lightweight object that is not
   * reference counted, so the copy constructor is public. */
  ImageIORegion(const Self& region);
  
  /** operator=. ImageIORegion is a lightweight object that is not reference
   * counted, so operator= is public. */
  void operator=(const Self& region);

  /** Set the index defining the corner of the region. */
  void SetIndex(const IndexType &index);

  /** Get index defining the corner of the region. */
  const IndexType & GetIndex() const;
  
  /** Set the size of the region. This plus the index determines the
   * rectangular shape, or extent, of the region. */
  void SetSize(const SizeType &size);

  /** Get the size of the region. */
  const SizeType & GetSize() const;

  /** Convenience methods to get the size of the image in a particular
   * coordinate direction i. Do not try to access image sizes beyond the
   * the ImageDimension. */
  SizeValueType GetSize(unsigned long i) const;
  IndexValueType GetIndex(unsigned long i) const;
  void SetSize(const unsigned long i, SizeValueType size);
  void SetIndex(const unsigned long i, IndexValueType idx);

  /** Compare two regions. */
  bool operator==(const Self &region) const;

  /** Compare two regions. */
  bool operator!=(const Self &region) const;
  
  /** Test if an index is inside */
  bool IsInside(const IndexType &index) const;
 
  /** Test if a region (the argument) is completly inside of this region */
  bool IsInside(const Self &region) const;

  /** Get the number of pixels contained in this region. This just
   * multiplies the size components. */
  SizeValueType GetNumberOfPixels( void ) const;

protected:
  /** Methods invoked by Print() to print information about the object
   * including superclasses. Typically not called by the user (use Print()
   * instead) but used in the hierarchical print process to combine the
   * output of several classes.  */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

private:
  unsigned int      m_ImageDimension;
  IndexType         m_Index;
  SizeType          m_Size;
};


// Declare operator<<
extern std::ostream & operator<<(std::ostream &os, const ImageIORegion &region); 

/** \class ImageIORegionAdaptor
 * \brief Helper class for converting ImageRegions into ImageIORegions and back.
 *
 **/
template< unsigned int VDimension >
class ImageIORegionAdaptor
{
public:
  typedef ImageRegion<VDimension>  ImageRegionType;
  typedef ImageIORegion            ImageIORegionType;

  typedef typename ImageRegionType::SizeType  ImageSizeType;
  typedef typename ImageRegionType::IndexType ImageIndexType;

  itkLegacyMacro(static void Convert( const ImageRegionType & outImageRegion, ImageIORegionType & inIORegion) );

  static void Convert( const ImageRegionType & inImageRegion, ImageIORegionType & outIORegion, const ImageIndexType &largestRegionIndex)
    {
    //
    // The ImageRegion and ImageIORegion objects may have different dimensions.
    // Here we only copy the common dimensions between the two. If the ImageRegion
    // has more dimensions than the ImageIORegion, then the defaults of the ImageRegion
    // will take care of the remaining codimension. If the ImageRegion has less dimensions
    // than the ImageIORegion, then the remaining IO dimensions are simply ignored.
    //
    const unsigned int ioDimension = outIORegion.GetImageDimension();
    const unsigned int imageDimension = VDimension;

    unsigned int minDimension = ( ioDimension > imageDimension ) ? imageDimension : ioDimension;

    ImageSizeType  size  = inImageRegion.GetSize();
    ImageIndexType index = inImageRegion.GetIndex();
    
    for( unsigned int i = 0; i < minDimension; i++ )
      {
      outIORegion.SetSize(  i, size[i] );
      outIORegion.SetIndex( i, index[i] - largestRegionIndex[i]);
      }

    //
    // Fill in the remaining codimension (if any) with default values
    //
    for( unsigned int k = minDimension; k < ioDimension; k++ )
      {
      outIORegion.SetSize(  k, 1 ); // Note that default size in IO is 1 not 0
      outIORegion.SetIndex( k, 0 );
      }
    }

  itkLegacyMacro(static void Convert( const ImageIORegionType & inIORegion, ImageRegionType & outImageRegion) );
  

  static void Convert( const ImageIORegionType & inIORegion, ImageRegionType & outImageRegion, const ImageIndexType &largestRegionIndex )
    {
    ImageSizeType  size;
    ImageIndexType index;

    size.Fill(1);  // initialize with default values
    index.Fill(0);

    //
    // The ImageRegion and ImageIORegion objects may have different dimensions.
    // Here we only copy the common dimensions between the two. If the ImageRegion
    // has more dimensions than the ImageIORegion, then the defaults of the ImageRegion
    // will take care of the remaining codimension. If the ImageRegion has less dimensions
    // than the ImageIORegion, then the remaining IO dimensions are simply ignored.
    //
    const unsigned int ioDimension = inIORegion.GetImageDimension();
    const unsigned int imageDimension = VDimension;

    unsigned int minDimension = ( ioDimension > imageDimension ) ? imageDimension : ioDimension;

    for(unsigned int i=0; i<minDimension; i++)
      {
      size[i]  = inIORegion.GetSize(i);
      index[i] = inIORegion.GetIndex(i) + largestRegionIndex[i];
      }

    outImageRegion.SetSize( size );
    outImageRegion.SetIndex( index );
    }
};

#if !defined(ITK_LEGACY_REMOVE)
template< unsigned int VDimension >
void ImageIORegionAdaptor<VDimension>::Convert( const ImageRegionType & inImageRegion, ImageIORegionType & outIORegion ) 
{
  itkGenericLegacyBodyMacro(ImageIORegionAdaptor::Convert, 4.0);
  // the index argument is required
  ImageIndexType index;
  index.Fill(0);
  ImageIORegionAdaptor<VDimension>::Convert(inImageRegion, outIORegion, index);
}

template< unsigned int VDimension >
void ImageIORegionAdaptor<VDimension>::Convert( const ImageIORegionType & inIORegion, ImageRegionType & outImageRegion )
{
  itkGenericLegacyBodyMacro(ImageIORegionAdaptor::Convert, 4.0);
  // the index argument is required
  ImageIndexType index;
  index.Fill(0);
  ImageIORegionAdaptor<VDimension>::Convert(inIORegion, outImageRegion, index);
}
#endif

} // end namespace itk

#endif
