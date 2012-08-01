#ifndef __FLUC_UTILS_AUTOMATA_HH__
#define __FLUC_UTILS_AUTOMATA_HH__

#include <list>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string>


namespace Fluc {
namespace Utils {


/**
 * Defines a condition based on the char.
 *
 * @ingroup utils
 */
class Condition
{
public:
  /**
   * Destructor.
   */
  virtual ~Condition() { }

  /**
   * This operator implements the actual condition, returns true if the given char matches the
   * condition.
   */
  virtual bool operator() (char c) = 0;
};


/**
 * Defines a condition for a single input-value.
 *
 * @ingroup utils
 */
class SingleCondition : public Condition
{
protected:
  /**
   * The expected char.
   */
  char c;

public:
  /**
   * Constructor.
   */
  SingleCondition(char c)
    : c(c) { /* pass... */ }

  /**
   * Returns true, if the given chat matches @c c.
   */
  virtual bool operator() (char c) { return c == this->c; }
};


/**
 * Defines a condition for a range of input-value.
 *
 * @ingroup utils
 */
class RangeCondition : public Condition
{
protected:
  /**
   * Min value.
   */
  char min;

  /**
   * Max value.
   */
  char max;

public:
  /**
   * Constructor.
   */
  RangeCondition(char min, char max)
    : min(min), max(max) { /* pass */ }

  /**
   * Returns true, if the given char is in range.
   */
  virtual bool operator() (char c) { return (c >= min && c <= max); }
};



/**
 * Negation.
 *
 * @ingroup utils
 */
class NotCondition : public Condition
{
protected:
  /**
   * Owns the encapsulated condition.
   */
  Condition *other;

public:
  /**
   * Constructor.
   */
  NotCondition(Condition *other)
    : other(other) { /* pass... */ }

  /**
   * Destructor, also frees the encapsulated condition.
   */
  virtual ~NotCondition() { delete other; }

  /**
   * Returns true if the encapsulated condition fails.
   */
  virtual bool operator() (char c) { return ! (*other)(c); }
};



/**
 * A state.
 *
 * @ingroup utils
 */
class State
{
protected:
  /**
   * Holds the list of possible transitions.
   */
  std::list< std::pair<Condition *, State*> > transitions;

  /**
   * If true, the state is taken as possible final state.
   */
  bool is_final;


protected:
  /**
   * Hidden constructor use @c DFA::newState to construct a new state.
   */
  State(bool is_final = false);


public:
  /**
   * Destructor.
   */
  ~State();

  /**
   * Adds a transition from this state to the given state if the given condition is satisfied.
   */
  void addTransition(Condition *condition, State *state);

  /**
   * Returns the next state if the given char matches a transition of this state or 0 if
   * no transition matches.
   */
  State *accept(char c);

  /**
   * Returns true if this state is a possible finial state.
   */
  bool isFinal() const;

  /**
   * Adds a transistion on white-space input.
   */
  void onWhiteSpace(State *state);

  /**
   * Adds a transition on "[a-z]".
   */
  void onLowerAlpha(State *state);

  /**
   * Adds a transition on "[A-Z]".
   */
  void onUpperAlpha(State *state);

  /**
   * Adds a transition on "[a-zA-Z]".
   */
  void onAlpha(State *state);

  /**
   * Adds a transition on "[0-9]".
   */
  void onNumber(State *state);

  /**
   * Adds a transition on "[1-9]".
   */
  void onPosNumber(State *state);

  /**
   * Adds a transition on "[0-9a-zA-Z]".
   */
  void onAlphaNum(State *state);

  /**
   * Adds a transition on given char.
   */
  void onChar(char c, State *state);

  /**
   * Adds a transition for anything but the given char.
   */
  void onNotChar(char c, State *state);

  /** @c DFA may access the constructor. */
  friend class DFA;
};



/**
 * Implements a deterministic finite automata (DFA aka. regular expression).
 *
 * @ingroup utils
 */
class DFA
{
protected:
  /**
   * Hold the pointer to all states associated with this DFA.
   */
  std::set<State *> states;

  /**
   * Holds the initial state of the DFA.
   */
  State *initial_state;

  /**
   * Holds a pointer to the current state of the DFA.
   */
  State *current_state;

  /**
   * A buffer of input matched.
   */
  std::stringstream buffer;


public:
  /**
   * Constructs a DFA.
   */
  DFA(bool initial_is_final=false);

  /**
   * Destructor.
   */
  virtual ~DFA();

  /**
   * Resets the DFA.
   */
  void reset();

  /**
   * Returns the initial state of the DFA.
   */
  State *getInitialState();

  /**
   * Returns the current state.
   */
  State *getCurrentState();

  /**
   * Constructs a new state.
   */
  State *newState(bool is_final=false);

  /**
   * Processes the given intput.
   */
  State *accept(char c);

  /**
   * Tests if the given input would be accepted by the current state.
   */
  bool accepts(char c);

  /**
   * Returns true if the DFA is in the initial state.
   */
  bool inInitialState() const;

  /**
   * Returns true if the DFA is in an error state.
   */
  bool inErrorState() const;

  /**
   * Returns true if the DFA is in a final state.
   */
  bool inFinalState() const;

  /**
   * Returns the matched string.
   */
  std::string getString() const;
};



/**
 * This class implements a non-deterministic finite automata (NFA).
 *
 * @ingroup utils
 */
class NFA
{
protected:
  /**
   * Holds a list of all deterministic automatas.
   */
  std::list<DFA *> automatas;

public:
  /**
   * Constructs a new, empty NFA.
   */
  NFA();

  /**
   * Destructor.
   */
  ~NFA();

  /**
   * Constructs a new DFA and adds it to the NFA.
   */
  DFA *newAutomata(bool initial_is_final=false);

  /**
   * Adds the given DFA to the NFA.
   *
   * @note The ownership of the automata is taken by the NFA.
   */
  void addAutomata(DFA *automata);

  /**
   * Resets the NFA, by resetting all DFAs.
   */
  void reset();

  /**
   * Tests if the given intput is accepted by the NFA, that means if at least one DFA would accept
   * this input.
   */
  bool accepts(char c);

  /**
   * Processes the input.
   */
  void accept(char c);

  /**
   * Returns true, if the NFA is in an error state: if and only if all DFAs are in an error state.
   */
  bool inErrorState();

  /**
   * Returns true if the NFA is in a final state: if and only if exactly one DFA is in a final state
   * while all other DFAs are in an error state.
   */
  bool inFinalState();

  /**
   * If in final state, returns the matched input.
   */
  std::string getString() const;

  /**
   * If in final state, retruns the matching DFA.
   */
  DFA *getMatchingDFA();
};


}
}

#endif // AUTOMATA_HH
