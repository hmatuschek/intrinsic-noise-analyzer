#ifndef __INA_TRAFO_MODELDIFF_HH__
#define __INA_TRAFO_MODELDIFF_HH__

#include <list>
#include <vector>
#include <ast/model.hh>
#include <exception.hh>


namespace iNA {
namespace Trafo {


/** Base class of all modification items. Representing a single modification of a model. See
 * @c ModelHistory for details. */
class ModelDiffItem {
public:
  /** Constructor, does nothing. */
  ModelDiffItem() { }
  /** Destructor, does nothing. */
  virtual ~ModelDiffItem() { }

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model) = 0;
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model) = 0;

  /** Undo the modification on the given model. */
  virtual void undo(Ast::Model &model) = 0;
  /** Redo the modification on the given model. */
  virtual void redo(Ast::Model &model) = 0;
};



/** Base class for all simple modification of a single variable (Compartment, Species,
 * Parameter, ...). This class holds some information to address the variable uniquely. */
class VariableReferenceItem: public ModelDiffItem {
protected:
  /** A modification to a global variable. */
  VariableReferenceItem(const std::string &id);

  /** A modification to a local variable. */
  VariableReferenceItem(const std::string &id, const std::string &parent_id);

public:
  /** Destructor. */
  virtual ~VariableReferenceItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

protected:
  /** Helper function that searches for the addressed variable. */
  bool _hasVariable(const std::string &id, const Ast::Model &model);
  /** Helper function that searches for the addressed variable. */
  bool _hasVariable(const std::string &id, const std::string &parent, const Ast::Model &model);
  /** Helper function that returns the addressed variable. */
  Ast::VariableDefinition *_getVariable(const std::string &id, const Ast::Model &model);
  /** Helper function that returns the addressed variable. */
  Ast::VariableDefinition *_getVariable(const std::string &id, const std::string &parent, const Ast::Model &model);

protected:
  /** Holds the identifier of the addressed variable. */
  std::string _var_id;
  /** Holds the identifier of the parent (reaction) of this local variable. If empty, a global
   * variable is addressed. */
  std::string _parent_id;
};



/** Represents the simple modification of a single local or global variable.
 * @ingroup trafo */
class SetVariableIdentifierItem: public VariableReferenceItem {
public:
  /** Constructs the diff item for a simple identifier modification of a global variable. */
  SetVariableIdentifierItem(const std::string &id, const std::string &new_id);

  /** Constructs the diff item for a simple identifier modification of a local variable. */
  SetVariableIdentifierItem(
      const std::string &id, const std::string &new_id, const std::string &parent_id);

  /** Destructor. */
  virtual ~SetVariableIdentifierItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the identifier modification on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redoes the identifier modification on the given model. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the new identifier of the variable. */
  std::string _new_identifier;
};



/** Represents the simple modification of a single local or global variable.
 * @ingroup trafo */
class SetVariableNameItem: public VariableReferenceItem {
public:
  /** Constructs the diff item for a simple name modification of a global variable. */
  SetVariableNameItem(
      const std::string &id, const std::string &old_name, const std::string &new_name);

  /** Constructs the diff item for a simple name modification of a local variable. */
  SetVariableNameItem(
      const std::string &id, const std::string &parent_id,
      const std::string &old_name, const std::string &new_name);

  /** Destructor. */
  virtual ~SetVariableNameItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the name modification on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redoes the name modification on the given model. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the old name of the variable. */
  std::string _old_name;
  /** Holds the new name of the variable. */
  std::string _new_name;
};



/** Represents the simple modification of a single local or global variable.
 * @ingroup trafo */
class SetVariableConstFlagItem: public VariableReferenceItem {
public:
  /** Constructs the diff item for a simple name modification of a global variable. */
  SetVariableConstFlagItem(
      const std::string &id, bool old_state, bool new_state);

  /** Constructs the diff item for a simple name modification of a global variable. */
  SetVariableConstFlagItem(
      const std::string &id, const std::string &parent_id, bool old_state, bool new_state);

  /** Destructor. */
  virtual ~SetVariableConstFlagItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the name modification on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redoes the name modification on the given model. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the old state of the const flag of the variable. */
  bool _old_state;
  /** Holds the new state of the const flag of the variable. */
  bool _new_state;
};



/** Represents the simple modification of a single local or global variable.
 * @ingroup trafo */
class SetVariableValueItem: public VariableReferenceItem {
public:
  /** Constructs the diff item for a simple value modification of a global variable. */
  SetVariableValueItem(
      const std::string &id, const std::string &old_value, const std::string &new_value);

  /** Constructs the diff item for a simple value modification of a local variable. */
  SetVariableValueItem(
      const std::string &id, const std::string &parent_id,
      const std::string &old_value, const std::string &new_value);

  /** Destructor. */
  virtual ~SetVariableValueItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the name modification on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redoes the name modification on the given model. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the old value of the variable. */
  std::string _old_value;
  /** Holds the new value of the variable. */
  std::string _new_value;
};



/** This class represents the modification of the compartment of a species.
 * @ingroup trafo */
class SetSpeciesCompartmentItem: public VariableReferenceItem {
public:
  /** Constructor. */
  SetSpeciesCompartmentItem(
      const std::string &id, const std::string &old_compartment, const std::string &new_compartment);

  /** Destructor. */
  virtual ~SetSpeciesCompartmentItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the name modification on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redoes the name modification on the given model. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the old compartment id of the species. */
  std::string _old_compartment;
  /** Holds the new compartment id of the species. */
  std::string _new_compartment;
};



/** This class represents the modification of the unit of a parameter.
 * @ingroup trafo */
class SetParameterUnitItem: public VariableReferenceItem {
public:
  /** Constructs the modification of the unit of a global parameter. */
  SetParameterUnitItem(const std::string &id, const Ast::Unit &old_unit, const Ast::Unit &new_unit);
  /** Constructs the modification of the unit of a local parameter. */
  SetParameterUnitItem(const std::string &id, const std::string &parent,
                       const Ast::Unit &old_unit, const Ast::Unit &new_unit);

  /** Destructor. */
  virtual ~SetParameterUnitItem();

  /** Returns true if a diff item can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if a diff item can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the unit modification on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redoes the unit modification on the given model. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the old unit of the parameter. */
  Ast::Unit _old_unit;
  /** Holds the new unit of the parameter. */
  Ast::Unit _new_unit;
};



/** This class holds the complete information stored in a variable, it forms the base for the
 * removal and creation of variables. */
class FullVariableItem: public VariableReferenceItem
{
protected:
  /** Constructor, for global variables. */
  FullVariableItem(Ast::VariableDefinition *var, const Ast::Model &parent);
  /** Constructor, for local variables. */
  FullVariableItem(Ast::VariableDefinition *var, const Ast::Reaction *parent);

public:
  /** Destructor. */
  virtual ~FullVariableItem();

protected:
  /** Holds the name of the variable. */
  std::string _name;
  /** Holds the serialized expression of the value of the variable. */
  std::string _value;
  /** Holds the constant flag. */
  bool _is_constant;
};



/** Represents the addition of a species. Undo will remove the species redo will recreate the
 * species.
 * @ingroup trafo */
class AddSpeciesItem: public FullVariableItem
{
public:
  /** Constructor. */
  AddSpeciesItem(Ast::Species *species, const Ast::Model &parent);
  /** Destructor. */
  virtual ~AddSpeciesItem();

  /** Checks if the species creation can be undone. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the species can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the referenced speices. */
  virtual void undo(Ast::Model &model);
  /** Recreates the species. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the identifier of the compartment of the species. */
  std::string _compartment;
};


/** Represents the removal of a species. Undo will recreate the species redo will remove the
 * species again.
 * @ingroup trafo */
class RemSpeciesItem: public FullVariableItem
{
public:
  /** Constructor. */
  RemSpeciesItem(Ast::Species *species, const Ast::Model &parent);
  /** Destructor. */
  virtual ~RemSpeciesItem();

  /** Checks if the species creation can be undone. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the species can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the referenced speices. */
  virtual void undo(Ast::Model &model);
  /** Recreates the species. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the identifier of the compartment of the species. */
  std::string _compartment;
};



/** Represents the addition of a compartment. Undo will remove that compartment and redo
 * will recreate it.
 * @ingroup trafo */
class AddCompartmentItem: public FullVariableItem
{
public:
  /** Constructor. */
  AddCompartmentItem(Ast::Compartment *compartment, const Ast::Model &parent);
  /** Destructor. */
  virtual ~AddCompartmentItem();

  /** Checks if the compartment can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the compartment can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the compartment. */
  virtual void undo(Ast::Model &model);
  /** Recreates the compartment. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the spacial dimension of the compartment. */
  Ast::Compartment::SpatialDimension _dimension;
};


/** Represents the removal of a compartment. Undo will recreate that compartment and redo
 * will delete it.
 * @ingroup trafo */
class RemCompartmentItem: public FullVariableItem
{
public:
  /** Constructor. */
  RemCompartmentItem(Ast::Compartment *compartment, const Ast::Model &parent);
  /** Destructor. */
  virtual ~RemCompartmentItem();

  /** Checks if the compartment can be recreated. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the compartment can be removed. */
  virtual bool canRedo(const Ast::Model &model);

  /** Recreates the compartment. */
  virtual void undo(Ast::Model &model);
  /** Removes the compartment. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the spacial dimension of the compartment. */
  Ast::Compartment::SpatialDimension _dimension;
};



/** This class represents the addition of a global or local parameter. Undo will remove that
 * parameter and redo will recreate it.
 * @ingroup trafo */
class AddParameterItem: public FullVariableItem
{
public:
  /** Constructor for a global parameter. */
  AddParameterItem(Ast::Parameter *param, const Ast::Model &model);
  /** Constructor for a local parameter. */
  AddParameterItem(Ast::Parameter *param, const Ast::Reaction *reaction);
  /** Destructor. */
  virtual ~AddParameterItem();

  /** Checks if the parameter can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the parameter can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the paramter from the model. */
  virtual void undo(Ast::Model &model);
  /** Recreates the parameter. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the unit of the paramter. */
  Ast::Unit _unit;
};


/** This class represents the removal of a global or local parameter. Undo will recreate that
 * parameter and redo will remove it again.
 * @ingroup trafo */
class RemParameterItem: public FullVariableItem
{
public:
  /** Constructor for a global parameter. */
  RemParameterItem(Ast::Parameter *param, const Ast::Model &model);
  /** Constructor for a local parameter. */
  RemParameterItem(Ast::Parameter *param, const Ast::Reaction *reaction);
  /** Destructor. */
  virtual ~RemParameterItem();

  /** Checks if the parameter can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the parameter can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the paramter from the model. */
  virtual void undo(Ast::Model &model);
  /** Recreates the parameter. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the unit of the paramter. */
  Ast::Unit _unit;
};



/** This class is the basis of all reaction modifications, it only references a reaction. */
class ReactionReferenceItem: public ModelDiffItem {
protected:
  /** Hidden constructor. */
  ReactionReferenceItem(const std::string &reac_id);

public:
  /** Destructor. */
  virtual ~ReactionReferenceItem();

protected:
  /** Holds the identifier of the reaction. */
  std::string _identifier;
};


/** This class represents the creation of an empty reaction. */
class AddEmptyReactionItem: public ReactionReferenceItem
{
public:
  /** Constructor. */
  AddEmptyReactionItem(const Ast::Reaction *reaction, const Ast::Model &model);
  /** Destructor. */
  virtual ~AddEmptyReactionItem();

  /** Checks if the reaction can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the reaction can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the reaction. */
  virtual void undo(Ast::Model &model);
  /** Recreates the reaction. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the name of the reaction. */
  std::string _name;
  /** If reaction is reversible. */
  bool _is_reversible;
};


/** This class represents the removal of an empty reaction. */
class RemEmptyReactionItem: public ReactionReferenceItem
{
public:
  /** Constructor. */
  RemEmptyReactionItem(const Ast::Reaction *reaction, const Ast::Model &model);
  /** Destructor. */
  virtual ~RemEmptyReactionItem();

  /** Checks if the reaction can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the reaction can be recreated. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the reaction. */
  virtual void undo(Ast::Model &model);
  /** Recreates the reaction. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the name of the reaction. */
  std::string _name;
  /** If reaction is reversible. */
  bool _is_reversible;
};


/** This class holds the soichiometry for one species in a reaction. */
class SetStoichiometricItem: public ReactionReferenceItem
{
protected:
  /** Constructor. */
  SetStoichiometricItem(const std::string &species, GiNaC::ex old_value, GiNaC::ex new_value,
                     const Ast::Reaction *reaction, const Ast::Model &model);

public:
  /** Destructor. */
  virtual ~SetStoichiometricItem();

protected:
  /** Returns true if the old stoichiometric value is not null. */
  bool _hasOldStoichiometry();
  /** Returns true if the new stoichiometric value is not null. */
  bool _hasNewStoichiometry();

protected:
  /** Holds the identifier of the species. */
  std::string _species;
  /** Holds the old stoichiometric expression. */
  std::string _old_stoichiometry;
  /** Holds the new stoichiometric expression. */
  std::string _new_stoichiometry;
};


/** This class represents the modification of a reactant.
 * @ingroup trafo */
class SetReactantItem: public SetStoichiometricItem
{
public:
  /** Constructor. */
  SetReactantItem(const std::string &species, GiNaC::ex old_value, GiNaC::ex new_value,
                  const Ast::Reaction *reaction, const Ast::Model &model);
  /** Destructor. */
  ~SetReactantItem();

  /** Checks if the reactant can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the reactant can be added. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes a reactant from the reaction. */
  virtual void undo(Ast::Model &model);
  /** Adds the reactant to the reaction. */
  virtual void redo(Ast::Model &model);
};


/** This class represents the modification of a reaction product.
 * @ingroup trafo */
class SetProductItem: public SetStoichiometricItem
{
public:
  /** Constructor. */
  SetProductItem(const std::string &species, GiNaC::ex old_value, GiNaC::ex new_value,
                 const Ast::Reaction *reaction, const Ast::Model &model);
  /** Destructor. */
  ~SetProductItem();

  /** Checks if the reactant can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the reactant can be added. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes a reactant from the reaction. */
  virtual void undo(Ast::Model &model);
  /** Adds the reactant to the reaction. */
  virtual void redo(Ast::Model &model);
};


/** This class represents the addition of a reaction modifier.
 * @ingroup trafo */
class AddReactionModifierItem: public ReactionReferenceItem
{
public:
  /** Constructor. */
  AddReactionModifierItem(const std::string &species, const Ast::Reaction *reaction,
                          const Ast::Model &model);
  /** Destructor. */
  ~AddReactionModifierItem();

  /** Checks if the modifier can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the modifier can be added. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the modifier. */
  virtual void undo(Ast::Model &model);
  /** Adds the modifier. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the identifier of the modifier species. */
  std::string _species;
};


/** This class represents the removal of a reaction modifier.
 * @ingroup trafo */
class RemReactionModifierItem: public ReactionReferenceItem
{
public:
  /** Constructor. */
  RemReactionModifierItem(const std::string &species, const Ast::Reaction *reaction,
                          const Ast::Model &model);
  /** Destructor. */
  ~RemReactionModifierItem();

  /** Checks if the modifier can be removed. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the modifier can be added. */
  virtual bool canRedo(const Ast::Model &model);

  /** Removes the modifier. */
  virtual void undo(Ast::Model &model);
  /** Adds the modifier. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the identifier of the modifier species. */
  std::string _species;
};


/** This class represents the modification of the rate law of a reaction. */
class SetRatelawItem: public ReactionReferenceItem
{
public:
  /** Constructor. */
  SetRatelawItem(GiNaC::ex old_value, GiNaC::ex new_value,
                 const Ast::Reaction *reaction, const Ast::Model &model);
  /** Destructor. */
  virtual ~SetRatelawItem();

  /** Checks if the rate law can be reset. */
  virtual bool canUndo(const Ast::Model &model);
  /** Checks if the rate law can be reset. */
  virtual bool canRedo(const Ast::Model &model);

  /** Resets the rate law to old value. */
  virtual void undo(Ast::Model &model);
  /** Resets the rate law to new value. */
  virtual void redo(Ast::Model &model);

protected:
  /** Holds the old law expression. */
  std::string _old_law;
  /** Holds the new law expression. */
  std::string _new_law;
};



/** A series of several modification items handled as a single modification. */
class ModelDiffGroup : public ModelDiffItem {
public:
  /** Constructs an empty model modification group. */
  ModelDiffGroup();

  /** Constructs a model modification group with a single item in it. */
  ModelDiffGroup(ModelDiffItem *item);

  /** Destructor, also frees the associated diff items. */
  virtual ~ModelDiffGroup();

  /** Returns true if the modifications can be undone on the given model.
   * This fuction checks if the last item of the series can be undone. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if the modifications can be redone on the given model.
   * This function checks if the first item of the series can be redone. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undo the modifications on the given model, by successively undoing all items in reverse
   * order. */
  virtual void undo(Ast::Model &model);
  /** Redo the modifications on the given model, by successively redoing all items in forward
   * order. */
  virtual void redo(Ast::Model &model);

  /** Appends a modification item to the group. */
  virtual void addModification(ModelDiffItem *item);

protected:
  /** Holds the modification items that form this group. */
  std::vector<ModelDiffItem *> _items;
};



/** Represents the deletion of a complete reaction. */
class RemReactionItem: public ModelDiffGroup
{
public:
  /** Constructor. */
  RemReactionItem(const Ast::Reaction *reaction, const Ast::Model &model);
  /** Destructor. */
  ~RemReactionItem();

  /** Returns true if the reaction can be recreated. */
  virtual bool canUndo(const Ast::Model &model);
  /** Returns true if the reaction can be removed. */
  virtual bool canRedo(const Ast::Model &model);

  /** Recreates the complete reaction. */
  virtual void undo(Ast::Model &model);
  /** Removes the complete reaction. */
  virtual void redo(Ast::Model &model);
};



/** Tracks series of model modifications. This class can be used to implement a simple undo/redo
 * mechanism.
 * @ingroup trafo */
class ModelHistory: public ModelDiffGroup {
public:
  /** Constructor of an empty history. */
  ModelHistory();

  /** Destructor. */
  virtual ~ModelHistory();

  /** Returns true if the last modification can be undone. */
  virtual bool canUndo(const Ast::Model &model);
  /** Returns true if the next modification can be redone. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undoes the last modification. */
  virtual void undo(Ast::Model &model);
  /** Redoes the next modification. */
  virtual void redo(Ast::Model &model);

  /** Appends a modification item to the group. */
  virtual void addModification(ModelDiffItem *item);

protected:
  /** Holds the current index in history, the model is in. */
  size_t _current_index;
};


}
}

#endif // _INA_TRAFO_MODELDIFF_HH__
