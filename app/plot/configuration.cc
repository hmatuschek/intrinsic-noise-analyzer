#include "configuration.hh"
#include "../application.hh"

using namespace Plot;


Plot::Configuration *Plot::Configuration::global_config = 0;



/* ********************************************************************************************* *
 * Implementation of Configuration
 * ********************************************************************************************* */
Plot::Configuration::Configuration()
  : schemes()
{
  // Assemble DisplayScheme from Application wide config:
  QFont title_font(Application::getApp()->getH2Font());
  QFont axis_label_font(Application::getApp()->getH3Font());
  QFont legent_font(Application::getApp()->getTextFont());
  QFont ticks_font(Application::getApp()->getTextFont());
  QPen  default_pen; default_pen.setWidth(2);
  this->schemes.push_back(ConfigScheme(title_font, axis_label_font, ticks_font, legent_font, default_pen));

  // Assemble print scheme from display-scheme:
  ticks_font.setBold(true);
  legent_font.setBold(true);
  default_pen.setWidth(4);
  this->schemes.push_back(ConfigScheme(title_font, axis_label_font, ticks_font, legent_font, default_pen));
}


const Plot::ConfigScheme &
Plot::Configuration::getScheme(Scheme scheme)
{
  return this->schemes.at(int(scheme));
}


Plot::Configuration *
Plot::Configuration::getConfig()
{
  if (0 == Plot::Configuration::global_config)
  {
    Plot::Configuration::global_config = new Plot::Configuration();
  }

  return Plot::Configuration::global_config;
}




/* ********************************************************************************************* *
 * Implementation of ConfigScheme
 * ********************************************************************************************* */
Plot::ConfigScheme::ConfigScheme(const QFont &title_font, const QFont &axis_label_font,
                                 const QFont &ticks_font, const QFont legent_font,
                                 const QPen &default_pen)
  : title_font(title_font), axis_label_font(axis_label_font), ticks_font(ticks_font),
    legent_font(legent_font), default_pen(default_pen)
{
  // Pass...
}


Plot::ConfigScheme::ConfigScheme(const ConfigScheme &other)
  : title_font(other.title_font), axis_label_font(other.axis_label_font),
    ticks_font(other.ticks_font), legent_font(other.legent_font), default_pen(other.default_pen)
{
  // Pass...
}


const QFont &
Plot::ConfigScheme::titleFont() const
{
  return this->title_font;
}


const QFont &
Plot::ConfigScheme::axisLabelFont() const
{
  return this->axis_label_font;
}


const QFont &
Plot::ConfigScheme::ticksFont() const
{
  return this->ticks_font;
}


const QFont &
Plot::ConfigScheme::legentFont() const
{
  return this->legent_font;
}


const QPen &
Plot::ConfigScheme::defaultPen() const
{
  return this->default_pen;
}

