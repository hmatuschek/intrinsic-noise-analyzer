#include "rkf45.hh"

using namespace Fluc::ODE;

/// @cond 0
/// exclude from docs

const double RKF45Constants::a21 = 1.0/4.0;
const double RKF45Constants::a31 = 3.0/32.0;
const double RKF45Constants::a32 = 9.0/32.0;
const double RKF45Constants::a41 = 1932.0/2197.0;
const double RKF45Constants::a42 = -7200.0/2197.0;
const double RKF45Constants::a43 = 7296.0/2197.0;
const double RKF45Constants::a51 = 439.0/216.0;
const double RKF45Constants::a52 = -8.0;
const double RKF45Constants::a53 = 3680.0/513.0;
const double RKF45Constants::a54 = -845.0/4104.0;
const double RKF45Constants::a61 = 8.0/25.0;
const double RKF45Constants::a62 = 2.0;
const double RKF45Constants::a63 = -3544.0/2565.0;
const double RKF45Constants::a64 = 1859.0/4104.0;
const double RKF45Constants::a65 = -11.0/40.0;

const double RKF45Constants::c2  = 1.0/4.0;
const double RKF45Constants::c3  = 3.0/8.0;
const double RKF45Constants::c4  = 12.0/13.0;
const double RKF45Constants::c5  = 1.0;
const double RKF45Constants::c6  = 1.0/2.0;

const double RKF45Constants::b41  = 16.0/135.0;
const double RKF45Constants::b42  = 0.0;
const double RKF45Constants::b43  = 6656.0/12825.0;
const double RKF45Constants::b44  = 28561.0/56430.0;
const double RKF45Constants::b45  = -9.0/50.0;
const double RKF45Constants::b46  = 2.0/55.0;

const double RKF45Constants::b31  = 25.0/216.0;
const double RKF45Constants::b32  = 0.0;
const double RKF45Constants::b33  = 1408.0/2565.0;
const double RKF45Constants::b34  = 2197.0/4101.0;
const double RKF45Constants::b35  = -1.0/5.0;
const double RKF45Constants::b36  = 0.0;

/// @endcond
