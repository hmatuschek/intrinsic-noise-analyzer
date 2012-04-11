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

SET(libina_HEADERS fluctuator.hh smartptr.hh linkedlist.hh
    exception.hh
    erf.hh kroneckerproduct.hh incompletegamma.hh
    ginacsupportforeigen.hh math.hh openmp.hh mersennetwister.hh)

# Then, the AST classes:
SET(libina_ast_SOURCES
    ast/definition.cc ast/assembler.cc ast/functiondefinition.cc ast/node.cc ast/rule.cc
    ast/reaction.cc ast/scope.cc ast/variabledefinition.cc ast/constraint.cc ast/module.cc
    ast/evaluate.cc ast/unitdefinition.cc ast/species.cc ast/compartment.cc ast/parameter.cc
    ast/model.cc)
SET(libina_ast_HEADERS ast/ast.hh
    ast/definition.hh ast/assembler.hh ast/functiondefinition.hh ast/node.hh ast/rule.hh
    ast/reaction.hh ast/scope.hh ast/variabledefinition.hh ast/constraint.hh ast/module.hh
    ast/evaluate.hh ast/unitdefinition.hh ast/species.hh ast/compartment.hh ast/parameter.hh
    ast/model.hh)

SET(libina_ast_trafo_SOURCES
    ast/trafo/walker.cc ast/trafo/pass.cc ast/trafo/constantfolder.cc ast/trafo/dump.cc
    ast/trafo/assignmentruleinliner.cc ast/trafo/substitutioncollector.cc ast/trafo/modelwalker.cc
    ast/trafo/modelcopyist.cc)
SET(libina_ast_trafo_HEADERS ast/trafo/trafo.hh
    ast/trafo/walker.hh ast/trafo/pass.hh ast/trafo/constantfolder.hh ast/trafo/dump.hh
    ast/trafo/assignmentruleinliner.hh ast/trafo/substitutioncollector.hh ast/trafo/modelwalker.hh
    ast/trafo/modelcopyist.hh)

SET(libina_sbmlsh_SOURCES
    sbmlsh/lexer.cc sbmlsh/parser.cc sbmlsh/productions.cc)
SET(libina_sbmlsh_HEADERS
    sbmlsh/lexer.hh sbmlsh/parser.hh sbmlsh/productions.hh)

SET(libina_utils_SOURCES
    utils/exception.cc utils/automata.cc utils/lexer.cc utils/tokenrules.cc utils/parser.cc
    utils/unittest.cc utils/option_parser.cc utils/cputime.cc)
SET(libina_utils_HEADERS
    utils/exception.hh utils/automata.hh utils/lexer.hh utils/tokenrules.hh utils/parser.hh
    utils/unittest.hh utils/option_parser.hh utils/cputime.hh)

# Add sources for BaseModel and derived classes.
SET(libina_models_SOURCES
    models/basemodel.cc models/constantstoichiometrymixin.cc models/constcompartmentmixin.cc
    models/conservationanalysismixin.cc
    models/lnamixin.cc models/prelnamixin.cc
    models/lnabasemodel.cc
    models/linearnoiseapproximation.cc models/extensivespeciesmixin.cc
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
    models/lnainterpreter.cc
    models/steadystateanalysis.cc
)

SET(libina_models_HEADERS models/models.hh
    models/basemodel.hh models/constantstoichiometrymixin.hh models/constcompartmentmixin.hh
    models/conservationanalysismixin.hh
    models/lnamixin.hh models/prelnamixin.hh
    models/lnabasemodel.hh
    models/linearnoiseapproximation.hh models/extensivespeciesmixin.hh
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
    models/lnainterpreter.hh
    models/steadystateanalysis.hh
)

# some unsupported things

SET(libina_unsupported_SOURCES
)
SET(libina_unsupported_HEADERS
)

# Add source for nonlinear solvers
SET(libina_nlesolve_SOURCES
   nlesolve/nlesolve.cc)
SET(libina_nlesolve_HEADERS
   nlesolve/nlesolve.hh)

# Add sources for ODE solvers etc.
SET(libina_ode_SOURCES
    ode/rungekutta4.cc ode/eulerdriver.cc ode/integrationrange.cc ode/rkf45.cc
    ode/semiimpliciteuler.cc ode/rosenbrock3.cc ode/rosenbrock4.cc ode/odemodel.cc ode/dopri5.cc
    ode/dopri853.cc ode/stepper.cc)
SET(libina_ode_HEADERS ode/ode.hh
    ode/rungekutta4.hh ode/eulerdriver.hh ode/integrationrange.hh ode/rkf45.hh
    ode/semiimpliciteuler.hh ode/rosenbrock3.hh ode/rosenbrock4.hh ode/odemodel.hh ode/dopri5.hh
    ode/dopri853.hh ode/stepper.hh)

# Simple Stack Machine Interpreter
SET(libina_intprt_SOURCES
    intprt/intprt.cc intprt/assembler.cc intprt/interpreter.cc intprt/pass.cc
    intprt/dependencetree.cc intprt/instruction.cc intprt/codestatistics.cc intprt/compiler.cc)
SET(libina_intprt_HEADERS
    intprt/intprt.hh intprt/assembler.hh intprt/interpreter.hh intprt/pass.hh
    intprt/dependencetree.hh intprt/instruction.hh intprt/codestatistics.hh intprt/compiler.hh)

#
# Sources for the evaluation sub-system:
#
SET(libina_eval_direct_SOURCES
    eval/direct/code.cc eval/direct/compiler.cc eval/direct/interpreter.cc)
SET(libina_eval_direct_HEADERS
    eval/direct/code.hh eval/direct/compiler.hh eval/direct/interpreter.hh)

SET(libina_eval_bytecode_SOURCES
    eval/bci/code.cc eval/bci/compiler.cc eval/bci/assembler.cc eval/bci/interpreter.cc
    eval/bci/dependencetree.cc eval/bci/pass.cc)
SET(libina_eval_direct_HEADERS
    eval/bci/code.hh eval/bci/compiler.hh eval/bci/assembler.hh eval/bci/interpreter.hh
    eval/bci/dependencetree.hh eval/bci/pass.hh)

SET(libina_eval_bytecode_mp_SOURCES
    eval/bcimp/code.cc eval/bcimp/compiler.cc eval/bcimp/interpreter.cc)
SET(libina_eval_bytecode_mp_HEADERS
    eval/bcimp/code.hh eval/bcimp/compiler.hh eval/bcimp/interpreter.hh)

SET(libina_eval_libjit_SOURCES
    eval/libjit/code.cc eval/libjit/builder.cc eval/libjit/assembler.cc eval/libjit/compiler.cc
    eval/libjit/interpreter.cc)
SET(libina_eval_libjit_HEADERS
    eval/libjit/code.hh eval/libjit/builder.hh eval/libjit/assembler.hh eval/libjit/compiler.hh
    eval/libjit/interpreter.hh)

SET(libina_eval_llvm_SOURCES
    eval/llvm/code.cc eval/llvm/interpreter.cc eval/llvm/compiler.cc eval/llvm/assembler.cc
    eval/llvm/builder.cc)
SET(libina_eval_llvm_HEADERS
    eval/llvm/code.hh eval/llvm/interpreter.hh eval/llvm/compiler.hh eval/llvm/assembler.hh
    eval/llvm/builder.hh)

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

IF(WITH_EXECUTION_ENGINE_LIBJIT)
   SET(libina_eval_SOURCES ${libina_eval_SOURCES} ${libina_eval_libjit_SOURCES})
   SET(libina_eval_HEADERS ${libina_eval_HEADERS} ${libina_eval_libjit_HEADERS})
ENDIF(WITH_EXECUTION_ENGINE_LIBJIT)

IF(WITH_EXECUTION_ENGINE_LLVM)
  SET(libina_eval_SOURCES ${libina_eval_SOURCES} ${libina_eval_llvm_SOURCES})
  SET(libina_eval_HEADERS ${libina_eval_HEADERS} ${libina_eval_llvm_HEADERS})
ENDIF(WITH_EXECUTION_ENGINE_LLVM)


# Finally assemble list of all sources
SET(libina_SOURCES
    ${libina_SOURCES}
    ${libina_utils_SOURCES}
    ${libina_sbmlsh_SOURCES}
    ${libina_ast_SOURCES}
    ${libina_ast_trafo_SOURCES}
    ${libina_models_SOURCES}
    ${libina_nlesolve_SOURCES}
    ${libina_ode_SOURCES}
    ${libina_intprt_SOURCES}
    ${libina_unsupported_SOURCES}
    ${libina_eval_SOURCES})

# and list of all headers:
SET(libina_HEADERS
    ${libina_HEADERS}
    ${libina_utils_HEADERS}
    ${libina_sbmlsh_HEADERS}
    ${libina_ast_HEADERS}
    ${libina_ast_trafo_HEADERS}
    ${libina_models_HEADERS}
    ${libina_nlesolve_HEADERS}
    ${libina_ode_HEADERS}
    ${libina_intprt_HEADERS}
    ${libina_unsupported_HEADERS}
    ${libina_eval_HEADERS})

#
# Compile library
#
ADD_LIBRARY(libina SHARED ${libina_SOURCES})
SET_TARGET_PROPERTIES(libina PROPERTIES OUTPUT_NAME "ina")
TARGET_LINK_LIBRARIES(libina ${LIBS})


INSTALL(TARGETS libina
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
