#ifndef __INA_APP_VIEWS_LOGWINDOW_HH__
#define __INA_APP_VIEWS_LOGWINDOW_HH__

#include <QTableView>
#include <QString>
#include <QComboBox>

#include "utils/logger.hh"
#include "../models/logmessagemodel.hh"


/** A simple log-table widget. */
class LogTable : public QTableView
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit LogTable(QWidget *parent = 0);

public slots:
  /** Saves the log messages into the given file.
   * Returns @c true on success. */
  bool saveLog(const QString &filename);
  /** Sets the filter level. */
  void setFilterLevel(iNA::Utils::Message::Level level);

protected slots:
  /** Will be called on new messages being visible, scrolles to bottom. */
  virtual void rowsInserted(const QModelIndex &parent, int start, int end);

private:
  /** Holds the message model. */
  FilteredLogMessageModel *_model;
};


/** The log window. */
class LogWindow : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  LogWindow(QWidget *parent=0);

private slots:
  /** Callback for the save button. */
  void onSaveLog();
  /** Callback for the level selector. */
  void onLevelSelected(int index);

private:
  /** Holds the logtable instance. */
  LogTable *_logtable;
  /** Holds the log level selector. */
  QComboBox *_level_selector;
};

#endif
