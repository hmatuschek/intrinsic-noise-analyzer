#include "rosenbrock3.hh"

using namespace Fluc::ODE;


const double Rosenbrock3Constants::gamma = 0.5;

const double Rosenbrock3Constants::a21 =  2.0;
const double Rosenbrock3Constants::c2  =  2.0;
const double Rosenbrock3Constants::c21 = -8.0;

const double Rosenbrock3Constants::a31 = 48.0/25.0;
const double Rosenbrock3Constants::a32 = 6.0/25.0;
const double Rosenbrock3Constants::c3  = 3.0/5.0;
const double Rosenbrock3Constants::c31 = 372.0/25.0;
const double Rosenbrock3Constants::c32 = 12.0/5.0;

const double Rosenbrock3Constants::a41 = 48.0/25.0;
const double Rosenbrock3Constants::a42 = 6.0/25.0;
const double Rosenbrock3Constants::c4  = 3.0/5.0;
const double Rosenbrock3Constants::c41 = -112.0/125.0;
const double Rosenbrock3Constants::c42 = -54.0/125.0;
const double Rosenbrock3Constants::c43 = -2.0/5.0;

const double Rosenbrock3Constants::b31  = 186.0/108.0;
const double Rosenbrock3Constants::b32  = 22.0/72.0;
const double Rosenbrock3Constants::b33  = 50.0/216.0;

const double Rosenbrock3Constants::b41  = 38.0/18.0;
const double Rosenbrock3Constants::b42  = 2.0/4.0;
const double Rosenbrock3Constants::b43  = 50.0/216.0;
const double Rosenbrock3Constants::b44  = 250.0/216.0;

const double Rosenbrock3Constants::e1  = 34.0/108.0;
const double Rosenbrock3Constants::e2  = 14.0/72.0;
const double Rosenbrock3Constants::e3  = 0.0;
const double Rosenbrock3Constants::e4  = 250.0/216.0;
