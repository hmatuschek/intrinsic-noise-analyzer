#
# Central build-script for all the tiny CLI programs
#

#
# All the sources for the unit tests
#

SET(sbml_lna_SOURCES
    sbml_lna.cc)
SET(sbml_lna_HEADERS
    sbml_lna.hh)

SET(sbml_lna_time_SOURCES
    sbml_lna_time.cc)
SET(sbml_lna_time_HEADERS
    sbml_lna_time.hh)

SET(sbml_ssa_SOURCES
    sbml_ssa.cc)
SET(sbml_ssa_HEADERS
    sbml_ssa.hh)

SET(sbml_ssa_time_SOURCES
    sbml_ssa_time.cc)
SET(sbml_ssa_time_HEADERS
    sbml_ssa_time.hh)

#SET(sbml_cle_SOURCES
#    sbml_cle.cc)
#SET(sbml_cle_HEADERS
#    sbml_cle.hh)


#SET(sbml_fft_SOURCES
#    sbml_fft.cc)
#SET(sbml_fft_HEADERS
#    sbml_fft.hh)

#SET(sbml_fsp_SOURCES
#    sbml_fsp.cc)
#SET(sbml_fsp_HEADERS
#    sbml_fsp.hh)

ADD_EXECUTABLE(sbml_lna ${sbml_lna_SOURCES})
ADD_EXECUTABLE(sbml_ssa ${sbml_ssa_SOURCES})
#ADD_EXECUTABLE(sbml_cle ${sbml_cle_SOURCES})

ADD_EXECUTABLE(sbml_lna_time ${sbml_lna_time_SOURCES})
ADD_EXECUTABLE(sbml_ssa_time ${sbml_ssa_time_SOURCES})

#ADD_EXECUTABLE(sbml_fft ${sbml_fft_SOURCES})
#ADD_EXECUTABLE(sbml_fsp ${sbml_fsp_SOURCES})

TARGET_LINK_LIBRARIES(sbml_lna ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_ssa ${LIBS} libina)
#TARGET_LINK_LIBRARIES(sbml_cle ${LIBS} libina)
#TARGET_LINK_LIBRARIES(sbml_fft ${LIBS} libina)
#TARGET_LINK_LIBRARIES(sbml_fsp ${LIBS} libina)

TARGET_LINK_LIBRARIES(sbml_ssa_time ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_lna_time ${LIBS} libina)
