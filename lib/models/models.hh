/**
 * @defgroup models Component Based Modeling
 *
 * This module collects some model-classes and mix-ins, that allows to implement new methods
 * easily.
 */


#ifndef __FLUC_MODELS_HH__
#define __FLUC_MODELS_HH__

#include "basemodel.hh"
#include "constantstoichiometrymixin.hh"
#include "constcompartmentmixin.hh"
#include "extensivespeciesmixin.hh"
#include "conservationanalysismixin.hh"

#include "assertnoalgebraicconstraintmixin.hh"
#include "assertnoconstantspeciesmixin.hh"
#include "assertconstantparametermixin.hh"
#include "reasonablemodelmixin.hh"

#include "lnamixin.hh"
#include "linearnoiseapproximation.hh"

#include "steadystateanalysis.hh"
#include "lnainterpreter.hh"
#include "lnaevaluator.hh"

#include "stochasticsimulator.hh"
#include "gillespieSSA.hh"
#include "optimizedSSA.hh"

//#include "unsupported.hh"

#endif
