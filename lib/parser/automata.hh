#ifndef __INA_PARSER_AUTOMATA_HH__
#define __INA_PARSER_AUTOMATA_HH__

#include <list>
#include <vector>
#include <map>
#include <exception.hh>


namespace Fluc {
namespace Parser {

/**
 * An efficient implementation of a deterministic finite automate using a simple bytecode
 * interpreting state machine.
 *
 * @note As the DFA is a state machine it is not thread save, make sure, that only on thrad accesses
 *       a DFA at a time.
 * @ingroup parser
 */
template <class Value>
class DFA
{
public:
  /** An instruction that assembles the @c Condition. */
  class Instruction {
  public:
    /** Defines the possible op-codes for the instruction. */
    typedef enum {
      ON_VALUE,   ///< Checks if a given value has a certain value.
      IN_RANGE,   ///< Checks if a given value is within a certain range.
      NEGATION,   ///< Negates the previous condition.
      AND_OP,     ///< AND operation.
      OR_OP       ///< OR operation.
    } Type;

  protected:
    /** Determines the type of the condition. */
    Type _type;
    /** Minimum value if @c IN_RANGE instruction or exact value if @c ON_VALUE instruction. */
    Value _min;
    /** Maximum value if @c IN_RANGE instruction. */
    Value _max;

  protected:
    /** Hidden constructor. Use factory methods to assemble a condition. */
    Instruction(Type type, const Value &min = Value(), const Value &max = Value())
      : _type(type), _min(min), _max(max) { }

  public:
    /** Copy constructor. */
    Instruction(const Instruction &other) : _type(other._type), _min(other._min), _max(other._max) { }

  public:
    /** Creates a condition that matched if the input is a certain value. */
    static Instruction createOnValue(const Value &value) {
      return Instruction(ON_VALUE, value);
    }

    /** Creates a condition that matches if the input is within a certain range. */
    static Instruction createInRange(const Value &min, const Value &max) {
      return Instruction(IN_RANGE, min, max);
    }

    /** Creates a condition that matches if the previous condition did not match. */
    static Instruction createNot() {
      return Instruction(NEGATION);
    }

    /** Creates a condition that matches if both of the previous two conditions matched. */
    static Instruction createAnd() {
      return Instruction(AND_OP);
    }

    /** Creates a condition that matches if one of the previous two conditions matched. */
    static Instruction createOr() {
      return Instruction(OR_OP);
    }

  public:
    /** Evaluates the condition on the given stack. */
    inline void eval(const Value &value, std::vector<bool> &stack) const {
      bool tmp = false;
      // Dispatch OPCODE:
      switch (_type) {
      case ON_VALUE:
        stack.push_back(value == _min);
        break;
      case IN_RANGE:
        stack.push_back((value >= _min) && (value <= _max));
        break;
      case NEGATION:
        stack.back() = !stack.back();
        break;
      case AND_OP:
        tmp = stack.back(); stack.pop_back();
        stack.back() = (tmp && stack.back());
        break;
      case OR_OP:
        tmp = stack.back(); stack.pop_back();
        stack.back() = (tmp || stack.back());
        break;
      }
    }
  };


  /** A condition defines when to change a @c State. */
  class Condition {
  protected:
    /** Holds the instructions. */
    std::vector<Instruction> _code;

  public:
    /** Constructor. */
    Condition() : _code() { }

    /** Copy constructor. */
    Condition(const Condition &other) : _code(other._code) { }

    /** Adds a certain instruction. */
    void add(const Instruction &condition) {
      _code.push_back(condition);
    }

    /** Add a on-value instruction. */
    void addOnValue(const Value &value) {
      _code.push_back(Instruction::createOnValue(value));
    }

    /** Add a in-range instruction. */
    void addInRange(const Value &min, const Value &max) {
      _code.push_back(Instruction::createInRange(min, max));
    }

    /** Add a NOT instruction. */
    void addNot() {
      _code.push_back(Instruction::createNot());
    }

    /** Add a AND instruction. */
    void addAnd() {
      _code.push_back(Instruction::createAnd());
    }

    /** Add a OR instruction. */
    void AddOr() {
      _code.push_back(Instruction::createOr());
    }

    /** Evaluates byte-code and returns result. */
    bool matches(const Value &value, std::vector<bool> &stack) const {
      // evaluate code on stack:
      for (size_t i=0; i<_code.size(); i++) {
        _code[i].eval(value, stack);
      }
      // If there is no condition -> return false
      //if (0 == stack.size()) return false;
      // If there are more than one element on the stack:
      if (1 != stack.size()) {
        InternalError err;
        err << "In " << __FILE__ << " @line " << (unsigned int)(__LINE__)
            << ": Can not evaluate DFA condition, condition left with unclean stack.";
        throw err;
      }
      // otherwise: return value, clean stack
      bool ret = stack.back(); stack.pop_back();
      return ret;
    }
  };


  /** This class represents a simple transition to a state if a condition is true. */
  class Transition {
  protected:
    /** Holds the condition of the transition. */
    Condition _condition;
    /** Holds the index of the next state. */
    size_t _next_idx;

  public:
    /** Constructor. */
    Transition(size_t next_idx)
      : _condition(), _next_idx(next_idx) { }

    /** Copy constructor. */
    Transition(const Transition &other)
      : _condition(other._condition), _next_idx(other._next_idx) { }

    /** Returns a reference to the condition. */
    Condition &cond() { return _condition; }
    /** Retunrs a constant reference to the condition. */
    const Condition &cond() const { return _condition; }
    /** Returns the index of the next state. */
    size_t next_idx() const { return _next_idx; }
  };


  /** Represents a state of the @c DFA. */
  class State {
  public:
    /** Defines iterator type over transitions. */
    typedef typename std::list<Transition>::const_iterator iterator;

  protected:
    /** Holds the state index within the DFA. */
    size_t _index;
    /** If true, this state is a valid final state. */
    bool _is_final;
    /** The list of transitions to other states, the first transition that matches will be used. */
    std::list<Transition> _transitions;

  public:
    /** Constructor. */
    State(size_t index, bool is_final)
      : _index(index), _is_final(is_final), _transitions() {
    }

    /** Copy constructor. */
    State(const State &other)
      : _index(other._index), _is_final(other._is_final), _transitions(other._transitions) {
    }

    /** Creates a new transition. */
    Condition &createTransition(State *state) {
      _transitions.push_back( Transition(state->index()) );
      return _transitions.back().cond();
    }

    /** Returns the next state index or -1 if no transition matches. */
    int accept(const Value &value, std::vector<bool> &stack) const {
      for (iterator tran=_transitions.begin(); tran!=_transitions.end(); tran++) {
        if (tran->cond().matches(value, stack))
          return tran->next_idx();
      }
      // If no transition matches -> return -1
      return -1;
    }

    /** Returns true if the state is a final state. */
    bool isFinal() const {
      return _is_final;
    }

    /** Returns the index of the state within the DFA. */
    size_t index() const {
      return _index;
    }
  };


protected:
  /** List of possible states of this DFA. */
  std::vector<State> _states;
  /** The current state index. */
  int _current_state_index;
  /** A stack used to evaluate conditions. */
  std::vector<bool> _evaluation_stack;

public:
  /** Constructor. */
  DFA() : _states(), _current_state_index(-1), _evaluation_stack() {
    _states.reserve(10);
    _evaluation_stack.reserve(10);
  }

  /** Copy constructor, the current state is not preserved. */
  DFA(const DFA &other) : _states(other._states), _current_state_index(-1), _evaluation_stack() {
    std::cerr << "Warning: Copying complete DFA..." << std::endl;
    _evaluation_stack.reserve(10);
    if (0 < _states.size()) {
      _current_state_index = _states.front().index();
    }
  }

  /** Pre-allocates n states for the DFA. This must be done before creating a new state.
   * @warning This function call may destroy the references to states of this DFA! */
  void allocStates(size_t n) {
    _states.reserve(n);
  }

  /** Creates a new state for the DFA.
   * @note The first state created will be the initial state as returned by @c getInitialState.
   * @warning You should pre allocate enough states before calling this method the first time. */
  State *createState(bool is_final=false) {
    _states.push_back(State(_states.size(), is_final));
    if (1 == _states.size()) {
      _current_state_index = _states.back().index();
    }
    return &(_states.back());
  }

  /** Returns the initial state or NULL if no state has been created yet. */
  State *getInitialState() {
    if (0 != _states.size()) {
      return &(_states.front());
    }
    return 0;
  }

  /** Returns the current state, if 0 DFA is in an error state. */
  State *getCurrentState() {
    if (0 > _current_state_index) return 0;
    return &(_states[_current_state_index]);
  }

  /** Processes the given value. */
  State *accept(const Value &value) {
    _current_state_index = _states[_current_state_index].accept(value, _evaluation_stack);
    if (0 > _current_state_index) return 0;
    return &(_states[_current_state_index]);
  }

  /** Checks if the given value would be accepted (if applied, the DFA will not go into the
   * error state). */
  bool accepts(const Value &value) {
    if (0 > _current_state_index) return false;
    return 0 <= _states[_current_state_index].accept(value, _evaluation_stack);
  }

  /** Resets the parser into the initial state. */
  void reset() {
    if (0 == _states.size()) _current_state_index = -1;
    else _current_state_index = _states.front().index();
  }

  /** Returns true if the DFA is in the initial state. */
  bool inInitialState() const {
    return _current_state_index == _states.front().index();
  }

  /** Returns true if the DFA is in the error state. */
  bool inErrorState() const {
    return  0 > _current_state_index;
  }

  /** Returns true, if the current state is a final state. */
  bool inFinalState() const {
    return _states[_current_state_index].isFinal();
  }
};



/**
 * Implements a non-deterministic finite automata (NFA, a collection of determinisitic finite
 * automata, @c DFA).
 * @ingroup parser
 */
template <class Value>
class NFA
{
protected:
  /** Iterator type over DFAs. */
  typedef typename std::list< DFA<Value> *>::iterator iterator;

  /** Iterator type over DFAs. */
  typedef typename std::list< DFA<Value> *>::const_iterator const_iterator;

  /** Holds the list of DFAs that form this NFA. */
  std::list< DFA<Value> *> _automata;

public:
  /** Constructor. */
  NFA() : _automata() { }

  /** Copy constructor, the current states are not preserved. */
  NFA(const NFA &other) : _automata(other._automata) { }

  /** Destructor, also frees the owned DFAs. */
  ~NFA() {
    for (iterator item = _automata.begin(); item != _automata.end(); item++) {
      //delete *item;
    }
  }

  /** Adds the given automata to this NFA, the ownership is transferred to the NFA. */
  void addAutomata(DFA<Value> *automata) {
    _automata.push_back(automata);
  }

  /** Resets the NFA by resetting all DFAs. */
  void reset() {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      (*item)->reset();
    }
  }

  /** Tests if the input value would be accepted by the NFA, means if there is at least one DFA,
   * that would accept this input. */
  bool accepts(const Value &value)  {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if ((*item)->accepts(value)) return true;
    }
    return false;
  }

  /** Updates the states of all DFAs, that are not in an error state. */
  void accept(const Value &value) {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if (! (*item)->inErrorState()) { (*item)->accept(value); }
    }
  }

  /** Returns true, iff all DFAs are in an error state. */
  bool inErrorState() const {
    for (const_iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if (! (*item)->inErrorState()) { return false; }
    }
    return true;
  }

  /** Returns true, iff exactly one DFA is in a final state while all other DFAs are in an error
   * state. */
  bool inFinalState() const {
    for (const_iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if ( (! (*item)->inErrorState()) && ((*item)->inFinalState())) { return true; }
    }
    return false;
  }

  /** If in final state, returns the matching DFA, NULL otherwise. */
  DFA<Value> *getMatchingAutomata() {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if ( (! (*item)->inErrorState()) && ((*item)->inFinalState()) ) { return *item; }
    }
    return 0;
  }
};


}
}

#endif // __INA_PARSER_AUTOMATA_HH__
