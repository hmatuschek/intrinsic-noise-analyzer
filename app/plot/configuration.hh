#ifndef __FLUC_APP_PLOT_CONFIGURATION_HH__
#define __FLUC_APP_PLOT_CONFIGURATION_HH__

#include <QFont>
#include <QPen>
#include "../models/timeseries.hh"
#include "formulaparser.hh"
#include <ginac/ginac.h>
#include <eval/bci/bci.hh>
#include <parser/exception.hh>
#include "mapping.hh"


namespace Plot {

// Forward declaration
class Graph;
class Figure;
class PlotConfig;

/** Represents a plot-scheme.
 * A plot-scheme is a collection of settings that can be applied at once to some @c Figure.
 * Currently there are two schemes, one optimizing the plot for display and one for print.
 * @ingroup plot */
class ConfigScheme
{
protected:
  /** Specifies the font for the figure-title. */
  QFont _title_font;
  /** Specifies the font for the axis labels. */
  QFont _axis_label_font;
  /** Specifies the font for the axis-ticks labels. */
  QFont _ticks_font;
  /** Specifies the font for the legend-labels. */
  QFont _legent_font;
  /** Specifies the default-pen. */
  QPen  _default_pen;

public:
  /** Constructs a complete plot-scheme. */
  ConfigScheme(const QFont &_title_font, const QFont &_axis_label_font, const QFont &_ticks_font,
               const QFont _legent_font, const QPen &_default_pen);

  /** Copy constructor. */
  ConfigScheme(const ConfigScheme &other);

  /** Returns the figure-title font. */
  const QFont &titleFont() const;
  /** Returns the axis-label font. */
  const QFont &axisLabelFont() const ;
  /** Returns the axis-ticks font. */
  const QFont &ticksFont() const;
  /** Returns the legend-item font. */
  const QFont &legendFont() const;
  /** Returns the default pen. */
  const QPen &defaultPen() const;
};


/** Derives the plot-fonts and pens from the application-wide configuration.
 * @ingroup plot */
class Configuration
{
public:
  /** Defines the available plot-schemes. */
  typedef enum {
    DISPLAY_SCHEME = 0,   ///< Optimized for the display.
    PRINT_SCHEME          ///< Optimized for print.
  } Scheme;

protected:
  /** Constructs a default configuration. */
  Configuration();

public:
  /** Returns the scheme. */
  const ConfigScheme &getScheme(Scheme scheme);

  /** Factory method for application global config. */
  static Configuration *getConfig();

protected:
  /** Holds a list of schemes. */
  QList<ConfigScheme> schemes;

private:
  /** Holds the application-wide plot-configuration. */
  static Configuration *global_config;
};


/** Defines the interface for all graph configurations.
 * @ingroup plot */
class AbstractGraphConfig
{
public:
  /** Default constructor. */
  AbstractGraphConfig();
  /** Copy constructor. */
  AbstractGraphConfig(const AbstractGraphConfig &other);
  /** Destructor. */
  virtual ~AbstractGraphConfig();

  /** Copies the graph configuration. */
  virtual AbstractGraphConfig *copy(PlotConfig *config) const = 0;
  virtual Graph *createGraph() = 0;

  /** Returns true, if the graph is shown in the legend (false if no lable is set). */
  bool showInLegend() const;
  /** (Re-) Sets if the graph is shown in the legend. */
  void setShowInLegend(bool show);

  /** Retunrs true if a label is set. */
  bool hasLabel() const;
  /** Returns the label of the graph. */
  const QString &label() const;
  /** (Re-) Sets the graph lable. */
  void setLabel(const QString &label);

public:
  /** Holds a list of default colors. */
  static QList<QColor> defaultColors;

protected:
  /** If true, the graph will be shown in the legend. */
  bool _show_in_legend;
  /** The graph label. */
  QString _label;
};


/** Holds the generic configuration of a plot, including axis labels etc.
 * @todo Allow for the specification of the plot-rage and range-policy.
 * @ingroup plot */
class PlotConfig
{
public:
  /** Defines the iterator type over the associated graphs. */
  typedef QList<AbstractGraphConfig *>::Iterator iterator;

public:
  /** Constructs a default plot config. */
  PlotConfig(Table &data);
  /** Copy constructor. */
  PlotConfig(const PlotConfig &other);
  /** Destroys the plot config an all graphs. */
  virtual ~PlotConfig();

  /** Assembles a @c Plot::Figure from the configuration. */
  Figure *createFigure();

  /** Returns a weak reference to the data. */
  Table *data();

  /** Returns true if a plot title is set. */
  bool hasTitle() const;
  /** Returns the plot title. */
  const QString &title() const;
  /** (Re-) Sets the plot title. */
  void setTile(const QString &title);
  /** Returns true if a X label is set. */

  bool hasXLabel() const;
  /** Returns the X label. */
  const QString &xLabel() const;
  /** (Re-) Sets the X label. */
  void setXLabel(const QString &label);
  /** Returns true if a Y label is set. */

  bool hasYLabel() const;
  /** Returns the Y label. */
  const QString &yLabel() const;
  /** (Re-) Sets the Y label. */
  void setYLabel(const QString &label);

  /** Returns true if the legend is shown. */
  bool showLegend() const;
  /** Enables/Disables the legend. */
  void setShowLegend(bool show);

  /** Returns the x range. */
  const Range &xRange() const;
  /** Sets the x range. */
  void setXRange(const Range &range);
  /** Returns the x range policy. */
  const RangePolicy &xRangePolicy() const;
  /** Sets the X range policy. */
  void setXRangePolicy(const RangePolicy &policy);

  /** Returns the y range. */
  const Range &yRange() const;
  /** Sets the y range. */
  void setYRange(const Range &range);
  /** Returns the y range policy. */
  const RangePolicy &yRangePolicy() const;
  /** Sets the Y range policy. */
  void setYRangePolicy(const RangePolicy &policy);

  /** Returns the number of graphs. */
  size_t numGraphs() const;
  /** Returns the iterator pointing to the first graph. */
  iterator begin();
  /** Returns the iterator pointing right after the last graph. */
  iterator end();
  /** Returns the i-th graph. */
  AbstractGraphConfig *graph(size_t i);
  /** Removes the graph at the given index, the instance will be destroyed */
  void removeGraph(size_t i);
  /** Removes the given graph, the instance will be destroyed */
  void removeGraph(AbstractGraphConfig *graph);
  /** Adds a graph to the list of graphs. */
  void addGraph(AbstractGraphConfig *graph);
  /** Replaces the graph at the given position with the given one. */
  void replaceGraph(size_t i, AbstractGraphConfig *graph);

protected:
  /** Holds the main title. */
  QString _title;
  /** Holds the X label. */
  QString _xlabel;
  /** Holds the Y label. */
  QString _ylabel;
  /** If true, the legend is shown. */
  bool _show_legend;
  /** Holds the range policy for the X axis. */
  RangePolicy _xRangePolicy;
  /** Holds the range policy for the X axis. */
  Range _xRange;
  /** Holds the range policy for the Y axis. */
  RangePolicy _yRangePolicy;
  /** Holds the range policy for the Y axis. */
  Range _yRange;
  /** Holds the graph configurations. */
  QList<AbstractGraphConfig *> _graphs;
  /** Holds the data for the graphs. */
  Table _data;
};


/** A line graph configuration.
 * @ingroup plot */
class LineGraphConfig : public AbstractGraphConfig
{
public:
  /** Empty constructor. */
  LineGraphConfig(Table *data, size_t colorIdx);
  /** Copy constructor. */
  LineGraphConfig(const LineGraphConfig &other, Table *data);
  /** Destructor. */
  ~LineGraphConfig();

  /** Implements the AbstractGraphConfig interface. */
  virtual AbstractGraphConfig *copy(PlotConfig *config) const;
  /** Creates the graph from this config. */
  virtual Graph *createGraph();

  /** Returns the list of column labels. */
  const QStringList &columnNames() const;
  /** Returns the symbol for the specified column. */
  GiNaC::symbol columnSymbol(size_t column);
  /** Checks if the given expression is valid. */
  bool checkExpression(const QString &expression);
  /** Returns the expression of the X values. */
  QString xExpression();
  /** Parses and sets the given expression as the X value expression.
   * @throws iNA::Parser::ParserError if the given expression can not be parsed. */
  void setXExpression(const QString &expression) throw (iNA::Parser::ParserError);
  /** Sets the given expression as the X value expression. This expression must only contain
   * column-symbols (see @c columbSymbol). */
  void setXExpression(const GiNaC::ex &expression);
  /** Returns the expression of the Y values. */
  QString yExpression();
  /** Parses and sets the given expression as the Y value expression.
   * @throws iNA::Parser::ParserError if the given expression can not be parsed. */
  void setYExpression(const QString &expression) throw (iNA::Parser::ParserError);
  /** Sets the given expression as the Y value expression. This expression must only contain
   * column-symbols (see @c columbSymbol). */
  void setYExpression(const GiNaC::ex &expression);

  /** Returns the line color. */
  const QColor &lineColor() const;
  /** (Re-) Sets the line color. */
  void setLineColor(const QColor &color);

  /** Returns the pen drawing the line graph. */
  const QPen &linePen() const;

protected:
  /** Internal used method to compile the stored expressions. */
  virtual void compileExpressions();

protected:
  /** Holds a weak reference to the data to be used for plotting. */
  Table *_data;
  /** Holds the list of column names. */
  QStringList _columnNames;
  /** Holds the parser context for plot formulas. */
  FormulaParser::Context _parserContext;
  /** Maps the column symbols back to column indices. */
  std::map<GiNaC::symbol, size_t> _symbolTable;
  /** Holds the expression for the X values. */
  GiNaC::ex _xExpression;
  /** Holds the compiled expression for the X values. */
  iNA::Eval::bci::Code _xCode;
  /** Holds the interpreter instance for the X values. */
  iNA::Eval::bci::Interpreter<Eigen::VectorXd> _xInterpreter;
  /** Holds the expression for the Y values. */
  GiNaC::ex _yExpression;
  /** Holds the compiled expression for the Y values. */
  iNA::Eval::bci::Code _yCode;
  /** Holds the interpreter instance for the Y values. */
  iNA::Eval::bci::Interpreter<Eigen::VectorXd> _yInterpreter;
  /** Holds the pen used to draw the line graph. */
  QPen _linePen;
};


/** Extends the @c LineGraphConfig by a variance expression.
 * @ingroup plot */
class VarianceLineGraphConfig: public LineGraphConfig
{
public:
  /** Default constructor. */
  VarianceLineGraphConfig(Table *data, size_t colorIdx);
  /** Copy constructor. */
  VarianceLineGraphConfig(const VarianceLineGraphConfig &other, Table *data);
  /** Destructor. */
  virtual ~VarianceLineGraphConfig();

  /** Copies the config. */
  virtual AbstractGraphConfig *copy(PlotConfig *config) const;
  /** Creates the @c VarianceLineGraph. */
  virtual Graph *createGraph();

  /** Returns the expression of the Y values. */
  QString varExpression();
  /** Parses and sets the given expression as the variance expression.
   * @throws iNA::Parser::ParserError if the given expression can not be parsed. */
  void setVarExpression(const QString &expression) throw (iNA::Parser::ParserError);

  /** Returns the fill color. */
  const QColor &fillColor() const;
  /** (Re-) Sets the fill color. */
  void setFillColor(const QColor &color);

  /** Returns the pen drawing the confidence intervals. */
  const QPen &fillPen() const;

protected:
  /** Internal used method to compile plot formulas. */
  virtual void compileExpressions();

protected:
  /** Holds the variance expression. */
  GiNaC::ex _varExpression;
  /** Holds the compiled expression for the var values. */
  iNA::Eval::bci::Code _varCode;
  /** Holds the interpreter instance for the var values. */
  iNA::Eval::bci::Interpreter<Eigen::VectorXd> _varInterpreter;
  /** Holds the pen used to draw the confidence intervals. */
  QPen _fillPen;
};


}

#endif // CONFIGURATION_HH
