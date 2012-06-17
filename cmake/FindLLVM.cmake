SET(LLVM_CONFIG_EXEC_NAME "llvm-config" "llvm-config-mp-2.9" "llvm-config-2.9" "llvm-config-2.8" "llvm-config-2.7")
FIND_PROGRAM(LLVM_CONFIG_EXEC NAMES ${LLVM_CONFIG_EXEC_NAME})


IF(LLVM_CONFIG_EXEC)
  SET(LLVM_CONFIG_FOUND TRUE)
  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--version"
    OUTPUT_VARIABLE LLVM_VERSION)
ENDIF(LLVM_CONFIG_EXEC)


IF(LLVM_CONFIG_FOUND AND LLVM_VERSION STREQUAL "2.9")
  # get linker flags
  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--ldflags"
    OUTPUT_VARIABLE LLVM_LD_FLAGS)

  # get include dirs:
  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--includedir"
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR)

  # get lib dirs:
  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--libdir"
    OUTPUT_VARIABLE LLVM_LIB_DIR)

  # get libraries:
  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--libs engine interpreter"
    OUTPUT_VARIABLE LLVM_LIBRARY)

  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--cflags"
    OUTPUT_VARIABLE LLVM_C_FLAGS)

  EXEC_PROGRAM(${LLVM_CONFIG_EXEC}
    ARGS "--cxxflags"
    OUTPUT_VARIABLE LLVM_CXX_FLAGS)

ENDIF(LLVM_CONFIG_FOUND AND LLVM_VERSION STREQUAL "2.9")



INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LLVM DEFAULT_MSG
                                  LLVM_INCLUDE_DIR LLVM_CXX_FLAGS LLVM_LIBRARY LLVM_LD_FLAGS)

SET(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIR})
SET(LLVM_LIBRARIES ${LLVM_LIBRARY} m ffi dl)

mark_as_advanced(LLVM_LIBRARY LLVM_INCLUDE_DIR LLVM_CXX_FLAGS LLVM_LD_FLAGS)
