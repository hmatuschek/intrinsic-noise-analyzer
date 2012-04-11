#
# Try to find GiNaCs CLN library
#

# Search for libraries
find_library(GINAC_CLN_LIBRARY
             NAMES cln)

SET(GINAC_CLN_LIBRARIES ${GINAC_CLN_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GinacCLN  DEFAULT_MSG
                                  GINAC_CLN_LIBRARY)

mark_as_advanced(GINAC_CLN_LIBRARY)
