#include "figure.hh"
#include "linegraph.hh"
#include "linegraph.hh"
#include "variancelinegraph.hh"
#include "configuration.hh"
#include "../tinytex/tinytex.hh"

#include <QImage>
#include <QGraphicsSceneMouseEvent>
#include <QtSvg/QSvgGenerator>


using namespace Plot;


Figure::Figure(const QString &title, QObject *parent) :
  QGraphicsScene(parent), _math_ctx(), _titlesize(0,0), _titletext(title), _title(0),
  _legend_pos(TOP_RIGHT),_measures_enabled(true)
{
  const double fac = 255.;

  // Construct default palette for lines-graphs:
  _styles << GraphStyle(Qt::SolidLine, QColor(0, 0, 125))
          << GraphStyle(Qt::SolidLine, QColor(125, 0, 0))
          << GraphStyle(Qt::SolidLine, QColor(0, 125, 0))
          << GraphStyle(Qt::SolidLine, QColor(125, 125, 0))
          << GraphStyle(Qt::SolidLine, QColor(0, 125, 125))
          << GraphStyle(Qt::SolidLine, QColor(125, 0, 125))
          << GraphStyle(Qt::SolidLine, QColor(0.803922*fac, 0.309804*fac, 0.0705882*fac))
          << GraphStyle(Qt::SolidLine, QColor(1.*fac, 0.72549*fac, 0.0941176*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.952941*fac, 0.980392*fac, 0.572549*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.411765*fac, 0.592157*fac, 0.4*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.270588*fac, 0.184314*fac, 0.376471*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.878431*fac, 0.101961*fac, 0.207843*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.8*fac, 0.0588235*fac, 0.0745098*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.2472*fac, 0.24*fac, 0.6*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.6*fac, 0.24*fac, 0.442893*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.24*fac, 0.6*fac, 0.33692*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.24*fac, 0.353173*fac, 0.6*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.6*fac, 0.24*fac, 0.563266*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.24*fac, 0.473545*fac, 0.6*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.516361*fac, 0.24*fac, 0.6*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.6*fac, 0.306268*fac, 0.24*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.383825*fac, 0.6*fac, 0.24*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.24*fac, 0.593918*fac, 0.6*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.395989*fac, 0.24*fac, 0.6*fac))
          << GraphStyle(Qt::SolidLine, QColor(0.6*fac, 0.24*fac, 0.294104*fac));


  // Render title:
  _math_ctx = MathContext(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).titleFont());
  MathItem *title_item = TinyTex::parseInlineQuoted(title.toStdString());
  _title = title_item->layout(_math_ctx); _titlesize = title_item->metrics().size();
  delete title_item; _title->setPos(0,0);

  // Construct axis (the area, where the data is plotted).
  _axis = new Axis();

  // Construct and hide legend:
  _legend = new Legend();
  _legend->setPos(0,0);

  // Add items to QGraphicsScene:
  addItem(_title);
  addItem(_axis);
  addItem(_legend);
}


Plot::Axis *
Figure::getAxis() {
  return _axis;
}


void
Figure::setScheme(Configuration::Scheme scheme)
{
  // Remove "old" title item;
  removeItem(_title); delete _title;
  // Set font of title-label:
  _math_ctx = MathContext(Configuration::getConfig()->getScheme(scheme).titleFont());
  MathItem *title_item = TinyTex::parseInlineQuoted(_titletext.toStdString());
  _title = title_item->layout(_math_ctx); _titlesize = title_item->metrics().size();
  delete title_item; _title->setPos(0,0);
  addItem(_title);

  // Update scheme of legend and axis:
  _axis->setScheme(scheme);
  _legend->setScheme(scheme);
  updateAxes();
}


void
Figure::updateAxes()
{
  // Some extra margin:
  double margin_left   = 10;
  double margin_right  = 20;
  double margin_top    = 10;
  double margin_bottom = 10;
  double title_y = margin_top;

  // Adds title height to top-margin:
  margin_top += _titlesize.height() + 10;

  // Update effective width and heigth of axes:
  double effective_width = this->width()-margin_left-margin_right;
  double effective_height = this->height()-margin_top-margin_bottom;

  // Update Axis:
  this->_axis->setAxisSize(QSizeF(effective_width, effective_height));
  this->_axis->setPos(margin_left, margin_top);

  // Move title to proper position:
  if (_titlesize.width() > _axis->getPlotArea().width())
    _title->setPos(width()/2-_titlesize.width()/2, margin_top);
  else {
    double offset = margin_left + _axis->getPlotArea().x();
    _title->setPos(offset + _axis->getPlotArea().width()/2-_titlesize.width()/2, title_y);
  }

  // Update legend position:
  this->_legend->updateLayout();
  switch (this->_legend_pos)
  {
  case TOP_LEFT:
    this->_legend->setPos(margin_left+10, margin_top+10);
    break;

  case TOP_RIGHT:
    this->_legend->setPos(margin_left+effective_width-10-this->_legend->boundingRect().width(),
                          margin_top+10);
    break;

  case BOTTOM_LEFT:
    this->_legend->setPos(margin_left+10,
                          margin_top+effective_height-10-this->_legend->boundingRect().height());
    break;

  case BOTTOM_RIGHT:
    this->_legend->setPos(margin_left+effective_width-10-this->_legend->boundingRect().width(),
                          margin_top+effective_height-10-this->_legend->boundingRect().height());
    break;
  }

  // Update Axis:
  this->_axis->updatePlotSize();
  this->_axis->updatePlotRange();
  this->_axis->forceRedraw();
}



QString
Figure::getTitle() const {
  return _titletext;
}


void
Figure::setTitle(const QString &title)
{
  // Remove "old" title item;
  removeItem(_title); delete _title;
  // Set new title text
  _titletext = title;
  // Update title:
  MathItem *title_item = TinyTex::parseInlineQuoted(_titletext.toStdString());
  _title = title_item->layout(_math_ctx); _titlesize = title_item->metrics().size();
  delete title_item; _title->setPos(0,0); addItem(_title);

  // layout everything
  updateAxes();
}


void
Figure::addToLegend(const QString &label, Graph *graph)
{
  _legend->addGraph(label, graph);
}


GraphStyle
Figure::getStyle(int n) const
{
  return _styles.value( n % (_styles.size()));
}


void
Figure::setXLabel(const QString &label)
{
  _axis->setXLabel(label);
}


void
Figure::setYLabel(const QString &label)
{
  _axis->setYLabel(label);
}


void
Figure::save(const QString &filename, FileType type)
{
  if (Figure::FILETYPE_PNG == type) {
    // Construct image to paint on:
    QImage *image = new QImage(this->width(), this->height(), QImage::Format_ARGB32_Premultiplied);

    // Clear image (set all transparent):
    image->fill(QColor(0,0,0,0).rgba());

    // Construct painter to render graphics-sceene into bitmap:
    QPainter *painter = new QPainter(image);
    // make it look good:
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                            QPainter::TextAntialiasing);

    // Render plot on painter:
    this->render(painter);

    // save image:
    image->save(filename, "PNG");

    // destroy painter & image:
    delete painter;
    delete image;
  } else {
    // Create SVG generator & painter
    QSvgGenerator *generator = new QSvgGenerator();
    generator->setFileName(filename);
    generator->setTitle(QString("iNA: ")+this->getTitle().replace("&","&amp;"));
    generator->setResolution(91);
    generator->setSize(QSize(width(), height()));
    QPainter *painter = new QPainter(generator);
    // Render graphics
    this->render(painter);
    painter->end();
    // delete painter, generator & close file.
    delete painter;
    delete generator;
  }
}


void
Figure::enableMesure(bool enabled)
{
  _measures_enabled = enabled;
}

void
Figure::showMeasure(const QPointF &point)
{
  // Skip if measures are disabled:
  if (! _measures_enabled) { return; }
  // Translate coordinates and forward to Axis instance.
  this->_axis->showMeasure(point-_axis->pos());
}



/* ******************************************************************************************** *
 * Implementation of ConfiguredPlot
 * ******************************************************************************************** */
ConfiguredPlot::ConfiguredPlot(PlotConfig *config, QObject *parent)
  : Figure("", parent), _config(config)
{
  // Set title and lables:
  setTitle(_config->title());
  if (_config->hasXLabel()) { setXLabel(_config->xLabel()); }
  if (_config->hasYLabel()) { setYLabel(_config->yLabel()); }

  // Add graphs:
  for(PlotConfig::iterator item=_config->begin(); item!=_config->end(); item++) {
    Graph *graph = (*item)->createGraph();
    getAxis()->addGraph(graph);
    if ((*item)->showInLegend()) { addToLegend((*item)->label(), graph); }
  }

  // set ranges and range policies:
  this->getAxis()->setXRangePolicy(_config->xRangePolicy());
  this->getAxis()->setXRange(_config->xRange());
  this->getAxis()->setYRangePolicy(_config->yRangePolicy());
  this->getAxis()->setYRange(_config->yRange());

  // Layout figure
  this->updateAxes();
}
