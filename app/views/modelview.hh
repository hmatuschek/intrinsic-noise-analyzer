#ifndef __INA_APP_VIEWS_MODELVIEW_HH__
#define __INA_APP_VIEWS_MODELVIEW_HH__

#include <QWidget>
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
};

#endif // MODELVIEW_HH
