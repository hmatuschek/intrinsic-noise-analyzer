#ifndef SAVEDIALOG_HH
#define SAVEDIALOG_HH

#include <QDialog>


/**
 * This dialog asks the user custom the filename and resolution, the plot is saved in.
 *
 * @ingroup plot
 */
class SaveDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * Constructor.
   */
  explicit SaveDialog(QWidget *parent = 0);
};

#endif // SAVEDIALOG_HH
