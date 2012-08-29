#include "modelview.hh"

#include "unitrenderer.hh"
#include "uniteditor.hh"
#include "../application.hh"

#include <QLabel>
#include <QPalette>
#include <QGroupBox>
#include <QFormLayout>


ModelView::ModelView(ModelItem *model_item, QWidget *parent)
  : QWidget(parent), _model(&(model_item->getModel()))
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  QLabel *label = new QLabel(tr("Model Parameters"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  QGroupBox *unit_frame = new QGroupBox(tr("Units"));
  QFormLayout *unit_layout = new QFormLayout();
  unit_layout->addRow(tr("Substance unit"), new UnitEditor(_model->getDefaultSubstanceUnit()));
  unit_layout->addRow(tr("Volume unit"), new UnitEditor(_model->getDefaultVolumeUnit()));
  unit_layout->addRow(tr("Area unit"), new UnitEditor(_model->getDefaultAreaUnit()));
  unit_layout->addRow(tr("Length unit"), new UnitEditor(_model->getDefaultLengthUnit()));
  unit_layout->addRow(tr("Time unit"), new UnitEditor(_model->getDefaultTimeUnit()));
  unit_frame->setLayout(unit_layout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  layout->addWidget(unit_frame);
  setLayout(layout);

  // Connect to "desroyed" signal of model_item to close view.
  QObject::connect(model_item, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
