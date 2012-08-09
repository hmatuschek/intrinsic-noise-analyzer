#ifndef __INA_APP_FILESELECTOR_HH__
#define __INA_APP_FILESELECTOR_HH__

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

/** Tiny widget to select a file. Opens a file dialog if choose is clicked. */
class FileSelector : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit FileSelector(const QString &filter = QString(), QWidget *parent = 0);

  /** Set filter. */
  void setFilter(const QString &filter);

  /** Returns the selected filename. */
  QString getFileName() const;

signals:
  void fileSelected(QString filename);

private slots:
  /** Callback if the "choose" button is pressed. */
  void __on_choose_file();

  /** Callback on text of line edit "_filename" changed. */
  void __on_filename_changed();

private:
  QString _filter;
  QLineEdit *_filename;
  QPushButton *_choose;
};

#endif // __INA_APP_FILESELECTOR_HH__
