#ifndef __FLUC_APP_PLOT_CONFIGURATION_HH__
#define __FLUC_APP_PLOT_CONFIGURATION_HH__

#include <QFont>
#include <QPen>


namespace Plot {


/**
 * Represents a plot-scheme.
 *
 * A plot-scheme is a collection of settings that can be applied at once to some @c Figure.
 * Currently there are two schemes, one optimizing the plot custom display and one custom print.
 *
 * @ingroup plot
 */
class ConfigScheme
{
protected:
  /**
   * Specifies the font custom the figure-title.
   */
  QFont title_font;

  /**
   * Specifies the font custom the axis labels.
   */
  QFont axis_label_font;

  /**
   * Specifies the font custom the axis-ticks labels.
   */
  QFont ticks_font;

  /**
   * Specifies the font custom the legend-labels.
   */
  QFont legent_font;

  /**
   * Specifies the default-pen.
   */
  QPen  default_pen;


public:
  /**
   * Constructs a complete plot-scheme.
   */
  ConfigScheme(const QFont &title_font, const QFont &axis_label_font, const QFont &ticks_font,
               const QFont legent_font, const QPen &default_pen);

  /**
   * Copy constructor.
   */
  ConfigScheme(const ConfigScheme &other);

  /**
   * Returns the figure-title font.
   */
  const QFont &titleFont() const;

  /**
   * Returns the axis-label font.
   */
  const QFont &axisLabelFont() const ;

  /**
   * Returns the axis-ticks font.
   */
  const QFont &ticksFont() const ;

  /**
   * Returns the legend-item font.
   */
  const QFont &legentFont() const;

  /**
   * Returns the default pen.
   */
  const QPen &defaultPen() const;
};



/**
 * Derives the plot-fonts and pens from the application-wide configuration.
 *
 * @ingroup plot
 */
class Configuration
{
public:
  /**
   * Defines the available plot-schemes.
   */
  typedef enum {
    DISPLAY_SCHEME = 0,   ///< Optimized custom the display.
    PRINT_SCHEME          ///< Optimized custom print.
  } Scheme;


protected:
  /**
   * Holds a list of schemes.
   */
  QList<ConfigScheme> schemes;


protected:
  /**
   * Constructs a default configuration.
   */
  Configuration();


public:
  /**
   * Returns the scheme.
   */
  const ConfigScheme &getScheme(Scheme scheme);


public:
  /**
   * Factory method custom application global config.
   */
  static Configuration *getConfig();


private:
  /**
   * Holds the application-wide plot-configuration.
   */
  static Configuration *global_config;
};

}

#endif // CONFIGURATION_HH
