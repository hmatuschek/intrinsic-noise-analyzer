#include "fsp.hh"

namespace Fluc {
namespace Models {

FSP::FSP(libsbml::Model *model) :
  FSPBase(model),
  domain(4*Eigen::VectorXi::Ones(numIndSpecies())),
  cmeM(generateTransitionM(epsilon))
{


    std::cout << "initialized: "<< getDimension();

    Eigen::FullPivLU<Eigen::MatrixXd> LU(cmeM);

    std::cout << LU.kernel();

}

bool FSP::InRange(Eigen::VectorXi indices)

{
   // check if indices are within bounds
    for(int i=0;i<indices.size();i++)
        if( indices(i)>domain(i)) return 0;

    return 1;

}

int FSP::getIndex(Eigen::VectorXi indices)
{

    int last = indices(0);
    for(int i=1;i<indices.size();i++)
        last=last*domain(i)+indices(i);

    return last;

}

Eigen::VectorXi FSP::getState(int idx)
{

    Eigen::VectorXi indices(numIndSpecies());

    for(int i=indices.size()-1;i>=0;i--)
    {
        indices(i) = idx % domain(i);
        idx -= indices(i);
    }

    return indices;

}

size_t FSP::getDimension()
{
    dim = this->domain(0);
    for(int i=1;i<this->domain.size();i++)
        dim *= domain(i);
    return dim;
}

Eigen::MatrixXd FSP::generateTransitionM(double &epsilon)
{

    Eigen::MatrixXd smat(getDimension(),getDimension());

    Eigen::VectorXd prop(numReactions());

    Eigen::VectorXi x;

    epsilon = 0.;

    for(size_t i=0; i<getDimension(); i++)
    {
        Eigen::VectorXi state = getState(i);

        interpreter.run(state,prop);
        smat.coeffRef(i,i) = - prop.sum();
        for(size_t m=0;m<numReactions(); m++)
        {

          if(prop(m)!=0){
          x = state+(this->reduced_stoichiometry.col(m)).cast<int>();
          // get index for state (may be outside of reach).
          if(InRange(x)){
            int ind=getIndex(x);
            smat.coeffRef(i,ind) += prop(m);
          }
          else
            epsilon += prop(m);

          }
        }
    }

    return smat;

}


}
}
