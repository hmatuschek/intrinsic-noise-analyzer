#include "plotview.hh"

#include <iostream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "../plot/canvas.hh"
#include "../plot/plotrangedialog.hh"
#include "../plot/configuration.hh"
#include "../plot/plotconfigdialog.hh"


PlotView::PlotView(PlotItem *plot_item, QWidget *parent) :
  QWidget(parent), _plotitem(plot_item), _canvas(0), _schemeBox(0)
{
  // Get figure
  Plot::Figure *plot = _plotitem->getPlot();

  // Set proper background style:
  this->setBackgroundRole(QPalette::Window);

  // Constuct canvas:
  this->_canvas = new Plot::Canvas(this);
  this->_canvas->setPlot(plot);
  this->_canvas->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

  // Save button:
  QPushButton *save_button = new QPushButton(tr("Save to file"));
  save_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(onSavePlot()));

  // Set plot range button:
  QPushButton *range_button = new QPushButton(tr("Set plot range"));
  range_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  if (_plotitem->hasConfig()) { range_button->setVisible(false); }

  // Configure plot button:
  QPushButton *config_plot = new QPushButton(tr("Edit plot"));
  config_plot->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  if (! _plotitem->hasConfig()) { config_plot->setVisible(false); }

  // Plot Scheme selection:
  this->_schemeBox = new QComboBox();
  this->_schemeBox->addItem(tr("Display"), QVariant(0));
  this->_schemeBox->addItem(tr("Print"), QVariant(1));

  // Assemble panel:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->_canvas);

  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(save_button);
  button_box->addWidget(range_button);
  button_box->addWidget(config_plot);
  button_box->addWidget(this->_schemeBox);
  layout->addLayout(button_box);
  this->setLayout(layout);

  // If the plot-wrapper gest destroyed -> close the plot-view:
  QObject::connect(_plotitem, SIGNAL(destroyed()), this, SLOT(onPlotDestroy()));
  // If new scheme is selected:
  QObject::connect(this->_schemeBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(onSchemeSelected(int)));
  QObject::connect(range_button, SIGNAL(clicked()), this, SLOT(onSetPlotRange()));
  QObject::connect(config_plot, SIGNAL(clicked()), this, SLOT(onConfigPlot()));

}


void
PlotView::onSavePlot()
{
  QString selected_filter("");
  QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save plot as ..."), "", tr("Portable Network Graphics (*.png);;Scalable Vector Graphics (*.svg)"), &selected_filter);

  // on cancel:
  if (0 == fileName.size()) { return; }

  // Determine file type by selected filter
  Plot::Figure::FileType type;
  if (tr("Portable Network Graphics (*.png)") == selected_filter) {
    type=Plot::Figure::FILETYPE_PNG;
    // check if file has proper suffix.
    QFileInfo info(fileName); if ("png" != info.suffix()) { fileName.append(".png"); }
  } else if (tr("Scalable Vector Graphics (*.svg)") == selected_filter) {
    type=Plot::Figure::FILETYPE_SVG;
    // check if file has proper suffix.
    QFileInfo info(fileName); if ("svg" != info.suffix()) { fileName.append(".svg"); }
  } else {
    QMessageBox::critical(0, tr("Can not save plot."),
                          tr("Unknown file type: {0}").arg(selected_filter));
    return;
  }

  // Save plot in file...
  this->_canvas->saveAs(fileName, type);
}


void
PlotView::onSetPlotRange()
{
  // Construct dialog
  Plot::PlotRangeDialog dialog(this->_canvas->getPlot()->getAxis()->getXRange(),
                               this->_canvas->getPlot()->getAxis()->getXRangePolicy(),
                               this->_canvas->getPlot()->getAxis()->getYRange(),
                               this->_canvas->getPlot()->getAxis()->getYRangePolicy(),
                               this);
  dialog.setModal(true);

  // Exec:
  if (QDialog::Rejected == dialog.exec())
    return;

  this->_canvas->getPlot()->getAxis()->setXRangePolicy(dialog.getXRangePolicy());
  this->_canvas->getPlot()->getAxis()->setYRangePolicy(dialog.getYRangePolicy());
  this->_canvas->getPlot()->getAxis()->setXRange(dialog.getXRange().min(), dialog.getXRange().max());
  this->_canvas->getPlot()->getAxis()->setYRange(dialog.getYRange().min(), dialog.getYRange().max());

  this->_canvas->getPlot()->updateAxes();
  this->_canvas->getPlot()->updateAxes();
  this->_canvas->update();
}


void
PlotView::onPlotDestroy()
{
  // If the plot got destoyed, destroy the corresponding view:
  this->deleteLater();
}


void
PlotView::onSchemeSelected(int idx)
{
  switch (idx) {
  case 0:
    this->_canvas->setScheme(Plot::Configuration::DISPLAY_SCHEME);
    break;

  case 1:
    this->_canvas->setScheme(Plot::Configuration::PRINT_SCHEME);
    break;

  default:
    break;
  }
}


void
PlotView::onConfigPlot() {
  if (! _plotitem->hasConfig()) { return; }
  Plot::PlotConfigDialog dialog(_plotitem->config());
  if (QDialog::Accepted != dialog.exec()) { return; }
  _plotitem->updatePlot();
  _canvas->setPlot(_plotitem->getPlot());
  _canvas->getPlot()->updateAxes();
  _canvas->getPlot()->updateAxes();
  _canvas->update();
}
