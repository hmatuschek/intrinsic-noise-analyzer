#ifndef __INA_APP_VIEWS_MODELVIEW_HH__
#define __INA_APP_VIEWS_MODELVIEW_HH__

#include <QWidget>
#include <QComboBox>
#include "../doctree/modelitem.hh"
#include "uniteditor.hh"


/** Displays a short summary of a model. */
class ModelView : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ModelView(ModelItem *model_item, QWidget *parent = 0);

private slots:
  /** If the model identifier has been changed. */
  void onModelIdentifierChanged();
  /** If the model name has been changed. */
  void onModelNameChanged();
  /** Callback to update species units. */
  void onSpeciesUnitSelected(int index);
  /** Callback to update substance unit of model. */
  void onSubstanceUnitChanged();
  /** Callback to update volume unit of model. */
  void onVolumeUnitChanged();
  /** Callback to update area unit of model. */
  void onAreaUnitChanged();
  /** Callback to update length unit of model. */
  void onLengthUnitChanged();
  /** Callback to update time unit of model. */
  void onTimeUnitChanged();

private:
  /** Holds a weak reference to the model instance. */
  iNA::Ast::Model *_model;

  /** Holds the line editor for the model identifier. */
  QLineEdit *_model_identifier;
  /** Holds the line editor for the model name. */
  QLineEdit *_model_name;
  /** Holds the line editor for the model name. */
  /** Holds the unit editor for the substance unit. */
  UnitEditor *_substance_unit_editor;
  /** Holds the unit editor for the volume unit. */
  UnitEditor *_volume_unit_editor;
  /** Holds the unit editor for the area unit. */
  UnitEditor *_area_unit_editor;
  /** Holds the unit editor for the length unit. */
  UnitEditor *_length_unit_editor;
  /** Holds the unit editor for the time unit. */
  UnitEditor *_time_unit_editor;

  /** Holds the combobox to select species units in amount or concentrations. */
  QComboBox *_species_unit_selector;
};

#endif // MODELVIEW_HH
