#include "configuration.hh"
#include <QApplication>


Configuration::Configuration()
  : text_font(QApplication::font()),
    h3_font(text_font.family(), text_font.pointSize()+2, QFont::Bold),
    h2_font(h3_font.family(), h3_font.pointSize()+2, QFont::Bold),
    h1_font(h2_font.family(), h2_font.pointSize()+2, QFont::Bold)
{
  // pass...
}


const QFont &
Configuration::getH1Font()
{
  return this->h1_font;
}


const QFont &
Configuration::getH2Font()
{
  return this->h2_font;
}


const QFont &
Configuration::getH3Font()
{
  return this->h3_font;
}


const QFont &
Configuration::getTextFont()
{
  return this->text_font;
}
