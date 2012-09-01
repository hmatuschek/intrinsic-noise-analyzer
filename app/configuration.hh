#ifndef __FLUC_GUI_CONFIGURATION_HH__
#define __FLUC_GUI_CONFIGURATION_HH__

#include <QFont>
#include <QSettings>


/** Provides some configurations. */
class Configuration
{
protected:
  /**
   * The default (system) text-font.
   */
  QFont text_font;

  /**
   * The default font for headings level 3.
   */
  QFont h3_font;

  /**
   * The default font for headings level 2.
   */
  QFont h2_font;

  /**
   * The default font for headings level 1.
   */
  QFont h1_font;


public:
  /**
   * Constructor.
   */
  Configuration();

  /**
   * Returns the H1 font.
   */
  const QFont &getH1Font();

  /**
   * Returns the H2 font.
   */
  const QFont &getH2Font();

  /**
   * Returns the H3 font.
   */
  const QFont &getH3Font();

  /**
   * Returns the default text-font.
   */
  const QFont &getTextFont();
};

#endif // CONFIGURATION_HH
