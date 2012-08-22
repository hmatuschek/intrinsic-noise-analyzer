#include "unitparser.hh"
#include "math.hh"

using namespace Fluc;


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
  return parser.processUnit(cst[0][0]);
}


Ast::Unit
UnitParser::processUnit(Parser::ConcreteSyntaxTree &node) {
  Ast::Unit unit = processBaseUnit(node[0]);

  if (node[1].matched()) {
    if (0 == node[1][0][0].getAltIdx()) {
      unit = unit * processUnit(node[1][0][1]);
    } else {
      unit = unit / processUnit(node[1][0][1]);
    }
  }

  return unit;
}


Ast::Unit
UnitParser::processBaseUnit(Fluc::Parser::ConcreteSyntaxTree &node)
{
  /// \todo Implement!!!
  return Ast::Unit();
}
