#include "sbml_ssa.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "models/unsupported.hh"

using namespace iNA;

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  // Do the work:
  try
  {

    // Construct SSA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);
    Models::OptimizedSSA model(sbml_model, 30, 1024);
    double dt=0.1;

    // Construct Steady state analysis
    Models::IOSmodel SSEmodel(sbml_model);
    Models::SteadyStateAnalysis<Models::IOSmodel> analysis(SSEmodel);
    Eigen::VectorXd state(SSEmodel.getDimension()); SSEmodel.getInitialState(state);
    analysis.calcSteadyState(state);

    Eigen::VectorXd first;
    Eigen::MatrixXd second;
    Eigen::VectorXd third;
    Eigen::VectorXd fourth;

    SSEmodel.getCentralMoments(state,first,second,third,fourth);

    Eigen::VectorXd moments(2);
    moments(0) = first(2);
    moments(1) = second(2,2);

    std::cerr << moments.transpose() << std::endl;

//    Eigen::VectorXd moments(4);
//    moments(0) = 0;
//    moments(1) = 0.2;
//    moments(2) = 0.05;
//    moments(3) = 0.1;


    Eigen::VectorXd domain(200);

    //double Min = -1; double Max = 1;
    double Min =1.e-7, Max= 1.e-6;

    double x=Min;
    for(int i=0; i<domain.size(); i++, x+=(Max-Min)/domain.size())
        domain(i) = x;

    Models::MaximumEntropyPDF MEP;
    Eigen::VectorXd pdf = MEP.computePDF(domain,moments);

    std::ofstream histfile;
    histfile.open ("MEPhistogram.dat");
    for(int i=0; i<domain.size(); i++)
        histfile << domain(i) << "\t" << pdf(i) << "\t"<<std::endl;
    histfile.close();

    Eigen::VectorXd mean;
    Eigen::MatrixXd variance;

    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());

    //std::map<double,double> hist;

    Models::Histogram<double> hist;

    //std::ofstream histfile;
    model.run(1);
    for(double t=0.; t<100.; t+=dt)
    {

       // std::cout << "t=" << t <<std::endl;

       model.run(dt);
       model.getHistogram(2,hist);

       histfile.open ("histogram.dat");
       std::map<double,double> temp=hist.getNormalized();
       for(std::map<double,double>::iterator it=temp.begin();it!=temp.end();it++)
           histfile << it->first << "\t" << it->second << "\t"<<std::endl;
       histfile.close();

    }


  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << std::endl;
    return -1;
  }
}
