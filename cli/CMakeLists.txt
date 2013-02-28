
SET(ina_CLI_SOURCES 
    ina_cli.cc ina_cli_steadystate.cc ina_cli_importmodel.cc ina_cli_fileextension.cc
    ina_cli_listmodel.cc ina_cli_paramscan.cc ina_cli_savetable.cc ina_cli_ssetimecourse.cc
    ina_cli_utils.cc ina_cli_ssa.cc)
SET(ina_CLI_HEADERS
    ina_cli.hh ina_cli_steadystate.hh ina_cli_importmodel.hh ina_cli_fileextension.hh
    ina_cli_listmodel.hh ina_cli_paramscan.hh ina_cli_savetable.hh ina_cli_ssetimecourse.hh
    ina_cli_utils.hh ina_cli_ssa.hh)

#ADD_EXECUTABLE(ina-cli ${ina_CLI_SOURCES})
#TARGET_LINK_LIBRARIES(ina-cli ${LIBS} libina)
 
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


#SET(sbml_med_SOURCES
#    sbml_med.cc)
#SET(sbml_med_HEADERS
#    sbml_med.hh)

SET(sbml_dist_SOURCES
    sbml_dist.cc)
SET(sbml_dist_HEADERS
    sbml_dist.hh)

SET(sbml_ssa_SOURCES
    sbml_ssa.cc)
SET(sbml_ssa_HEADERS
    sbml_ssa.hh)

SET(sbml_ssacov_SOURCES
    sbml_ssacov.cc)
SET(sbml_ssacov_HEADERS
    sbml_ssacov.hh)

SET(sbml_pscan_SOURCES
    sbml_pscan.cc)
SET(sbml_pscan_HEADERS
    sbml_pscan.hh)

SET(sbml_pscan2_SOURCES
    sbml_pscan2.cc)
SET(sbml_pscan2_HEADERS
    sbml_pscan2.hh)

SET(sbml_fft_SOURCES
    sbml_fft.cc)
SET(sbml_fft_HEADERS
    sbml_fft.hh)

#ADD_EXECUTABLE(sbml_lna ${sbml_lna_SOURCES})
ADD_EXECUTABLE(sbml_pscan ${sbml_pscan_SOURCES})
ADD_EXECUTABLE(sbml_pscan2 ${sbml_pscan2_SOURCES})

#ADD_EXECUTABLE(sbml_ssa ${sbml_ssa_SOURCES})
#ADD_EXECUTABLE(sbml_ssacov ${sbml_ssacov_SOURCES})
#ADD_EXECUTABLE(sbml_dist ${sbml_dist_SOURCES})
#ADD_EXECUTABLE(sbml_med ${sbml_med_SOURCES})
ADD_EXECUTABLE(sbml_fft ${sbml_fft_SOURCES})

#TARGET_LINK_LIBRARIES(sbml_lna ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_pscan ${LIBS} libina)
TARGET_LINK_LIBRARIES(sbml_pscan2 ${LIBS} libina)
#TARGET_LINK_LIBRARIES(sbml_ssacov ${LIBS} libina)
#TARGET_LINK_LIBRARIES(sbml_ssa ${LIBS} libina libunsupported)
#TARGET_LINK_LIBRARIES(sbml_med ${LIBS} libina libunsupported)
#TARGET_LINK_LIBRARIES(sbml_dist ${LIBS} libina libunsupported)
TARGET_LINK_LIBRARIES(sbml_fft ${LIBS} libina libunsupported)
