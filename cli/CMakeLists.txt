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

ADD_EXECUTABLE(sbml_lna ${sbml_lna_SOURCES})
ADD_EXECUTABLE(sbml_ssa ${sbml_ssa_SOURCES})

TARGET_LINK_LIBRARIES(sbml_lna ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_ssa ${LIBS} libina)
