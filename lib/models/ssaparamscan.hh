#ifndef __INA_SSASTEADYSTATE_HH
#define __INA_SSASTEADYSTATE_HH

#include "optimizedSSA.hh"
#include "ssebasemodel.hh"


namespace iNA {
namespace Models {

class SSAparamScan
{
protected:

    Ast::Model& sbml_model;
    double transientTime;

    std::vector<Models::OptimizedSSA*> models;

    size_t _n;

    Eigen::MatrixXd _mean;
    Eigen::MatrixXd _cov;

    // Something to store the result
    std::vector<Eigen::VectorXd> state;

public:

    SSAparamScan(Ast::Model &model, std::vector<ParameterSet> &parameterSets, double transientTime, size_t numThreads=OpenMP::getMaxThreads())
        : sbml_model(model), transientTime(transientTime),
          models(parameterSets.size()),
          _n(1), state(numThreads)
    {

        _mean = Eigen::MatrixXd::Zero(parameterSets.size(),sbml_model.numSpecies());
        _cov  = Eigen::MatrixXd::Zero(parameterSets.size(),sbml_model.numSpecies()*(sbml_model.numSpecies()+1)/2);

        Ast::Model *mod;
        // Iterate over all parameter sets
        for(size_t j = 0; j < parameterSets.size(); j++)
        {

            // Copy model
            mod = new Ast::Model(sbml_model);
            // Apply parameter set
            for(ParameterSet::iterator it=parameterSets[j].begin(); it!=parameterSets[j].end(); it++)
                mod->getParameter((*it).first)->setValue((*it).second);
            // Create SSA
            models[j] = new Models::OptimizedSSA(*mod, 1, 1024);
            // Advance state
            models[j]->run(transientTime);

            delete mod;

        }

    }

    void run(double timestep)
    {

        size_t numThreads = state.size();

        // Do the average average over time
#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic)
        for(size_t i = 0; i < models.size(); i++)
        {
                models[i]->run(timestep);
                state[OpenMP::getThreadNum()] = models[i]->getState().row(0);
                size_t idx =0;
                for(size_t j=0; j<models[i]->numSpecies(); j++)
                {
                    _mean(i,j) =( _mean(i,j)*(_n-1) + state[OpenMP::getThreadNum()](j) )/_n;

                    for (size_t k=0; k<=j; k++)
                    {
                        _cov(i,idx) = ( _cov(i,idx)*(_n-1) + (state[OpenMP::getThreadNum()](k)-_mean(i,k))*(state[OpenMP::getThreadNum()](j)-_mean(i,j)) )/_n;
                        idx++;
                    }

                }
         }

        // Increment internal counter
        _n++;

    }


    const Eigen::MatrixXd& getMean() const
    {
        return this->_mean;
    }


    const Eigen::MatrixXd& getCovariance() const
    {
        return this->_cov;
    }

};



}}


#endif // SSASTEADYSTATE_HH
