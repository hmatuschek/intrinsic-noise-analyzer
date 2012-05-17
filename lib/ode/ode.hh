/**
 * @defgroup ode Integrators for Ordinary Differential Equations
 *
 * This group collects some classes implementing integration methods of ordinary differential
 * equations (ODEs).
 *
 * There are 3 simple, fixed step-size methods: @c EulerDriver, @c SemiImplicitEuler and
 * @c RungeKutta4.
 *
 * There are also some steppers with adaptive step-size control: @c RKF45, @c Dopri5Stepper,
 * @c Dopri853Stepper, @c Rosenbrock3TimeInd and @c Rosenbrock4TimeInd.
 *
 * There is also a convenience class (@c ODEModel) that assembles an ODE system from a vector of
 * GiNaC symbols and GiNaC expressions, it also automatically derives the Jacobian from the ODE
 * for the (semi-) implicit methods.
 */

#ifndef __FLUC_ODE_HH__
#define __FLUC_ODE_HH__

#include "integrationrange.hh"
#include "odemodel.hh"
#include "stepper.hh"

#include "eulerdriver.hh"
#include "rungekutta4.hh"
#include "rkf45.hh"
#include "dopri5.hh"
#include "dopri853.hh"
#include "lsodadriver.hh"

#include "semiimpliciteuler.hh"
#include "rosenbrock3.hh"
#include "rosenbrock4.hh"

#endif // ODE_HH
