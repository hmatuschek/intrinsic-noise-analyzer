#include "unitparser.hh"
#include "math.hh"

using namespace Fluc;


UnitParser::Lexer::Lexer(std::istream &input)
  : Parser::Lexer(input) {
  /// \todo Implement!!!
}



  UnitParser::UnitParser()
  : _unit_table()
{
  _unit_table["ampere"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::AMPERE);
  _unit_table["avogadro"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::AVOGADRO);
  _unit_table["becquerel"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::BECQUEREL);
  _unit_table["candela"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::CANDELA);
  _unit_table["celsius"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::CELSIUS);
  _unit_table["coulomb"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::COULOMB);
  _unit_table["farad"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::FARAD);
  _unit_table["gram"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::GRAM);
  _unit_table["gray"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::GRAY);
  _unit_table["henry"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::HENRY);
  _unit_table["herz"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::HERTZ);
  _unit_table["joule"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::JOULE);
  _unit_table["katal"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::KATAL);
  _unit_table["kelvin"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::KELVIN);
  _unit_table["kilogram"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::KILOGRAM);
  _unit_table["litre"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::LITRE);
  _unit_table["lumen"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::LUMEN);
  _unit_table["lux"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::LUX);
  _unit_table["metre"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::METRE);
  _unit_table["mole"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::MOLE);
  _unit_table["newton"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::NEWTON);
  _unit_table["ohm"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::OHM);
  _unit_table["pascal"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::PASCAL);
  _unit_table["radian"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::RADIAN);
  _unit_table["second"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::SECOND);
  _unit_table["siemens"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::SIEMENS);
  _unit_table["sievert"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::SIEVERT);
  _unit_table["steradian"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::STERADIAN);
  _unit_table["tesla"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::TESLA);
  _unit_table["volt"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::VOLT);
  _unit_table["watt"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::WATT);
  _unit_table["weber"] = Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::WEBER);
}


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
  production->notify(lexer, cst);

  // If grammar is empty:
  if (! cst[0].matched()) {
    return Ast::Unit();
  }

  // Else, parse unit...
  UnitParser parser;
  // Grammar := [Unit] EOS
  return parser.processUnit(cst[0][0], lexer);
}


Ast::Unit
UnitParser::processUnit(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer) {
  // Unit := BaseUnit [("*"|"/") Unit];
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
UnitParser::processBaseUnit(Fluc::Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer)
{
  // BaseUnit := (Scale | Pow | "(" Unit ")")
  if (0 == node.getAltIdx()) {
    return processScale(node[0], lexer);
  } else if (1 == node.getAltIdx()) {
    return processPow(node[0], lexer);
  } else if (2 == node.getAltIdx()) {
    return processUnit(node[0][1], lexer);
  }

  return Ast::Unit();
}


Ast::Unit
UnitParser::processScale(Fluc::Parser::ConcreteSyntaxTree &node, Fluc::Parser::Lexer &lexer)
{
  /** Scale = FLOAT [("e"|"E") INTEGER] */
  int scale = 0;
  double multiplier = 1.0;

  std::stringstream buffer;
  buffer.str(lexer[node[0].getTokenIdx()].getValue()); buffer >> multiplier;

  if (node[1].matched()) {
    buffer.str(lexer[node[1][0][1].getTokenIdx()].getValue()); buffer >> scale;
  }

  return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::DIMENSIONLESS, multiplier, scale, 1);
}


Ast::Unit
UnitParser::processPow(Fluc::Parser::ConcreteSyntaxTree &node, Fluc::Parser::Lexer &lexer)
{
  // PowProduction := UnitId [('**'|'^') INTEGER]

  // Get unit ide and try to resolve it to a scaled base unit:
  std::string baseunit = lexer[node[0].getTokenIdx()].getValue();
  std::map<std::string, Ast::ScaledBaseUnit>::iterator item = _unit_table.find(baseunit);
  if (_unit_table.end() == item) {
    SymbolError err;
    err << "Can not resolve unit name " << baseunit;
    throw err;
  }

  Ast::ScaledBaseUnit unit = item->second;

  if (node[1].matched()) {
    // Parse exponent:
    int exponent=1; std::stringstream buffer(lexer[node[1][0][1].getTokenIdx()].getValue());
    buffer >> exponent;
    // Modify unit:
    unit = Ast::ScaledBaseUnit(unit.getBaseUnit(), unit.getMultiplier(), unit.getScale(),
                               unit.getExponent()*exponent);
  }

  return unit;
}


UnitParser::GrammarProduction::GrammarProduction()
  : Parser::Production()
{
  GrammarProduction::instance = this;

  elements.push_back(UnitProduction::factory());
  elements.push_back(new Parser::TokenProduction(Parser::Token::END_OF_INPUT));
}

UnitParser::GrammarProduction *UnitParser::GrammarProduction::instance = 0;

Parser::Production *
UnitParser::GrammarProduction::factory() {
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
UnitParser::UnitProduction::factory() {
  if (0 == UnitProduction::instance) {
    return new UnitProduction();
  }

  return UnitProduction::instance = 0;
}


UnitParser::BaseUnitProduction::BaseUnitProduction()
  : Parser::AltProduction()
{
  BaseUnitProduction::instance = this;

  alternatives.push_back(ScaleProduction::factory());
  alternatives.push_back(PowProduction::factory());
  alternatives.push_back(
        new Parser::Production(
          3, new Parser::TokenProduction(Lexer::LPAR_TOKEN), UnitProduction::factory(),
          new Parser::TokenProduction(Lexer::RPAR_TOKEN)));
}

UnitParser::BaseUnitProduction *UnitParser::BaseUnitProduction::instance = 0;

Parser::Production *
UnitParser::BaseUnitProduction::factory() {
  if (0 == BaseUnitProduction::instance) {
    return new BaseUnitProduction();
  }

  return BaseUnitProduction::instance;
}


UnitParser::ScaleProduction::ScaleProduction()
  : Parser::Production()
{
  ScaleProduction::instance = this;

  elements.push_back(new Parser::TokenProduction(Lexer::FLOAT_TOKEN));
  elements.push_back(
        new Parser::OptionalProduction(
          new Parser::Production(
            2, new Parser::TokenProduction(Lexer::EXP_TOKEN),
            new Parser::TokenProduction(Lexer::INTEGER_TOKEN))));
}

UnitParser::ScaleProduction *UnitParser::ScaleProduction::instance = 0;

Parser::Production *
UnitParser::ScaleProduction::factory()
{
  if (0 == ScaleProduction::instance) {
    return new ScaleProduction();
  }
  return ScaleProduction::instance;
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
