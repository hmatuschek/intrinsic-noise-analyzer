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
#include <QComboBox>

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
  /** Resolves the given symbol to the identifier of the variable. */
  virtual std::string identifier(GiNaC::symbol symbol);
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
  /** Creates the reaction editor wizard, if reaction != 0, the reaction equation and propensity
   * is taken from this reaction. */
  explicit ReactionEditor(iNA::Ast::Model &model, iNA::Ast::Reaction *reaction=0, QWidget *parent=0);

  /** Returns a weak reference to the model instance. */
  iNA::Ast::Model &model();

  /** Returns a weak reference to the defined reaction (or 0 if there is no reaction defined). */
  iNA::Ast::Reaction *reaction();

  iNA::Ast::Scope *reactionScope();

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

protected:
  typedef enum {
    MASSACTION_SINGLE, MASSACTION_MULTI, USER_DEFINED
  } KineticLawType;


public:
  /** Constructor. */
  ReactionEditorPage(iNA::Ast::Reaction *reaction, ReactionEditor *editor);

  /** Validates the stoichiometry/reaction equation and kinetic law. */
  virtual bool validatePage();


protected:
  /** Returns the currently selected kinetic law type. */
  KineticLawType kineticLawType() const;
  /** Resets the currently selected kinetic law type. */
  void setKineticLawType(KineticLawType type);


private slots:
  /** Implements the automatic update of the kinetic law. */
  void _updateKineticLaw();

  /** Whenever the type of kinetic law changed. */
  void _onKineticLawTypeChanged(int index);

  /** Whenever the user defined kinetic law changes. */
  void _onKineticLawExpressionChanged();


private:
  /** Serializes the reaction equation. */
  QString _serializeReactionEquation();
  /** Serializes the propensity of the reaction. */
  QString _serializePropensity();

  /** Parses a (incomplete) reaction equation. */
  bool _parseEquation(const QString &text, QList< QPair<int, QString> > &reactants,
                     QList< QPair<int, QString> > &product, bool &reversible);
  /** Parses a single stoichiometry expression. */
  bool _parseStoichiometry(const QString &text, QList< QPair<int, QString> > &stoichiometry);
  /** Helper function to parse identifier. */
  bool _parseIdentifier(QString &text);

  /** Returns the set of compartments, the reaction takes place in.*/
  std::set<iNA::Ast::Compartment *> _collectCompartments(QList< QPair<int, QString> > &reactants,
                                                         QList< QPair<int, QString> > &products);


  /** Collect reactants and assemble stoichiometries */
  std::map<QString,int> _collectStoichiometries(QList<QPair<int, QString> > &reactants);

  /** Renders the kinetic law. */
  MathItem *_renderKineticLaw(bool is_reversible, QList< QPair<int, QString> > &reactants,
                              QList< QPair<int, QString> > &products);

  /** Helper function to render a factor. */
  MathItem *_renderFactor(const QString &id, int exponent);
  /** Renders the name of the compartment of the specified species. */
  MathItem *_renderCompartmentOf(const QString &id);
  /** Renders the name of the compartment of the specified species. */
  MathItem *_renderCompartment(iNA::Ast::Compartment *compartment);
  /** Assembles the name of an identifier. */
  MathItem *_renderName(const QString &id);

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
  void _createMAKineticLaw(iNA::Ast::Reaction *reaction);
  GiNaC::ex _createMAFactor(iNA::Ast::Species *species, GiNaC::ex stoichiometry);
  GiNaC::ex _createMASingleFactor(iNA::Ast::Species *species, int stoichiometry);
  GiNaC::ex _createMAMultiFactor(iNA::Ast::Species *species, int stiochiometry);

  /** Creates the kinetic law for this reaction from the kinetic law expression editor. */
  void _parseAndCreateKineticLaw(iNA::Ast::Reaction *reaction);

  /** Updates the current reaction. */
  void _updateCurrentReaction(const QString &name, QList< QPair<int, QString> > &reactants,
                              QList< QPair<int, QString> > &products, bool is_reversible,
                              iNA::Ast::Scope *scope);

  /** Updates the kinetic law of the current reaction. */
  void _updateCurrentReactionKineticLaw();


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
  /** Combobox to select the type of kinetic law. */
  QComboBox *_kinetic_law_type;
  /** A delay timer for the update function. */
  QTimer    *_delayTimer;
  /** Holds the default background color of the equaiton editor. */
  QColor _default_background;
  /** Holds the error background color (if the reaction equation is invalid, red). */
  QColor _error_background;
  /** Holds the identifier of the expression being edited. Is empty on new reaction. */
  iNA::Ast::Reaction *_current_reaction;
};


/**
 * A simple wizard page summarizing the reaction.
 */
class ReactionEditorSummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ReactionEditorSummaryPage(ReactionEditor *wizard);

  /** Updates the reaction preview. */
  virtual void initializePage();

private:
  /** Holds the weak reference to the model. */
  iNA::Ast::Model &_model;
  /** Displays the preview of the reaction. */
  QLabel *_reaction_preview;
  /** Displays the species being created. */
  QLabel *_created_species;
};


#endif // REACTIONEDITOR_HH
