#ifndef __FLUC_PLOT_PLOTRANGEDIALOG_HH__
#define __FLUC_PLOT_PLOTRANGEDIALOG_HH__

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>

#include "mapping.hh"


namespace Plot {

/**
 * A simple dialog to configure the plot range.
 *
 * @ingroup plot
 */
class PlotRangeDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * Constructs the dialog from the given plot ranges and ploicies.
   */
  explicit PlotRangeDialog(const Range &xrange, const RangePolicy &xpolicy,
                           const Range &yrange, const RangePolicy &ypolicy, QWidget *parent = 0);

  /**
   * Returns the selected x-range.
   */
  const Range & getXRange() const;

  /**
   * Returns the selected x-range-policy.
   */
  const RangePolicy &getXRangePolicy() const;

  /**
   * Returns the selected y-range.
   */
  const Range & getYRange() const;

  /**
   * Returns the selected y-range-policy.
   */
  const RangePolicy &getYRangePolicy() const;

  /**
   * Validates the dialog.
   */
  virtual void accept();


private:
  /**
   * Holds the selected x-range.
   */
  Range xrange;

  /**
   * Holds the selected x-range-policy.
   */
  RangePolicy xpolicy;

  /**
   * Editor for the x-range-min.
   */
  QLineEdit *xmin_edit;

  /**
   * Checkbox to select x-range-min-policy.
   */
  QCheckBox *xmin_fixed;

  /**
   * Editor for the x-range-max.
   */
  QLineEdit *xmax_edit;

  /**
   * Checkbox to select the x-range-max-policy.
   */
  QCheckBox *xmax_fixed;

  /**
   * Holds the y-range.
   */
  Range yrange;

  /**
   * Holds the y-range-policy.
   */
  RangePolicy ypolicy;

  /**
   * Editor for the y-range-min.
   */
  QLineEdit *ymin_edit;

  /**
   * Checkbox for the y-range-min-policy.
   */
  QCheckBox *ymin_fixed;

  /**
   * Editor for the y-range-max.
   */
  QLineEdit *ymax_edit;

  /**
   * Checkbox for the y-range-max-policy.
   */
  QCheckBox *ymax_fixed;


private slots:
  /**
   * Updates the x-range-min-policy.
   */
  void onXMinToggled(bool toggled);

  /**
   * Updates the x-range-max-policy.
   */
  void onXMaxToggled(bool toggled);

  /**
   * Updates the y-range-min-policy.
   */
  void onYMinToggled(bool toggled);

  /**
   * Updates the y-range-max-policy.
   */
  void onYMaxToggled(bool toggled);
};

}


#endif
