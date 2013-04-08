#include "canvas.hh"
#include "figure.hh"
#include <QMouseEvent>

using namespace Plot;


Canvas::Canvas(QWidget *parent)
  : QWidget(parent), _plot(0)
{
}


Plot::Figure *
Canvas::getPlot()
{
  return this->_plot;
}


void
Canvas::setPlot(Plot::Figure *plot)
{
  // Store plot
  this->_plot = plot;

  // Resize plot to this size:
  QSize size = this->size();
  this->_plot->setSceneRect(0,0, size.width(), size.height());

  // Signal plot to update axes:
  this->_plot->updateAxes();

  // Update widget:
  this->update();
}


void
Canvas::saveAs(const QString &filename, Figure::FileType type)
{
  if (0 == this->_plot)
    return;
  this->_plot->save(filename, type);
}


void
Canvas::setScheme(Configuration::Scheme scheme)
{
  if (0 == this->_plot)
    return;

  // Set scheme of plot:
  this->_plot->setScheme(scheme);
  // Signal plot to update itself:
  this->_plot->updateAxes();
  this->_plot->updateAxes();

  // redraw plot:
  this->update();
}


void
Canvas::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);

  if (0 == this->_plot)
    return;

  QSize size = this->size();
  this->_plot->setSceneRect(0,0, size.width(), size.height());

  // Signal plot to update axes:
  this->_plot->updateAxes();
}


void
Canvas::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);

  if (0 == this->_plot)
    return;

  QPainter painter(this);
  // make it look good:
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                         QPainter::TextAntialiasing);
  painter.fillRect(0,0, this->size().width(), this->size().height(), Qt::white);

  this->_plot->render(&painter);
}


void
Canvas::mousePressEvent(QMouseEvent *event)
{
  if (0 == _plot) { return; }
  // Do not call default handler!
  _plot->showMeasure(event->pos());
  this->update();
}
