#include "exportmodeldialog.hh"
#include <QFormLayout>
#include <QFileInfo>


ExportModelDialog::ExportModelDialog(QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle("Export Model...");

  _file_selector = new FileSelector(
        FileSelector::SaveFile,
        tr("SBML models (*.xml *.sbml);;SBML-SH models (*.mod *.sbmlsh)"));
  _file_selector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  _format_selector = new QComboBox();
  _format_selector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  _format_selector->addItem(tr("SBML Model"), unsigned(SBML_MODEL));
  _format_selector->addItem(tr("SBML-SH Model"), unsigned(SBMLSH_MODEL));
  _format_selector->setCurrentIndex(0);

  _buttons = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);

  QFormLayout *form_layout = new QFormLayout();
  form_layout->addRow(tr("Select a file:"), _file_selector);
  form_layout->addRow(tr("Select the format:"), _format_selector);
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
ExportModelDialog::__on_filename_selected(QString filename)
{
  // Get file extension:
  QFileInfo info(filename);

  if ( ("xml" == info.suffix()) || ("sbml" == info.suffix()) ) {
    _format_selector->setCurrentIndex(0);
  } else if ( ("mod" == info.suffix()) || ("sbmlsh" == info.suffix()) ){
    _format_selector->setCurrentIndex(1);
  }
}


QString
ExportModelDialog::getFileName() const
{
  return _file_selector->getFileName();
}


ExportModelDialog::Format
ExportModelDialog::getFormat() const
{
  return Format(_format_selector->itemData(_format_selector->currentIndex()).toUInt());
}
