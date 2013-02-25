#ifndef __CLI_SBML_FFT_HH__
#define __CLI_SBML_FFT_HH__

#include <iostream>
#include "ina.hh"
#include "unsupported.hh"
#include <sbml/SBMLTypes.h>
#include <ginac/ginac.h>

namespace iNA {
namespace Models {

class LNASpec
    : public SteadyStateAnalysis<LNAmodel>
{

    InitialConditions ICs;

    Eigen::MatrixXd Jac;
    Eigen::MatrixXd BB;

public:

    LNASpec(LNAmodel &model)
        : SteadyStateAnalysis<LNAmodel>(model),
          ICs(model),
          BB(model.numIndSpecies(),model.numIndSpecies())
    {

        Eigen::VectorXd x(model.numIndSpecies());

        // Collect all the values of constant parameters except variable parameters
        Trafo::ConstantFolder constants(model, Trafo::Filter::ALL_CONST);

        this->calcConcentrations(x);

        GiNaC::exmap subs;

        for(size_t i=0; i<model.numIndSpecies(); i++)
            subs.insert(std::pair<GiNaC::symbol,GiNaC::ex>(model.getREvar(i),x(i)));

        ParameterFolder params(subs);

        Jac = Eigen::ex2double(params.apply(ICs.apply(constants.apply(model.getJacobian()))));
        BB = Eigen::ex2double(params.apply(ICs.apply(constants.apply(model.getDiffusionMatrix()))));

    }

    Eigen::MatrixXd getSpectrum(const Eigen::VectorXd &freq)

    {

        Eigen::MatrixXd spec(freq.size(),this->sseModel.numSpecies());
        Eigen::MatrixXcd F;

        for(int i=0; i<freq.size(); i++)
        {

            std::complex<double> omega(0,2*constants::pi*freq(i));
            F = ICs.getLinkCMatrix().cast< std::complex<double> >()*(Jac.cast< std::complex<double> >()-omega*Eigen::MatrixXcd::Identity(this->sseModel.numIndSpecies(),this->sseModel.numIndSpecies())).inverse();
            spec.row(i) =  (F*BB.cast< std::complex<double> >()*F.adjoint()).diagonal().array().abs();

        }

        return spec;





    }




};

}}

#endif
