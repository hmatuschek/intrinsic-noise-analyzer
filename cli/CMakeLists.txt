
SET(ina_CLI_SOURCES 
    ina_cli.cc ina_cli_steadystate.cc ina_cli_importmodel.cc ina_cli_fileextension.cc
    ina_cli_listmodel.cc ina_cli_paramscan.cc ina_cli_savetable.cc ina_cli_ssetimecourse.cc)
SET(ina_CLI_HEADERS
    ina_cli.hh ina_cli_steadystate.hh ina_cli_importmodel.hh ina_cli_fileextension.hh
    ina_cli_listmodel.hh ina_cli_paramscan.hh ina_cli_savetable.hh ina_cli_ssetimecourse.hh)

ADD_EXECUTABLE(ina-cli ${ina_CLI_SOURCES})
TARGET_LINK_LIBRARIES(ina-cli ${LIBS} libina)
 
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
TARGET_LINK_LIBRARIES(sbml_ssa ${LIBS} libina libunsupported)
TARGET_LINK_LIBRARIES(sbml_fft ${LIBS} libina libunsupported)
