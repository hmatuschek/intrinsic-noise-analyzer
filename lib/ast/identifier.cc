#include "identifier.hh"
using namespace iNA::Ast;


inline bool __is_letter(char c) {
  return (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')));
}

inline bool __is_digit(char c) {
  return ((c>='0') && (c<='9'));
}


bool
iNA::Ast::isValidId(const std::string &id) {
  // Must consist of at least on char
  if (0 == id.size()) { return false; }
  // Check string
  unsigned state = 0;
  for (size_t i=0; i<id.size(); i++) {
    switch (state) {
    case 0: // Initial state expect (letter | '_')
      if (__is_letter(id[i]) || ('_' == id[i])) { state = 1; continue; }
      return false;
    case 1:
      if (__is_letter(id[i]) || __is_digit(id[i]) || ('_' == id[i])) { state = 1; continue; }
      return false;
    }
  }
  return true;
}
