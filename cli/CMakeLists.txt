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

SET(sbml_ssa_SOURCES
    sbml_ssa.cc)
SET(sbml_ssa_HEADERS
    sbml_ssa.hh)

SET(sbml_pscan_SOURCES
    sbml_pscan.cc)
SET(sbml_pscan_HEADERS
    sbml_pscan.hh)


SET(sbml_fft_SOURCES
    sbml_fft.cc)
SET(sbml_fft_HEADERS
    sbml_fft.hh)

ADD_EXECUTABLE(sbml_lna ${sbml_lna_SOURCES})
ADD_EXECUTABLE(sbml_pscan ${sbml_pscan_SOURCES})
ADD_EXECUTABLE(sbml_ssa ${sbml_ssa_SOURCES})
ADD_EXECUTABLE(sbml_fft ${sbml_fft_SOURCES})

TARGET_LINK_LIBRARIES(sbml_lna ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_pscan ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_ssa ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_fft ${LIBS} libina)
