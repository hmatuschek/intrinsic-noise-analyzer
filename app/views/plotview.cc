#include "plotview.hh"

#include <iostream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>


PlotView::PlotView(PlotItem *plot_wrapper, QWidget *parent) :
    QWidget(parent)
{
  // Get figure
  Plot::Figure *plot = plot_wrapper->getPlot();

  // Set proper background style:
  this->setBackgroundRole(QPalette::Window);

  // Constuct canvas:
  this->canvas = new Plot::Canvas(this);
  this->canvas->setPlot(plot);
  this->canvas->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

  // Save button:
  QPushButton *save_button = new QPushButton(tr("Save to file"));
  save_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(onSavePlot()));

  // Set plot range button:
  QPushButton *range_button = new QPushButton(tr("Set plot range"));
  range_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QObject::connect(range_button, SIGNAL(clicked()), this, SLOT(onSetPlotRange()));

  // Plot Scheme selection:
  this->schemeBox = new QComboBox();
  this->schemeBox->addItem(tr("Display"), QVariant(0));
  this->schemeBox->addItem(tr("Print"), QVariant(1));

  // Assemble panel:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->canvas);

  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(save_button);
  button_box->addWidget(range_button);
  button_box->addWidget(this->schemeBox);
  layout->addLayout(button_box);
  this->setLayout(layout);

  // If the plot-wrapper gest destroyed -> close the plot-view:
  QObject::connect(plot_wrapper, SIGNAL(destroyed()), this, SLOT(onPlotDestroy()));

  // If new scheme is selected:
  QObject::connect(this->schemeBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(onSchemeSelected(int)));
}


void
PlotView::onSavePlot()
{
  QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save plot as ..."), "", tr("png (*.png)"));

  this->canvas->saveAs(fileName, Plot::Figure::FILETYPE_PNG);
}


void
PlotView::onSetPlotRange()
{
  // Construct dialog
  Plot::PlotRangeDialog dialog(this->canvas->getPlot()->getAxis()->getXRange(),
                               this->canvas->getPlot()->getAxis()->getXRangePolicy(),
                               this->canvas->getPlot()->getAxis()->getYRange(),
                               this->canvas->getPlot()->getAxis()->getYRangePolicy(),
                               this);
  dialog.setModal(true);

  // Exec:
  if (QDialog::Rejected == dialog.exec())
    return;

  this->canvas->getPlot()->getAxis()->setXRangePolicy(dialog.getXRangePolicy());
  this->canvas->getPlot()->getAxis()->setYRangePolicy(dialog.getYRangePolicy());
  this->canvas->getPlot()->getAxis()->setXRange(dialog.getXRange().min(), dialog.getXRange().max());
  this->canvas->getPlot()->getAxis()->setYRange(dialog.getYRange().min(), dialog.getYRange().max());

  this->canvas->getPlot()->updateAxes();
  this->canvas->getPlot()->updateAxes();
  this->canvas->update();
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
    this->canvas->setScheme(Plot::Configuration::DISPLAY_SCHEME);
    break;

  case 1:
    this->canvas->setScheme(Plot::Configuration::PRINT_SCHEME);
    break;

  default:
    break;
  }
}
