#
# Central build-script for libina
#

#
# All the sources for libina
#

# First, the base utils
SET(libina_SOURCES
    exception.cc erf.cc incompletegamma.cc
    ginacsupportforeigen.cc math.cc openmp.cc mersennetwister.cc)

SET(libina_HEADERS ina.hh smartptr.hh
    exception.hh erf.hh incompletegamma.hh
    ginacsupportforeigen.hh math.hh openmp.hh mersennetwister.hh)

# Then, the AST classes:
SET(libina_ast_SOURCES
    ast/definition.cc ast/functiondefinition.cc ast/node.cc ast/rule.cc
    ast/reaction.cc ast/scope.cc ast/variabledefinition.cc ast/constraint.cc
    ast/unitdefinition.cc ast/species.cc ast/compartment.cc ast/parameter.cc
    ast/model.cc ast/visitor.cc ast/unitconverter.cc ast/modelcopyist.cc)
SET(libina_ast_HEADERS
    ast/ast.hh
    ast/definition.hh ast/functiondefinition.hh ast/node.hh ast/rule.hh
    ast/reaction.hh ast/scope.hh ast/variabledefinition.hh ast/constraint.hh
    ast/unitdefinition.hh ast/species.hh ast/compartment.hh ast/parameter.hh
    ast/model.hh ast/visitor.hh ast/unitconverter.hh ast/modelcopyist.hh)

SET(libina_trafo_SOURCES
    trafo/assertions.cc trafo/substitution.cc trafo/constantfolder.cc
    trafo/assignmentruleinliner.cc
    trafo/reversiblereactionconverter.cc trafo/variablescaling.cc
    trafo/consistencycheck.cc trafo/modeldiff.cc trafo/referencecounter.cc trafo/makeparamglobal.cc)
SET(libina_trafo_HEADERS trafo/trafo.hh trafo/filterflags.hh
    trafo/assertions.hh trafo/substitution.hh trafo/constantfolder.hh
    trafo/assignmentruleinliner.hh
    trafo/reversiblereactionconverter.hh trafo/variablescaling.hh
    trafo/consistencycheck.hh trafo/modeldiff.hh trafo/referencecounter.hh trafo/makeparamglobal.hh)

SET(libina_sbmlsh_parser_SOURCES
    parser/sbmlsh/lexer.cc parser/sbmlsh/parser.cc parser/sbmlsh/productions.cc
    parser/sbmlsh/assembler.cc parser/sbmlsh/writer.cc parser/sbmlsh/sbmlsh.cc)
SET(libina_sbmlsh_parser_HEADERS
    parser/sbmlsh/lexer.hh parser/sbmlsh/parser.hh parser/sbmlsh/productions.hh
    parser/sbmlsh/assembler.hh parser/sbmlsh/writer.hh parser/sbmlsh/sbmlsh.hh)

SET(libina_sbml_parser_SOURCES
    parser/sbml/sbml.cc parser/sbml/assembler.cc parser/sbml/writer.cc)
SET(libina_sbml_parser_HEADERS
    parser/sbml/sbml.hh parser/sbml/assembler.hh parser/sbml/writer.hh)

SET(libina_expr_parser_SOURCES
    parser/expr/lexer.cc parser/expr/parser.cc parser/expr/productions.cc parser/expr/assembler.cc
    parser/expr/ir.cc)
SET(libina_expr_parser_HEADERS parser/expr/expr.hh
    parser/expr/lexer.hh parser/expr/parser.hh parser/expr/productions.hh parser/expr/assembler.hh
    parser/expr/ir.hh)

SET(libina_unit_parser_SOURCES
    parser/unit/unitparser.cc)
SET(libina_unit_parser_HEADERS
    parser/unit/unitparser.hh)

SET(libina_parser_SOURCES
    parser/automata.cc parser/lexer.cc parser/production.cc parser/exception.cc parser/ll1.cc
    ${libina_sbmlsh_parser_SOURCES}
    ${libina_sbml_parser_SOURCES}
    ${libina_expr_parser_SOURCES}
    ${libina_unit_parser_SOURCES})
SET(libina_parser_HEADERS
    parser/automata.hh parser/lexer.hh parser/production.hh parser/exception.hh  parser/ll1.hh
    ${libina_sbmlsh_parser_HEADERS}
    ${libina_sbml_parser_HEADERS}
    ${libina_expr_parser_HEADERS}
    ${libina_unit_parser_HEADERS})

SET(libina_utils_SOURCES
    utils/exception.cc utils/unittest.cc utils/option_parser.cc utils/cputime.cc utils/logger.cc
    utils/matexport.cc)
SET(libina_utils_HEADERS
    utils/exception.hh utils/unittest.hh utils/option_parser.hh utils/cputime.hh utils/logger.hh
    utils/matexport.hh)

# Add sources for BaseModel and derived classes.
SET(libina_models_SOURCES
    models/basemodel.cc models/constantstoichiometrymixin.cc models/constcompartmentmixin.cc
    models/conservationanalysismixin.cc
    models/conservationanalysis.cc
    models/propensityexpansion.cc
    models/ssebasemodel.cc
    models/REmodel.cc
    models/LNAmodel.cc
    models/IOSmodel.cc
    models/extensivespeciesmixin.cc
    models/intensivespeciesmixin.cc
    models/stochasticsimulator.cc
    models/histogram.cc
    models/gillespieSSA.cc
    models/optimizedSSA.cc
    models/assertnoconstantspeciesmixin.cc
    models/assertconstantparametermixin.cc
    models/assertnoassignmentrule.cc models/assertnoraterule.cc
    models/assertirreversiblereactions.cc models/assertnonempty.cc
    models/reasonablemodelmixin.cc
    models/baseunitmixin.cc
    models/particlenumbersmixin.cc
    models/sseinterpreter.cc
    models/steadystateanalysis.cc
    models/initialconditions.cc
    models/ssaparamscan.cc
    models/sseparamscan.cc
)

SET(libina_models_HEADERS models/models.hh
    models/basemodel.hh models/constantstoichiometrymixin.hh models/constcompartmentmixin.hh
    models/conservationanalysismixin.hh
    models/conservationanalysis.hh
    models/propensityexpansion.hh
    models/ssebasemodel.hh
    models/REmodel.hh
    models/LNAmodel.hh
    models/IOSmodel.hh
    models/extensivespeciesmixin.hh
    models/intensivespeciesmixin.hh
    models/stochasticsimulator.hh
    models/histogram.hh
    models/gillespieSSA.hh
    models/optimizedSSA.hh
    models/assertnoconstantspeciesmixin.hh
    models/assertconstantparametermixin.hh
    models/assertnoassignmentrule.hh models/assertnoraterule.hh
    models/assertirreversiblereactions.hh models/assertnonempty.hh
    models/reasonablemodelmixin.hh
    models/baseunitmixin.hh
    models/particlenumbersmixin.hh
    models/sseinterpreter.hh
    models/steadystateanalysis.hh
    models/initialconditions.hh
    models/ssaparamscan.hh
    models/sseparamscan.hh
)

# Add source for nonlinear solvers
SET(libina_nlesolve_SOURCES
   nlesolve/nlesolver.cc
   nlesolve/newtonraphson.cc
   nlesolve/hybridsolver.cc
   nlesolve/precisionsolve.cc)
SET(libina_nlesolve_HEADERS
   nlesolve/nlesolve.hh
   nlesolve/nlesolver.hh
   nlesolve/newtonraphson.hh
   nlesolve/hybridsolver.hh
   nlesolve/precisionsolve.hh)


# Add sources for ODE solvers etc.
SET(libina_ode_SOURCES
    ode/lsodadriver.cc
    ode/lsoda.cc
    ode/rungekutta4.cc ode/eulerdriver.cc ode/integrationrange.cc ode/rkf45.cc
    ode/semiimpliciteuler.cc ode/rosenbrock3.cc ode/rosenbrock4.cc ode/odemodel.cc ode/dopri5.cc
    ode/dopri853.cc ode/stepper.cc)
SET(libina_ode_HEADERS ode/ode.hh
    ode/lsodadriver.hh
    ode/lsoda.hh
    ode/rungekutta4.hh ode/eulerdriver.hh ode/integrationrange.hh ode/rkf45.hh
    ode/semiimpliciteuler.hh ode/rosenbrock3.hh ode/rosenbrock4.hh ode/odemodel.hh ode/dopri5.hh
    ode/dopri853.hh ode/stepper.hh)

#
# Sources for the evaluation sub-system:
#
SET(libina_eval_direct_SOURCES
    eval/direct/code.cc eval/direct/compiler.cc eval/direct/interpreter.cc eval/direct/engine.cc)
SET(libina_eval_direct_HEADERS
    eval/direct/code.hh eval/direct/compiler.hh eval/direct/interpreter.hh eval/direct/engine.hh)

SET(libina_eval_bytecode_SOURCES
    eval/bci/code.cc eval/bci/compiler.cc eval/bci/assembler.cc eval/bci/interpreter.cc
    eval/bci/dependencetree.cc eval/bci/pass.cc eval/bci/engine.cc)
SET(libina_eval_bytecode_HEADERS
    eval/bci/code.hh eval/bci/compiler.hh eval/bci/assembler.hh eval/bci/interpreter.hh
    eval/bci/dependencetree.hh eval/bci/pass.hh eval/bci/engine.hh)

SET(libina_eval_bytecode_mp_SOURCES
    eval/bcimp/code.cc eval/bcimp/compiler.cc eval/bcimp/interpreter.cc eval/bcimp/engine.cc)
SET(libina_eval_bytecode_mp_HEADERS
    eval/bcimp/code.hh eval/bcimp/compiler.hh eval/bcimp/interpreter.hh eval/bcimp/engine.hh)

SET(libina_eval_llvm_SOURCES
    eval/jit/code.cc eval/jit/interpreter.cc eval/jit/compiler.cc eval/jit/assembler.cc
    eval/jit/builder.cc eval/jit/engine.cc)
SET(libina_eval_llvm_HEADERS
    eval/jit/code.hh eval/jit/interpreter.hh eval/jit/compiler.hh eval/jit/assembler.hh
    eval/jit/builder.hh eval/jit/engine.hh)

SET(libina_eval_SOURCES
    eval/eval.cc eval/compilercommon.cc
    ${libina_eval_direct_SOURCES}
    ${libina_eval_bytecode_SOURCES}
    ${libina_eval_bytecode_mp_SOURCES})
SET(libina_eval_HEADERS
    eval/eval.hh eval/compilercommon.hh
    ${libina_eval_direct_HEADERS}
    ${libina_eval_bytecode_HEADERS}
    ${libina_eval_bytecode_mp_HEADERS})

IF(WITH_EXECUTION_ENGINE_LLVM)
  SET(libina_eval_SOURCES ${libina_eval_SOURCES} ${libina_eval_llvm_SOURCES})
  SET(libina_eval_HEADERS ${libina_eval_HEADERS} ${libina_eval_llvm_HEADERS})
ENDIF(WITH_EXECUTION_ENGINE_LLVM)

# Finally assemble list of all sources
SET(libina_SOURCES
    ${libina_SOURCES}
    ${libina_utils_SOURCES}
    ${libina_parser_SOURCES}
    ${libina_ast_SOURCES}
    ${libina_trafo_SOURCES}
    ${libina_models_SOURCES}
    ${libina_nlesolve_SOURCES}
    ${libina_ode_SOURCES}
    ${libina_eval_SOURCES})

# and list of all headers:
SET(libina_HEADERS
    ${libina_HEADERS}
    ${libina_utils_HEADERS}
    ${libina_parser_HEADERS}
    ${libina_ast_HEADERS}
    ${libina_trafo_HEADERS}
    ${libina_models_HEADERS}
    ${libina_nlesolve_HEADERS}
    ${libina_ode_HEADERS}
    ${libina_eval_HEADERS})


#
# Recursive copy header files to ${CMAKE_BINARY_DIR}/include/libina
#
COPY_HEADERS_WITH_DIRECTORY("${libina_HEADERS}" "${CMAKE_CURRENT_BINARY_DIR}/include/libina")

#
# Compile library
#
IF(INA_ENABLE_STATIC)
 # As static library:
ADD_LIBRARY(libina STATIC ${libina_SOURCES})
ELSE()
 # As shared library:
 ADD_LIBRARY(libina SHARED ${libina_SOURCES})
 SET_TARGET_PROPERTIES(libina PROPERTIES
  OUTPUT_NAME "ina"
  VERSION "${INA_VERSION_MAJOR}.${INA_VERSION_MINOR}.${INA_VERSION_PATCH}"
  SOVERION "${INA_VERSION_MAJOR}"
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
ENDIF()

TARGET_LINK_LIBRARIES(libina ${LIBS})

# Install library files
INSTALL(TARGETS libina
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Install header files
INSTALL_HEADERS_WITH_DIRECTORY("${libina_HEADERS}" "${CMAKE_INSTALL_INCLUDEDIR}/libina")
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.hh" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libina")
