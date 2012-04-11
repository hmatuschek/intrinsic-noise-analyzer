#include "incompletegamma.hh"
#include <cmath>
#include "math.hh"

using namespace Fluc;

double
Fluc::incompletegamma(double a, double x)
{
  double igammaepsilon;
  double ans;
  double ax;
  double c;
  double r;
  double tmp;
  double result;


  igammaepsilon = 0.000000000000001;

  if( x <= 0 || a<=0 )
  {
    result = 0;
    return result;
  }

  if( x > 1 && x>a)
  {
    result = 1-incompletegammac(a, x);
    return result;
  }

  ax = a*std::log(x)-x-lngamma(a, &tmp);
  if( ax < -709.78271289338399 )
  {
    result = 0;
    return result;
  }

  ax = std::exp(ax);
  r = a;
  c = 1;
  ans = 1;

  do
  {
    r = r+1;
    c = c*x/r;
    ans = ans+c;
  } while( c/ans > igammaepsilon);

  result = ans*ax/a;
  return result;
}



double
Fluc::incompletegammac(double a, double x)
{
  double igammaepsilon;
  double igammabignumber;
  double igammabignumberinv;
  double ans;
  double ax;
  double c;
  double yc;
  double r;
  double t;
  double y;
  double z;
  double pk;
  double pkm1;
  double pkm2;
  double qk;
  double qkm1;
  double qkm2;
  double tmp;
  double result;


  igammaepsilon = 0.000000000000001;
  igammabignumber = 4503599627370496.0;
  igammabignumberinv = 2.22044604925031308085*0.0000000000000001;

  if( x <= 0 || a <= 0 )
  {
    result = 1;
    return result;
  }

  if( (x < 1) || (x < a) )
  {
    result = 1-incompletegamma(a, x);
    return result;
  }

  ax = a*std::log(x)-x-lngamma(a, &tmp);
  if( ax < -709.78271289338399 )
  {
    result = 0;
    return result;
  }

  ax = std::exp(ax);
  y = 1-a;
  z = x+y+1;
  c = 0;
  pkm2 = 1;
  qkm2 = x;
  pkm1 = x+1;
  qkm1 = z*x;
  ans = pkm1/qkm1;
  do
  {
    c = c+1;
    y = y+1;
    z = z+2;
    yc = y*c;
    pk = pkm1*z-pkm2*yc;
    qk = qkm1*z-qkm2*yc;

    if( qk != 0 )
    {
      r = pk/qk;
      t = std::fabs((ans-r)/r);
      ans = r;
    }
    else
    {
      t = 1;
    }

    pkm2 = pkm1;
    pkm1 = pk;
    qkm2 = qkm1;
    qkm1 = qk;

    if( std::fabs(pk) > igammabignumber)
    {
      pkm2 = pkm2*igammabignumberinv;
      pkm1 = pkm1*igammabignumberinv;
      qkm2 = qkm2*igammabignumberinv;
      qkm1 = qkm1*igammabignumberinv;
    }
  } while(t > igammaepsilon);

  result = ans*ax;
  return result;
}


double
Fluc::lngamma(double x, double* sgngam)
{
  double a;
  double b;
  double c;
  double p;
  double q;
  double u;
  double w;
  double z;
  int i;
  double logpi;
  double ls2pi;
  double tmp;
  double result;

  *sgngam = 0;
  *sgngam = 1;
  logpi = 1.14472988584940017414;
  ls2pi = 0.91893853320467274178;

  if( x < -34.0 )
  {
    q = -x;
    w = lngamma(q, &tmp);
    p = std::floor(q);
    i = int(std::floor(p+0.5));

    if( i%2==0 )
    {
      *sgngam = -1;
    }
    else
    {
      *sgngam = 1;
    }

    z = q-p;
    if( z > 0.5 )
    {
      p = p+1;
      z = p-q;
    }

    z = q*std::sin(Fluc::constants::pi*z);
    result = logpi-std::log(z)-w;
    return result;
  }

  if( x < 13 )
  {
    z = 1;
    p = 0;
    u = x;

    while(u >= 3)
    {
      p = p-1;
      u = x+p;
      z = z*u;
    }

    while( u < 2 )
    {
      z = z/u;
      p = p+1;
      u = x+p;
    }

    if( z < 0 )
    {
      *sgngam = -1;
      z = -z;
    }
    else
    {
      *sgngam = 1;
    }

    if( u == 2 )
    {
      result = std::log(z);
      return result;
    }

    p = p-2;
    x = x+p;
    b = -1378.25152569120859100;
    b = -38801.6315134637840924+x*b;
    b = -331612.992738871184744+x*b;
    b = -1162370.97492762307383+x*b;
    b = -1721737.00820839662146+x*b;
    b = -853555.664245765465627+x*b;
    c = 1;
    c = -351.815701436523470549+x*c;
    c = -17064.2106651881159223+x*c;
    c = -220528.590553854454839+x*c;
    c = -1139334.44367982507207+x*c;
    c = -2532523.07177582951285+x*c;
    c = -2018891.41433532773231+x*c;
    p = x*b/c;
    result = std::log(z)+p;
    return result;
  }

  q = (x-0.5)*std::log(x) - x + ls2pi;
  if( x > 100000000 )
  {
    result = q;
    return result;
  }

  p = 1/(x*x);
  if( x >= 1000.0 )
  {
    q = q+((7.9365079365079365079365*0.0001*p-2.7777777777777777777778*0.001)*p+0.0833333333333333333333)/x;
  }
  else
  {
    a = 8.11614167470508450300*0.0001;
    a = -5.95061904284301438324*0.0001+p*a;
    a = 7.93650340457716943945*0.0001+p*a;
    a = -2.77777777730099687205*0.001+p*a;
    a = 8.33333333333331927722*0.01+p*a;
    q = q+a/x;
  }

  result = q;
  return result;
}

