#include "sbml_hybrid.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"
#include "trafo/reversiblereactionconverter.hh"

using namespace iNA;

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  // Ensemble size
  int ens = 1000;

    // Construct hybrid model from SBMLsh model
    Ast::Model model; Parser::Sbmlsh::importModel(model, argv[1]);

    // Expand reversible reactions
    Trafo::ReversibleReactionConverter converter; converter.apply(model);

    // List reactions
    std::cerr << "List of reactions (" << model.numReactions() << "):" << std::endl;
    for(size_t i=0; i<model.numReactions(); i++)
      std::cerr << model.getReaction(i)->getName() << std::endl;
    std::cerr << "====" << std::endl << std::endl;

    // Name species of interest
    std::set<std::string> exS;
    exS.insert("Gon");

    Models::HybridModel hybrid(model,exS);

    size_t nInt = hybrid.numSpecies();
    size_t nExt = hybrid.getExternalModel().numSpecies();

    // Do the work:

      double dt=1;

      // Construct SSA model from SBML model
      Models::HybridSimulator simulator(hybrid, ens, 1.e-8, 1.e-4);

      Models::Histogram<double> hist;

//      std::cout << "Start SSA" << std::endl;
//      simulator.run(100);
//      std::cout << "Transients passed..." << std::endl;

      Eigen::VectorXd mean(model.numSpecies());

      Eigen::VectorXd skew(model.numSpecies());
      Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());

      Eigen::VectorXd istate;
      simulator.getInitial(istate);

      std::vector<Eigen::VectorXd> state(ens,istate);

      std::map<Eigen::VectorXd,Eigen::VectorXd,Models::HybridSimulator::lessVec> condMean;
      std::map<Eigen::VectorXd,Eigen::MatrixXd,Models::HybridSimulator::lessVec> condVar;

      Eigen::MatrixXd mechVar;

      try{
        dt=0.01;
        for(double t=0.; t<10.; t+=dt)
        {

           //Single trajectory
//           std::cout << t << "\t"
//                     << state[0].head(nInt).transpose() << "\t"
//                     << state[0].tail(nExt).transpose() << std::endl;

          std::cout << t << "\t"
                    << (condMean.begin()->second).transpose() << "\t"
                    << (condVar.begin()->second).diagonal().transpose() << "\t"
                    << (mechVar).diagonal().transpose() << std::endl;

           simulator.mechError(state,mechVar);
           simulator.dynError(state, condMean, condVar);
           simulator.runHybrid(state, t, t+dt);

           //simulator.stats(mean,cov,skew);

        }

      }
      catch (iNA::NumericError &err)
      {
          std::cerr << "Numeric error: " << err.what() << std::endl;
      }



}
