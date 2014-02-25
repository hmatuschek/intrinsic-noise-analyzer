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
#include "../tinytex/ginac2formula.hh"


/** This class collects all changes being made to the model or reaction, means all compartments
 * species and reaction local parameters that are created along with a reaction. It also
 * implements the @c iNA::Parser::Expr::ScopeContext interface, hence it can be used to
 * resolve symbols in expressions. During parsing, the context collects and hence defines
 * some symbols that are not defined in a present scope.
 * If a new reaction is created, the root scope is the model. If an existing reaction is
 * edited, the root scope is the kinetic law of the reaction being edited. This allows
 * to resolve the reaction local parameters already defined. */
class ReactionEditorContext : public iNA::Parser::Expr::ScopeContext
{
public:
  /** Constructor.
   * @param root Defines the scope of the context. All symbols that are not defined in this
   *        scope are collected as new (reaction local) parameters. */
  ReactionEditorContext(iNA::Ast::Model *model, iNA::Ast::Scope *root=0);

  /** Resolves or creates a symbol for the given identifier. */
  virtual GiNaC::symbol resolve(const std::string &identifier);
  /** Resolves the given symbol to the identifier of the variable. */
  virtual std::string identifier(GiNaC::symbol symbol);

  /** Returns the table (id -> symbol) of undefined symbols in the expression. */
  const std::map<std::string, GiNaC::symbol> &undefinedParameters() const;
  GiNaC::symbol getOrCreateLocalParameter(const std::string &id);

  /** Returns @c true if the given ID is a symbol for a local parameter, species or compartment or
   * if it is defined within the root scope. */
  bool hasIdentifier(const std::string &id) const;

  /** Returns the list of undefined species that need to be defined in the model. */
  const std::map<std::string, GiNaC::symbol> &undefinedSpecies() const;
  bool hasSpecies(const std::string &id) const;
  GiNaC::symbol getOrCreateSpecies(const std::string &id);

  /** Returns the first compartment defined in the model or creates a new "dummy" symbol and
   * ID for it. */
  const GiNaC::symbol &compartmentSymbol() const;
  const std::string &compartmentIdentifier() const;
  bool compartmentIsUndefined() const;

  /** Returns true if the model is defined in concentration units. */
  bool hasConcentrationUnits() const;

  /** Resets the context. All pre-defined species and parameters are removed. */
  void reset();

private:
  /** Holds a weak reference to the model. */
  iNA::Ast::Model *_model;
  /** Holds the list of undefined parameter symbols. */
  std::map<std::string, GiNaC::symbol> _param_symbols;
  /** Reverse lookup table for undefined parameter symbols. */
  std::map<GiNaC::symbol, std::string, GiNaC::ex_is_less> _param_ids;
  /** Hols the list of undefined species symbols. */
  std::map<std::string, GiNaC::symbol> _species_symbols;
  /** Hols the list of undefined species symbols. */
  std::map<GiNaC::symbol, std::string, GiNaC::ex_is_less> _species_ids;
  /** The symbol of the first compartment in the model or a dummy symbol for it. */
  GiNaC::symbol _compartment_symbol;
  /** The ID of the first compartment in the model or a dummy for it. */
  std::string _compartment_id;
  /** If true, the compartment symbol is a dummy. */
  bool _compartment_undefined;
};


/** A simple wrapper class that wraps a ReactionEditorContext used to render expressions
 * refering to pre-defined species, compartments and parameters properly. */
class ReactionEditorDisplayContext: public ExpressionContext {
public:
  ReactionEditorDisplayContext(ReactionEditorContext &context);

  /** Resolves or creates a symbol for the given identifier. */
  virtual GiNaC::symbol resolve(const std::string &identifier);

  /** Resolves the given symbol to the identifier of the variable. */
  virtual std::string identifier(GiNaC::symbol symbol);

  bool hasConcentrationUnits(const GiNaC::symbol &symbol);

protected:
  ReactionEditorContext &_context;
};


typedef QPair<int, QString> StoichiometryPair;
typedef QList<StoichiometryPair> StoichiometryList;


/** A wizard to create or edit reactions. */
class ReactionEditor : public QWizard
{
  Q_OBJECT

public:
  /** Creates the reaction editor wizard. If @c reaction != 0, the reaction is edited, otherwise a
   * new reaction is created. */
  explicit ReactionEditor(iNA::Ast::Model &model, iNA::Ast::Reaction *reaction=0, QWidget *parent=0);

  /** Returns a weak reference to the model instance. */
  iNA::Ast::Model &model();

  /** Returns a weak reference to the defined reaction (or 0 if there is no reaction, means a
   * new reaction is created). */
  iNA::Ast::Reaction *reaction();

  ReactionEditorContext &context();

  const StoichiometryList &reactants() const;
  StoichiometryList &reactants();
  void setReactants(const StoichiometryList &list);

  const StoichiometryList &products() const;
  StoichiometryList &products();
  void setProducts(const StoichiometryList &list);

  const GiNaC::ex &kineticLaw() const;
  GiNaC::ex &kineticLaw();
  void setKinteticLaw(const GiNaC::ex &expr);

  bool isReversible() const;
  void setReversible(bool rev);

  const QString &reactionName() const;
  void setReactionName(const QString &name);

private:
  /** A weak reference to the model. */
  iNA::Ast::Model &_model;
  /** Holds the reaction being edited. If 0, a new reaction is created. */
  iNA::Ast::Reaction *_current_reaction;
  /** Holds the current reaction editor context that collects the defined compartments,
   * species and parameters. */
  ReactionEditorContext _context;
  /** The reactants stoichiometry. */
  StoichiometryList _reactants;
  /** The product stoichiometry. */
  StoichiometryList _products;
  /** The kintetic law. */
  GiNaC::ex _kineticLaw;
  bool _is_reversible;
  QString _reactionName;
};



/** Wizard page to define reaction & kinetic law. */
class ReactionEditorPage : public QWizardPage
{
  Q_OBJECT

protected:
  /** Defines the possible types of kinetic laws. */
  typedef enum {
    MASSACTION_SINGLE, ///< Single compartment mass action.
    MASSACTION_MULTI,  ///< Multi compartment mass action.
    USER_DEFINED       ///< User defined kinetic law.
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
  /** Implements the automatic update of the kinetic law. Get called if the type of the kinetic law
   * is not user defined. */
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
  bool _parseReactionEquation(const QString &text, QList< QPair<int, QString> > &reactants,
                     QList< QPair<int, QString> > &product, bool &reversible);
  /** Parses a single stoichiometry expression. */
  bool _parseStoichiometry(const QString &text, QList< QPair<int, QString> > &stoichiometry);
  /** Helper function to parse identifier. */
  bool _parseIdentifier(QString &text);

  /** Returns the set of compartments, the reaction takes place in.*/
  std::set<std::string> _collectCompartments(QList< QPair<int, QString> > &reactants,
                                             QList< QPair<int, QString> > &products);


  /** Collect reactants and assemble stoichiometries */
  std::map<QString,int> _collectStoichiometries(QList<QPair<int, QString> > &reactants);

  /** Renders the kinetic law. */
  MathItem *_renderKineticLaw(bool is_reversible, StoichiometryList &reactants,
                              StoichiometryList &products);

  /** Creates a species definition for each undefined species in the stoichiometry. */
  void _defineUnknownSpecies(const StoichiometryList &reactants,
                             const StoichiometryList &products);

  /** Creates the kinetic law for the given reaction. */
  GiNaC::ex _createKineticLaw(const StoichiometryList &reactants, const StoichiometryList &products,
                              bool is_reversible);

  /** Creates the mass action kinetic law for the given reaction. */
  GiNaC::ex _createMAKineticLaw(const StoichiometryList &reactants,
                                const StoichiometryList &products, bool is_reversible);
  GiNaC::ex _createMAFactor(const QString &species, GiNaC::ex stoichiometry);
  GiNaC::ex _createMASingleFactor(const QString &species, int stoichiometry);
  GiNaC::ex _createMAMultiFactor(const QString &species, int stiochiometry);

  /** Creates the kinetic law for this reaction from the kinetic law expression editor. */
  GiNaC::ex _parseAndCreateKineticLaw();

  /** Updates the current reaction. */
  void _updateCurrentReaction(const QString &name, QList< QPair<int, QString> > &reactants,
                              QList< QPair<int, QString> > &products, bool is_reversible,
                              iNA::Ast::Scope *scope);


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
  ReactionEditor *_editor;
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

protected:
  MathItem *_renderReactionPreview(const StoichiometryList &reactants,
                                   const StoichiometryList &products,
                                   const GiNaC::ex &kineticLaw, bool is_reversible);

private:
  /** Holds the weak reference to the model. */
  iNA::Ast::Model &_model;
  /** Holds a weak reference to the reaction editor. */
  ReactionEditor *_editor;
  /** Displays the preview of the reaction. */
  QLabel *_reaction_preview;
  /** Displays the species being created. */
  QLabel *_created_species;
  /** Displays the compartments being created. */
  QLabel *_created_compartments;
  /** Displays the parameters being created. */
  QLabel *_created_parameters;

};


#endif // REACTIONEDITOR_HH
