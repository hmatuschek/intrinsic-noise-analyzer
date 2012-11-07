#include "ina_cli_steadystate.hh"
#include "ina_cli_importmodel.hh"

#include <ast/model.hh>
#include <models/IOSmodel.hh>
#include <models/steadystateanalysis.hh>
#include <iostream>
#include <eigen3/Eigen/Eigen>
#include <fstream>


using namespace iNA;

// Helper function to save the results
int saveSteadyStateAnalysis(
    iNA::Utils::Opt::Parser &option_parser,
    Eigen::VectorXd &re_mean, Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
    Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr);

// Helper function to save results as CVS
int saveSteadyStateAnalysisCSV(
    std::ostream &stream,
    Eigen::VectorXd &re_mean, Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
    Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr);

// Helper function to save results as MAT Matlab format 5
int saveSteadyStateAnalysisMAT(
    std::ostream &stream,
    Eigen::VectorXd &re_mean, Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
    Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr);



int
performSteadyStateAnalysis(Utils::Opt::Parser &option_parser)
{
  // Parse model
  iNA::Ast::Model *model = 0;
  if (0 == (model = importModel(option_parser))) { return -1; }

  /// @todo Add some more options like max-steps, etc.

  // Assemble and perform stready state analysis
  Models::IOSmodel ios_model(*model); delete model;
  Models::SteadyStateAnalysis<Models::IOSmodel> steadystate(ios_model);
  Eigen::VectorXd reduced_state(ios_model.getDimension());
  steadystate.calcSteadyState(reduced_state);

  // Get full state and covariance and EMRE corrections for steady state;
  Eigen::VectorXd re_concentrations(ios_model.numSpecies());
  Eigen::VectorXd emre_corrections(ios_model.numSpecies());
  Eigen::VectorXd ios_corrections(ios_model.numSpecies());
  Eigen::MatrixXd lna_covariances(ios_model.numSpecies(), ios_model.numSpecies());
  Eigen::MatrixXd ios_covariances(ios_model.numSpecies(), ios_model.numSpecies());
  Eigen::VectorXd thirdOrder(ios_model.numSpecies());
  ios_model.fullState(reduced_state, re_concentrations, lna_covariances, emre_corrections,
                      ios_covariances, thirdOrder, ios_corrections);

  // Save results into files or to stdout:
  return saveSteadyStateAnalysis(option_parser, re_concentrations, lna_covariances, emre_corrections,
                                 ios_covariances, ios_corrections);
}


int
saveSteadyStateAnalysis(Utils::Opt::Parser &option_parser, Eigen::VectorXd &re_mean,
                        Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
                        Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr)
{
  if (option_parser.has_option("output-csv")) {
    std::fstream file(option_parser.get_option("output-csv").front().c_str());
    if (! file.is_open()) {
      std::cerr << "Can not open file " << option_parser.get_option("output-csv").front() << std::endl;
      return -1;
    }
    return saveSteadyStateAnalysisCSV(file, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
  } else if (option_parser.has_option("output-mat")) {
    std::fstream file(option_parser.get_option("output-csv").front().c_str());
    if (! file.is_open()) {
      std::cerr << "Can not open file " << option_parser.get_option("output-csv").front() << std::endl;
      return -1;
    }
    return saveSteadyStateAnalysisMAT(file, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
  } else if (option_parser.has_option("output")) {
    std::cerr << "This feature is not implemented yet. " << std::endl;
    return -1;
  } else if (option_parser.has_flag("output-stdout")) {
    return saveSteadyStateAnalysisCSV(std::cout, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
  }

  std::cerr << "No ouput is given!" << std::cerr;
  return -1;
}



int
saveSteadyStateAnalysisCSV(std::ostream &stream, Eigen::VectorXd &re_mean,
                           Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
                           Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr)
{
  return 0;
}


int
saveSteadyStateAnalysisMAT(std::ostream &stream, Eigen::VectorXd &re_mean,
                           Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
                           Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr)
{
  return 0;
}




