#include "canvas.hh"

using namespace Plot;


Canvas::Canvas(QWidget *parent)
  : QWidget(parent), plot(0)
{
}


Plot::Figure *
Canvas::getPlot()
{
  return this->plot;
}


void
Canvas::setPlot(Plot::Figure *plot)
{
  // Store plot
  this->plot = plot;

  // Resize plot to this size:
  QSize size = this->size();
  this->plot->setSceneRect(0,0, size.width(), size.height());

  // Signal plot to update axes:
  this->plot->updateAxes();

  // Update widget:
  this->update();
}


void
Canvas::saveAs(const QString &filename, Figure::FileType type)
{
  if (0 == this->plot)
    return;
  this->plot->save(filename, type);
}


void
Canvas::setScheme(Configuration::Scheme scheme)
{
  if (0 == this->plot)
    return;

  // Set scheme of plot:
  this->plot->setScheme(scheme);
  // Signal plot to update itself:
  this->plot->updateAxes();
  this->plot->updateAxes();

  // redraw plot:
  this->update();
}


void
Canvas::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);

  if (0 == this->plot)
    return;

  QSize size = this->size();
  this->plot->setSceneRect(0,0, size.width(), size.height());

  // Signal plot to update axes:
  this->plot->updateAxes();
}


void
Canvas::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);

  if (0 == this->plot)
    return;

  QPainter painter(this);
  // make it look good:
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                         QPainter::TextAntialiasing);
  painter.fillRect(0,0, this->size().width(), this->size().height(), Qt::white);

  this->plot->render(&painter);
}
