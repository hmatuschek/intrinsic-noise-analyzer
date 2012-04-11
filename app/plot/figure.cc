#include "figure.hh"
#include "linegraph.hh"
#include "linegraph.hh"
#include "variancelinegraph.hh"
#include "configuration.hh"

#include <QImage>

using namespace Plot;


Figure::Figure(const QString &title, QObject *parent) :
  QGraphicsScene(parent), legend_pos(TOP_RIGHT)
{

  const double fac = 255.;

  // Construct default palette for lines-graphs:
  this->styles
      << GraphStyle(Qt::SolidLine, QColor(0, 0, 125))
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

  this->title = new QGraphicsTextItem(title);
  this->title->setFont(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).titleFont());
  this->title->setPos(0,0);

  // Construct axis (the area, where the data is plotted).
  this->axis = new Axis();

  // Construct and hide legend:
  this->legend = new Legend();
  this->legend->setPos(0,0);

  // Add items to QGraphicsScene:
  this->addItem(this->title);
  this->addItem(this->axis);
  this->addItem(this->legend);
}


Plot::Axis *
Figure::getAxis()
{
  return this->axis;
}


void
Figure::setScheme(Configuration::Scheme scheme)
{
  // Set font of title-label:
  this->title->setFont(Configuration::getConfig()->getScheme(scheme).titleFont());

  // Update scheme of legend and axis:
  this->axis->setScheme(scheme);
  this->legend->setScheme(scheme);
}


void
Figure::updateAxes()
{
  // Some extra margin:
  double margin_left   = 10;
  double margin_right  = 20;
  double margin_top    = 10;
  double margin_bottom = 10;

  // Move title to proper position:
  this->title->setPos(this->width()/2-this->title->boundingRect().width()/2, margin_top);
  // Adds title height to top-margin:
  margin_top += this->title->boundingRect().height() + 10;

  // Update effective width and heigth of axes:
  double effective_width = this->width()-margin_left-margin_right;
  double effective_height = this->height()-margin_top-margin_bottom;

  // Update Axis:
  this->axis->setAxisSize(QSizeF(effective_width, effective_height));
  this->axis->setPos(margin_left, margin_top);

  // Update legend position:
  switch (this->legend_pos)
  {
  case TOP_LEFT:
    this->legend->setPos(margin_left+10, margin_top+10);
    break;

  case TOP_RIGHT:
    this->legend->setPos(margin_left+effective_width-10-this->legend->boundingRect().width(),
                         margin_top+10);
    break;

  case BOTTOM_LEFT:
    this->legend->setPos(margin_left+10,
                         margin_top+effective_height-10-this->legend->boundingRect().height());
    break;

  case BOTTOM_RIGHT:
    this->legend->setPos(margin_left+effective_width-10-this->legend->boundingRect().width(),
                         margin_top+effective_height-10-this->legend->boundingRect().height());
    break;
  }

  // Update Axis:
  this->axis->updatePlotSize();
  this->axis->updatePlotRange();
  this->axis->forceRedraw();
}



QString
Figure::getTitle() const
{
  return this->title->toPlainText();
}


void
Figure::setTitle(const QString &title)
{
  this->title->setPlainText(title);
  this->updateAxes();
}


void
Figure::addToLegend(const QString &label, Graph *graph)
{
  this->legend->addGraph(label, graph);
}


GraphStyle
Figure::getStyle(int n) const
{
  return this->styles.value( n % (this->styles.size()));
}


void
Figure::setXLabel(const QString &label)
{
  this->axis->setXLabel(label);
}


void
Figure::setYLabel(const QString &label)
{
  this->axis->setYLabel(label);
}


void
Figure::save(const QString &filename, FileType type)
{
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

  // Determine format:
  const char *format = 0;
  switch (type) {
  case Figure::FILETYPE_PNG:
    format = "PNG";
    break;
  }

  // save image:
  image->save(filename, format);

  // destroy painter:
  delete painter;

  // free image:
  delete image;
}

