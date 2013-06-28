#include "stochasticsimulator.hh"
#include "trafo/constantfolder.hh"
#include "utils/logger.hh"

using namespace iNA;
using namespace iNA::Models;

StochasticSimulator::StochasticSimulator(const Ast::Model &model, int size, int seed, size_t threads,
                                         const std::vector<const GiNaC::symbol *> &parameters)
    : BaseModel(model),
      ParticleNumbersMixin((BaseModel &)(*this)),
      ReasonableModelMixin((BaseModel &)(*this)),
      num_threads(threads),
      rand(1),
      observationMatrix(size,numSpecies()+parameters.size()),
      ics(numSpecies()), Omega(numSpecies()), ensembleSize(size)

{

  // set number of threads
  if(num_threads > OpenMP::getMaxThreads())
    this->num_threads = OpenMP::getMaxThreads();

  // seed random number generators with a single seed
  rand.resize(this->num_threads);
  rand[0].seed(seed);
  for(size_t i=1;i<rand.size();i++)
      rand[i].seed(rand[i-1].rand_int());

  // make index table
  for(size_t i=0; i<this->numSpecies(); i++)
     this->stateIndex.insert(std::make_pair(this->getSpecies(i)->getSymbol(),i));

  // add parameters
  for(size_t i=0; i<parameters.size(); i++)
    this->stateIndex.insert(std::make_pair(*(parameters[i]),this->numSpecies()+i));

  // fold all constants
  Trafo::ConstantFolder constants(*this);
  for(size_t i=0;i<this->propensities.size();i++)
        this->propensities[i] = constants.apply(this->propensities[i]);

  // evaluate initial concentrations & get volumes
  Trafo::InitialValueFolder evICs(*this);

  for(size_t i=0; i<species.size();i++)
  {
     ics(i)=evICs.evaluate(this->species[i]);
     if(ics(i)>0.)
     {
        //
        ics(i)=std::floor( evICs.evaluate(this->species[i]) + 0.5 );
        if(ics(i)==0.)
        {
            InternalError err;
            err << "Could not initiate Stochastic Simulation since initial particle number of species <i>"
                << (this->getSpecies(i)->hasName() ? this->getSpecies(i)->getName() : this->getSpecies(i)->getIdentifier())
                << "</i> evaluated to zero.";
            throw err;
            throw InternalError();
        }
     }
     else if(ics(i)<0.)
     {
         InternalError err;
         err << "Could not initiate Stochastic Simulation since initial particle number of species <i>"
             << (this->getSpecies(i)->hasName() ? this->getSpecies(i)->getName() : this->getSpecies(i)->getIdentifier())
             << "</i> evaluated to a value < 0.";
         throw err;
         throw InternalError();
     }

     this->Omega(i)=evICs.evaluate(this->volumes(i));

     if(this->Omega(i)<=0)
     {
         InternalError err;
         err << "Could not initiate Stochastic Simulation since compartment <i>"
             << (this->getSpecies(i)->getCompartment()->hasName() ? this->getSpecies(i)->getCompartment()->getName() : this->getSpecies(i)->getCompartment()->getIdentifier())
             << "</i> evaluated to non-positive value.";
         throw err;
         throw InternalError();
     }
  }

  Utils::Message msg = LOG_MESSAGE(Utils::Message::INFO);
  msg << "SSA initial copy numbers: ";
  for(size_t i=0; i<numSpecies(); i++)
  {
      if(this->getSpecies(i)->hasName())
        msg<<this->getSpecies(i)->getName();
      else
        msg<<this->getSpecies(i)->getIdentifier();

      msg<<"="<<ics(i)<<" ";
  }
  Utils::Logger::get().log(msg);

  // initialize ensemble
  for(int i=0; i<this->ensembleSize;i++)
  {
     this->observationMatrix.row(i).head(this->numSpecies())=ics;
  }

}

void
StochasticSimulator::reset()

{

    // initialize ensemble
    for(int i=0; i<this->ensembleSize;i++)
    {
       this->observationMatrix.row(i).head(this->numSpecies()) = ics;
    }

}



StochasticSimulator::~StochasticSimulator()

{
    //...
}


void
StochasticSimulator::evaluate(const Eigen::VectorXd &populationVec, Eigen::VectorXd &propensities)
{
  // Assemble substitutions
  GiNaC::exmap substitutions;
  for (size_t i=0; i<numSpecies(); i++) {
    substitutions[getSpecies(i)->getSymbol()] = populationVec[i];
  }

  // then evaluate propensities
  for (size_t i=0; i<this->numReactions(); i++) {
    GiNaC::ex value = GiNaC::evalf(this->propensities[i].subs(substitutions));
    if (! GiNaC::is_a<GiNaC::numeric>(value)) {
      SymbolError err;
      err << "Can not evaluate propensity of reaction " << i
          << ": Propensity not reduced to value. Minimal expression: " << value;
      throw err;
    }
    propensities(i) = Eigen::ex2double(value);
  }
}


void
StochasticSimulator::getState(Eigen::MatrixXd &state)

{

    state = (observationMatrix*this->Omega.asDiagonal().inverse());

}

Eigen::MatrixXd
StochasticSimulator::getState() const

{

    return (observationMatrix.leftCols(this->numSpecies())*this->Omega.asDiagonal().inverse());

}


void
StochasticSimulator::getHistogram(size_t speciesIdx,std::map<double,double> &hist)
{

    //hist.clear();
    for(int sid=0; sid<observationMatrix.rows(); sid++)
    {
        double val = observationMatrix(sid,speciesIdx);
        std::map<double,double>::iterator it = hist.find(val);
        if(it==hist.end())
            hist.insert(std::make_pair<double,double>(val,1.));
        else
            it->second+=1.;
    }
}

void
StochasticSimulator::getHistogram(size_t specIdx, Histogram<double> &hist)
{
    // Divide by volume and add to histogram.
    hist.insert(observationMatrix.col(specIdx) / this->Omega(specIdx));
}

void
StochasticSimulator::stats(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance, Eigen::VectorXd &skewness)

{

  mean = Eigen::VectorXd::Zero(this->numSpecies());
  covariance = Eigen::MatrixXd::Zero(this->numSpecies(),this->numSpecies());
  skewness = Eigen::VectorXd::Zero(this->numSpecies());

  // calculate mean numbers
  for(int ids=0;ids<this->ensembleSize;ids++){
    mean += this->observationMatrix.row(ids).head(this->numSpecies());
  }

  mean /= double(this->ensembleSize);

  int idx=0;

  for(size_t i=0; i<this->numSpecies(); i++)
  {
    for(size_t j=0;j<=i;j++)
    {

      for(int ids=0;ids<this->ensembleSize;ids++){
        covariance(i,j) += this->observationMatrix.row(ids)(i)*this->observationMatrix.row(ids)(j);
      }

      // normalize
      if(this->ensembleSize>1) covariance(i,j) /= (this->ensembleSize-1);

      // substract mean concentration squared
      if(this->ensembleSize>1)
        covariance(i,j) -= double(this->ensembleSize)/double(this->ensembleSize-1)*mean(i)*mean(j);
      else
        covariance(i,j)=0;

      //make concentrations
      covariance(i,j) /= this->Omega(i)*this->Omega(j);
      // symmetrize
      covariance(j,i)=covariance(i,j);

      idx++;

    }
  }

  // a factor which corrects the estimator of the skewness.
  double skewEstFac = 0;
  if (this->ensembleSize>2) skewEstFac = std::sqrt(this->ensembleSize*(this->ensembleSize-1))/(this->ensembleSize-2)/this->ensembleSize;

  for(size_t i=0; i<this->numSpecies(); i++)
  {
    // third moment
    for(int ids=0;ids<this->ensembleSize;ids++)
    {
        double t = this->observationMatrix.row(ids)(i)-mean(i);
        skewness(i) += t*t*t;
    }

   //make concentrations
   mean(i)/=this->Omega(i);

   // compute skewness from third moment
   if(covariance(i,i)>0.)
       skewness(i)=skewEstFac*skewness(i)/covariance(i,i)/sqrt(covariance(i,i))/(this->Omega(i)*this->Omega(i)*this->Omega(i));
   else
       skewness(i)=0.;

  }

}

Eigen::MatrixXd &
StochasticSimulator::getObservationMatrix()
{
  return this->observationMatrix;
}


void
StochasticSimulator::fluxStatistics(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance)

{

    Eigen::VectorXd prop(this->numReactions());

    mean = Eigen::VectorXd::Zero(this->numReactions());
    covariance = Eigen::MatrixXd::Zero(this->numReactions(),this->numReactions());

    // calculate mean fluxes
    for(int ids=0;ids<this->ensembleSize;ids++){
     this->evaluate(this->observationMatrix.row(ids), prop);
     mean += prop;
     covariance += prop*prop.transpose();
    }
    mean /= this->ensembleSize;
    covariance /= (this->ensembleSize-1);

    covariance-=mean*mean.transpose()*(this->ensembleSize)/(this->ensembleSize-1);

    //

}




size_t
StochasticSimulator::size()

{
    return this->ensembleSize;
}


double
StochasticSimulator::uniform()
{
  return this->rand[0].rand();
}


const size_t &StochasticSimulator::numThreads()
{
    return this->num_threads;
}


Ast::Unit StochasticSimulator::getConcentrationUnit() const
{
    return concentrationUnit;
}
