#include "lexer.hh"
#include "exception.hh"
#include "utils/exception.hh"


using namespace Fluc::Parser;



/* ******************************************************************************************** *
 * Implementation of Token:
 * ******************************************************************************************** */
Token::Token()
  : id(Token::END_OF_INPUT), line(0), value("")
{
  // pass...
}

Token::Token(unsigned id, size_t line, const std::string &value)
  : id(id), line(line), value(value)
{
  // pass...
}

unsigned
Token::getId() const {
  return this->id;
}

unsigned int
Token::getLine() const
{
  return this->line;
}

const std::string &
Token::getValue() const
{
  return this->value;
}



/* ******************************************************************************************** *
 * Implementation of TokenRule:
 * ******************************************************************************************** */
TokenRule::TokenRule(unsigned id)
  : DFA<char>(), id(id)
{
  /* pass... */
}

TokenRule::TokenRule(const TokenRule &other)
  : DFA<char>(other), id(other.id)
{
  // pass...
}

Token
TokenRule::getToken(const std::string &value, size_t line) const
{
  return Token(this->id, line, value);
}

unsigned
TokenRule::getId() const
{
  return this->id;
}

void
TokenRule::onChar(char value, State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addOnValue(value);
}

void
TokenRule::onNotChar(char c, State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addOnValue(c);
  cond.addNot();
}

void
TokenRule::onWhiteSpace(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addOnValue(' ');
  cond.addOnValue('\t');
  cond.addOr();
}

void
TokenRule::onLowerAlpha(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addInRange('a','z');
}

void
TokenRule::onUpperAlpha(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addInRange('A','Z');
}

void
TokenRule::onAlpha(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addInRange('a','z');
  cond.addInRange('A','Z');
  cond.addOr();
}

void
TokenRule::onNumber(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addInRange('0', '9');
}

void
TokenRule::onPosNumber(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addInRange('1', '9');
}

void
TokenRule::onAlphaNum(State *A, State *B)
{
  Condition &cond = A->createTransition(B);
  cond.addInRange('0','9');
  cond.addInRange('a','z');
  cond.addInRange('A','Z');
  cond.addOr();
  cond.addOr();
}

/* ******************************************************************************************** *
 * Implementation of Lexer:
 * ******************************************************************************************** */
Lexer::Lexer(std::istream &input)
  : input(input), history(0), reader()
{
  // Initialize stack of token pointer:
  this->stack.push_back(State(0,1,1));

  // Store default EOF token-name:
  this->addTokenName(Token::END_OF_INPUT, "end-of-input");
}


void
Lexer::addRule(TokenRule *token) {
  this->reader.addAutomata(token);
}

const Token &
Lexer::next()
{
  // Needs to read token from input?
  if ((this->stack.back().idx+1) == this->history.size()) {
    this->parseToken();
  }

  // If token already read:
  this->stack.back().idx++;
  // Return token from history
  return this->history[this->stack.back().idx];
}


const Token &
Lexer::current() {
  // If there is no token parsed -> parse one more
  if (this->stack.back().idx == this->history.size()) {
    this->parseToken();
  }
  return this->history[this->stack.back().idx];
}


size_t
Lexer::currentIndex() const
{
  return this->stack.back().idx;
}


void
Lexer::push_state()
{
  //std::cerr << "Save LexerState: " << this->stack.back() << std::endl;
  this->stack.push_back(this->stack.back());
}


void
Lexer::drop_state()
{
  // Save current state
  State current = this->stack.back(); this->stack.pop_back();
  //std::cerr << "Drop state " << this->stack.back() << std::endl;
  // remove previous state and push current state back on the stack:
  this->stack.pop_back(); this->stack.push_back(current);
}


void
Lexer::restore_state() {
  this->stack.pop_back();
}


void
Lexer::reset()
{
  this->stack.clear();
  this->stack.push_back(State(0, 1, 1));
}


void
Lexer::addTokenName(unsigned id, const std::string &name)
{
  this->token_table[id] = name;
}


std::string
Lexer::getTokenName(unsigned id)
{
  std::map<unsigned, std::string>::iterator item;
  if (this->token_table.end() == (item = this->token_table.find(id)))
  {
    std::stringstream str;
    str << "Token(" << id << ")";
    return str.str();
  }

  return item->second;
}


void
Lexer::addIgnoredToken(unsigned id)
{
  this->ignored_token.insert(id);
}


const Token &
Lexer::operator [](size_t idx) const {
  return history[idx];
}


void
Lexer::parseToken()
{
  // Reset NFA:
  reader.reset();
  // Clear buffer
  _buffer.str("");

  // If end of stream -> push back a EOS token
  if (input.eof()) {
    history.push_back(Token(Token::END_OF_INPUT, 0, ""));
    return;
  }

  input.peek();
  // Read chars until EOF or NFA does not accept the next char:
  while ((!input.eof()) &&reader.accepts(input.peek()))
  {
    char c; input.get(c); input.peek();
    reader.accept(c);
    _buffer << c;
  }

  // If the reader is not in a final state:
  if (! reader.inFinalState()) {
    Utils::ParserError err;
    err << "@line: " << stack.back().line
        << "Lexer: unexpected char: " << (unsigned char)(input.peek());
    throw err;
  }

  // Cast DFA to TokenRule...
  TokenRule *rule = static_cast<TokenRule *>(reader.getMatchingAutomata());

  // Store parsed token in history:
  Token token = rule->getToken(_buffer.str(), stack.back().line);

  // If token is NEW_LINE token:
  if (0 != new_line_token.count(token.getId())) {
    stack.back().line++;
  }

  // If parsed token is ignored -> read another one...
  if (this->ignored_token.end() != this->ignored_token.find(token.getId())) {
    this->parseToken();
    return;
  }

  history.push_back(token);
}


/* ********************************************************************************************* *
 * Default TokenRules...
 *
 * Implementation of WhiteSpaceToken rule:
 * ********************************************************************************************* */
WhiteSpaceTokenRule::WhiteSpaceTokenRule(unsigned id)
  : TokenRule(id)
{
  allocStates(2);
  State *s1 = createState(false);
  State *s2 = createState(true);
  onWhiteSpace(s1,s2);
  onWhiteSpace(s2,s2);
}


EOLTokenRule::EOLTokenRule(unsigned id)
  : TokenRule(id)
{
  allocStates(4);
  State *s1 = createState(false);
  State *s2 = createState(true);
  State *s3 = createState(true);
  State *s4 = createState(true);
  onChar('\n', s1, s2);
  onChar('\r', s1, s3);
  onChar('\r', s3, s4);
  onChar('\n', s3, s4);
}


IdentifierTokenRule::IdentifierTokenRule(unsigned id)
  : TokenRule(id)
{
  allocStates(2);
  State *s1 = createState(false);
  State *s2 = createState(true);
  onAlpha(s1, s2);
  onChar('_', s1, s2);
  onAlphaNum(s2,s2);
  onChar('_', s2, s2);
}


StringTokenRule::StringTokenRule(unsigned id)
  : TokenRule(id)
{
  allocStates(3);
  State *si = createState(false);
  State *s2 = createState(false);
  State *sf = createState(true);
  onChar('"', si, s2);
  onChar('"', s2, sf);
  onNotChar('"', s2, s2);
}


KeyWordTokenRule::KeyWordTokenRule(unsigned id, const std::string &keyword)
  : TokenRule(id)
{
  allocStates(keyword.size()+1);
  State *state = createState(false);
  for (size_t i=0; i<keyword.size(); i++) {
    State *nextState = createState((i+1)==keyword.size());
    onChar(keyword[i], state, nextState);
    state = nextState;
  }
}


IntegerTokenRule::IntegerTokenRule(unsigned id)
  : TokenRule(id)
{
  allocStates(2);
  State *si = createState(false);
  State *sf = createState(true);
  onNumber(si, sf);
  onNumber(sf, sf);
}


FloatTokenRule::FloatTokenRule(unsigned id)
  : TokenRule(id)
{
  allocStates(6);
  State *si  = createState(false);
  State *s3  = createState(false);
  State *sff = createState(true);
  State *sei = createState(false);
  State *se1 = createState(false);
  State *sef = createState(true);
  onNumber(si, s3);
  onNumber(s3, s3);
  onChar('.', s3, sff);
  onChar('e', s3, sei);
  onChar('E', s3, sei);
  onNumber(sff, sff);
  onChar('e', sff, sei);
  onChar('E', sff, sei);
  onChar('-', sei, se1);
  onNumber(sei, sef);
  onNumber(se1, sef);
  onNumber(sef, sef);
}

