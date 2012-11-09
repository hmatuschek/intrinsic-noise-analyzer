#include "ina_cli_steadystate.hh"
#include "ina_cli_importmodel.hh"
#include "ina_cli_fileextension.hh"

#include <ast/model.hh>
#include <models/IOSmodel.hh>
#include <models/steadystateanalysis.hh>
#include <utils/matexport.hh>

#include <iostream>
#include <fstream>
#include <eigen3/Eigen/Eigen>


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

  // Process steady state specific options
  size_t max_iter = 100;
  if (option_parser.has_option("max-iter")) {
    std::stringstream buffer(option_parser.get_option("max-iter").front()); buffer >> max_iter;
  }

  double epsilon  = 1e-9;
  if (option_parser.has_option("eps")) {
    std::stringstream buffer(option_parser.get_option("eps").front()); buffer >> epsilon;
  }

  double max_t    = 1e9;
  if (option_parser.has_option("max-dt")) {
    std::stringstream buffer(option_parser.get_option("max-dt").front()); buffer >> max_t;
  }

  double dt       = 1e-1;
  if (option_parser.has_option("min-dt")) {
    std::stringstream buffer(option_parser.get_option("min-dt").front()); buffer >> dt;
  }

  // Assemble and perform stready state analysis
  try {
    Models::IOSmodel ios_model(*model); delete model;
    Models::SteadyStateAnalysis<Models::IOSmodel> steadystate(
          ios_model, max_iter, epsilon, max_t, dt);
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
    return saveSteadyStateAnalysis(option_parser, re_concentrations, lna_covariances,
                                   emre_corrections, ios_covariances, ios_corrections);
  } catch (Exception &err) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Error during stady state analysis: " << err.what();
    Utils::Logger::get().log(message);
  }

  return -1;
}


int
saveSteadyStateAnalysis(Utils::Opt::Parser &option_parser, Eigen::VectorXd &re_mean,
                        Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
                        Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr)
{
  if (option_parser.has_option("output-csv")) {
    std::string filename(option_parser.get_option("output-csv").front());
    std::fstream file(filename.c_str(), std::ios_base::out);
    if (! file.is_open()) {
      std::cerr << "Can not open file " << option_parser.get_option("output-csv").front() << std::endl;
      return -1;
    }
    return saveSteadyStateAnalysisCSV(file, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
  } else if (option_parser.has_option("output-mat")) {
    std::string filename(option_parser.get_option("output-mat").front());
    std::fstream file(filename.c_str(), std::ios_base::out);
    if (! file.is_open()) {
      std::cerr << "Can not open file " << option_parser.get_option("output-mat").front() << std::endl;
      return -1;
    }
    return saveSteadyStateAnalysisMAT(file, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
  } else if (option_parser.has_option("output")) {
    // Get filename & extension
    std::string filename = option_parser.get_option("output").front().c_str();
    std::string extension = getFileExtension(filename);
    // Dispatch...
    if ("csv" == extension) {
      std::fstream file(filename.c_str(), std::ios_base::out);
      if (! file.is_open()) {
        std::cerr << "Can not open file " << filename << std::endl; return -1;
      }
      return saveSteadyStateAnalysisCSV(file, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
    } else if ("mat" == extension) {
      std::fstream file(filename.c_str(), std::ios_base::out);
      if (! file.is_open()) {
        std::cerr << "Can not open file " << filename << std::endl; return -1;
      }
      return saveSteadyStateAnalysisMAT(file, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
    }
    // If extension is unknown:
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not export results to \"" << filename << "\" unrecognized file extension "
            << extension << std::endl;
    Utils::Logger::get().log(message);
    return -1;
  } else if (option_parser.has_flag("output-stdout")) {
    return saveSteadyStateAnalysisCSV(std::cout, re_mean, lna_cov, emre_corr, ios_cov, ios_corr);
  }

  // This should not happen...
  std::cerr << "No ouput is given!" << std::cerr;
  return -1;
}



int
saveSteadyStateAnalysisCSV(std::ostream &stream, Eigen::VectorXd &re_mean,
                           Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
                           Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr)
{
  stream << "# First line: REs steady state," << std::endl
         << "# Second line: EMRE steady state," << std::endl
         << "# Third line: IOS steady state." << std::endl;

  stream << re_mean(0);
  for (int i=1; i<re_mean.rows(); i++) { stream << "\t" << re_mean(i); }
  stream << std::endl;

  stream << re_mean(0) + emre_corr(0);
  for (int i=1; i<re_mean.rows(); i++) { stream << "\t" << re_mean(i) + emre_corr(i); }
  stream << std::endl;

  stream << re_mean(0) + emre_corr(0) + ios_corr(0);
  for (int i=1; i<re_mean.rows(); i++) { stream << "\t" << re_mean(i) + emre_corr(i) + ios_corr(i); }
  stream << std::endl;

  for (int i=0; i<re_mean.rows(); i++) {
    stream << lna_cov(i,0) + ios_cov(i,0);
    for (int j=1; j<re_mean.rows(); j++) {
      stream << "\t" << lna_cov(i,j) + ios_cov(i,j);
    }
    stream << std::endl;
  }

  return 0;
}


int
saveSteadyStateAnalysisMAT(std::ostream &stream, Eigen::VectorXd &re_mean,
                           Eigen::MatrixXd &lna_cov, Eigen::VectorXd &emre_corr,
                           Eigen::MatrixXd &ios_cov, Eigen::VectorXd &ios_corr)
{
  // Create and assemble MAT file
  Utils::MatFile mat;
  mat.add("RE_means", re_mean);
  mat.add("EMRE_means", re_mean+emre_corr);
  mat.add("IOS_means", re_mean+emre_corr+ios_corr);
  mat.add("LNA_cov", lna_cov);
  mat.add("IOS_cov", lna_cov+ios_cov);

  // Serialize into given stream:
  mat.serialize(stream);

  // done.
  return 0;
}




