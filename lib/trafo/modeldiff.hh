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
class ModelDiffVariableItem: public ModelDiffItem {
protected:
  /** A modification to a global variable. */
  ModelDiffVariableItem(const std::string &id);

  /** A modification to a local variable. */
  ModelDiffVariableItem(const std::string &id, const std::string &parent_id);

public:
  /** Destructor. */
  virtual ~ModelDiffVariableItem();

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
class SetVariableIdentifierItem: public ModelDiffVariableItem {
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
class SetVariableNameItem: public ModelDiffVariableItem {
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
class SetVariableConstFlagItem: public ModelDiffVariableItem {
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
class SetVariableValueItem: public ModelDiffVariableItem {
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
class SetSpeciesCompartmentItem: public ModelDiffVariableItem {
public:
  /** Constructor. */
  SetSpeciesCompartmentItem(const std::string &id, const std::string &old_compartment, const std::string &new_compartment);

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
class SetParameterUnitItem: public ModelDiffVariableItem {
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



/** A collection of several modification items handled as a single modification. */
class ModelDiffGroup : public ModelDiffItem {
public:
  /** Constructs an empty model modification group. */
  ModelDiffGroup();

  /** Constructs a model modification group with a single item in it. */
  ModelDiffGroup(ModelDiffItem *item);

  /** Destructor, also frees the associated diff items. */
  virtual ~ModelDiffGroup();

  /** Returns true if the modifications can be undone on the given model. */
  virtual bool canUndo(const Ast::Model &model);
  /** Retruns true if the modifications can be redone on the given model. */
  virtual bool canRedo(const Ast::Model &model);

  /** Undo the modifications on the given model. */
  virtual void undo(Ast::Model &model);
  /** Redo the modifications on the given model. */
  virtual void redo(Ast::Model &model);

  /** Appends a modification item to the group. */
  virtual void addModification(ModelDiffItem *item);

protected:
  /** Holds the modification items that form this group. */
  std::vector<ModelDiffItem *> _items;
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
