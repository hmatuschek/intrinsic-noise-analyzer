/* This file is part of the Intrinsic Noise Analyzer.
 * Copyright (C) 2011,2012  R. Grima, P. Thomas, H. Matuschek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. */


#ifndef __FLUC_INA_HH__
#define __FLUC_INA_HH__

/**
 * \mainpage Intrinsic Noise Analyzer
 *
 * intrinsic noise analyzer (iNA) is an easy-to-use computational tool for efficient analysis
 * of intrinsic noise in biochemical reaction networks. The SBML-based software combines two
 * complementary approaches to analyze the Chemical Master Equation:
 *
 * @li The System Size Expansion - a systematic analytical approximation method,
 * @li the Stochastic Simulation Algorithm - a widely used Monte Carlo method.
 *
 * iNA is based on the computer algebra system Ginac and the compiler framework LLVM.
 *
 * The initial release has been introduced in PLoS One \cite ina2012.
 *
 * This documentation covers the API of the core library, implenting all algorithms needed to
 * perform the analyses listed above. To perform an analysis, usually 3 steps are required:
 *
 * @li Parsing the SBML file of the model to be analyzed,
 * @li constructing an analysis, that performs all analytical operations on the model to derive
 *     a mathematical representation of the analysis and finally,
 * @li performing the analysis using one of the execution engines.
 */

#include "ast/ast.hh"
#include "models/models.hh"
#include "ode/ode.hh"
#include "parser/parser.hh"

#include "utils/cputime.hh"
#include "exception.hh"

#include "erf.hh"
#include "incompletegamma.hh"

#endif // __FLUC_INA_HH__
