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
#include <parser/expr/parser.hh>
#include "../tinytex/formula.hh"



/** This class collects symbols in the kinetic law expression of the reaction that are not defined
 * in the given scope. */
class ReactionEditorContext : public iNA::Parser::Expr::ScopeContext
{
public:
  /** Constructor.
   * @param root Defines the root scope of the context. All symbols that are not defined in this
   *        scope are collected. */
  ReactionEditorContext(iNA::Ast::Scope *root);

  /** Resolves or creates a symbol for the given identifier. */
  virtual GiNaC::symbol resolve(const std::string &identifier);

  /** Returns the table (id -> symbol) of undefined symbols in the expression. */
  const std::map<std::string, GiNaC::symbol> &undefinedSymbols() const;

private:
  /** Holds the list of undefined symbols during expression parsing. */
  std::map<std::string, GiNaC::symbol> _undefined_symbols;
};



/** A wizard to create or edit reactions. */
class ReactionEditor : public QWizard
{
  Q_OBJECT

public:
  explicit ReactionEditor(iNA::Ast::Model &model, QWidget *parent=0);

  /** Returns a weak reference to the model instance. */
  iNA::Ast::Model &model();

  /** Returns a weak reference to the defined reaction (or 0 if there is no reaction defined). */
  iNA::Ast::Reaction *reaction();

  /** REsets the current reaction scope. This scope holds the reaction definition
   * as well as all newly created species for that scope. */
  void setReactionScope(iNA::Ast::Scope *reaction_scope);

  /** Commits the reaction and species defined in the current reaction scope to the model. */
  void commitReactionScope();

private:
  /** A weak reference to the model. */
  iNA::Ast::Model &_model;
  iNA::Ast::Scope *_current_reaction_scope;
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
  void _onMassActionToggled(bool state);

  /** Implements the automatic update of the kinetic law. */
  void _updateKineticLaw();


private:
  /** Parses a (incomplete) reaction equation. */
  bool _parseEquation(const QString &text, QList< QPair<int, QString> > &reactants,
                     QList< QPair<int, QString> > &product, bool &reversible);
  /** Parses a single stoichiometry expression. */
  bool _parseStoichiometry(const QString &text, QList< QPair<int, QString> > &stoichiometry);
  /** Helper function to parse identifier. */
  bool _parseIdentifier(QString &text);

  /** Helper function to render a factor. */
  MathItem *_assembleFactor(QString &id, int exponent);
  /** Assembles the name of the compartment. */
  MathItem *_assembleCompartment();
  /** Assembles the name of an identifier. */
  MathItem *_assembleName(const QString &id);

  /** Creates a species definition for each undefined species in the stoichiometry. */
  void _defineUnknownSpecies(QList< QPair<int, QString> > &reactants,
                             QList< QPair<int, QString> > &products,
                             iNA::Ast::Scope *scope);

  /** Creates a new reaction for the given products and reactants, kinetic law is initially empty. */
  iNA::Ast::Reaction *_createReaction(const QString &name, QList< QPair<int, QString> > &reactants,
                                      QList< QPair<int, QString> > &products, bool is_reversible,
                                      iNA::Ast::Scope *scope);

  /** Creates the kinetic law for the given reaction. */
  void _createKineticLaw(iNA::Ast::Reaction *reaction);

  /** Creates the mass action kinetic law for the given reaction. */
  void _createMassActionKineticLaw(iNA::Ast::Reaction *reaction);

  /** Creates the kinetic law for this reaction from the kinetic law expression editor. */
  void _parseAndCreateKineticLaw(iNA::Ast::Reaction *reaction);

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


/**
 * A simple wizard page summarizing the reaction.
 */
class ReactionEditorSummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit ReactionEditorSummaryPage(ReactionEditor *wizard);

  /** Updates the reaction preview. */
  virtual void initializePage();

private:
  /** Holds the weak reference to the model. */
  iNA::Ast::Model &_model;
  /** Displays the preview of the reaction. */
  QLabel *_reaction_preview;
};


#endif // REACTIONEDITOR_HH
