#include "generaltaskconfig.hh"
#include "openmp.hh"


/* ******************************************************************************************** *
 * Implementation of GeneralTaskConfig
 * ******************************************************************************************** */
GeneralTaskConfig::GeneralTaskConfig(QObject *parent) :
  QObject(parent)
{
  // Pass...
}


GeneralTaskConfig::~GeneralTaskConfig()
{
  // Pass...
}



/* ******************************************************************************************** *
 * Implementation of ModelSelectionTaskConfig
 * ******************************************************************************************** */
ModelSelectionTaskConfig::ModelSelectionTaskConfig()
  : document(0)
{
  // Pass...
}

ModelSelectionTaskConfig::ModelSelectionTaskConfig(const ModelSelectionTaskConfig &other)
  : document(other.document)
{
  // Pass...
}

ModelSelectionTaskConfig::~ModelSelectionTaskConfig()
{
  // Pass...
}

void
ModelSelectionTaskConfig::setModelDocument(DocumentItem *document)
{
  this->document = document;
}

DocumentItem *
ModelSelectionTaskConfig::getModelDocument()
{
  return this->document;
}


/* ******************************************************************************************** *
 * Implementation of SpeciesSelectionTaskConfig
 * ******************************************************************************************** */
SpeciesSelectionTaskConfig::SpeciesSelectionTaskConfig()
  : selected_species()
{
  // Pass...
}

SpeciesSelectionTaskConfig::SpeciesSelectionTaskConfig(const SpeciesSelectionTaskConfig &other)
  : selected_species(other.selected_species)
{
  // Pass...
}

SpeciesSelectionTaskConfig::~SpeciesSelectionTaskConfig()
{
  // Pass...
}

const QStringList &
SpeciesSelectionTaskConfig::getSelectedSpecies() const
{
  return this->selected_species;
}

size_t
SpeciesSelectionTaskConfig::getNumSpecies() const
{
  return this->selected_species.size();
}

void
SpeciesSelectionTaskConfig::setSelectedSpecies(const QStringList &list)
{
  this->selected_species = list;
}



/* ******************************************************************************************** *
 * Implementation of ODEIntTaskConfig
 * ******************************************************************************************** */
ODEIntTaskConfig::ODEIntTaskConfig()
  : integrator((Integrator)0), integration_range(0,0,0), intermediate_steps(0),
    num_threads(OpenMP::getMaxThreads()), opt_level(1), epsilon_abs(0), epsilon_rel(0)
{
  // Pass...
}

ODEIntTaskConfig::ODEIntTaskConfig(const ODEIntTaskConfig &other)
  : integrator(other.integrator), integration_range(other.integration_range),
    intermediate_steps(other.intermediate_steps), num_threads(other.num_threads),
    opt_level(other.opt_level), epsilon_abs(other.epsilon_abs), epsilon_rel(other.epsilon_rel)
{
  // Pass...
}

ODEIntTaskConfig::~ODEIntTaskConfig()
{
  // Pass...
}

ODEIntTaskConfig::Integrator
ODEIntTaskConfig::getIntegrator() const
{
  return this->integrator;
}

void
ODEIntTaskConfig::setIntegrator(Integrator integrator)
{
  this->integrator = integrator;
}

const Fluc::ODE::IntegrationRange &
ODEIntTaskConfig::getIntegrationRange() const
{
  return this->integration_range;
}

void
ODEIntTaskConfig::setIntegrationRange(const Fluc::ODE::IntegrationRange &range)
{
  this->integration_range = range;
}

void
ODEIntTaskConfig::setIntegrationRange(double t_end, size_t steps)
{
  this->integration_range = Fluc::ODE::IntegrationRange(0, t_end, steps);
}

size_t
ODEIntTaskConfig::getIntermediateSteps() const
{
  return this->intermediate_steps;
}

void
ODEIntTaskConfig::setIntermediateSteps(size_t steps)
{
  this->intermediate_steps = steps;
}

size_t
ODEIntTaskConfig::getOptLevel() const
{
  return this->opt_level;
}

void
ODEIntTaskConfig::setOptLevel(size_t level)
{
  this->opt_level = level;
}

size_t
ODEIntTaskConfig::getNumThreads() const
{
  return this->num_threads;
}

void
ODEIntTaskConfig::setNumThreads(size_t num)
{
  this->num_threads = num;
}

double
ODEIntTaskConfig::getEpsilonAbs() const
{
  return this->epsilon_abs;
}

void
ODEIntTaskConfig::setEpsilonAbs(double epsilon)
{
  this->epsilon_abs = epsilon;
}

double
ODEIntTaskConfig::getEpsilonRel() const
{
  return this->epsilon_rel;
}

void
ODEIntTaskConfig::setEpsilonRel(double epsilon)
{
  this->epsilon_rel = epsilon;
}

void
ODEIntTaskConfig::setEpsilon(double abs, double rel)
{
  this->epsilon_abs = abs;
  this->epsilon_rel = rel;
}

