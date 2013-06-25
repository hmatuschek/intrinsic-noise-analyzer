# Add source for unsupported modules
SET(libina_unsupported_SOURCES
   nlesolve/minpack.cc
   models/maximumentropy.cc
   models/spectrumrecorder.cc
   models/tcparamscan.cc
   models/hybridmodel.cc
   models/hybridsimulator.cc
   models/hybridSSA.cc
   models/gillespieCLE.cc
#   models/histogramD.cc
)

SET(libina_unsupported_HEADERS
   nlesolve/minpack.hh
   models/maximumentropy.hh
   models/spectrumrecorder.hh
   models/tcparamscan.hh
   models/hybridmodel.hh
   models/hybridsimulator.hh
   models/hybridSSA.hh
   models/gillespieCLE.hh
#   models/histogramD.hh
   )

#
# Compile library
#
IF(INA_ENABLE_STATIC)
ADD_LIBRARY(libunsupported STATIC ${libina_unsupported_SOURCES})
ELSE()
ADD_LIBRARY(libunsupported SHARED ${libina_unsupported_SOURCES})
ENDIF()

SET_TARGET_PROPERTIES(libunsupported PROPERTIES OUTPUT_NAME "inaunsupported")
TARGET_LINK_LIBRARIES(libunsupported ${LIBS} libina)


# No installation needed or?
INSTALL(TARGETS libunsupported
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
