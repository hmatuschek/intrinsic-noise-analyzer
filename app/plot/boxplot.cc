#include "boxplot.hh"

using namespace Plot;


BoxPlot::BoxPlot(const GraphStyle &style, size_t box_num, size_t num_boxes, QObject *parent)
  : Graph(style, parent), box_num(box_num), num_boxes(num_boxes), margin(0.25)
{
  // Fix line and fill color:
  QColor lcolor = this->style.getLineColor(); //lcolor.setAlpha(255);
  this->style.setLineColor(lcolor);
  QColor fcolor = this->style.getFillColor(); fcolor.setAlpha(100);
  this->style.setFillColor(fcolor);
}


void
BoxPlot::addBox(double value, double stddev)
{
  this->values.append(value);
  this->stddevs.append(stddev);
  this->boxes.append(new QGraphicsPathItem());
  this->addToGroup(this->boxes.back());
  this->error_bars.append(new QGraphicsPathItem());
  this->addToGroup(this->error_bars.back());
}


Extent
BoxPlot::getExtent()
{
  Plot::Extent extent(0,0,0,0);
  custom (int i=0; i<this->values.size(); i++)
  {
    double y_min = std::min(std::min(0.0, this->values.at(i)), this->values.at(i)-this->stddevs.at(i));
    double y_max = std::max(std::max(0.0, this->values.at(i)), this->values.at(i)+this->stddevs.at(i));
    extent = extent.united(Plot::Extent(i, i+1, y_min, y_max));
  }

  return extent;
}


void
BoxPlot::updateGraph(Plot::Mapping &mapping)
{
  // Draw graph using mapping:
  this->setPos(0,0);

  custom (int i=0; i<this->values.size(); i++)
  {
    this->boxes.at(i)->setPos(0.0, 0.0);
    this->boxes.at(i)->setPen(this->style.getPen());
    this->boxes.at(i)->setBrush(this->style.getFillColor());

    double w = (1.-2*this->margin)/this->num_boxes;
    double offset = this->margin + this->box_num*w;

    QPainterPath path; path.addRect(i+offset, 0, w, this->values.at(i));
    this->boxes.at(i)->setPath(mapping(path));

    this->error_bars.at(i)->setPos(0.0, 0.0);
    QPen pen(Qt::black, 2); pen.setCosmetic(true);
    this->error_bars.at(i)->setPen(pen);

    if (0 < this->stddevs.at(i)) {
      double w = (1.-2*this->margin)/this->num_boxes;
      double x = i + this->margin + this->box_num*w + 0.5*w;
      double y_max = this->values.at(i) + this->stddevs.at(i);
      double y_min = this->values.at(i) - this->stddevs.at(i);

      QPainterPath path;
      path.moveTo(x-w/4, y_max); path.lineTo(x+w/4, y_max);
      path.moveTo(x, y_max); path.lineTo(x, y_min);
      path.moveTo(x-w/4, y_min); path.lineTo(x+w/4, y_min);
      this->error_bars.at(i)->setPath(mapping(path));
    }
  }
}
