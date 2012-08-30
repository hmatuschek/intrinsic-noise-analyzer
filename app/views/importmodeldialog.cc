#include "importmodeldialog.hh"
#include <QFormLayout>
#include <QFileInfo>


ImportModelDialog::ImportModelDialog(QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle("Import model");

  _file_selector = new FileSelector(
        FileSelector::OpenFile,
        tr("SBML/shorthand (*.xml *.sbml *.mod *.sbmlsh);;SBML format (*.xml *.sbml);;SBML shorthand format (*.mod *.sbmlsh)"));
  _file_selector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  _format_selector = new QComboBox();
  _format_selector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  _format_selector->addItem(tr("SBML"), unsigned(SBML_MODEL));
  _format_selector->addItem(tr("SBML shorthand"), unsigned(SBMLSH_MODEL));
  _format_selector->setCurrentIndex(0);

  _buttons = new QDialogButtonBox(QDialogButtonBox::Open|QDialogButtonBox::Cancel);

  QFormLayout *form_layout = new QFormLayout();
  form_layout->addRow(tr("Model file"), _file_selector);
  form_layout->addRow(tr("Format"), _format_selector);
  form_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(form_layout);
  layout->addWidget(_buttons);
  setLayout(layout);

  QObject::connect(_file_selector, SIGNAL(fileSelected(QString)),
                   this, SLOT(__on_filename_selected(QString)));
  QObject::connect(_buttons, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(_buttons, SIGNAL(rejected()), this, SLOT(reject()));

  setModal(true);
}


void
ImportModelDialog::__on_filename_selected(QString filename)
{
  // Get file extension:
  QFileInfo info(filename);

  if ( ("xml" == info.suffix()) || ("sbml" == info.suffix()) ) {
    _format_selector->setCurrentIndex(0);
  } else if ( ("mod" == info.suffix()) || ("sbmlsh" == info.suffix()) ){
    _format_selector->setCurrentIndex(1);
  }

  if (info.exists()) {
    _buttons->button(QDialogButtonBox::Open)->setEnabled(true);
  } else {
    _buttons->button(QDialogButtonBox::Open)->setEnabled(false);
  }
}


QString
ImportModelDialog::getFileName() const
{
  return _file_selector->getFileName();
}


ImportModelDialog::Format
ImportModelDialog::getFormat() const
{
  return Format(_format_selector->itemData(_format_selector->currentIndex()).toUInt());
}
