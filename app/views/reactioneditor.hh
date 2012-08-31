#ifndef __INA_APP_VIEWS_REACTIONEDITOR_HH__
#define __INA_APP_VIEWS_REACTIONEDITOR_HH__

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QList>
#include <QPair>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>

#include <ast/model.hh>
#include "../tinytex/formula.hh"


/** Simple editor dialog to create/modify a reaction. */
class ReactionCreator : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  ReactionCreator(iNA::Ast::Model &model, QWidget *parent = 0);

private slots:
  /** Called if the automatic update of the kinetic law is toggled. */
  void onMassActionToggled(bool state);

  /** Implements the automatic update of the kinetic law. */
  void updateKineticLaw();

private:
  /** Parses a (incomplete) reaction equation. */
  bool parseEquation(const QString &text, QList< QPair<int, QString> > &reactants,
                     QList< QPair<int, QString> > &product, bool &reversible);
  /** Parses a single stoichiometry expression. */
  bool parseStoichiometry(const QString &text, QList< QPair<int, QString> > &stoichiometry);
  /** Helper function to parse identifier. */
  bool parseIdentifier(QString &text);

  /** Helper function to render a factor. */
  MathItem *assembleFactor(QString &id, int exponent);
  /** Assembles the name of the compartment. */
  MathItem *assembleCompartment();
  /** Assembles the name of an identifier. */
  MathItem *assembleName(const QString &id);

private:
  /** Holds a weak reference to the model. */
  iNA::Ast::Model &_model;
  /** Line edit for the model name. */
  QLineEdit *_name;
  /** Line editor for the reaction equation. */
  QLineEdit *_equation;
  /** Holds a line edit and a view for the kinetic law. */
  QStackedWidget *_kineticLaw;
  /** The line-editor to edit the kinetic law directly. */
  QLineEdit *_kineticLawEditor;
  /** View of the kinetic law. */
  QLabel    *_kineticLawFormula;
  /** Checkbox to enable the automatic update of the kinetic law. */
  QCheckBox *_autoupdate;
  /** A delay timer for the update function. */
  QTimer    *_delayTimer;
};


#endif // REACTIONEDITOR_HH
