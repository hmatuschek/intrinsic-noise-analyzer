#include "logwindow.hh"
#include <QDateTime>
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include "../models/logmessagemodel.hh"




LogTable::LogTable(QWidget *parent)
  : QTableView(parent), _model(0)
{
  verticalHeader()->setVisible(false);
  horizontalHeader()->setStretchLastSection(true);
  setShowGrid(false);
  setMinimumSize(640, 100);

  // Install message model:
  _model = new FilteredLogMessageModel(this);
  setModel(_model);
}


bool
LogTable::saveLog(const QString &filename)
{
  QFile file(filename);
  if (! file.open(QFile::WriteOnly)) {
    QMessageBox::critical(
          0, tr("Can not create/open file"),
          tr("Can not write log to file %1: Can not be opened for writing.").arg(filename));
    return false;
  }

  _model->saveLog(file);
  file.close();
  return true;
}


/*  case iNA::Utils::Message::DEBUG:
    time_item->setTextColor(QColor::fromRgb(0xa0, 0xa0, 0xa0));
    text_item->setTextColor(QColor::fromRgb(0xa0, 0xa0, 0xa0));
    break;

  case iNA::Utils::Message::INFO:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xff, 0xff));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xff, 0xff));
    break;

  case iNA::Utils::Message::WARN:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xf0, 0xc0));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xf0, 0xc0));
    break;

  case iNA::Utils::Message::ERROR:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xc0, 0xc0));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xc0, 0xc0));
    break;
  }

  this->scrollToItem(time_item, QAbstractItemView::PositionAtBottom);
*/


void
LogTable::setFilterLevel(iNA::Utils::Message::Level level) {
  _model->setFilterLevel(level);
}


void
LogTable::rowsInserted(const QModelIndex &parent, int start, int end) {
  QTableView::rowsInserted(parent, start, end);
  scrollToBottom();
}


/* ******************************************************************************************** *
 * Implementation of LogWindow.
 * ******************************************************************************************** */
LogWindow::LogWindow(QWidget *parent)
  : QWidget(parent)
{
  setWindowTitle(tr("Log Messages"));

  // Create log view:
  _logtable = new LogTable();
  // Save button:
  QPushButton *save_button = new QPushButton(tr("Save"));
  save_button->setToolTip(tr("Saves the log into a file."));
  // level selector
  _level_selector = new QComboBox();
  _level_selector->setToolTip(tr("Selects the log-level to filter messages."));
  _level_selector->addItem(tr("Debug"));
  _level_selector->addItem(tr("Info"));
  _level_selector->addItem(tr("Warning"));
  _level_selector->addItem(tr("Error"));
  _level_selector->setCurrentIndex(0);

  // Assemble layout
  QHBoxLayout *buttons = new QHBoxLayout();
  buttons->addStretch(1);
  buttons->addWidget(_level_selector);
  buttons->addWidget(save_button);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  layout->addWidget(_logtable);
  layout->addLayout(buttons);
  setLayout(layout);

  QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(onSaveLog()));
  QObject::connect(_level_selector, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(onLevelSelected(int)));
}

void
LogWindow::onSaveLog() {
  QString filename = QFileDialog::getSaveFileName(
        0, tr("Save log to"), "", tr("Text files (*.txt)"));
  if (0 == filename.size()) { return; }
  _logtable->saveLog(filename);
}

void
LogWindow::onLevelSelected(int index) {
  switch (index) {
  case 0: _logtable->setFilterLevel(iNA::Utils::Message::DEBUG); break;
  case 1: _logtable->setFilterLevel(iNA::Utils::Message::INFO); break;
  case 2: _logtable->setFilterLevel(iNA::Utils::Message::WARN); break;
  case 3: _logtable->setFilterLevel(iNA::Utils::Message::ERROR); break;
  default: break;
  }
}
