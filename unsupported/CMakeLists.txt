# Add source for unsupported modules
SET(libina_unsupported_SOURCES
   models/maximumentropy.cc
   models/spectrumrecorder.cc
   models/gibsonbruckSSA.cc)

SET(libina_unsupported_HEADERS
   models/maximumentropy.hh
   models/spectrumrecorder.hh
   models/gibsonbruckSSA.hh)

#
# Compile library
#
ADD_LIBRARY(libunsupported SHARED ${libina_unsupported_SOURCES})
SET_TARGET_PROPERTIES(libunsupported PROPERTIES OUTPUT_NAME "inaunsupported")
TARGET_LINK_LIBRARIES(libunsupported ${LIBS})
