#ifndef __CLI_SBML_HYBRID_HH__
#define __CLI_SBML_HYBRID_HH__

#include <iostream>
#include "ina.hh"
#include <eigen3/Eigen/Eigen>
#include <ginac/ginac.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <string>
#include <sstream>
#include <fstream>


void doSSE(iNA::Ast::Model &model);

void printProgBar( int percent );

std::vector<std::string> readSoI(const std::string &parameterFile);

std::string Confucius();

#endif
