#include "sbml_hybridSSA.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"
#include "trafo/reversiblereactionconverter.hh"

using namespace iNA;


void ensembleAverage(iNA::Models::HybridSSA &simulator, int nExt, double dt, double tmax);

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
    int ensExt = 100000;
    // Timestep
    double dt=0.25;
    // Final time
    double tmax = 15;

    // Construct hybrid model from SBMLsh model
    Ast::Model model; Parser::Sbmlsh::importModel(model, argv[1]);

    // Expand reversible reactions
    Trafo::ReversibleReactionConverter converter; converter.apply(model);

    // Name species of interest
    std::vector<std::string> exS;
    exS.push_back("GeneOn");

    Models::HybridModel hybrid(model,exS);

    size_t nExt = hybrid.getExternalModel().numSpecies();

    Models::HybridSSA simulator(hybrid, ensExt, 4);

    Models::Histogram<double> hist;

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

    std::vector<Eigen::VectorXd> m;
    std::vector<Eigen::MatrixXd> c;

    // Some wisdom
    std::cerr<< "Confucius says '" << Confucius() << "'"<<
                std::endl << "Please wait for progress..." << std::endl;

    double progress=0;
    try{

      ensembleAverage(simulator, nExt, dt, tmax);

    }
    catch (iNA::NumericError &err)
    {
        std::cerr << "Numeric error: " << err.what() << std::endl;
    }

    printProgBar(100);
    std::cerr<<std::endl;

    trajFile.close();

}

void ensembleAverage(iNA::Models::HybridSSA &simulator, int ensExt, double dt, double tmax)

{

  std::vector<Eigen::VectorXd> state(ensExt, simulator.getInitial());


  Eigen::VectorXd meanEx(simulator.getExtModel().numSpecies());
  Eigen::VectorXd skew(simulator.getExtModel().numSpecies());
  Eigen::MatrixXd covEx(simulator.getExtModel().numSpecies(),simulator.getExtModel().numSpecies());

  int num=tmax/dt;

  Eigen::VectorXd mean;
  Eigen::MatrixXd mechErr;
  Eigen::MatrixXd fidErr;
  Eigen::MatrixXd transErr;

  Eigen::MatrixXd cov;

  size_t numSp = simulator.getIntModel().numSpecies();

  std::vector<Eigen::VectorXd> meanE(num,Eigen::VectorXd::Zero(numSp));
  std::vector<Eigen::MatrixXd> mechErrE(num,Eigen::MatrixXd::Zero(numSp,numSp));
  std::vector<Eigen::MatrixXd> fidErrE(num,Eigen::MatrixXd::Zero(numSp,numSp));
  std::vector<Eigen::MatrixXd> transErrE(num,Eigen::MatrixXd::Zero(numSp,numSp));
  std::vector<Eigen::MatrixXd> covE(num,Eigen::MatrixXd::Zero(numSp,numSp));

  std::vector<Eigen::VectorXd> m;
  std::vector<Eigen::MatrixXd> c;

  double progress = 0;

  for(int n=1; n<=1; n++)
  {

   int sid = 0;
   for(double t=0.; t<tmax; t+=dt, sid++)
   {

          // Update progress bar
          if((t/tmax)>progress){ printProgBar(progress*100); progress+=0.01; }

//        simulator.getInternalStats(state[0],0,m,c);
//        //Single trajectory
//        trajFile << t << "\t"
//                  << m[0].head(nInt).transpose() << "\t"
//                  << c[0].diagonal() << "\t"
//                  << state[0].tail(nExt).transpose() << "\t" << std::endl;

     simulator.mechError(state, mechErr);
     simulator.fidError(state, mean, transErr, fidErr);
     simulator.covar(cov);

     meanE[sid]     +=mean;
     mechErrE[sid]  +=mechErr;
     transErrE[sid] +=transErr;
     fidErrE[sid]   +=fidErr;
     covE[sid]      +=cov;

      simulator.runHybrid(state, t, t+dt);

      // External model statistics
      simulator.stats(meanEx,covEx,skew);

//         outfile << t;
//         for(size_t i=0; i<nExt; i++)
//              outfile << "\t" << meanEx(i);
//         for(size_t i=0; i<nExt; i++)
//              outfile << "\t" << covEx(i,i);
//         outfile<<std::endl;

   }

   simulator.resetEnsemble();

   std::ofstream intfile;
   intfile.open ("internal.dat");

   sid=0;
   for(double t=0.; t<tmax; t+=dt, sid++)
   {

       intfile << t << "\t"
                 << meanE[sid].transpose()/n << "\t"
                 << (transErrE[sid]).diagonal().transpose()/n << "\t"
                 << (fidErrE[sid]-mechErrE[sid]).diagonal().transpose()/n << "\t"
                 << (mechErrE[sid]).diagonal().transpose()/n << "\t"
                 << (covE[sid]).diagonal().transpose()/n << "\t"
                 << std::endl;
   }

   intfile.close();


  }



}

