#include "sbml_hybridSSA.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"
#include "trafo/reversiblereactionconverter.hh"

using namespace iNA;

int main(int argc, char *argv[])
{

  static const char *optString = "opumstdera";

  static const struct option longOpts[] = {
      { "intfile",        required_argument, NULL, 'o' },
      { "extfile",        required_argument, NULL, 'p' },
      { "trajfile",        required_argument, NULL, 'u' },
      { "model",          required_argument, NULL, 'm' },
      { "soi",            required_argument, NULL, 's' },
      { "tmax",           required_argument, NULL, 't' },
      { "timestep",      required_argument, NULL, 'd' },
      { "ensemble-size",  required_argument, NULL, 'e' },
      { "eps-rel",        required_argument, NULL, 'r' },
      { "eps-abs",        required_argument, NULL, 'a' },
      { NULL, no_argument, NULL, 0 }
  };

  const char *parameterFileName=NULL;
  const char *modelFileName=0;
  const char *intFileName="internal.dat";
  const char *extFileName="external.dat";
  const char *trajFileName="traj.dat";

  double tfinal = 10;
  double dt=0.1;
  double ensSize = 1000;

  int longIndex;
  int opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  while( opt != -1 ) {
      switch( opt ) {

          case 'o':
              intFileName = optarg;
              break;

          case 'p':
              extFileName = optarg;
              break;

          case 'u':
              trajFileName = optarg;
              break;

          case 'm':
              modelFileName = optarg;
              break;

          case 's':
              parameterFileName = optarg;
              break;

          case 't':
              tfinal = atof(optarg);
              break;

          case 'd':
              dt = atof(optarg);
              break;

          case 'e':
              ensSize = atoi(optarg);
              break;

//          case 'r':
//              epsilon_rel = atof(optarg);
//              break;

//          case 'a':
//              epsilon_abs = atof(optarg);
//              break;

          default:
              break;
      }

      opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
  }

    // Construct hybrid model from SBMLsh model
    Ast::Model model; Parser::Sbmlsh::importModel(model, modelFileName);

    // Expand reversible reactions
    Trafo::ReversibleReactionConverter converter; converter.apply(model);

    // Read SoI from file
    std::vector<std::string> signal;
    try
    {
      signal = readSoI(parameterFileName);
    }
    catch (std::exception &err)
    {
      std::cerr << "Argh, again, could not open SoI file: " << parameterFileName << std::endl;
      return -1;
    }

    // Construct hybrid model
    Models::HybridModel hybrid(model,signal);

    hybrid.dump(std::cout);

    size_t nInt = hybrid.numSpecies();
    size_t nExt = hybrid.getExternalModel().numSpecies();

    Models::DualReporter simulator(hybrid, ensSize);

    std::vector<Eigen::MatrixXd> state(ensSize,simulator.getInitial());

    Eigen::VectorXd mean;
    Eigen::MatrixXd mechErr,cov;
    Eigen::MatrixXd fidErr;
    Eigen::MatrixXd transErr;

    Eigen::VectorXd meanEx(nExt);
    Eigen::VectorXd skew(nExt);
    Eigen::MatrixXd covEx(nExt,nExt);

    // Open file for external statistics
    std::ofstream trajFile;
    trajFile.open (trajFileName);

    // Header
    int col=1;
    trajFile << "# [" << col++ <<"]time";
    for(size_t i=0; i<nExt; i++)
        trajFile << "\t [" << col++ <<"]"<< hybrid.getExternalModel().getSpecies(i)->getIdentifier();
    for(size_t i=0; i<nInt; i++)
    {
        trajFile << "\t [" << col++ <<"]R1(" << hybrid.getSpecies(i)->getIdentifier() <<"|u^H)";
        trajFile << "\t [" << col++ <<"]R2(" << hybrid.getSpecies(i)->getIdentifier() <<"|u^H)";
    }
    trajFile << std::endl;

    std::ofstream intFile;
    intFile.open(intFileName);

    col=1;
    // Header
    intFile << "# [" << col++ <<"]time";
    for(size_t i=0; i<hybrid.numSpecies(); i++)
        intFile << "\t [" << col++ <<"]" << hybrid.getSpecies(i)->getIdentifier();
    for(size_t i=0; i<hybrid.numSpecies(); i++)
        intFile << "\t [" << col++ <<"]dR(" << hybrid.getSpecies(i)->getIdentifier() << ")";
    for(size_t i=0; i<hybrid.numSpecies(); i++)
        intFile << "\t [" << col++ <<"]dE(" << hybrid.getSpecies(i)->getIdentifier() << ")";
    for(size_t i=0; i<hybrid.numSpecies(); i++)
        intFile << "\t [" << col++ <<"]mE(" << hybrid.getSpecies(i)->getIdentifier() << ")";
    intFile << std::endl;

    std::ofstream extFile;
    extFile.open(extFileName);

    // Header
    extFile << "# time";
    for(size_t i=0; i<nExt; i++)
        extFile << "\t mean(" << hybrid.getExternalModel().getSpecies(i)->getIdentifier() <<")";
    for(size_t i=0; i<nExt; i++)
        extFile << "\t var(" << hybrid.getExternalModel().getSpecies(i)->getIdentifier() <<")";
    extFile << std::endl;

    // Some wisdom
    double progress=0.;
    std::cout<< "Confucius says '" << Confucius() << "'"<<
                std::endl << "Please wait for progress..." << std::endl;

    try{
      for(double t=0.; t<tfinal; t+=dt)
      {

        // Update progress bar
        if((t/tfinal)>progress){ printProgBar(progress*100); progress+=0.01; }

        //Single trajectory
        trajFile << t;
        for(size_t i=0; i<nExt; i++)
          trajFile << "\t" << state[0].row(0).tail(nExt)(i);
        for(size_t i=0; i<nInt; i++)
          trajFile << "\t" << state[0].row(0)(i) << "\t" << state[0].row(1)(i);
        trajFile << std::endl;

        // External model statistics
        simulator.stats(meanEx, covEx, skew);

        extFile << t;
        for(size_t i=0; i<nExt; i++)
            extFile << "\t" << meanEx(i);
        for(size_t i=0; i<nExt; i++)
            extFile << "\t" << covEx(i,i);
        extFile<<std::endl;

        simulator.mechError(state, mechErr);
        simulator.fidError(state, mean, transErr, fidErr);
        simulator.covariance(state, cov);

        simulator.makeConcentrations(mean);
        simulator.makeConcentrations(mechErr);
        simulator.makeConcentrations(transErr);
        simulator.makeConcentrations(fidErr);

        intFile << t << "\t";
        for(size_t i=0; i<hybrid.numSpecies(); i++)
            intFile << "\t" << mean(i);
        for(size_t i=0; i<hybrid.numSpecies(); i++)
            intFile << "\t" << (transErr).diagonal()(i);
        for(size_t i=0; i<hybrid.numSpecies(); i++)
            intFile << "\t" << (fidErr-mechErr).diagonal()(i);
        for(size_t i=0; i<hybrid.numSpecies(); i++)
            intFile << "\t" << (mechErr).diagonal()(i);
        intFile << std::endl;

        simulator.runHybrid(state, t, t+dt);

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

void split(const std::string& str,
        std::vector<std::string>& tokens)
{

  std::istringstream iss(str);
  std::string word;
  while(iss >> word) tokens.push_back(word);

}

std::vector<std::string> readSoI(const std::string &parameterFile)

{

  // Obtain SoI sets from file
  std::vector<std::string> line;

  std::string str;
  std::fstream file(parameterFile.c_str(),std::ios::in);

  if(!file.is_open()) { std::exception err; throw(err);}

  std::vector<std::string> vecString;
  std::vector<std::string>::iterator vecStringIt;

  //already ahead of header line
  while(!file.eof()) {
    std::getline(file, str);
    vecString.clear();
    //split line
    split(str, vecString);
    size_t idx = 0;
    //for each string in the vector convert it to a number -PROPERLY -    and put it in vecNumbers
    for (vecStringIt = vecString.begin(); vecStringIt != vecString.end(); ++vecStringIt, idx++) {
      line.push_back(*vecStringIt);
    }
  }
  file.close();

  return line;

}


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

  std::cout<< "\r" "[" << bar << "] ";
  std::cout.width( 3 );
  std::cout<< percent << "%     " << std::flush;
}


std::string Confucius()
{

  MersenneTwister mt(time(0));

  // have a look at http://en.wikipedia.org/wiki/Fortune_(Unix)
  // or http://en.wikipedia.org/wiki/Cowsay ... ;-)

  std::vector<std::string> confucius;
  confucius.push_back("Man who stand on hill with mouth open will wait long time for roast duck to drop in.");
  confucius.push_back("It does not matter how slowly you go as long as you do not stop.");
  confucius.push_back("Life is really simple, but we insist on making it complicated.");
  confucius.push_back("When it is obvious that the goals cannot be reached, don't adjust the goals, adjust the action steps.");
  confucius.push_back("Real knowledge is to know the extent of one's ignorance.");
  confucius.push_back("I hear and I forget. I see and I remember. I do and I understand.");

  return confucius[mt.rand()*confucius.size()];

}
