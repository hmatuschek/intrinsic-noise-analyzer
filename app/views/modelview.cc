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
  : QWidget(parent), _model_item(model_item), _model(&(model_item->getModel()))
{
  // Basic layout
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // label
  _title = new QLabel(tr("Model %1").arg(_model->getName().c_str()));
  _title->setFont(Application::getApp()->getH1Font());
  _title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  _title->setAlignment(Qt::AlignRight);

  // Assemble model identifier & name
  _model_identifier = new QLineEdit(_model->getIdentifier().c_str());
  _model_name = new QLineEdit(_model->getName().c_str());

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

  // Identifier & name box:
  QGroupBox *name_frame = new QGroupBox(tr("General"));
  name_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  QFormLayout *name_layout = new QFormLayout();
  name_layout->addRow(tr("Identifier"), _model_identifier);
  name_layout->addRow(tr("Display name"), _model_name);
  name_frame->setLayout(name_layout);

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
  layout->addWidget(_title, 0);
  layout->addWidget(name_frame, 0);
  layout->addWidget(unit_frame, 0);
  layout->addStretch(1);
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
  QObject::connect(
        _model_identifier, SIGNAL(editingFinished()), this, SLOT(onModelIdentifierChanged()));
  QObject::connect(
        _model_name, SIGNAL(editingFinished()), this, SLOT(onModelNameChanged()));
}


void
ModelView::onModelIdentifierChanged()
{
  if (! QRegExp("[_a-zA-Z][_a-zA-Z0-9]*").exactMatch(_model_identifier->text())) {
    QMessageBox::critical(0, tr("Invalid model identifier"),
                          tr("Model identifier must constist of alpha-numeric chars and '_', "
                             "and must not start with a number."));
    _model_identifier->setText(_model->getIdentifier().c_str());
    return;
  }

  // Or simply set identifier.
  _model->setIdentifier(_model_identifier->text().toStdString());

  // Update view:
  onUpdateModelView();
}

void
ModelView::onModelNameChanged()
{
  // Simply set the name
  _model->setName(_model_name->text().simplified().toStdString());

  // Update model document label.
  DocumentItem *document = dynamic_cast<DocumentItem *>(_model_item->getTreeParent());
  if (0 != document) {
    document->updateItemData();
  }
  // Update view:
  onUpdateModelView();
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

void
ModelView::onUpdateModelView() {
  // Update labels:
  _model_identifier->setText(_model->getIdentifier().c_str());
  _model_name->setText(_model->getName().c_str());
  _title->setText(QString("Model %1").arg(_model_name->text().simplified()));

  // Update units:
  _substance_unit_editor->setUnit(_model->getSubstanceUnit());
  _volume_unit_editor->setUnit(_model->getVolumeUnit());
  _area_unit_editor->setUnit(_model->getAreaUnit());
  _length_unit_editor->setUnit(_model->getLengthUnit());
  _time_unit_editor->setUnit(_model->getTimeUnit());
}
