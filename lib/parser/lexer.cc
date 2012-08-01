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
Token::getId() const
{
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
  : DFA<char>(), id(id), _current_value()
{
  /* pass... */
}

Token
TokenRule::getToken(size_t line) const
{
  return Token(this->id, line, _current_value.str());
}

unsigned
TokenRule::getId() const
{
  return this->id;
}

void
TokenRule::reset()
{
  _current_value.str("");
  DFA<char>::reset();
}

DFA<char>::State *
TokenRule::accept(const char &value)
{
  State *new_state = DFA<char>::accept(value);
  if (0 != new_state) {
    _current_value << value;
  }
  return new_state;
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
  cond.AddOr();
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
  cond.AddOr();
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
  cond.AddOr();
  cond.AddOr();
}

/* ******************************************************************************************** *
 * Implementation of Lexer:
 * ******************************************************************************************** */
Lexer::Lexer(std::istream &input)
  : input(input), history(0)
{
  // Initialize stack of token pointer:
  this->stack.push_back(State(0,1,1));

  // Store default EOF token-name:
  this->addTokenName(Token::END_OF_INPUT, "end-of-input");
}


void
Lexer::addRule(const TokenRule &token)
{
  this->reader.addAutomata(token);
}


const Token &
Lexer::next()
{
  // Needs to read token from input?
  if ((this->stack.back().idx+1) == this->history.size())
  {
    this->parseToken();
  }

  // If token already read:
  this->stack.back().idx++;

  // If new token is new-line:
  if (0 != this->new_line_token.count(this->history[this->stack.back().idx].getId()))
  {
    this->stack.back().line++;
  }

  return this->history[this->stack.back().idx];
}


const Token &
Lexer::current()
{
  if (this->stack.back().idx == this->history.size())
  {
    this->parseToken();
    // If new token is new-line:
    if (0 != this->new_line_token.count(this->history[this->stack.back().idx].getId()))
    {
      this->stack.back().line++;
    }
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
Lexer::restore_state()
{
  this->stack.pop_back();
  //std::cerr << "Restore state: " << this->stack.back() << std::endl;
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
  this->reader.reset();

  // If end of stream -> push back a EOS token
  if (this->input.eof()) {
    this->history.push_back(Token(Token::END_OF_INPUT, 0, ""));
    return;
  }

  // Read chars until ROF or NFA does not accept the next char:
  while ((!this->input.eof()) && this->reader.accepts(this->input.peek()))
  {
    char c; this->input.get(c);
    this->reader.accept(c);
  }

  // If the reader is not in a final state:
  if (! this->reader.inFinalState()) {
    Utils::LexerError err;
    err << "@line: ???"
        << "Lexer: unexpected char: " << (unsigned char)(this->input.peek());
    return;
  }

  // Cast DFA to TokenRule...
  TokenRule *rule = static_cast<TokenRule *>(this->reader.getMatchingAutomata());

  // If parsed token is ignored -> read another one...
  if (this->ignored_token.end() != this->ignored_token.find(rule->getId()))
  {
    this->parseToken();
    return;
  }

  // Store parsed token in history:
  this->history.push_back(rule->getToken(this->stack.back().line));

  //std::cerr << "Parsed token " << this->history.back().getId()
  //          << ": " << this->history.back().getValue() << std::endl;
}


/* ********************************************************************************************* *
 * Implementation of default TokenRules:
 * ********************************************************************************************* */
/* ********************************************************************************************* *
 * Implementation of WhiteSpaceToken rule:
 * ********************************************************************************************* */
WhiteSpaceTokenRule::WhiteSpaceTokenRule(unsigned id)
  : TokenRule(id)
{
  State *s1 = createState(false);
  State *s2 = createState(true);
  onWhiteSpace(s1,s2);
  onWhiteSpace(s2,s2);
}



/* ********************************************************************************************* *
 * Implementation of EOLToken rule:
 * ********************************************************************************************* */
EOLTokenRule::EOLTokenRule(unsigned id)
  : TokenRule(id)
{
  State *s1 = createState(false);
  State *s2 = createState(true);
  State *s3 = createState(true);
  State *s4 = createState(true);
  onChar('\n', s1, s2);
  onChar('\r', s1, s3);
  onChar('\r', s3, s4);
  onChar('\n', s3, s4);
}



/* ********************************************************************************************* *
 * Implementation of IdentifierToken rule:
 * ********************************************************************************************* */
IdentifierTokenRule::IdentifierTokenRule(unsigned id)
  : TokenRule(id)
{
  State *s1 = createState(false);
  State *s2 = createState(true);
  onAlpha(s1, s2);
  onChar('_', s1, s2);
  onAlphaNum(s2,s2);
  onChar('_', s2, s2);
}



/* ********************************************************************************************* *
 * Implementation of StringToken rule:
 * ********************************************************************************************* */
StringTokenRule::StringTokenRule(unsigned id)
  : TokenRule(id)
{
  State *si = createState(false);
  State *s2 = createState(false);
  State *sf = createState(true);
  onChar('"', si, s2);
  onChar('"', s2, sf);
  onNotChar('"', s2, s2);
}



/* ********************************************************************************************* *
 * Implementation of KeyWordToken rule:
 * ********************************************************************************************* */
KeyWordTokenRule::KeyWordTokenRule(unsigned id, const std::string &keyword)
  : TokenRule(id)
{
  State *state = createState(false);
  for (size_t i=0; i<keyword.size(); i++) {
    State *nextState = createState((i+1)==keyword.size());
    onChar(keyword[i], state, nextState);
    state = nextState;
  }
}



/* ********************************************************************************************* *
 * Implementation of IntegerToken rule:
 * ********************************************************************************************* */
IntegerTokenRule::IntegerTokenRule(unsigned id)
  : TokenRule(id)
{
  State *si = createState(false);
  State *sf = createState(true);
  onNumber(si, sf);
  onNumber(sf, sf);
}



/* ********************************************************************************************* *
 * Implementation of FloatToken rule:
 * ********************************************************************************************* */
FloatTokenRule::FloatTokenRule(unsigned id)
  : TokenRule(id)
{
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
  onNumber(sff, sff);
  onChar('e', sff, sei);
  onChar('-', sei, se1);
  onNumber(sei, sef);
  onNumber(se1, sef);
  onNumber(sef, sef);
}

