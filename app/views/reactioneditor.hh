#ifndef __INA_APP_VIEWS_REACTIONEDITOR_HH__
#define __INA_APP_VIEWS_REACTIONEDITOR_HH__

#include <QWizard>
#include <QWizardPage>
#include <QLineEdit>
#include <QCheckBox>
#include <QList>
#include <QPair>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>

#include <ast/model.hh>
#include "../tinytex/formula.hh"


/** A wizard to create or edit reactions. */
class ReactionEditor : public QWizard
{
  Q_OBJECT

public:
  explicit ReactionEditor(iNA::Ast::Model &model, QWidget *parent=0);

  /** Returns a weak reference to the model instance. */
  iNA::Ast::Model &model();

private:
  /** A weak reference to the model. */
  iNA::Ast::Model &_model;
};



/** Wizard page to define reaction & kinetic law. */
class ReactionEditorPage : public QWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  ReactionEditorPage(ReactionEditor *editor);

  /** Validates the stoichiometry/reaction equation and kinetic law. */
  virtual bool validatePage();


private slots:
  /** Called if the automatic update of the kinetic law is toggled. */
  void onMassActionToggled(bool state);

  /** Implements the automatic update of the kinetic law. */
  void updateKineticLaw();

private:
  /** Parses a (incomplete) reaction equation. */
  bool _parseEquation(const QString &text, QList< QPair<int, QString> > &reactants,
                     QList< QPair<int, QString> > &product, bool &reversible);
  /** Parses a single stoichiometry expression. */
  bool _parseStoichiometry(const QString &text, QList< QPair<int, QString> > &stoichiometry);
  /** Helper function to parse identifier. */
  bool _parseIdentifier(QString &text);

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
  /** Holds the default background color of the equaiton editor. */
  QColor _default_background;
  /** Holds the error background color (if the reaction equation is invalid, red). */
  QColor _error_background;
};


#endif // REACTIONEDITOR_HH
