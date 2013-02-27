# Add source for unsupported modules
SET(libina_unsupported_SOURCES
   nlesolve/minpack.cc
   models/maximumentropy.cc
   models/spectrumrecorder.cc
   models/tcparamscan.cc)

SET(libina_unsupported_HEADERS
   nlesolve/minpack.hh
   models/maximumentropy.hh
   models/spectrumrecorder.hh
   models/tcparamscan.hh)

#
# Compile library
#
IF(INA_ENABLE_STATIC)
ADD_LIBRARY(libunsupported STATIC ${libina_unsupported_SOURCES})
ELSE()
ADD_LIBRARY(libunsupported SHARED ${libina_unsupported_SOURCES})
ENDIF()

SET_TARGET_PROPERTIES(libunsupported PROPERTIES OUTPUT_NAME "inaunsupported")
TARGET_LINK_LIBRARIES(libunsupported ${LIBS})


# Install library files
INSTALL(TARGETS libunsupported
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
