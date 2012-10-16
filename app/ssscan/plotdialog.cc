#include "plotdialog.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>


using namespace iNA;


SSScanPlotDialog::SSScanPlotDialog(Ast::Model *model, QWidget *parent)
  : QDialog(parent), _model(model)
{
  // Allocate label.
  _head_label = new QLabel();
  _head_label->setVisible(false);

  _species_list = new QListWidget();
  for(size_t i=0; i<_model->numSpecies(); i++)
  {
    // Determine name of species:
    QString name = _model->getSpecies(i)->getIdentifier().c_str();
    QString id   = _model->getSpecies(i)->getIdentifier().c_str();
    if (_model->getSpecies(i)->hasName()) {
      name = _model->getSpecies(i)->getName().c_str();
    }

    // Create list item for species
    QListWidgetItem *item = new QListWidgetItem(name, _species_list);
    item->setData(Qt::UserRole, id);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    _species_list->addItem(item);
  }

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_head_label);
  layout->addWidget(_species_list);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccepted()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


QList<QString>
SSScanPlotDialog::getSelectedSpecies() const {
  // Assemble list of enabled species:
  QList<QString> selected_species;
  for(size_t i=0; i<_model->numSpecies(); i++) {
    if (Qt::Checked == _species_list->item(i)->checkState()) {
      selected_species.append(
            _species_list->item(i)->data(Qt::UserRole).toString());
    }
  }
  return selected_species;
}


void
SSScanPlotDialog::setTitle(const QString &text) {
  _head_label->setText(text);
  _head_label->setVisible(true);
}

void
SSScanPlotDialog::onAccepted() {
  QList<QString> selected_species = getSelectedSpecies();
  if (0 == selected_species.size()) { return; }
  accept();
}
