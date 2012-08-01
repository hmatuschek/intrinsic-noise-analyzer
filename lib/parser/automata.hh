#ifndef __INA_PARSER_AUTOMATA_HH__
#define __INA_PARSER_AUTOMATA_HH__

#include <list>
#include <vector>
#include <exception.hh>


namespace Fluc {
namespace Parser {

/**
 * A more efficient implementation of the @c Utils::DFA using a simple bytecode interpreting
 * stack machine.
 *
 * @note As the DFA is a state machine it is not thread save!
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
        stack.back() = tmp && stack.back();
        break;
      case OR_OP:
        tmp = stack.back(); stack.pop_back();
        stack.back() = tmp && stack.back();
        break;
      }
    }
  };


  /** A condition that defines, when to change a @c State. */
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
      // If there was no condition -> return false
      if (0 == stack.size()) return false;
      // If there are more than one element on the stack:
      if (1 != stack.size()) {
        InternalError err;
        err << "In " << __FILE__ << " @line " << (unsigned int)(__LINE__)
            << ": Can not evaluate DFA condition, condition left with unclean stack.";
        throw err;
      }
      // otherwise, return value
      bool ret = stack.back(); stack.pop_back();
      return ret;
    }
  };

  /** Represents a state of the @c DFA. */
  class State {
  protected:
    /** If true, this state is a valid final state. */
    bool _is_final;
    /** The list of transitions to other states, the first transition that matches will be used. */
    std::vector< std::pair<Condition, State *> > _transitions;

  public:
    /** Constructor. */
    State(bool is_final=false) : _is_final(is_final), _transitions() { }

    /** Creates a new transition. */
    Condition &createTransition(State *state) {
      _transitions.push_back( std::pair<Condition, State *>(Condition(), state) );
      return _transitions.back().first;
    }

    /** Returns the next state or 0 if no transition matches. */
    State *accept(const Value &value, std::vector<bool> &stack) const {
      for (size_t i=0; i<_transitions.size(); i++) {
        if (_transitions[i].first.matches(value, stack))
          return _transitions[i].second;
      }
      // If no transition matches -> return 0
      return 0;
    }

    /** Returns true if the state is a final state. */
    bool isFinal() const {
      return _is_final;
    }
  };

protected:
  /** List of possible states of this DFA. */
  std::list<State> _states;
  /** The current state. */
  State *_current_state;
  /** A stack used to evaluate conditions. */
  std::vector<bool> _evaluation_stack;

public:
  /** Constructor. */
  DFA() : _states(), _current_state(0), _evaluation_stack() { _evaluation_stack.reserve(10); }

  /** Copy constructor, the current state is not preserved. */
  DFA(const DFA &other) : _states(other._states), _current_state(0), _evaluation_stack() {
    _evaluation_stack.reserve(10);
    if (0 < _states.size()) {
      _current_state = &(_states.front());
    }
  }

  /** Creates a new state for the DFA.
   * @note The first state created will be the initial state as returned by @c getInitialState. */
  State *createState(bool is_final=false) {
    _states.push_back(State(is_final));
    if (1 == _states.size()) {
      _current_state = &(_states.back());
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
    return _current_state;
  }

  /** Processes the given value. */
  State *accept(const Value &value) {
    _current_state = _current_state->accept(value, _evaluation_stack);
    return _current_state;
  }

  /** Checks if the given value would be accepted (if applied, the DFA will not go into the
   * error state). */
  bool accepts(const Value &value) {
    return 0 != _current_state->accept(value, _evaluation_stack);
  }

  /** Resets the parser into the initial state. */
  void reset() {
    _current_state = &(_states.front());
  }

  /** Returns true if the DFA is in the initial state. */
  bool inInitialState() const {
    return _current_state == &(_states.front());
  }

  /** Returns true if the DFA is in the error state. */
  bool inErrorState() const {
    return 0 == _current_state;
  }

  /** Returns true, if the current state is a final state. */
  bool inFinalState() const {
    return _current_state->isFinal();
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
  typedef typename std::list< DFA<Value> >::iterator iterator;

  /** Iterator type over DFAs. */
  typedef typename std::list< DFA<Value> >::const_iterator const_iterator;

  /** Holds the list of DFAs that form this NFA. */
  std::list< DFA<Value> > _automata;

public:
  /** Constructor. */
  NFA() : _automata() { }

  /** Copy constructor, the current states are not preserved. */
  NFA(const NFA &other) : _automata(other._automata) { }

  /** Creates a new, empty @c DFA and adds it to this NFA. */
  DFA<Value> *createAutomata() {
    _automata.push_back(DFA<Value>());
    return &(_automata.back());
  }

  /** Adds a copy of the given automata to this NFA. */
  void addAutomata(const DFA<Value> &automata) {
    _automata.push_back(automata);
  }

  /** Resets the NFA by resetting all DFAs. */
  void reset() {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      item->reset();
    }
  }

  /** Tests if the input value would be accepted by the NFA, means if there is at least one DFA,
   * that would accept this input. */
  bool accepts(const Value &value)  {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if (item->accepts(value)) return true;
    }
    return false;
  }

  /** Updates the states of all DFAs, that are not in an error state. */
  void accept(const Value &value) {
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if (! item->inErrorState()) { item->accept(value); }
    }
  }

  /** Returns true, iff all DFAs are in an error state. */
  bool inErrorState() const {
    for (const_iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if (! item->inErrorState()) { return false; }
    }
    return true;
  }

  /** Returns true, iff exactly one DFA is in a final state while all other DFAs are in an error
   * state. */
  bool inFinalState() const {
    size_t num_final = 0;
    for (const_iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if ( (! item->inErrorState()) && (! item->inFinalState())) { return false; }
      if ( (! item->inErrorState()) ) { num_final++; }
    }
    return 1 == num_final;
  }

  /** If in final state, returns the matching DFA, NULL otherwise. */
  DFA<Value> *getMatchingAutomata() {
    size_t num_final = 0; DFA<Value> *matching = 0;
    for (iterator item=_automata.begin(); item!=_automata.end(); item++) {
      if ( (! item->inErrorState()) && (! item->inFinalState())) { return 0; }
      if ( (! item->inErrorState()) ) { matching = &(*item); num_final++; }
    }

    if (1 != num_final) return 0;
    return matching;
  }
};


}
}

#endif // __INA_PARSER_AUTOMATA_HH__
