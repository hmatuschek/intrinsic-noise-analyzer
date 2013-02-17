#include "sbml_pscan.hh"
#include "../unsupported/unsupported.hh"
#include <parser/sbml/sbml.hh>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <string>
#include <sstream>
#include <fstream>



using namespace iNA;

void split(const std::string& str,
        std::vector<std::string>& tokens)
{

  std::istringstream iss(str);
  std::string word;
  while(iss >> word) tokens.push_back(word);

}

std::vector<iNA::Models::ParameterSet> readParametersFromFile(const std::string &parameterFile)

{


  // Obtain parameter sets from file
  std::vector<iNA::Models::ParameterSet> parameters;

  std::string str;
  std::fstream file(parameterFile.c_str(),std::ios::in);

  if(!file.is_open()) { std::exception err; throw(err);}

  //Get the first line
  std::vector<std::string> header;
  if (!file.eof()) {
    std::getline(file, str);
    split(str,header);
  }

  std::vector<std::string> vecString;
  std::vector<std::string>::iterator vecStringIt;

  iNA::Models::ParameterSet line;

  //already ahead of header line
  while(!file.eof()) {
    std::getline(file, str);
    line.clear();
    vecString.clear();
    //split the line
    split(str, vecString);
    size_t idx = 0;
    //for each string in the vector convert it to a number -PROPERLY -    and put it in vecNumbers
    for (vecStringIt = vecString.begin(); vecStringIt != vecString.end(); ++vecStringIt, idx++) {
      std::istringstream i(*vecStringIt);
      double value; i >> value;
      line.insert(std::pair<std::string,double>(header[idx],value));
    }
    parameters.push_back(line);
  }
  file.close();

  return parameters;

}

int main(int argc, char *argv[])
{


      const char *parameterFile=NULL;
      const char *modelFile=0;

      double tfinal = 1;
      double dt = 0.01;
      double epsilon_abs=1e-10;
      double epsilon_rel=1.e-4;

      static const char *optString = "mptdra";

      static const struct option longOpts[] = {
          { "model", required_argument, NULL, 'm' },
          { "parameters", required_argument, NULL, 'p' },
          { "tmax", required_argument, NULL, 't' },
          { "timestep", required_argument, NULL, 'd' },
          { "eps-rel", required_argument, NULL, 'r' },
          { "eps-abs", required_argument, NULL, 'a' },
          { NULL, no_argument, NULL, 0 }
      };


      int longIndex;
      int opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
      while( opt != -1 ) {
          switch( opt ) {

              case 'm':
                  modelFile = optarg;
                  break;

              case 'p':
                  parameterFile = optarg;
                  break;

              case 't':
                  tfinal = atof(optarg);
                  break;

              case 'd':
                  dt = atof(optarg);
                  break;

              case 'r':
                  epsilon_rel = atof(optarg);
                  break;

              case 'a':
                  epsilon_abs = atof(optarg);
                  break;

              default:
                  break;
          }

          opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
      }

    std::vector<iNA::Models::ParameterSet> parameters;
    try
    {
      parameters = readParametersFromFile(parameterFile);
    }
    catch (std::exception &err)
    {
      std::cerr << "Argh, again, could not open parameter file: " << parameterFile << std::endl;
      return -1;
    }

    // Construct LNA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, modelFile);

    Models::IOSmodel model(sbml_model);
    Models::TimeCourseParameterScan<Models::IOSmodel,Eval::bci::Engine<Eigen::VectorXd>, Eval::bci::Engine<Eigen::MatrixXd> > pscan(model);

    // perform the parameter scan
    std::vector<Eigen::VectorXd> result;
    pscan.parameterScan(parameters,tfinal,dt,epsilon_abs,epsilon_rel,1);




}
