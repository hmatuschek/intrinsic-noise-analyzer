#include "gillespieSSA.hh"
#include "openmp.hh"

namespace Fluc {
namespace Models {


GillespieSSA::GillespieSSA(libsbml::Model *model, int size, int seed, size_t opt_level, size_t num_threads) :
    StochasticSimulator(model, size, seed, num_threads),
    ConstantStoichiometryMixin((BaseModel &)(*this)),
    interpreter(this->numThreads()), prop( this->numThreads(), Eigen::VectorXd::Zero(this->numReactions()) )
{
    Evaluate::bci::Compiler<Eigen::VectorXd> compiler(this->stateIndex);
    compiler.setCode(&bytecode);

    // and compile propensities for byte code evaluation
    for(size_t i=0; i<this->numReactions(); i++)
        compiler.compileExpressionAndStore(this->propensities[i],i);

    // optimize and store
    compiler.finalize(opt_level);
    for(size_t i=0; i<this->numThreads(); i++) {
        this->interpreter[i].setCode(&bytecode);
    }
}


void
GillespieSSA::run(double step)
{
    // initialization
    double propensitySum;	// sum of propensities
    double t,tau;			// time between reactions
    size_t reaction;		// reaction number selected


    #pragma omp parallel for if(this->numThreads()>1) num_threads(this->numThreads()) schedule(dynamic) private(propensitySum,tau,t,reaction)
    for(int sid=0;sid<this->ensembleSize;sid++)
    {
        t=0;
        while(t < step)
        {
            // update propensity vector
            interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid), this->prop[OpenMP::getThreadNum()]);

            // evaluate propensity sum
            propensitySum = this->prop[OpenMP::getThreadNum()].sum();

            // sample tau
            if(propensitySum > 0) {
                tau = -std::log(this->rand[OpenMP::getThreadNum()].rand()) / propensitySum;
            } else {
                break;
            }


            // select reaction
            double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
            double sum = this->prop[OpenMP::getThreadNum()](0);
            reaction = 0;
            while(sum < r)
                sum += this->prop[OpenMP::getThreadNum()](++reaction);

            // update chemical species
            this->observationMatrix.row(sid)+=this->stoichiometry.col(reaction);

            // time
            t += tau;
        } //end time step loop
    } // end ensemble loop
}

}
}
