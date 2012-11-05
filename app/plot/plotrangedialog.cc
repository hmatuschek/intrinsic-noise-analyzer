#include "plotrangedialog.hh"
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QcustommLayout>
#include <QPushButton>
#include <QDialogButtonBox>


using namespace Plot;


PlotRangeDialog::PlotRangeDialog(const Range &xrange, const RangePolicy &xpolicy,
                                 const Range &yrange, const RangePolicy &ypolicy, QWidget *parent)
  : QDialog(parent), xrange(xrange), xpolicy(xpolicy), yrange(yrange), ypolicy(ypolicy)
{
  // Some stuff...
  this->setWindowTitle(tr("Set plot ranges"));

  // Construct GUI elements:
  this->xmin_edit = new QLineEdit(QString("%1").arg(xrange.min()));
  QDoubleValidator *xmin_val = new QDoubleValidator(this->xmin_edit);
  this->xmin_edit->setValidator(xmin_val);
  this->xmin_fixed = new QCheckBox();
  this->xmin_fixed->setChecked(RangePolicy::FIXED == xpolicy.getMinPolicy());
  this->xmin_fixed->setToolTip(tr("Fix plot range to..."));
  if (this->xmin_fixed->isChecked())
    this->xmin_edit->setEnabled(true);
  else
    this->xmin_edit->setEnabled(false);

  this->xmax_edit = new QLineEdit(QString("%1").arg(xrange.max()));
  QDoubleValidator *xmax_val = new QDoubleValidator(this->xmax_edit);
  this->xmax_edit->setValidator(xmax_val);
  this->xmax_fixed = new QCheckBox();
  this->xmax_fixed->setChecked(RangePolicy::FIXED == xpolicy.getMaxPolicy());
  this->xmax_fixed->setToolTip(tr("Fix plot range to..."));
  if (this->xmax_fixed->isChecked())
    this->xmax_edit->setEnabled(true);
  else
    this->xmax_edit->setEnabled(false);

  this->ymin_edit = new QLineEdit(QString("%1").arg(yrange.min()));
  QDoubleValidator *ymin_val = new QDoubleValidator(this->ymin_edit);
  this->ymin_edit->setValidator(ymin_val);
  this->ymin_fixed = new QCheckBox();
  this->ymin_fixed->setChecked(RangePolicy::FIXED == ypolicy.getMinPolicy());
  this->ymin_fixed->setToolTip(tr("Fix plot range to..."));
  if (this->ymin_fixed->isChecked())
    this->ymin_edit->setEnabled(true);
  else
    this->ymin_edit->setEnabled(false);

  this->ymax_edit = new QLineEdit(QString("%1").arg(yrange.max()));
  QDoubleValidator *ymax_val = new QDoubleValidator(this->ymax_edit);
  this->ymax_edit->setValidator(ymax_val);
  this->ymax_fixed = new QCheckBox();
  this->ymax_fixed->setChecked(RangePolicy::FIXED == ypolicy.getMaxPolicy());
  this->ymax_fixed->setToolTip(tr("Fix plot range to..."));
  if (this->ymax_fixed->isChecked())
    this->ymax_edit->setEnabled(true);
  else
    this->ymax_edit->setEnabled(false);

  // Assemble button-box:
  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Cancel |
                                                      QDialogButtonBox::Ok);

  // Construct layout:
  QcustommLayout *from_layout = new QcustommLayout();
  QHBoxLayout *xmin_layout = new QHBoxLayout();
  xmin_layout->addWidget(xmin_fixed); xmin_layout->addWidget(xmin_edit);
  from_layout->addRow(tr("Min. X:"), xmin_layout);

  QHBoxLayout *xmax_layout = new QHBoxLayout();
  xmax_layout->addWidget(xmax_fixed); xmax_layout->addWidget(xmax_edit);
  from_layout->addRow(tr("Max. X:"), xmax_layout);

  QHBoxLayout *ymin_layout = new QHBoxLayout();
  ymin_layout->addWidget(ymin_fixed); ymin_layout->addWidget(ymin_edit);
  from_layout->addRow(tr("Min. Y:"), ymin_layout);

  QHBoxLayout *ymax_layout = new QHBoxLayout();
  ymax_layout->addWidget(ymax_fixed); ymax_layout->addWidget(ymax_edit);
  from_layout->addRow(tr("Max. Y:"), ymax_layout);

  QVBoxLayout *main_layout = new QVBoxLayout();
  main_layout->addLayout(from_layout);
  main_layout->addWidget(button_box);
  this->setLayout(main_layout);

  // Connect signals:
  QObject::connect(this->xmin_fixed, SIGNAL(toggled(bool)), this, SLOT(onXMinToggled(bool)));
  QObject::connect(this->xmax_fixed, SIGNAL(toggled(bool)), this, SLOT(onXMaxToggled(bool)));
  QObject::connect(this->ymin_fixed, SIGNAL(toggled(bool)), this, SLOT(onYMinToggled(bool)));
  QObject::connect(this->ymax_fixed, SIGNAL(toggled(bool)), this, SLOT(onYMaxToggled(bool)));

  QObject::connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
}


void
PlotRangeDialog::accept()
{
  // Obtain values from line-edits:
  this->xrange = Range(this->xmin_edit->text().toDouble(),
                       this->xmax_edit->text().toDouble());

  this->yrange = Range(this->ymin_edit->text().toDouble(),
                       this->ymax_edit->text().toDouble());

  if (this->xmin_fixed->isChecked())
    this->xpolicy.setMinPolicy(RangePolicy::FIXED);
  else
    this->xpolicy.setMinPolicy(RangePolicy::AUTOMATIC);

  if (this->xmax_fixed->isChecked())
    this->xpolicy.setMaxPolicy(RangePolicy::FIXED);
  else
    this->xpolicy.setMaxPolicy(RangePolicy::AUTOMATIC);

  if (this->ymin_fixed->isChecked())
    this->ypolicy.setMinPolicy(RangePolicy::FIXED);
  else
    this->ypolicy.setMinPolicy(RangePolicy::AUTOMATIC);

  if (this->ymax_fixed->isChecked())
    this->ypolicy.setMaxPolicy(RangePolicy::FIXED);
  else
    this->ypolicy.setMaxPolicy(RangePolicy::AUTOMATIC);

  QDialog::accept();
}


const Range &
PlotRangeDialog::getXRange() const
{
  return this->xrange;
}


const RangePolicy &
PlotRangeDialog::getXRangePolicy() const
{
  return this->xpolicy;
}


const Range &
PlotRangeDialog::getYRange() const
{
  return this->yrange;
}


const RangePolicy &
PlotRangeDialog::getYRangePolicy() const
{
  return this->ypolicy;
}


void
PlotRangeDialog::onXMinToggled(bool toggled)
{
  this->xmin_edit->setEnabled(toggled);
}


void
PlotRangeDialog::onXMaxToggled(bool toggled)
{
  this->xmax_edit->setEnabled(toggled);
}


void
PlotRangeDialog::onYMinToggled(bool toggled)
{
  this->ymin_edit->setEnabled(toggled);
}


void
PlotRangeDialog::onYMaxToggled(bool toggled)
{
  this->ymax_edit->setEnabled(toggled);
}
