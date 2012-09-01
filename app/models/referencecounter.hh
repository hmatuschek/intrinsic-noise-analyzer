#ifndef __INA_APP_MODELS_REFERENCECOUNTER_HH__
#define __INA_APP_MODELS_REFERENCECOUNTER_HH__

#include "ast/ast.hh"
#include <QList>
#include <QObject>


/** This class terverses the @c Ast::Model and collects all references to a defined species.
 * This class is used within the model editor to determine if a variable can savely be deleted. */
class ReferenceCounter
    : public QObject, public iNA::Ast::Visitor, public iNA::Ast::Compartment::Visitor,
    public iNA::Ast::Species::Visitor, public iNA::Ast::Parameter::Visitor,
    public iNA::Ast::AlgebraicConstraint::Visitor, public iNA::Ast::Reaction::Visitor,
    public iNA::Ast::KineticLaw::Visitor
{
  Q_OBJECT

public:
  explicit ReferenceCounter(iNA::Ast::VariableDefinition *var, QObject *parent=0);

  void visit(const iNA::Ast::Compartment *var);
  void visit(const iNA::Ast::Species *var);
  void visit(const iNA::Ast::Parameter *var);
  void visit(const iNA::Ast::AlgebraicConstraint *var);
  void visit(const iNA::Ast::Reaction *reac);
  void visit(const iNA::Ast::KineticLaw *law);

  const QList<QString> &references() const;

private:
  /** Counts the references of this variable. */
  iNA::Ast::VariableDefinition *_var;
  /** Collects the references found. */
  QList<QString> _references;
};


#endif // REFERENCECOUNTER_HH
