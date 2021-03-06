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
 *  Freetype.cc:
 *
 *  Written by:
 *   McKay Davis
 *   Scientific Computing and Imaging Institute
 *   University of Utah
 *   October 2004
 *
 */

#include <Core/Exceptions/InternalError.h>
#include <Core/Exceptions/FileNotFound.h>

#include <Core/Geom/FreeType.h>
#include <Core/Math/MiscMath.h>
#include <Core/Util/Assert.h>
#include <iostream>
#include <sys/stat.h>

namespace SCIRun {
using std::ostream;

FreeTypeLibrary::FreeTypeLibrary() :
  ref_cnt(0)
{
  if (FT_Init_FreeType(&library_))
    throw InternalError("FreeType Unable to Initialize", __FILE__, __LINE__);
}



FreeTypeLibrary::~FreeTypeLibrary() 
{
  if (FT_Done_FreeType(library_))
    throw InternalError("FreeType did not close properly", __FILE__, __LINE__);
}

FreeTypeFace *
FreeTypeLibrary::load_face(const std::string& filename)
{
  FreeTypeFace *face = new FreeTypeFace(this, filename);
  return face;
}


// This one is used by SCIRun
FreeTypeFace::FreeTypeFace(const std::string& filename) 
  : ref_cnt(0),
    ft_face_(0),
    points_(12.0),
    x_dpi_(72),
    y_dpi_(72),
    filename_(filename)
{
  library_ = new FreeTypeLibrary();
  struct stat buf;
  if (stat(filename.c_str(),&buf) < 0)
    throw FileNotFound(filename, __FILE__, __LINE__);

  FT_Error error = FT_New_Face(library_->library_, filename.c_str(), 0, &ft_face_);
  if (error == FT_Err_Unknown_File_Format)
    throw InternalError("FreeType Unknown Face File Format "+filename, __FILE__, __LINE__);
  else if (error)
    throw InternalError("Freetype Cannot Initialize Face "+filename, __FILE__, __LINE__);
}


// Fog Seg3D
FreeTypeFace::FreeTypeFace(FreeTypeLibrary *lib, const std::string& filename) 
  : ref_cnt(0),
    ft_face_(0),
    points_(12.0),
    x_dpi_(72),
    y_dpi_(72),
    library_(lib),
    filename_(filename)
{
  struct stat buf;
  if (stat(filename.c_str(),&buf) < 0)
    throw FileNotFound(filename, __FILE__, __LINE__);

  FT_Error error = FT_New_Face(library_->library_, filename.c_str(), 0, &ft_face_);
  if (error == FT_Err_Unknown_File_Format)
    throw InternalError("FreeType Unknown Face File Format "+filename, __FILE__, __LINE__);
  else if (error)
    throw InternalError("Freetype Cannot Initialize Face "+filename, __FILE__, __LINE__);
}


FreeTypeFace::~FreeTypeFace() 
{
  if (FT_Done_Face(ft_face_))
    throw InternalError("FreeType Face did not close properly", __FILE__, __LINE__);
}


std::string
FreeTypeFace::get_family_name() 
{
  return std::string(static_cast<char *>(ft_face_->family_name));
}

std::string
FreeTypeFace::get_style_name() 
{
  return std::string(static_cast<char *>(ft_face_->style_name));
}


bool
FreeTypeFace::has_kerning_p() 
{
  return ft_face_->face_flags & FT_FACE_FLAG_KERNING;  
}


bool
FreeTypeFace::scalable_p() 
{
  return ft_face_->face_flags & FT_FACE_FLAG_SCALABLE;
}


void
FreeTypeFace::set_dpi(unsigned int x_dpi, unsigned int y_dpi) 
{
  x_dpi_ = x_dpi;
  y_dpi_ = y_dpi;
  if (FT_Set_Char_Size(ft_face_, Round(points_*64.0), Round(points_*64.0), x_dpi_, y_dpi_)) 
  {
    throw InternalError("FreeType Cannot set_dpi.", __FILE__, __LINE__);
  }
}

void
FreeTypeFace::set_points(double points) 
{
  points_ = points;
  if (FT_Set_Char_Size(ft_face_, Round(points_*64.0), Round(points_*64.0), x_dpi_, y_dpi_)) 
    throw InternalError("FreeType Cannot set_points.", __FILE__, __LINE__);
}

std::string
FreeTypeFace::get_filename() 
{
  return filename_;
}


FreeTypeGlyph::FreeTypeGlyph()
  : glyph_(0),
    index_(0),
    position_(0.0, 0.0, 0.0)
{
}

FreeTypeGlyph::~FreeTypeGlyph()
{
  if (glyph_) 
    FT_Done_Glyph(glyph_);
}

FT_Vector
FreeTypeGlyph::ft_position() {
  FT_Vector ret_val;
  ret_val.x = Round(position_.x()*64.0);
  ret_val.y = Round(position_.y()*64.0);
  return ret_val;
}
  

FreeTypeText::FreeTypeText(std::string text, FreeTypeFace *face, Point *pos)
  : text_(text),
    face_(face),
    glyphs_(),
    position_(0.0,0.0,0.0),
    anchor_(sw)
{
  // This position is for the placement of the anchor of the entire string
  if (pos) position_ = *pos;
  layout();
}

void
FreeTypeText::layout()
{
  // This position is just the relative cursor for glyph layout
  Point position(0.0,0.0,0.0); // just using x and y for layout
  FT_UInt last_index = 0;
  const bool do_kerning = face_->has_kerning_p();
  glyphs_.resize(text_.length());
  for (unsigned int i = 0; i < text_.length(); i++ ) 
  {
    FreeTypeGlyph *glyph = new FreeTypeGlyph();
    glyphs_[i] = glyph;

    FT_ULong charcode = text_.c_str()[i];
    glyph->index_ = FT_Get_Char_Index(face_->ft_face_, charcode); 
    if (last_index && do_kerning && glyph->index_) 
    { 
      FT_Vector kern; 
      FT_Get_Kerning(face_->ft_face_, last_index, glyph->index_, 
		     FT_KERNING_DEFAULT, &kern); 
      position(0) += kern.x / 64.0;
      position(1) += kern.y / 64.0;      
    } 


    if (FT_Load_Glyph(face_->ft_face_, glyph->index_, FT_LOAD_DEFAULT))
      throw InternalError("FreeType Unable to Load Glyph: "+text_[i], __FILE__, __LINE__);
    if (FT_Get_Glyph(face_->ft_face_->glyph, &glyph->glyph_))
      throw InternalError("FreeType Unable to Get Glyph: "+text_[i], __FILE__, __LINE__);

    glyph->position_ = position;
    FT_Vector delta;
    delta.x = Round(position.x()*64.0);
    delta.y = Round(position.y()*64.0);
    FT_Glyph_Transform(glyph->glyph_, 0, &delta); 
    
    position.x(position.x()+face_->ft_face_->glyph->advance.x / 64.0);
    position.y(position.y()+face_->ft_face_->glyph->advance.y / 64.0);
    
    last_index = glyph->index_;
  }
}


FreeTypeText::~FreeTypeText() 
{
  while (!glyphs_.empty()) 
  {
    delete glyphs_.back();
    glyphs_.pop_back();
  }
}

FreeTypeFace *
FreeTypeText::get_face() 
{
  return face_;
}

void
FreeTypeText::get_bounds(BBox& in_bb)
{
  // Todo, fix for position and anchor
  FT_BBox ft_bbox;
  for (unsigned int i = 0; i < glyphs_.size(); ++i) 
  {
    FT_Glyph_Get_CBox(glyphs_[i]->glyph_, ft_glyph_bbox_truncate, &ft_bbox);

    Point ll(ft_bbox.xMin, ft_bbox.yMin, 0.0);
    Point ur(ft_bbox.xMax+1.0, ft_bbox.yMax+1.0, 0.0);
    ll = (ll + position_).asPoint();
    ur = (ur + position_).asPoint();
    in_bb.extend(ll);
    in_bb.extend(ur);
  } 
}



void
FreeTypeText::render(int width, int height, unsigned char *buffer)
{
  FT_BBox glyph_bbox;
  FT_Vector delta;
  delta.x = Round(position_.x()*64.0);
  delta.y = Round(position_.y()*64.0);

  for (unsigned int i = 0; i < glyphs_.size(); ++i) {
    FT_Glyph temp_glyph;
    if (FT_Glyph_Copy(glyphs_[i]->glyph_, &temp_glyph))
      throw InternalError("FreeType Unable to Copy Glyph: "+
                          text_[i], __FILE__, __LINE__);

    FT_Glyph_Transform(temp_glyph, 0, &delta);
    FT_Glyph_Get_CBox(temp_glyph, ft_glyph_bbox_truncate, &glyph_bbox);

    if (glyph_bbox.xMax > 0 && glyph_bbox.xMin < width &&
	glyph_bbox.yMax > 0 && glyph_bbox.yMin < height) {

      if (FT_Glyph_To_Bitmap(&temp_glyph, FT_RENDER_MODE_NORMAL, 0, 1)) {
        FT_Done_Glyph(temp_glyph);
        throw InternalError("FreeType Unable to Render Glyph: "+
                            text_[i], __FILE__, __LINE__);
      }

      FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)temp_glyph;

      ASSERT(bitmap_glyph->bitmap.num_grays == 256);
      ASSERT(bitmap_glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);

      int top = bitmap_glyph->top;
      int left = bitmap_glyph->left;
      // render glyph here
      for (int y = 0; y < bitmap_glyph->bitmap.rows; ++y) {
        if (top-y < 0 || top-y >= height) continue;
        for (int x = 0; x < bitmap_glyph->bitmap.width; ++x) {
          if (left+x < 0 || left+x >= width) continue;
          buffer[(top-y)*width+left+x] =
            bitmap_glyph->bitmap.buffer[y*Abs(bitmap_glyph->bitmap.pitch)+x];
        }
      }
    }
    FT_Done_Glyph(temp_glyph);
  }
}


void
FreeTypeText::set_position(const Point &pt) 
{
  position_ = pt;
}

} // End namespace SCIRun
