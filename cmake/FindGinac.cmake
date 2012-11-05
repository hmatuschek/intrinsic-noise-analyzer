#
# Try to find GiNaC and its dependencies.
#

# GiNaC requires CLN
find_package(GinacCLN REQUIRED)

# Search for libraries
find_library(GINAC_LIBRARY
             NAMES ginac)

# Search for headers:
find_path(GINAC_INCLUDE_DIR ginac.h
          PATH_SUFFIXES ginac)

set(GINAC_LIBRARIES ${GINAC_LIBRARY} ${GINAC_CLN_LIBRARY})
set(GINAC_INCLUDE_DIRS ${GINAC_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ginac  DEFAULT_MSG
                                  GINAC_LIBRARY GINAC_INCLUDE_DIR)

mark_as_advanced(GINAC_INCLUDE_DIR GINAC_LIBRARY)
