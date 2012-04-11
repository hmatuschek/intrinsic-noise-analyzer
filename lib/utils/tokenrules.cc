#include "tokenrules.hh"


using namespace Fluc::Utils;



/* ********************************************************************************************* *
 * Implementation of WhiteSpaceToken rule:
 * ********************************************************************************************* */
WhiteSpaceTokenRule::WhiteSpaceTokenRule(unsigned id)
  : TokenRule(id, false)
{
  State *s1 = this->getInitialState();
  State *s2 = this->newState(true);
  s1->onWhiteSpace(s2);
  s2->onWhiteSpace(s2);
}



/* ********************************************************************************************* *
 * Implementation of EOLToken rule:
 * ********************************************************************************************* */
EOLTokenRule::EOLTokenRule(unsigned id)
  : TokenRule(id, false)
{
  State *s1 = this->getInitialState();
  State *s2 = this->newState(true);
  State *s3 = this->newState(true);
  State *s4 = this->newState(true);
  s1->onChar('\n', s2);
  s1->onChar('\r', s3);
  s2->onChar('\r', s4);
  s3->onChar('\n', s4);
}



/* ********************************************************************************************* *
 * Implementation of IdentifierToken rule:
 * ********************************************************************************************* */
IdentifierTokenRule::IdentifierTokenRule(unsigned id)
  : TokenRule(id, false)
{
  State *s1 = this->getInitialState();
  State *s2 = this->newState(true);
  s1->onAlpha(s2);
  s1->onChar('_', s2);
  s2->onAlphaNum(s2);
  s2->onChar('_', s2);
}



/* ********************************************************************************************* *
 * Implementation of StringToken rule:
 * ********************************************************************************************* */
StringTokenRule::StringTokenRule(unsigned id)
  : TokenRule(id, false)
{
  State *si = this->getInitialState();
  State *s2 = this->newState(false);
  State *sf = this->newState(true);
  si->onChar('"', s2);
  s2->onChar('"', sf);
  s2->onNotChar('"', s2);
}



/* ********************************************************************************************* *
 * Implementation of KeyWordToken rule:
 * ********************************************************************************************* */
KeyWordTokenRule::KeyWordTokenRule(unsigned id, const std::string &keyword)
  : TokenRule(id, false)
{
  State *state = this->getInitialState();
  for (size_t i=0; i<keyword.size(); i++) {
    State *nextState = this->newState((i+1)==keyword.size());
    state->onChar(keyword[i], nextState);
    state = nextState;
  }
}



/* ********************************************************************************************* *
 * Implementation of IntegerToken rule:
 * ********************************************************************************************* */
IntegerTokenRule::IntegerTokenRule(unsigned id)
  : TokenRule(id, false)
{
  State *si = this->getInitialState();
  State *sf = this->newState(true);
  si->onNumber(sf);
  sf->onNumber(sf);
}



/* ********************************************************************************************* *
 * Implementation of FloatToken rule:
 * ********************************************************************************************* */
FloatTokenRule::FloatTokenRule(unsigned id)
  : TokenRule(id, false)
{
  State *si  = this->getInitialState();
  State *s3  = this->newState(false);
  State *sff = this->newState(true);
  State *sei = this->newState(false);
  State *se1 = this->newState(false);
  State *sef = this->newState(true);
  si->onNumber(s3);
  s3->onNumber(s3);
  s3->onChar('.', sff);
  s3->onChar('e', sei);
  sff->onNumber(sff);
  sff->onChar('e', sei);
  sei->onChar('-', se1);
  sei->onNumber(sef);
  se1->onNumber(sef);
  sef->onNumber(sef);
}

