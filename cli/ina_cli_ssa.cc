#include "ina_cli_ssa.hh"
#include "ina_cli_utils.hh"
#include "ina_cli_importmodel.hh"
#include "ina_cli_savetable.hh"

#include <models/optimizedSSA.hh>
#include <eval/jit/engine.hh>

using namespace iNA;


int saveSSATimecourseAnalysis(Eigen::MatrixXd &table, Utils::Opt::Parser &parser)
{
  std::stringstream header; header << "# Result of SSA time-course analysis:" << std::endl;
  return saveTable(header.str(), "SSA", table, parser);
}



int performSSATimecourseAnalysis(Utils::Opt::Parser &option_parser)
{
  // Determine time rage:
  std::string range = option_parser.get_option("range").front();
  double t_min, t_max; size_t N_steps=100;
  if (! ina_cli_parseRange(range, t_min, t_max, N_steps)) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Invalid range format in '" << range << "'. Must be FROM:TO[:STEPS]";
    Utils::Logger::get().log(message);
    return -1;
  }
  double dt = (t_max-t_min)/N_steps;

  // Get ensemble size:
  long int ensemble_size=0;
  if(! ina_cli_string2Int(option_parser.get_option("ensemble").front(), ensemble_size)) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Can not parse ensemble size ("
            << option_parser.get_option("ensemble").front() << ")as integer!";
    Utils::Logger::get().log(message);
    return -1;
  }
  if (0 >= ensemble_size) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Invalid ensemble size: " << ensemble_size << ". Must be grater than 0.";
    Utils::Logger::get().log(message);
    return -1;
  }

  // Load model and construct analysis:
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }

  // Create simulator
  Models::GenericOptimizedSSA< Eval::jit::Engine<Eigen::VectorXd> > simulator(
        *model, (size_t)ensemble_size, time(0), 0, OpenMP::getMaxThreads());

  // Allocate space for statistics
  size_t _Ns = model->numSpecies();
  Eigen::VectorXd mean(_Ns);
  Eigen::MatrixXd cov(_Ns, _Ns);
  Eigen::VectorXd skewness(_Ns);
  Eigen::MatrixXd timeseries(1+N_steps, 1+_Ns+(_Ns*(_Ns+1))/2);

  // Perform simulation:
  for (size_t i=0; i<(N_steps+1); i++)
  {
    // Set current simulation statistics:
    simulator.stats(mean, cov, skewness);

    // Assemble timeseries row:
    timeseries(i, 0) = i*dt;
    for (size_t j=0; j<_Ns; j++) {
      timeseries(i, 1+j) = mean(j);
      timeseries(i, 1+_Ns+(_Ns*(_Ns+1))/2+j) = skewness(j);
      for (size_t k=j; k<_Ns; k++) {
        size_t cov_jk = 1+_Ns+j*(_Ns+1)-(j*(j+1))/2 + (k-j);
        timeseries(i, cov_jk) = cov(j, k);
      }
    }

    // Perfrom simulation:
    simulator.run(dt);
  }

  // Done.
  return saveSSATimecourseAnalysis(timeseries, option_parser);
}
