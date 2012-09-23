#include "unitparser.hh"
#include "math.hh"

using namespace iNA;
using namespace iNA::Parser::Unit;

UnitParser::Lexer::Lexer(std::istream &input)
  : Parser::Lexer(input)
{
  addRule(new iNA::Parser::IdentifierTokenRule(UNIT_TOKEN));
  addRule(new UnitParser::IntegerTokenRule(INTEGER_TOKEN));
  addRule(new UnitParser::FloatTokenRule(FLOAT_TOKEN));
  addRule(new iNA::Parser::KeyWordTokenRule(TIMES_TOKEN, "*"));
  addRule(new iNA::Parser::KeyWordTokenRule(DIVIDE_TOKEN, "/"));
  addRule(new iNA::Parser::KeyWordTokenRule(POW_TOKEN, "^"));
  addRule(new iNA::Parser::KeyWordTokenRule(POW_TOKEN, "**"));
  addRule(new iNA::Parser::KeyWordTokenRule(LPAR_TOKEN, "("));
  addRule(new iNA::Parser::KeyWordTokenRule(RPAR_TOKEN, ")"));
  addRule(new iNA::Parser::WhiteSpaceTokenRule(WHITESPACE_TOKEN));

  addTokenName(UNIT_TOKEN, "unit-name"); addTokenName(FLOAT_TOKEN, "FLOAT");
  addTokenName(INTEGER_TOKEN, "INTEGER"); addTokenName(TIMES_TOKEN, "*");
  addTokenName(DIVIDE_TOKEN, "/"); addTokenName(POW_TOKEN, "^");
  addTokenName(LPAR_TOKEN, "("); addTokenName(RPAR_TOKEN, ")");

  addIgnoredToken(WHITESPACE_TOKEN);
}


UnitParser::FloatTokenRule::FloatTokenRule(unsigned id)
  : iNA::Parser::TokenRule(id)
{
  allocStates(7);

  State *start = createState(false);
  State *int_1 = createState(false);
  State *int_f = createState(true);
  State *frac  = createState(true);
  State *exp_0 = createState(false);
  State *exp_1 = createState(false);
  State *exp_f = createState(true);

  onChar('-', start, int_1);
  onPosNumber(int_1, int_f);
  onPosNumber(start, int_f);
  onNumber(int_f, int_f);
  onChar('.', int_f, frac);
  onNumber(frac, frac);
  onChar('e', frac, exp_0);
  onChar('E', frac, exp_0);
  onChar('e', int_f, exp_0);
  onChar('E', int_f, exp_0);
  onChar('-', exp_0, exp_1);
  onPosNumber(exp_1, exp_f);
  onPosNumber(exp_0, exp_f);
  onNumber(exp_f, exp_f);
}


UnitParser::IntegerTokenRule::IntegerTokenRule(unsigned id)
  : iNA::Parser::TokenRule(id)
{
  allocStates(3);
  State *start = createState(false);
  State *int_1 = createState(false);
  State *int_f = createState(true);

  onChar('-', start, int_1);
  onPosNumber(int_1, int_f);
  onPosNumber(start, int_f);
  onNumber(int_f, int_f);
}



UnitParser::UnitParser() { /* pass... */ }


Ast::Unit
UnitParser::parse(const std::string &unit) {
  std::stringstream buffer; buffer.str(unit);
  return parse(buffer);
}

Ast::Unit
UnitParser::parse(std::istream &unit)
{
  UnitParser::Lexer lexer(unit);

  Parser::ConcreteSyntaxTree cst;
  Parser::Production *production = GrammarProduction::factory();
  production->parse(lexer, cst);

  // If grammar is empty:
  if (! cst[0].matched()) {
    return Ast::Unit();
  }

  // Else, parse unit...
  UnitParser parser;
  // Grammar := [Unit] EOS
  return parser.processUnit(cst[0][0], lexer);
}


void
UnitParser::write(const Ast::Unit &unit, std::ostream &output)
{
  std::list<std::string> factors; std::stringstream buffer;

  if (1.0 != unit.getMultiplier()) {
    buffer.str(""); buffer << unit.getMultiplier(); factors.push_back(buffer.str());
  }

  if (0 != unit.getScale()) {
    buffer.str(""); buffer << "1e" << unit.getScale(); factors.push_back(buffer.str());
  }

  for (Ast::Unit::iterator iter=unit.begin(); iter!=unit.end(); iter++) {
    Ast::ScaledBaseUnit::BaseUnit bunit = iter->first; int exp = iter->second;
    buffer.str(""); buffer << Ast::ScaledBaseUnit::baseUnitName(bunit);
    if (1 != exp) { buffer << "^" << exp; }
    factors.push_back(buffer.str());
  }

  // Serialize
  if (factors.size() > 0) {
    std::list<std::string>::iterator factor = factors.begin();
    output << *factor; factor++;
    for(;factors.end() != factor; factor++) {
      output << "*" << *factor;
    }
  }
}


std::string
UnitParser::write(const Ast::Unit &unit) {
  std::stringstream buffer;
  UnitParser::write(unit, buffer);
  return buffer.str();
}


Ast::Unit
UnitParser::processUnit(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer) {
  // Unit := BaseUnit [(("*"|"/") Unit)];
  Ast::Unit unit = processBaseUnit(node[0], lexer);

  if (node[1].matched()) {
    if (0 == node[1][0][0].getAltIdx()) {
      unit = unit * processUnit(node[1][0][1], lexer);
    } else {
      unit = unit / processUnit(node[1][0][1], lexer);
    }
  }

  return unit;
}


Ast::Unit
UnitParser::processBaseUnit(iNA::Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer)
{
  // BaseUnit := (INT | FLOAT | Pow | "(" Unit ")")
  if (0 == node.getAltIdx()) {
    std::string value = lexer[node[0].getTokenIdx()].getValue();
    return iNA::Ast::ScaledBaseUnit(
          iNA::Ast::ScaledBaseUnit::DIMENSIONLESS, UnitParser::asValue<double>(value), 0, 1);
  } else if (1 == node.getAltIdx()) {
    std::string value = lexer[node[0].getTokenIdx()].getValue();
    size_t split_idx = value.find_first_of("eE");
    double mult = 1.0; int scale = 0;

    if (value.npos != split_idx) {
      mult = UnitParser::asValue<double>(value.substr(0, split_idx));
      scale = UnitParser::asValue<int>(value.substr(split_idx+1));
    } else {
      mult = UnitParser::asValue<double>(value);
    }

    return iNA::Ast::ScaledBaseUnit(
          iNA::Ast::ScaledBaseUnit::DIMENSIONLESS, mult, scale, 1);
  } else if (2 == node.getAltIdx()) {
    return processPow(node[0], lexer);
  } else if (3 == node.getAltIdx()) {
    return processUnit(node[0][1], lexer);
  }

  throw InternalError("Unknown alternative in processBaseUnit()!");
}


Ast::Unit
UnitParser::processPow(iNA::Parser::ConcreteSyntaxTree &node, iNA::Parser::Lexer &lexer)
{
  // PowProduction := UnitId [('**'|'^') INTEGER]

  // Get unit ide and try to resolve it to a scaled base unit:
  std::string baseunit = lexer[node[0].getTokenIdx()].getValue();
  if (! Ast::ScaledBaseUnit::isBaseUnitName(baseunit)) {
    Parser::ParserError err(lexer[node[0].getTokenIdx()].getLine());
    err << "Can not resolve unit name " << baseunit;
    throw err;
  }

  Ast::ScaledBaseUnit unit =
      Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::baseUnitByName(baseunit));

  if (node[1].matched()) {
    // Parse exponent:
    int exponent = UnitParser::asValue<int>(lexer[node[1][0][1].getTokenIdx()].getValue());
    // Modify unit, note that this only works since multiplier is 1 and scale 0!!!
    unit = Ast::ScaledBaseUnit(unit.getBaseUnit(), unit.getMultiplier(), unit.getScale(), exponent);
  }

  return unit;
}


UnitParser::GrammarProduction::GrammarProduction()
  : Parser::Production()
{
  GrammarProduction::instance = this;

  elements.push_back(
        new Parser::OptionalProduction(UnitProduction::factory()));
  elements.push_back(new Parser::TokenProduction(Parser::Token::END_OF_INPUT));
}

UnitParser::GrammarProduction *UnitParser::GrammarProduction::instance = 0;

Parser::Production *
UnitParser::GrammarProduction::factory()
{
  if ( 0 == GrammarProduction::instance) {
    return new GrammarProduction();
  }
  return GrammarProduction::instance;
}


UnitParser::UnitProduction::UnitProduction()
  : Parser::Production()
{
  UnitProduction::instance = this;

  elements.push_back(BaseUnitProduction::factory());
  elements.push_back(
        new Parser::OptionalProduction(
          new Parser::Production(
            2, new Parser::AltProduction(
              2, new Parser::TokenProduction(Lexer::TIMES_TOKEN),
              new Parser::TokenProduction(Lexer::DIVIDE_TOKEN)),
            this)));
}

UnitParser::UnitProduction *UnitParser::UnitProduction::instance = 0;

Parser::Production *
UnitParser::UnitProduction::factory()
{
  if (0 == UnitProduction::instance) {
    return new UnitProduction();
  }
  return UnitProduction::instance;
}


UnitParser::BaseUnitProduction::BaseUnitProduction()
  : Parser::AltProduction()
{
  BaseUnitProduction::instance = this;

  alternatives.push_back(new iNA::Parser::TokenProduction(UnitParser::Lexer::INTEGER_TOKEN));
  alternatives.push_back(new iNA::Parser::TokenProduction(UnitParser::Lexer::FLOAT_TOKEN));
  alternatives.push_back(PowProduction::factory());
  alternatives.push_back(
        new Parser::Production(
          3, new Parser::TokenProduction(Lexer::LPAR_TOKEN), UnitProduction::factory(),
          new Parser::TokenProduction(Lexer::RPAR_TOKEN)));
}

UnitParser::BaseUnitProduction *UnitParser::BaseUnitProduction::instance = 0;

Parser::Production *
UnitParser::BaseUnitProduction::factory()
{
  if (0 == BaseUnitProduction::instance) {
    return new BaseUnitProduction();
  }
  return BaseUnitProduction::instance;
}


UnitParser::PowProduction::PowProduction()
  : Parser::Production()
{
  PowProduction::instance = this;

  elements.push_back(new Parser::TokenProduction(Lexer::UNIT_TOKEN));
  elements.push_back(
        new Parser::OptionalProduction(
          new Parser::Production(
            2, new Parser::TokenProduction(Lexer::POW_TOKEN),
            new Parser::TokenProduction(Lexer::INTEGER_TOKEN))));
}

UnitParser::PowProduction *UnitParser::PowProduction::instance = 0;

Parser::Production *
UnitParser::PowProduction::factory() {
  if (0 == PowProduction::instance) {
    new PowProduction();
  }
  return PowProduction::instance;
}
