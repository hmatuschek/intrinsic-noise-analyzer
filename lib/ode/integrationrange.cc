#include "integrationrange.hh"

using namespace Fluc::ODE;


IntegrationRange::IntegrationRange(double t0, double t, size_t n)
  : t_start(t0), t_end(t), steps(n)
{
  // Pass...
}


IntegrationRange::IntegrationRange(const IntegrationRange &other)
  : t_start(other.t_start), t_end(other.t_end), steps(other.steps)
{
  // Pass...
}


const IntegrationRange &
IntegrationRange::operator =(const IntegrationRange &other)
{
  this->t_start = other.t_start;
  this->t_end   = other.t_end;
  this->steps   = other.steps;

  return *this;
}


double
IntegrationRange::getStartTime() const
{
  return this->t_start;
}


double
IntegrationRange::getEndTime() const
{
  return this->t_end;
}


double
IntegrationRange::getStepSize() const
{
  return (this->t_end-this->t_start)/this->steps;
}


size_t
IntegrationRange::getSteps() const
{
  return this->steps;
}
