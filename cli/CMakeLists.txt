
SET(ina_CLI_SOURCES 
    ina_cli.cc ina_cli_steadystate.cc ina_cli_importmodel.cc)
SET(ina_CLI_HEADERS
    ina_cli.hh ina_cli_steadystate.hh ina_cli_importmodel.hh)

ADD_EXECUTABLE(ina-cli ${ina_CLI_SOURCES})
TARGET_LINK_LIBRARIES(ina-cli ${LIBS} libina)
 
