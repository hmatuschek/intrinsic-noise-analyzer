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
    constantfoldertest.cc conservationanalysistest.cc unitparsertest.cc expressionparsertest.cc
    benchmark_pscan.cc iostest.cc retest.cc steadystatetest.cc ssatest.cc sseparamscantest.cc)

SET(ina_test_HEADERS
    main.hh unittest.hh lnatest.hh interpretertest.hh
    ginacforeigentest.hh regression_test.hh mathtest.hh mersennewistertest.hh
    sbmlshparsertest.hh optionparsertest.hh odetest.hh modelcopytest.hh benchmark.hh
    constantfoldertest.hh conservationanalysistest.hh unitparsertest.hh expressionparsertext.hh
    benchmark_pscan.hh iostest.hh retest.hh steadystatetest.hh ssatest.hh sseparamscantest.hh)

ADD_EXECUTABLE(ina-test ${ina_test_SOURCES})
TARGET_LINK_LIBRARIES(ina-test ${LIBS} libina)
