#include "fileselector.hh"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QCompleter>
#include <QFileSystemModel>


FileSelector::FileSelector(Mode mode, const QString &filter, QWidget *parent) :
  QWidget(parent), _mode(mode), _filter(filter)
{
  _filename = new QLineEdit();
  _choose = new QPushButton("choose");

  // Orgastic cmd-line experience:
  QCompleter *completer = new QCompleter(this);
  QFileSystemModel *fsmodel = new QFileSystemModel(completer);
  fsmodel->setRootPath(QDir::rootPath());
  completer->setModel(fsmodel);
  completer->setCompletionMode(QCompleter::InlineCompletion);
  _filename->setCompleter(completer);
  _filename->setMinimumWidth(300);

  // Assemble layout
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->addWidget(_filename);
  layout->addWidget(_choose);
  setLayout(layout);

  QObject::connect(_choose, SIGNAL(clicked()), this, SLOT(__on_choose_file()));
  QObject::connect(_filename, SIGNAL(editingFinished()), this, SLOT(__on_filename_changed()));
}


void FileSelector::setFilter(const QString &filter) { _filter = filter; }


void FileSelector::__on_choose_file() {
  QString filename;
  if (OpenFile == _mode) {
    filename = QFileDialog::getOpenFileName(0, "Choose file...", "", _filter);
  } else if (SaveFile == _mode) {
    filename = QFileDialog::getSaveFileName(0, "Choose file...", "", _filter);
  } else if (Directory == _mode) {
    filename = QFileDialog::getExistingDirectory(0, "Choose directory...", "");
  }

  _filename->setText(filename);
  emit fileSelected(filename);
}


QString FileSelector::getFileName() const { return _filename->text(); }

void FileSelector::__on_filename_changed() {
  emit fileSelected(_filename->text());
}
