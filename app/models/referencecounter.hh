#ifndef __INA_APP_MODELS_REFERENCECOUNTER_HH__
#define __INA_APP_MODELS_REFERENCECOUNTER_HH__

#include "ast/ast.hh"
#include <QList>
#include <QObject>


/** This class terverses the @c Ast::Model and collects all references to a defined species.
 * This class is used within the model editor to determine if a variable can savely be deleted. */
class ReferenceCounter
    : public QObject, public Fluc::Ast::Visitor, public Fluc::Ast::Compartment::Visitor,
    public Fluc::Ast::Species::Visitor, public Fluc::Ast::Parameter::Visitor,
    public Fluc::Ast::AlgebraicConstraint::Visitor, public Fluc::Ast::Reaction::Visitor,
    public Fluc::Ast::KineticLaw::Visitor
{
  Q_OBJECT

public:
  explicit ReferenceCounter(Fluc::Ast::VariableDefinition *var, QObject *parent=0);

  void visit(const Fluc::Ast::Compartment *var);
  void visit(const Fluc::Ast::Species *var);
  void visit(const Fluc::Ast::Parameter *var);
  void visit(const Fluc::Ast::AlgebraicConstraint *var);
  void visit(const Fluc::Ast::Reaction *reac);
  void visit(const Fluc::Ast::KineticLaw *law);

  const QList<QString> &references() const;

private:
  /** Counts the references of this variable. */
  Fluc::Ast::VariableDefinition *_var;
  /** Collects the references found. */
  QList<QString> _references;
};


#endif // REFERENCECOUNTER_HH
