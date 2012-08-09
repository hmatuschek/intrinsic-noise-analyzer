#
# Central build-script for unit tests
#

#
# All the sources for the unit tests
#
SET(ina_test_SOURCES
    main.cc unittest.cc lnatest.cc interpretertest.cc
    ginacforeigentest.cc regression_test.cc mathtest.cc mersennetwistertest.cc
    sbmlshparsertest.cc optionparsertest.cc odetest.cc modelcopytest.cc benchmark.cc
    constantfoldertest.cc)

SET(ina_test_HEADERS
    main.hh unittest.hh lnatest.hh interpretertest.hh
    ginacforeigentest.hh regression_test.hh mathtest.hh mersennewistertest.hh
    sbmlshparsertest.hh optionparsertest.hh odetest.hh modelcopytest.hh benchmark.hh
    constantfoldertest.hh)

IF(WITH_EXECUTION_ENGINE_LIBJIT)
SET(ina_test_SOURCES ${ina_test_SOURCES} libjitinterpretertest.cc)
SET(ina_test_HEADERS ${ina_test_HEADERS} libjitinterpretertest.hh)
ENDIF(WITH_EXECUTION_ENGINE_LIBJIT)

ADD_EXECUTABLE(ina-test ${ina_test_SOURCES})
TARGET_LINK_LIBRARIES(ina-test ${LIBS} libina)
