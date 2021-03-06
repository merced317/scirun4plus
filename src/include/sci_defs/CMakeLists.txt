#
#  For more information, please see: http://software.sci.utah.edu
# 
#  The MIT License
# 
#  Copyright (c) 2009 Scientific Computing and Imaging Institute,
#  University of Utah.
# 
#  
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#

# DV figure out mandatory sci_defs and finish

# configure some headers files to pass some of the CMake settings to the
# source code


CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/blas_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/blas_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/bits_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/bits_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/compile_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/compile_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/cmake_config_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/config_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/cmake_config_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/config_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/error_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/error_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/cmake_environment_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/environment_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/exe_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/exe_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/hashmap_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/hashmap_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/hdf5_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/hdf5_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/lapack_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/lapack_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/opengl_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/opengl_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/stat64_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/stat64_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/thread_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/thread_defs.h"
)

CONFIGURE_FILE (
  "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/wx_testdefs.h.in"
  "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/wx_defs.h"
)

CONFIGURE_FILE (
      "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/x11_testdefs.h.in"
      "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/x11_defs.h"
)

CONFIGURE_FILE (
      "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/teem_testdefs.h.in"
      "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/teem_defs.h"
)

CONFIGURE_FILE (
      "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/insight_testdefs.h.in"
      "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/insight_defs.h"
)

CONFIGURE_FILE (
      "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/version_testdefs.h.in"
      "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/version_defs.h"
)

IF(BUILD_TESTING)
  CONFIGURE_FILE (
        "${SCIRUN_CORE_SOURCE_DIR}/include/sci_defs/cmake_test_environment_testdefs.h.in"
        "${SCIRUN_CORE_BINARY_DIR}/include/sci_defs/cmake_test_environment_testdefs.h"
  )
ENDIF(BUILD_TESTING)
