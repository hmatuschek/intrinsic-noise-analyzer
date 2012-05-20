#include "stochasticsimulator.hh"

using namespace Fluc;

using namespace Fluc::Models;

StochasticSimulator::StochasticSimulator(libsbml::Model *model, int size, int seed, size_t threads)
    : BaseModel(model),
      ParticleNumbersMixin((BaseModel &)(*this)),
      ReasonableModelMixin((BaseModel &)(*this)),
      num_threads(threads),
      rand(1),
      interpreter(*this),
      observationMatrix(size,numSpecies()),
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

  // fold all constants
  for(size_t i=0;i<this->propensities.size();i++)
  {
      this->propensities[i]=this->foldConstants(this->propensities[i]);
  }

  // evaluate initial concentrations & get volumes
  Ast::EvaluateInitialValue evICs(*this);

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
            err << "Could not initiate Stochastic Simulation since initial particle number of species " << this->getSpecies(i)->getName();
            err << "evaluated to zero.";
            throw err;
            throw InternalError();
        }
     }
     else if(ics(i)<0.)
     {
         InternalError err;
         err << "Could not initiate Stochastic Simulation since initial particle number of species " << this->getSpecies(i)->getName();
         err << "evaluated to a value < 0.";
         throw err;
         throw InternalError();
     }

     this->Omega(i)=evICs.evaluate(this->volumes(i));
  }

  std::cerr<<"Initial conditions:"<<std::endl;
  std::cerr<<ics.transpose()<<std::endl<<std::endl;

  // initialize ensemble
  for(int i=0; i<this->ensembleSize;i++)
  {
     this->observationMatrix.row(i)=ics;
  }

}

StochasticSimulator::~StochasticSimulator()

{
    //...
}


void
StochasticSimulator::evaluate(const Eigen::VectorXd &populationVec, Eigen::VectorXd &propensities)
{
  // first update values for state:
  this->interpreter.setValues(populationVec);

  // then evaluate propensities
  for (size_t i=0; i<this->numReactions(); i++)
  {
    propensities(i) = this->interpreter.evaluate(this->propensities[i]);
  }
}


void
StochasticSimulator::getState(Eigen::MatrixXd &state)

{

    state = (observationMatrix*this->Omega.asDiagonal().inverse());

}


void
StochasticSimulator::getHistogram(size_t speciesId,std::map<double,double> &hist)
{

    hist.clear();
    for(int sid=0; sid<observationMatrix.rows(); sid++)
    {
        double val = observationMatrix(sid,speciesId);
        std::map<double,double>::iterator it = hist.find(val);
        if(it==hist.end())
            hist.insert(std::make_pair<double,double>(val,1.));
        else
            it->second+=1.;
    }
}


void
StochasticSimulator::stats(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance, Eigen::VectorXd &skewness)

{

  mean = Eigen::VectorXd::Zero(this->numSpecies());
  covariance = Eigen::MatrixXd::Zero(this->numSpecies(),this->numSpecies());
  skewness = Eigen::VectorXd::Zero(this->numSpecies());

  // calculate mean numbers
  for(int ids=0;ids<this->ensembleSize;ids++){
    mean += this->observationMatrix.row(ids);
  }


  mean /= this->ensembleSize;

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
   skewness(i)=skewEstFac*skewness(i)/covariance(i)/sqrt(covariance(i))/(this->Omega(i)*this->Omega(i)*this->Omega(i));

  }

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
