#
# Try to find libjit
#

# Search for libraries
find_library(LIBJIT_LIBRARY
             NAMES jit
             PATH_SUFFIXES x86 x86_64)

find_path(LIBJIT_INCLUDE_DIR jit/jit.h
          PATH_SUFFIXES jit)

SET(LIBJIT_LIBRARIES ${LIBJIT_LIBRARY})
SET(LIBJIT_INCLUDE_DIRS ${LIBJIT_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBJIT DEFAULT_MSG
                                  LIBJIT_LIBRARY LIBJIT_INCLUDE_DIR)

mark_as_advanced(LIBJIT_LIBRARY LIBJIT_INCLUDE_DIR)
