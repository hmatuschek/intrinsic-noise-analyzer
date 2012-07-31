#
# Central build-script for libina
#

#
# All the sources for libina
#

# First, the base utils
SET(libina_SOURCES
    exception.cc
    erf.cc kroneckerproduct.cc incompletegamma.cc
    ginacsupportforeigen.cc math.cc openmp.cc mersennetwister.cc)

SET(libina_HEADERS ina.hh smartptr.hh linkedlist.hh
    exception.hh
    erf.hh kroneckerproduct.hh incompletegamma.hh
    ginacsupportforeigen.hh math.hh openmp.hh mersennetwister.hh)

# Then, the AST classes:
SET(libina_ast_SOURCES
    ast/definition.cc ast/assembler.cc ast/functiondefinition.cc ast/node.cc ast/rule.cc
    ast/reaction.cc ast/scope.cc ast/variabledefinition.cc ast/constraint.cc ast/module.cc
    ast/evaluate.cc ast/unitdefinition.cc ast/species.cc ast/compartment.cc ast/parameter.cc
    ast/model.cc ast/converter.cc)
SET(libina_ast_HEADERS
    ast/ast.hh
    ast/definition.hh ast/assembler.hh ast/functiondefinition.hh ast/node.hh ast/rule.hh
    ast/reaction.hh ast/scope.hh ast/variabledefinition.hh ast/constraint.hh ast/module.hh
    ast/evaluate.hh ast/unitdefinition.hh ast/species.hh ast/compartment.hh ast/parameter.hh
    ast/model.hh ast/converter.hh)

SET(libina_ast_trafo_SOURCES
    ast/trafo/walker.cc ast/trafo/pass.cc ast/trafo/constantfolder.cc ast/trafo/dump.cc
    ast/trafo/assignmentruleinliner.cc ast/trafo/substitutioncollector.cc ast/trafo/modelwalker.cc
    ast/trafo/modelcopyist.cc)
SET(libina_ast_trafo_HEADERS ast/trafo/trafo.hh
    ast/trafo/walker.hh ast/trafo/pass.hh ast/trafo/constantfolder.hh ast/trafo/dump.hh
    ast/trafo/assignmentruleinliner.hh ast/trafo/substitutioncollector.hh ast/trafo/modelwalker.hh
    ast/trafo/modelcopyist.hh)

SET(libina_sbmlsh_parser_SOURCES
    parser/sbmlsh/lexer.cc parser/sbmlsh/parser.cc parser/sbmlsh/productions.cc
    parser/sbmlsh/assembler.cc parser/sbmlsh/exporter.cc parser/sbmlsh/sbmlsh.cc)
SET(libina_sbmlsh_parser_HEADERS
    parser/sbmlsh/lexer.hh parser/sbmlsh/parser.hh parser/sbmlsh/productions.hh
    parser/sbmlsh/assembler.hh parser/sbmlsh/exporter.hh parser/sbmlsh/sbmlsh.hh)

SET(libina_sbml_parser_SOURCES
    parser/sbml/sbml.cc parser/sbml/assembler.cc parser/sbml/exporter.cc)
SET(libina_sbml_parser_HEADERS
    parser/sbml/sbml.hh parser/sbml/assembler.hh parser/sbml/exporter.hh)

SET(libina_parser_SOURCES
    ${libina_sbmlsh_parser_SOURCES}
    ${libina_sbml_parser_SOURCES})
SET(libina_parser_HEADERS
    ${libina_sbmlsh_parser_HEADERS}
    ${libina_sbml_parser_HEADERS})

SET(libina_utils_SOURCES
    utils/exception.cc utils/automata.cc utils/lexer.cc utils/tokenrules.cc utils/parser.cc
    utils/unittest.cc utils/option_parser.cc utils/cputime.cc utils/logger.cc)
SET(libina_utils_HEADERS
    utils/exception.hh utils/automata.hh utils/lexer.hh utils/tokenrules.hh utils/parser.hh
    utils/unittest.hh utils/option_parser.hh utils/cputime.hh utils/logger.hh)

# Add sources for BaseModel and derived classes.
SET(libina_models_SOURCES
    models/basemodel.cc models/constantstoichiometrymixin.cc models/constcompartmentmixin.cc
    models/conservationanalysismixin.cc
    models/propensityexpansion.cc
    models/ssebasemodel.cc
    models/REmodel.cc
    models/LNAmodel.cc
    models/IOSmodel.cc
    models/extensivespeciesmixin.cc
    models/intensivespeciesmixin.cc
    models/stochasticsimulator.cc
    models/gillespieSSA.cc
    models/optimizedSSA.cc
    models/explicittimedependencemixin.cc
    models/assertnoalgebraicconstraintmixin.cc
    models/assertnoconstantspeciesmixin.cc
    models/assertconstantparametermixin.cc
    models/assertnoassignmentrule.cc models/assertnoraterule.cc
    models/assertirreversiblereactions.cc
    models/reasonablemodelmixin.cc
    models/baseunitmixin.cc
    models/particlenumbersmixin.cc
    models/sseinterpreter.cc
    models/steadystateanalysis.cc
)

SET(libina_models_HEADERS models/models.hh
    models/basemodel.hh models/constantstoichiometrymixin.hh models/constcompartmentmixin.hh
    models/conservationanalysismixin.hh
    models/propensityexpansion.hh
    models/ssebasemodel.hh
    models/REmodel.hh
    models/LNAmodel.hh
    models/IOSmodel.hh
    models/extensivespeciesmixin.hh
    models/intensivespeciesmixin.hh
    models/stochasticsimulator.hh
    models/gillespieSSA.hh
    models/optimizedSSA.hh
    models/partialsumSSA.hh
    models/optimizedSSAginac.hh
    models/gillespieSSAginac.hh
    models/gillespieCLE.hh
    models/explicittimedependencemixin.hh
    models/assertnoalgebraicconstraintmixin.hh
    models/assertnoconstantspeciesmixin.hh
    models/assertconstantparametermixin.hh
    models/assertnoassignmentrule.hh models/assertnoraterule.hh
    models/assertirreversiblereactions.hh
    models/reasonablemodelmixin.hh
    models/baseunitmin.hh
    models/particlenumbersmixin.hh
    models/sseinterpreter.hh
    models/steadystateanalysis.hh
)

# Add source for nonlinear solvers
SET(libina_nlesolve_SOURCES
   nlesolve/newtonraphson.cc
   nlesolve/hybridsolver.cc)
SET(libina_nlesolve_HEADERS
   nlesolve/newtonraphson.hh
   nlesolve/hybridsolver.hh)

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
SET(libina_eval_direct_HEADERS
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
    ${libina_ast_trafo_SOURCES}
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
    ${libina_ast_trafo_HEADERS}
    ${libina_models_HEADERS}
    ${libina_nlesolve_HEADERS}
    ${libina_ode_HEADERS}
    ${libina_eval_HEADERS})

#
# Compile library
#
ADD_LIBRARY(libina SHARED ${libina_SOURCES})
SET_TARGET_PROPERTIES(libina PROPERTIES OUTPUT_NAME "ina")
TARGET_LINK_LIBRARIES(libina ${LIBS})


INSTALL(TARGETS libina
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
