#include "configuration.hh"
#include "../models/application.hh"
#include "formulaparser.hh"
#include "graph.hh"
#include "figure.hh"
#include "linegraph.hh"
#include "variancelinegraph.hh"
#include <sstream>

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
  : _title_font(title_font), _axis_label_font(axis_label_font), _ticks_font(ticks_font),
    _legent_font(legent_font), _default_pen(default_pen)
{
  // Pass...
}


Plot::ConfigScheme::ConfigScheme(const ConfigScheme &other)
  : _title_font(other._title_font), _axis_label_font(other._axis_label_font),
    _ticks_font(other._ticks_font), _legent_font(other._legent_font), _default_pen(other._default_pen)
{
  // Pass...
}


const QFont &
Plot::ConfigScheme::titleFont() const {
  return this->_title_font;
}

const QFont &
Plot::ConfigScheme::axisLabelFont() const {
  return this->_axis_label_font;
}

const QFont &
Plot::ConfigScheme::ticksFont() const {
  return this->_ticks_font;
}

const QFont &
Plot::ConfigScheme::legendFont() const {
  return this->_legent_font;
}

const QPen &
Plot::ConfigScheme::defaultPen() const {
  return this->_default_pen;
}



/* ********************************************************************************************* *
 * Implementation of PlotConfig
 * ********************************************************************************************* */
PlotConfig::PlotConfig(Table &data)
  : _title(""), _xlabel(""), _ylabel(""), _show_legend(true),
    _xRangePolicy(RangePolicy::AUTOMATIC, RangePolicy::AUTOMATIC), _xRange(0,1),
    _yRangePolicy(RangePolicy::AUTOMATIC, RangePolicy::AUTOMATIC), _yRange(0,1), _graphs(),
    _data(data)
{
  // Pass...
}

PlotConfig::PlotConfig(const PlotConfig &other)
  : _title(other._title), _xlabel(other._xlabel), _ylabel(other._ylabel),
    _show_legend(other._show_legend), _xRangePolicy(other._xRangePolicy), _xRange(other._xRange),
    _yRangePolicy(other._yRangePolicy), _yRange(other._yRange), _graphs(), _data(other._data)
{
  QList<AbstractGraphConfig *>::ConstIterator item=other._graphs.constBegin();
  for(; item!=other._graphs.constEnd(); item++) {
    _graphs.append((*item)->copy(this));
  }
}

PlotConfig::~PlotConfig() {
  QList<AbstractGraphConfig *>::Iterator item=_graphs.begin();
  for(; item!=_graphs.end(); item++) {
    delete *item;
  }
  _graphs.clear();
}

Figure *
PlotConfig::createFigure() {
  Figure *plot = new Figure();

  // Set title and lables:
  plot->setTitle(title());
  if (hasXLabel()) { plot->setXLabel(xLabel()); }
  if (hasYLabel()) { plot->setYLabel(yLabel()); }

  // Create & add graphs
  for (iterator item=begin(); item!=end(); item++) {
    Graph *graph = (*item)->createGraph();
    plot->getAxis()->addGraph(graph);
    if ((*item)->showInLegend()) { plot->addToLegend((*item)->label(), graph); }
  }

  // set ranges and range policies:
  plot->getAxis()->setXRangePolicy(xRangePolicy());
  plot->getAxis()->setXRange(xRange());
  plot->getAxis()->setYRangePolicy(yRangePolicy());
  plot->getAxis()->setYRange(yRange());

  // Update plotrages from figure (in case of auto-scaleing):
  plot->getAxis()->updatePlotRange();
  setXRange(plot->getAxis()->getXRange());
  setYRange(plot->getAxis()->getYRange());

  return plot;
}


Table *
PlotConfig::data() {
  return &_data;
}

bool
PlotConfig::hasTitle() const {
  return "" != _title;
}

const QString &
PlotConfig::title() const {
  return _title;
}

void
PlotConfig::setTile(const QString &title) {
  _title = title;
}

bool
PlotConfig::hasXLabel() const {
  return "" != _xlabel;
}

const QString &
PlotConfig::xLabel() const {
  return _xlabel;
}

void
PlotConfig::setXLabel(const QString &label) {
  _xlabel = label;
}

bool
PlotConfig::hasYLabel() const {
  return "" != _ylabel;
}

const QString &
PlotConfig::yLabel() const {
  return _ylabel;
}

void
PlotConfig::setYLabel(const QString &label) {
  _ylabel = label;
}

bool
PlotConfig::showLegend() const {
  return _show_legend;
}

void
PlotConfig::setShowLegend(bool show) {
  _show_legend = show;
}

const Range &
PlotConfig::xRange() const {
  return _xRange;
}

void
PlotConfig::setXRange(const Range &range) {
  _xRange = range;
}

const RangePolicy &
PlotConfig::xRangePolicy() const {
  return _xRangePolicy;
}

void
PlotConfig::setXRangePolicy(const RangePolicy &policy) {
  _xRangePolicy = policy;
}

const Range &
PlotConfig::yRange() const {
  return _yRange;
}

void
PlotConfig::setYRange(const Range &range) {
  _yRange = range;
}

const RangePolicy &
PlotConfig::yRangePolicy() const {
  return _yRangePolicy;
}

void
PlotConfig::setYRangePolicy(const RangePolicy &policy) {
  _yRangePolicy = policy;
}

size_t
PlotConfig::numGraphs() const {
  return _graphs.size();
}

PlotConfig::iterator
PlotConfig::begin() {
  return _graphs.begin();
}

PlotConfig::iterator
PlotConfig::end() {
  return _graphs.end();
}

AbstractGraphConfig *
PlotConfig::graph(size_t i) {
  return _graphs.at(i);
}

void
PlotConfig::removeGraph(size_t i) {
  if (int(i) >= _graphs.size()) { return; }
  AbstractGraphConfig *item = _graphs.at(i);
  _graphs.removeAt(i);
  delete item;
}

void
PlotConfig::removeGraph(AbstractGraphConfig *graph) {
  if (0 < _graphs.removeAll(graph)) { return; }
  delete graph;
}

void
PlotConfig::addGraph(AbstractGraphConfig *graph) {
  _graphs.append(graph);
}

void
PlotConfig::replaceGraph(size_t i, AbstractGraphConfig *graph) {
  if (int(i) >= _graphs.size()) { _graphs.append(graph); return; }
  AbstractGraphConfig *old = _graphs.at(i);
  _graphs.replace(i, graph);
  delete old;
}



/* ********************************************************************************************* *
 * Implementation of AbstractGraphConfig
 * ********************************************************************************************* */
AbstractGraphConfig::AbstractGraphConfig()
  : _show_in_legend(true), _label("")
{
  // pass...
}

AbstractGraphConfig::AbstractGraphConfig(const AbstractGraphConfig &other)
  : _show_in_legend(other._show_in_legend), _label(other._label)
{
  // Pass...
}

AbstractGraphConfig::~AbstractGraphConfig() {
  // Pass...
}

bool
AbstractGraphConfig::showInLegend() const {
  return _show_in_legend;
}

void
AbstractGraphConfig::setShowInLegend(bool show) {
  _show_in_legend = show;
}

bool
AbstractGraphConfig::hasLabel() const {
  return "" != _label;
}

const QString &
AbstractGraphConfig::label() const {
  return _label;
}

void
AbstractGraphConfig::setLabel(const QString &label) {
  _label = label;
}

QList<QColor> AbstractGraphConfig::defaultColors = QList<QColor>()
    << QColor(0, 0, 125) << QColor(125, 0, 0) << QColor(0, 125, 0) << QColor(125, 125, 0)
    << QColor(0, 125, 125) << QColor(125, 0, 125) << QColor(205, 79, 18) << QColor(255, 185, 24)
    << QColor(243, 250, 146) << QColor(105, 151, 102) << QColor(69, 47, 96)
    << QColor(224, 26, 53) << QColor(204, 15, 19) << QColor(63, 61, 153) << QColor(153, 61, 113)
    << QColor(61, 153, 86) << QColor(61, 90, 153) << QColor(153, 61, 144) << QColor(61, 121, 153)
    << QColor(132, 61, 153) << QColor(153, 78, 61) << QColor(98, 153, 61) << QColor(61, 151, 153)
    << QColor(101, 61, 153) << QColor(153, 61, 75);


/* ********************************************************************************************* *
 * Implementation of LineGraphConfig
 * ********************************************************************************************* */
LineGraphConfig::LineGraphConfig(Table *data, size_t colorIdx)
  : AbstractGraphConfig(), _data(data), _columnNames(), _parserContext(_data), _symbolTable(),
    _xExpression(0), _xCode(), _xInterpreter(),
    _yExpression(0), _yCode(), _yInterpreter(), _linePen()
{
  for (size_t i=0; i<_data->getNumColumns(); i++) {
    _columnNames.append(_data->getColumnName(i));
    _symbolTable[_parserContext.getColumnSymbol(i)] = i;
  }
  if (0 < _data->getNumColumns()) {
    _xExpression = _parserContext.getColumnSymbol(0);
  }

  // Init pen from default color index:
  _linePen.setStyle(Qt::SolidLine);
  _linePen.setBrush(QBrush(defaultColors[(colorIdx%defaultColors.size())]));
  _linePen.setWidth(2); _linePen.setCosmetic(true);
  _linePen.setJoinStyle(Qt::MiterJoin); _linePen.setCapStyle(Qt::SquareCap);

  LineGraphConfig::compileExpressions();
}

LineGraphConfig::LineGraphConfig(const LineGraphConfig &other, Table *data)
  : AbstractGraphConfig(other), _data(data), _columnNames(other._columnNames),
    _parserContext(_data), _symbolTable(), _xExpression(0), _yExpression(0),
    _linePen(other._linePen)
{
  for (size_t i=0; i<_data->getNumColumns(); i++) {
    _symbolTable[_parserContext.getColumnSymbol(i)] = i;
  }
  // Serialize & parse plot formulas:
  std::stringstream buffer;
  FormulaParser::serialize(other._xExpression, buffer, other._parserContext);
  _xExpression = FormulaParser::parse(buffer.str().c_str(), _parserContext);
  buffer.str("");
  FormulaParser::serialize(other._yExpression, buffer, other._parserContext);
  _yExpression = FormulaParser::parse(buffer.str().c_str(), _parserContext);
  // Compile plot formulas:
  LineGraphConfig::compileExpressions();
}

LineGraphConfig::~LineGraphConfig() {
  // pass...
}

void
LineGraphConfig::compileExpressions() {
  // Clear code instances:
  _xCode.clear(); _yCode.clear();
  // Compile expressions:
  iNA::Eval::bci::Compiler<Eigen::VectorXd> compiler(_parserContext.symbolTable());
  compiler.setCode(&_xCode); compiler.compileExpressionAndStore(_xExpression, 0);
  compiler.finalize(0); _xInterpreter.setCode(&_xCode);
  compiler.setCode(&_yCode); compiler.compileExpressionAndStore(_yExpression, 0);
  compiler.finalize(0); _yInterpreter.setCode(&_yCode);
}

AbstractGraphConfig *
LineGraphConfig::copy(PlotConfig *config) const {
  return new LineGraphConfig(*this, config->data());
}

const QStringList &
LineGraphConfig::columnNames() const {
  return _columnNames;
}

GiNaC::symbol
LineGraphConfig::columnSymbol(size_t column) {
  return _parserContext.getColumnSymbol(column);
}

bool
LineGraphConfig::checkExpression(const QString &expression) {
  return FormulaParser::check(expression, _parserContext);
}

QString
LineGraphConfig::xExpression() {
  std::stringstream buffer; FormulaParser::serialize(_xExpression, buffer, _parserContext);
  return QString(buffer.str().c_str());
}

void
LineGraphConfig::setXExpression(const QString &expression) throw (iNA::Parser::ParserError) {
  setXExpression(FormulaParser::parse(expression, _parserContext));
}

void
LineGraphConfig::setXExpression(const GiNaC::ex &expression) {
  _xExpression = expression; compileExpressions();
}

QString
LineGraphConfig::yExpression() {
  std::stringstream buffer; FormulaParser::serialize(_yExpression, buffer, _parserContext);
  return QString(buffer.str().c_str());
}

void
LineGraphConfig::setYExpression(const QString &expression) throw (iNA::Parser::ParserError) {
  setYExpression(FormulaParser::parse(expression, _parserContext));
}

void
LineGraphConfig::setYExpression(const GiNaC::ex &expression) {
  _yExpression = expression; compileExpressions();
}

Graph *
LineGraphConfig::createGraph() {
  GraphStyle style(_linePen.style(), _linePen.brush().color(), _linePen.width());
  LineGraph *graph = new LineGraph(style);

  /// @bug This implementation is f***ing slow!!!
  Eigen::VectorXd output(1);
  // Evaluate y & x expressions on data:
  double x,y;
  for (size_t i=0; i<_data->getNumRows(); i++) {
    _xInterpreter.run(_data->getRow(i), output); x = output(0);
    _yInterpreter.run(_data->getRow(i), output); y = output(0);
    graph->addPoint(x,y);
  }
  graph->commit();

  // Done.
  return graph;
}

const QColor &
LineGraphConfig::lineColor() const {
  return _linePen.brush().color();
}

void
LineGraphConfig::setLineColor(const QColor &color) {
  QBrush brush = _linePen.brush(); brush.setColor(color);
  _linePen.setBrush(brush);
}

const QPen &
LineGraphConfig::linePen() const {
  return _linePen;
}



/* ********************************************************************************************* *
 * Implementation of VarianceLineGraphConfig
 * ********************************************************************************************* */
VarianceLineGraphConfig::VarianceLineGraphConfig(Table *data, size_t colorIdx)
  : LineGraphConfig(data, colorIdx), _varExpression(0), _fillPen()
{
  QColor fill_color = _linePen.brush().color(); fill_color.setAlpha(32);
  _fillPen.setBrush(QBrush(fill_color));
  VarianceLineGraphConfig::compileExpressions();
}

VarianceLineGraphConfig::VarianceLineGraphConfig(const VarianceLineGraphConfig &other, Table *data)
  : LineGraphConfig(other, data), _varExpression(0), _fillPen(other._fillPen)
{
  std::stringstream buffer;
  FormulaParser::serialize(other._varExpression, buffer, other._parserContext);
  _varExpression = FormulaParser::parse(buffer.str().c_str(), _parserContext);
  VarianceLineGraphConfig::compileExpressions();
}

VarianceLineGraphConfig::~VarianceLineGraphConfig() {
  // Pass...
}

void
VarianceLineGraphConfig::compileExpressions() {
  // First, compile expressions of line graph:
  LineGraphConfig::compileExpressions();
  _varCode.clear();
  iNA::Eval::bci::Compiler<Eigen::VectorXd> compiler(_parserContext.symbolTable());
  compiler.setCode(&_varCode); compiler.compileExpressionAndStore(_varExpression, 0);
  compiler.finalize(0); _varInterpreter.setCode(&_varCode);
}

AbstractGraphConfig *
VarianceLineGraphConfig::copy(PlotConfig *config) const {
  return new VarianceLineGraphConfig(*this);
}

Graph *
VarianceLineGraphConfig::createGraph() {
  GraphStyle style(_linePen.style(), _linePen.brush().color(), _linePen.width());
  VarianceLineGraph *graph = new VarianceLineGraph(style);

  // Evaluate y, x and var expressions on data:
  double x,y,v; Eigen::VectorXd output(1);
  for (size_t i=0; i<_data->getNumRows(); i++) {
    _xInterpreter.run(_data->getRow(i), output); x = output(0);
    _yInterpreter.run(_data->getRow(i), output); y = output(0);
    _varInterpreter.run(_data->getRow(i), output); v = output(0);
    graph->addPoint(x,y, std::sqrt(v));
  }
  graph->commit();

  // Done.
  return graph;
}

QString
VarianceLineGraphConfig::varExpression() {
  std::stringstream buffer; FormulaParser::serialize(_varExpression, buffer, _parserContext);
  return QString(buffer.str().c_str());
}

void
VarianceLineGraphConfig::setVarExpression(const QString &expression) throw (iNA::Parser::ParserError) {
  _varExpression = FormulaParser::parse(expression, _parserContext);
  this->compileExpressions();
}

const QColor &
VarianceLineGraphConfig::fillColor() const {
  return _fillPen.brush().color();
}

void
VarianceLineGraphConfig::setFillColor(const QColor &color) {
  QBrush brush = _fillPen.brush(); brush.setColor(color); _fillPen.setBrush(brush);
}

const QPen &
VarianceLineGraphConfig::fillPen() const {
  return _fillPen;
}

