#include "sbml_hybridSSA.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"
#include "trafo/reversiblereactionconverter.hh"

using namespace iNA;

void printProgBar( int percent ){
  std::string bar;

  for(int i = 0; i < 50; i++){
    if( i < (percent/2)){
      bar.replace(i,1,"=");
    }else if( i == (percent/2)){
      bar.replace(i,1,">");
    }else{
      bar.replace(i,1," ");
    }
  }

  std::cerr<< "\r" "[" << bar << "] ";
  std::cerr.width( 3 );
  std::cerr<< percent << "%     " << std::flush;
}


std::string Confucius()
{

  MersenneTwister mt(time(0));

  std::vector<std::string> confucius;
  confucius.push_back("Man who stand on hill with mouth open will wait long time for roast duck to drop in.");
  confucius.push_back("It does not matter how slowly you go as long as you do not stop.");
  confucius.push_back("Life is really simple, but we insist on making it complicated.");
  confucius.push_back("When it is obvious that the goals cannot be reached, don't adjust the goals, adjust the action steps.");
  confucius.push_back("Real knowledge is to know the extent of one's ignorance.");
  confucius.push_back("I hear and I forget. I see and I remember. I do and I understand.");

  return confucius[mt.rand()*confucius.size()];

}

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }


    // Ensemble size
    int ens = 50;
    // Timestep
    double dt=0.1;
    // Final time
    double tmax = 10;


    // Construct hybrid model from SBMLsh model
    Ast::Model model; Parser::Sbmlsh::importModel(model, argv[1]);

    // Expand reversible reactions
    Trafo::ReversibleReactionConverter converter; converter.apply(model);

    // Name species of interest
    std::set<std::string> exS;
    exS.insert("GeneOn");

    Models::HybridModel hybrid(model,exS);

    size_t nInt = hybrid.numSpecies();
    size_t nExt = hybrid.getExternalModel().numSpecies();

    Models::HybridSSA simulator(hybrid, ens, 1.e-8, 1.e-4);

    Models::Histogram<double> hist;

    Eigen::VectorXd istate;
    simulator.getInitial(istate);

    std::vector<Eigen::VectorXd> state(ens,istate);

    Eigen::VectorXd mean;
    Eigen::MatrixXd mechErr;
    Eigen::MatrixXd dynErr;
    Eigen::MatrixXd transErr;

    Eigen::VectorXd meanEx(nExt);
    Eigen::VectorXd skew(nExt);
    Eigen::MatrixXd covEx(nExt,nExt);

    // Open file for external statistics
    std::ofstream trajFile;
    trajFile.open ("traj.dat");

    std::ofstream outfile;
    outfile.open ("external.dat");

    outfile << "# time";
    for(size_t i=0; i<nExt; i++)
        outfile << "\t mean(" << hybrid.getExternalModel().getSpecies(i)->getIdentifier() <<")";
    for(size_t i=0; i<nExt; i++)
        outfile << "\t var(" << hybrid.getExternalModel().getSpecies(i)->getIdentifier() <<")";
    outfile << std::endl;

    // Some wisdom
    double progress=0.;
    std::cerr<< "Confucius says '" << Confucius() << "'"<<
                std::endl << "Please wait for progress..." << std::endl;
    try{
      for(double t=0.; t<tmax; t+=dt)
      {

        // Update progress bar
        if((t/tmax)>progress){ printProgBar(progress*100); progress+=0.01; }

        //Single trajectory
        trajFile << t << "\t"
                  << state[0].head(nInt).transpose() << "\t"
                  << state[0].tail(nExt).transpose() << std::endl;

        simulator.mechError(state, mechErr);
        simulator.dynError(state, dynErr);
        simulator.transError(state,mean,transErr);

        std::cout << t << "\t"
                  << mean.transpose() << "\t"
                  << (transErr).diagonal().transpose() << "\t"
                  << (dynErr).diagonal().transpose() << "\t"
                  << (mechErr).diagonal().transpose() << "\t"
                  << std::endl;

         simulator.runHybrid(state, t, t+dt);

         // External model statistics
         simulator.stats(meanEx,covEx,skew);

         outfile << t;
         for(size_t i=0; i<nExt; i++)
              outfile << "\t" << meanEx(i);
         for(size_t i=0; i<nExt; i++)
              outfile << "\t" << covEx(i,i);
         outfile<<std::endl;


      }

    }
    catch (iNA::NumericError &err)
    {
        std::cerr << "Numeric error: " << err.what() << std::endl;
    }

    printProgBar(100);
    std::cerr<<std::endl;

    trajFile.close();



}
