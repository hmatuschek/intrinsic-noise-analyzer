#include "dopri5.hh"

using namespace Fluc::ODE;

/// @cond 0
const double Dopri5Constants::c2 = 0.2;
const double Dopri5Constants::c3 = 0.3;
const double Dopri5Constants::c4 = 0.8;
const double Dopri5Constants::c5 = 8.0/9.0;

const double Dopri5Constants::a21 = 0.2;

const double Dopri5Constants::a31 = 3.0/40.0;
const double Dopri5Constants::a32 = 9.0/40.0;

const double Dopri5Constants::a41 = 44.0/45.0;
const double Dopri5Constants::a42 = -56.0/15.0;
const double Dopri5Constants::a43 = 32.0/9.0;

const double Dopri5Constants::a51 = 19372.0/6561.0;
const double Dopri5Constants::a52 = -25360.0/2187.0;
const double Dopri5Constants::a53 = 64448.0/6561.0;
const double Dopri5Constants::a54 = -212.0/729.0;

const double Dopri5Constants::a61 = 9017.0/3168.0;
const double Dopri5Constants::a62 = -355.0/33.0;
const double Dopri5Constants::a63 = 46732.0/5247.0;
const double Dopri5Constants::a64 = 49.0/176.0;
const double Dopri5Constants::a65 = -5103.0/18656.0;

const double Dopri5Constants::a71 = 35.0/384.0;
const double Dopri5Constants::a72 = 0.0;
const double Dopri5Constants::a73 = 500.0/1113.0;
const double Dopri5Constants::a74 = 125.0/192.0;
const double Dopri5Constants::a75 = -2187.0/6784.0;
const double Dopri5Constants::a76 = 11.0/84.0;

const double Dopri5Constants::e1 = 71.0/57600.0;
const double Dopri5Constants::e2 = 0.0;
const double Dopri5Constants::e3 = -71.0/16695.0;
const double Dopri5Constants::e4 = 71.0/1920.0;
const double Dopri5Constants::e5 = -17253.0/339200.0;
const double Dopri5Constants::e6 = 22.0/525.0;
const double Dopri5Constants::e7 = -1.0/40.0;

/// @endcond
