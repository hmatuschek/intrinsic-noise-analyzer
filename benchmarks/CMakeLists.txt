#
# Central build-script for benchmarks
#

#
# All the sources for the benchmarks
#
SET(fluctuator_benchmark_SOURCES
    main.cc benchmark.cc intprtvsdirect.cc)

SET(fluctuator_benchmark_HEADERS
    main.hh benchmark.hh intprtvsdirect.hh)

ADD_EXECUTABLE(fluctuator-benchmark ${fluctuator_benchmark_SOURCES})
TARGET_LINK_LIBRARIES(fluctuator-benchmark ${LIBS} liblnassa)
