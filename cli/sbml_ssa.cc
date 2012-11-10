#include "sbml_ssa.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"

using namespace iNA;

double pochammer(double a, int n)
{

    if(n==0) return 1;
    else if(n>0) return pochammer(a,n-1)*(a+n-1.);

    return 0.;

}

Eigen::VectorXd gammaPDF(Eigen::VectorXd domain,double alpha, double beta)
{

    Eigen::VectorXd pdf(domain.size());
    for(int i=0; i<domain.size(); i++)
    {
        double x=domain(i);
        pdf(i)=(exp(-(x/beta))*pow(x,(-1+alpha))*pow(beta,(-alpha)))/Eigen::ex2double(GiNaC::tgamma(alpha));
    }
    return pdf;

}

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  double alpha = 1.4985;
  double beta  = 1000;

  // Do the work:
  try
  {

    // Construct SSA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);
    Models::OptimizedSSA model(sbml_model, 30, 1024);
    double dt=0.1;

//    // Construct Steady state analysis
//    Models::IOSmodel SSEmodel(sbml_model);
//    Models::SteadyStateAnalysis<Models::IOSmodel> analysis(SSEmodel);
//    Eigen::VectorXd state(SSEmodel.getDimension()); SSEmodel.getInitialState(state);
//    analysis.calcSteadyState(state);

//    Eigen::VectorXd first;
//    Eigen::MatrixXd second;
//    Eigen::VectorXd third=Eigen::VectorXd::Zero(SSEmodel.numSpecies());
//    Eigen::VectorXd fourth=Eigen::VectorXd::Zero(SSEmodel.numSpecies());

    Eigen::VectorXd domain(100);
    double Min = -20e3, Max = 20.0e3;

    double x=Min;
    for(int i=0; i<domain.size(); i++, x+=(Max-Min)/(domain.size()-1))
        domain(i) = x;

//    SSEmodel.getCentralMoments(state,first,second,third,fourth);

    Eigen::VectorXd moments(2);
    // Moments of gamma distribution
    //for(int r=0; r<moments.size(); r++)
    //        moments(r) = pow(beta,r+1)*pochammer(alpha,r+1);

    moments(0) = 0.76;//1498.5;//first(2);
    moments(1) = 0.8816;//second(2,2);
    //moments(2) = 9.985e8;//third(2);
    //moments(3) = 1.96842e13;//fourth(2);

    std::cout << "Fitting the moments:" << std::endl;
    std::cout << moments.transpose() << std::endl;

    std::cout << "non-central:" << std::endl;
    std::cout << Models::MaximumEntropyPDF::getNonCentralMoments(moments).transpose() << std::endl;

    Models::MaximumEntropyPDF MEP;
    Eigen::VectorXd pdf = MEP.computePDFfromCentralMoments(domain,moments);
    Eigen::VectorXd gamma = gammaPDF(domain,alpha,beta);

    std::ofstream histfile;
    histfile.open ("MEPhistogram.dat");
    for(int i=0; i<domain.size(); i++)
        histfile << domain(i) << "\t" << pdf(i) << "\t" << gamma(i) <<std::endl;
    histfile.close();

    return 0;

    Models::Histogram<double> hist;

    std::cout << "Start SSA" << std::endl;
    model.run(100);
    std::cout << "Transients passed..." << std::endl;

    for(double t=0.; t<1000.; t+=dt)
    {

       //std::cout << "t=" << t <<std::endl;

       model.run(dt);
       model.getHistogram(2,hist);

       histfile.open ("histogram.dat");
       std::map<double,double> temp=hist.getDensity();
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
