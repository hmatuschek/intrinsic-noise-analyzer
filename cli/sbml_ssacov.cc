#include "sbml_ssa.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"

using namespace iNA;

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

    // Construct SSA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);
    Models::OptimizedSSA model(sbml_model, 1, 1024);

    double min=0.01, max=1.;
    size_t num = 50;

    std::vector<Models::OptimizedSSA*> models(num);
    Eigen::MatrixXd meanVec=Eigen::MatrixXd::Zero(num,model.numSpecies());
    Eigen::MatrixXd covVec=Eigen::MatrixXd::Zero(num,model.numSpecies());

    int i=0;
    for(double p=min; p<=max; p+=(max-min)/num,i++)
    {
        sbml_model.getParameter(5)->setValue(p);
        models[i] = new Models::OptimizedSSA(sbml_model, 1, 1024);
        models[i]->run(3);
    }

    Eigen::VectorXd mean(model.numSpecies());
    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());
    Eigen::VectorXd skew(model.numSpecies());

    std::cout<<"Parameter: "<<model.getParameter(5)->getIdentifier()<<std::endl;

    double dt=0.1;
    int n=0;
    for(double t=0;t<100000; t+=dt, n++)
    {

        // average
        int i=0;
        for(double p=min; p<=max; p+=(max-min)/num, i++)
        {
            models[i]->run(dt);
            models[i]->stats(mean,cov,skew);
            for(size_t j=0; j<models[i]->numSpecies(); j++)
            {
                covVec(i,j)+=mean(j)*mean(j);
                meanVec(i,j)+=mean(j);
            }
        }

        std::ofstream outfile;
        outfile.open ("cov.dat");

        // write to file
        i=0;
        for(double p=min; p<=max; p+=(max-min)/num, i++)
        {
            outfile << p << "\t";
            for(size_t j=0; j<models[i]->numSpecies(); j++)
            {
                double m = meanVec(i,j)/n;
                double cc= covVec(i,j)/n-m*m;
                outfile << std::sqrt(cc)/m << "\t";
            }

            outfile << std::endl;

        }
        outfile.close();



    }


    std::cout << "Finished..." << std::endl;



}
