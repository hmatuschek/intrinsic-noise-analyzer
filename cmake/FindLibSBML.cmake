#
# Try to find libSBML include dirs and libraries
#

find_path(LIBSBML_INCLUDE_DIR SBMLTypes.h
          PATH_SUFFIXES sbml)

find_library(LIBSBML_LIBRARY
             NAMES sbml)

set(LIBSBML_LIBRARIES ${LIBSBML_LIBRARY})
set(LIBSBML_INCLUDE_DIRS ${LIBSBML_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibSBML  DEFAULT_MSG
                                  LIBSBML_LIBRARY LIBSBML_INCLUDE_DIR)

mark_as_advanced(LIBSBML_INCLUDE_DIR LIBSBML_LIBRARY)
