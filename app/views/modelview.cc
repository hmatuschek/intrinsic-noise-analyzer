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
  // Basic layout
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // label
  QLabel *label = new QLabel(tr("Model %1").arg(_model->getName().c_str()));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  label->setAlignment(Qt::AlignRight);

  // Unit box
  QGroupBox *unit_frame = new QGroupBox(tr("Units"));
  unit_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  QFormLayout *unit_layout = new QFormLayout();
  unit_layout->addRow(tr("Substance unit"), new UnitEditor(_model->getSubstanceUnit()));
  unit_layout->addRow(tr("Volume unit"), new UnitEditor(_model->getVolumeUnit()));
  unit_layout->addRow(tr("Area unit"), new UnitEditor(_model->getAreaUnit()));
  unit_layout->addRow(tr("Length unit"), new UnitEditor(_model->getLengthUnit()));
  unit_layout->addRow(tr("Time unit"), new UnitEditor(_model->getTimeUnit()));
  unit_frame->setLayout(unit_layout);

  // Layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0);
  layout->addWidget(unit_frame, 1);
  setLayout(layout);

  // Connect to "desroyed" signal of model_item to close view.
  QObject::connect(model_item, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
