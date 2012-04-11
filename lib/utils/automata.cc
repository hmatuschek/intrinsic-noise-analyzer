#include "automata.hh"

using namespace Fluc::Utils;


/* ******************************************************************************************** *
 * Implementation of State
 * ******************************************************************************************** */
State::State(bool is_final)
  : is_final(is_final)
{
  // Pass...
}


State::~State()
{
  // Free all conditions:
  for (std::list< std::pair<Condition *, State *> >::iterator iter=this->transitions.begin();
       iter != this->transitions.end(); iter++)
  {
    delete iter->first;
  }
}


void
State::addTransition(Condition *condition, State *state)
{
  // Just add the transition with condition:
  this->transitions.push_back(std::pair<Condition *, State *>(condition, state));
}


State *
State::accept(char c)
{
  // Iterate through all transitions and test if a transition matches the char:
  for (std::list< std::pair<Condition *, State *> >::iterator iter=this->transitions.begin();
       iter != this->transitions.end(); iter++)
  {
    if ((*(iter->first))(c)) {
      return iter->second;
    }
  }

  // Return NULL to show that no transition condition matched:
  return 0;
}


bool
State::isFinal() const
{
  return this->is_final;
}


void
State::onWhiteSpace(State *state)
{
  this->addTransition(new SingleCondition(' '), state);
  this->addTransition(new SingleCondition('\t'), state);
}


void
State::onLowerAlpha(State *state)
{
  this->addTransition(new RangeCondition('a', 'z'), state);
}


void
State::onUpperAlpha(State *state)
{
  this->addTransition(new RangeCondition('A', 'Z'), state);
}


void
State::onAlpha(State *state)
{
  this->onLowerAlpha(state);
  this->onUpperAlpha(state);
}


void
State::onNumber(State *state)
{
  this->addTransition(new RangeCondition('0', '9'), state);
}


void
State::onPosNumber(State *state)
{
  this->addTransition(new RangeCondition('1', '9'), state);
}


void
State::onAlphaNum(State *state)
{
  this->onNumber(state);
  this->onAlpha(state);
}


void
State::onChar(char c, State *state)
{
  this->addTransition(new SingleCondition(c), state);
}


void
State::onNotChar(char c, State *state)
{
  this->addTransition(new NotCondition(new SingleCondition(c)), state);
}



/* ******************************************************************************************** *
 * Implementation of DeterministicAutomata: (a DFA)
 * ******************************************************************************************** */
DFA::DFA(bool initial_is_final)
{
  // Create an initial state:
  this->initial_state = this->newState(initial_is_final);
  // and take this state as the current state:
  this->current_state = this->initial_state;
}


DFA::~DFA()
{
  // Free all associated states:
  for (std::set<State *>::iterator iter = this->states.begin();
       iter != this->states.end(); iter++)
  {
    delete *iter;
  }
}


void
DFA::reset()
{
  // Set current state to initial state:
  this->current_state = this->initial_state;

  // and clear the buffer:
  this->buffer.str("");
}


State *
DFA::getInitialState()
{
  return this->initial_state;
}


State *
DFA::getCurrentState()
{
  return this->current_state;
}


State *
DFA::newState(bool is_final)
{
  // Construct new state and take ownership:
  State *state = new State(is_final);
  this->states.insert(state);
  return state;
}


State *
DFA::accept(char c)
{
  // error -> error : *
  if (0 == this->current_state)
    return 0;

  // Otherwise search for transition in current state:
  this->current_state = this->current_state->accept(c);

  // If char was accepted -> store in buffer:
  if (0 != this->current_state)
    this->buffer << c;

  // Return new "current" state:
  return this->current_state;
}


bool
DFA::accepts(char c)
{
  // We do not accept anything if we are in an error-state:
  if (0 == this->current_state)
    return false;

  return 0 != this->current_state->accept(c);
}


bool
DFA::inInitialState() const
{
  return this->initial_state == this->current_state;
}


bool
DFA::inErrorState() const
{
  return 0 == this->current_state;
}


bool
DFA::inFinalState() const
{
  return (0 != this->current_state && this->current_state->isFinal());
}


std::string
DFA::getString() const
{
  return this->buffer.str();
}



/* ******************************************************************************************** *
 * Implementation of NonDeterministicAutomata: (a NFA)
 * ******************************************************************************************** */
NFA::NFA()
{
  // Pass...
}


NFA::~NFA()
{
  for(std::list<DFA *>::iterator iter=this->automatas.begin();
      iter != this->automatas.end(); iter++)
  {
    delete *iter;
  }
}


DFA *
NFA::newAutomata(bool initial_is_final)
{
  this->automatas.push_back(new DFA(initial_is_final));
  return this->automatas.back();
}


void
NFA::addAutomata(DFA *automata)
{
  this->automatas.push_back(automata);
}


void
NFA::reset()
{
  for (std::list<DFA *>::iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    (*iter)->reset();
  }
}


bool
NFA::accepts(char c)
{
  for (std::list<DFA *>::iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    if ((*iter)->accepts(c))
      return true;
  }

  return false;
}


void
NFA::accept(char c)
{
  for (std::list<DFA *>::iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    (*iter)->accept(c);
  }
}


bool
NFA::inErrorState()
{
  for (std::list<DFA *>::iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    if (! (*iter)->inErrorState())
      return false;
  }

  return true;
}


bool
NFA::inFinalState()
{
  size_t final_count = 0;

  for (std::list<DFA *>::iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    if ((*iter)->inFinalState()) {
      final_count++;

      if (1 < final_count) {
        return false;
      }
    }
  }

  return 1 == final_count;
}


std::string
NFA::getString() const
{
  for (std::list<DFA *>::const_iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    if ((*iter)->inFinalState())
      return (*iter)->getString();
  }

  return "";
}



DFA *
NFA::getMatchingDFA()
{
  for (std::list<DFA *>::iterator iter = this->automatas.begin();
       iter != this->automatas.end(); iter++)
  {
    if ((*iter)->inFinalState())
      return (*iter);
  }

  return 0;
}
