#include "lexer.hh"
#include "exception.hh"


using namespace Fluc::Utils;



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
TokenRule::TokenRule(unsigned id, bool initial_is_final)
  : DFA(initial_is_final), id(id)
{
  /* pass... */
}

Token
TokenRule::getToken(size_t line) const
{
  return Token(this->id, line, this->getString());
}

unsigned
TokenRule::getId() const
{
  return this->id;
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
Lexer::addRule(TokenRule *token)
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
    LexerError err;
    err << "@line: ???"
        << "Lexer: unexpected char: " << (unsigned char)(this->input.peek());
    return;
  }

  // Cast DFA to TokenRule...
  TokenRule *rule = dynamic_cast<TokenRule *>(this->reader.getMatchingDFA());

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
