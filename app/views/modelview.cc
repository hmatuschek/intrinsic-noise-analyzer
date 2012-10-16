#include "modelview.hh"

#include "unitrenderer.hh"
#include "uniteditor.hh"
#include "../application.hh"
#include <utils/logger.hh>

#include <QLabel>
#include <QPalette>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>


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

  // Selector concentrations/amount
  _species_unit_selector = new QComboBox();
  _species_unit_selector->addItem(tr("concentrations"));
  _species_unit_selector->addItem(tr("amount"));
  if (_model->speciesHaveSubstanceUnits()) { _species_unit_selector->setCurrentIndex(1); }
  else { _species_unit_selector->setCurrentIndex(0); }

  // Editors:
  _substance_unit_editor = new UnitEditor(_model->getSubstanceUnit());
  _volume_unit_editor    = new UnitEditor(_model->getVolumeUnit());
  _area_unit_editor      = new UnitEditor(_model->getAreaUnit());
  _length_unit_editor    = new UnitEditor(_model->getLengthUnit());
  _time_unit_editor      = new UnitEditor(_model->getTimeUnit());

  // Unit box
  QGroupBox *unit_frame = new QGroupBox(tr("Units"));
  unit_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  QFormLayout *unit_layout = new QFormLayout();
  unit_layout->addRow(tr("Species in"), _species_unit_selector);
  unit_layout->addRow(tr("Substance unit"), _substance_unit_editor);
  unit_layout->addRow(tr("Volume unit"), _volume_unit_editor);
  unit_layout->addRow(tr("Area unit"), _area_unit_editor);
  unit_layout->addRow(tr("Length unit"), _length_unit_editor);
  unit_layout->addRow(tr("Time unit"), _time_unit_editor);
  unit_frame->setLayout(unit_layout);

  // Layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0);
  layout->addWidget(unit_frame, 1);
  setLayout(layout);

  // Connect to "desroyed" signal of model_item to close view.
  QObject::connect(model_item, SIGNAL(destroyed()), this, SLOT(deleteLater()));
  QObject::connect(
        _species_unit_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(onSpeciesUnitSelected(int)));
  QObject::connect(
        _substance_unit_editor, SIGNAL(unitChanged()), this, SLOT(onSubstanceUnitChanged()));
  QObject::connect(
        _volume_unit_editor, SIGNAL(unitChanged()), this, SLOT(onVolumeUnitChanged()));
  QObject::connect(
        _area_unit_editor, SIGNAL(unitChanged()), this, SLOT(onAreaUnitChanged()));
  QObject::connect(
        _length_unit_editor, SIGNAL(unitChanged()), this, SLOT(onLengthUnitChanged()));
  QObject::connect(
        _time_unit_editor, SIGNAL(unitChanged()), this, SLOT(onTimeUnitChanged()));
}


void
ModelView::onSpeciesUnitSelected(int index)
{
  if (0 == index) {
    _model->setSpeciesHaveSubstanceUnits(false);
  } else {
    _model->setSpeciesHaveSubstanceUnits(true);
  }
}

void
ModelView::onSubstanceUnitChanged() {
  try {
    _model->setSubstanceUnit(_substance_unit_editor->unit(), false);
  } catch (iNA::Exception &err) {
    QMessageBox::critical(0, "Can not set substance unit.", err.what());
  }
}

void
ModelView::onVolumeUnitChanged() {
  try {
    _model->setVolumeUnit(_volume_unit_editor->unit(), false);
  } catch (iNA::Exception &err) {
    QMessageBox::critical(0, "Can not set volume unit.", err.what());
  }
}

void
ModelView::onAreaUnitChanged() {
  try {
    _model->setAreaUnit(_area_unit_editor->unit(), false);
  } catch (iNA::Exception &err) {
    QMessageBox::critical(0, "Can not set area unit.", err.what());
  }
}

void
ModelView::onLengthUnitChanged() {
  try {
    _model->setLengthUnit(_length_unit_editor->unit(), false);
  } catch (iNA::Exception &err) {
    QMessageBox::critical(0, "Can not set length unit.", err.what());
  }
}

void
ModelView::onTimeUnitChanged() {
  try {
    _model->setTimeUnit(_time_unit_editor->unit(), false);
  } catch (iNA::Exception &err) {
    QMessageBox::critical(0, "Can not set time unit.", err.what());
  }
}
