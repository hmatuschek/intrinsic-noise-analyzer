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

    Eigen::VectorXd domain(1000);
    double Min = 0, Max = 100;

    double x=Min;
    for(int i=0; i<domain.size(); i++, x+=(Max-Min)/(domain.size()-1))
        domain(i) = x;

//    SSEmodel.getCentralMoments(state,first,second,third,fourth);

    Eigen::VectorXd moments(11);
    // Moments of gamma distribution
    //for(int r=0; r<moments.size(); r++)
    //   moments(r) = pow(beta,r)*pochammer(alpha,r);

//    moments(0) = 1498.5;//first(2);
//    moments(1) = 1.4985e6;//second(2,2);
//    moments(2) = 9.985e8;//third(2);
//    moments(3) = 6.73651e12;//fourth(2);

//    moments << 5994., 5.994e6, 1.1982e10, 1.3473e14, 4.98751e17, 1.86938e21;

    //moments<<1498.5, 1.4985e6, 2.9955e9, 1.57185e13, 3.69076e16,
    //         4.54883e19, 2.61582e22, 6.53629e24;
        //, 1.57697e25, 1.33975e29, 1.27212e33,
                  //1.33505e37;
    //Schloegl set 1
    moments << 1,34.1526,1232.75,45501.2,1.70699e6,6.49222e7,2.49923e9,9.72636e10,5.32507e11,1.97972e13,7.45706e14;
    //Schloegl set 2
    //moments << 1,22.8441,723.839,24724.3,872717.,3.14956e7,1.15697e9,4.31486e10,1.6308e12,6.23778e13;
    //Schloegl set 3
    //moments << 1,12.6268,321.046,9972.9,331565.,1.1423e7,4.03455e8,1.45351e10,5.32507e11,1.97972e13;
    //moments = Models::MaximumEntropyPDF::getNonCentralMoments(moments);

    std::cout << "Fitting the moments:" << std::endl;
    std::cout << moments.transpose() << std::endl;

    std::cout << "non-central:" << std::endl;
    std::cout << Models::MaximumEntropyPDF::getNonCentralMoments(moments).transpose() << std::endl;

    Models::MaximumEntropyPDF MEP;
    Eigen::VectorXd pdf = MEP.computePDF2(domain,moments);
    Eigen::VectorXd gamma = gammaPDF(domain,alpha,beta);

    std::ofstream histfile;
    histfile.open ("MEPhistogram.dat");
    for(int i=0; i<domain.size(); i++)
        histfile << domain(i) << " " << pdf(i) //<< "\t" << gamma(i)
                 <<std::endl;
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
