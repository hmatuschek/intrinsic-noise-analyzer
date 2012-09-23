/**
 * @defgroup models Models and Analyses
 *
 * This module collects some model-classes and mix-ins, that allows to implement new methods
 * easily.
 *
 * @defgroup sse System Size Expansion
 * @ingroup models
 *
 * @defgroup ssa Stochastic Simulation Algorithm
 * @ingroup models
 *
 */


#ifndef __FLUC_MODELS_HH__
#define __FLUC_MODELS_HH__

#include "basemodel.hh"
#include "constantstoichiometrymixin.hh"
#include "constcompartmentmixin.hh"
#include "extensivespeciesmixin.hh"
#include "conservationanalysismixin.hh"

#include "assertnoconstantspeciesmixin.hh"
#include "assertconstantparametermixin.hh"
#include "reasonablemodelmixin.hh"

#include "REmodel.hh"
#include "LNAmodel.hh"
#include "IOSmodel.hh"

#include "steadystateanalysis.hh"
#include "sseinterpreter.hh"

#include "stochasticsimulator.hh"
#include "gillespieSSA.hh"
#include "optimizedSSA.hh"
#include "gibsonbruckSSA.hh"

#endif
