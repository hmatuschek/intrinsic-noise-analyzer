#include "ina_cli_listmodel.h"

#include "ina_cli_importmodel.hh"
#include <parser/expr/parser.hh>
#include <utils/logger.hh>
#include <iostream>

using namespace iNA;


// strip all white-spaces from string
std::string &stringStrip(std::string &string) {
  std::string tmp;
  for (size_t i=0; i<string.size(); i++) {
    char c = string.at(i);
    if ( (' '==c) || ('\t'==c) || ('\n'==c) || ('\r'==c)) { continue; }
    tmp.append(1, c);
  }
  string=tmp;
  return string;
}



int listSpecies(Utils::Opt::Parser &option_parser)
{
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }

  std::cout << "Species of model \"" << model->getName()
            << "\" (id: " << model->getIdentifier() << "):" << std::endl;

  for (size_t i=0; i<model->numSpecies(); i++) {
    Ast::Species *species = model->getSpecies(i);
    std::cout << " id:" << species->getIdentifier() << std::endl;
    if (species->hasName()) {
      std::cout << "  name:\"" << species->getName() <<"\"" << std::endl;
    }
    if (species->hasValue()) {
      std::cout << "  value:";
      Parser::Expr::serializeExpression(species->getValue(), std::cout, model);
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}


int listParameters(Utils::Opt::Parser &option_parser)
{
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }

  std::cout << "Parameters of model \"" << model->getName()
            << "\" (id: " << model->getIdentifier() << "):" << std::endl;

  for (size_t i=0; i<model->numParameters(); i++) {
    Ast::Parameter *parameter = model->getParameter(i);
    std::cout << " id:" << parameter->getIdentifier() << std::endl;
    if (parameter->hasName()) {
      std::cout << "  name:\"" << parameter->getName() <<"\"" << std::endl;
    }
    if (parameter->hasValue()) {
      std::cout << "  value:";
      Parser::Expr::serializeExpression(parameter->getValue(), std::cout, model);
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}


int listCompartments(Utils::Opt::Parser &option_parser)
{
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }

  std::cout << "Compartments of model \"" << model->getName()
            << "\" (id: " << model->getIdentifier() << "):" << std::endl;

  for (size_t i=0; i<model->numCompartments(); i++) {
    Ast::Compartment *compartment = model->getCompartment(i);
    std::cout << " id:" << compartment->getIdentifier() << std::endl;
    if (compartment->hasName()) {
      std::cout << "  name:\"" << compartment->getName() <<"\"" << std::endl;
    }
    if (compartment->hasValue()) {
      std::cout << "  value:";
      Parser::Expr::serializeExpression(compartment->getValue(), std::cout, model);
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}


int setInitialValue(Ast::Model *model, Utils::Opt::Parser &option_parser)
{
  std::list<std::string> items = option_parser.get_option("update-value");

  for(std::list<std::string>::iterator item=items.begin(); item!=items.end(); item++)
  {
    // Get identifier and expression strings:
    std::string assignment = *item;
    size_t idx = assignment.find_first_of('=');
    std::string identifier = assignment.substr(0, idx);
    std::string expression = assignment.substr(idx+1);
    identifier = stringStrip(identifier);

    // check if an identifier is given:
    if (0 == identifier.size()) {
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Can not set value: Empty identifier given.";
      Utils::Logger::get().log(message);
      return -1;
    }

    // parse expression in global model context:
    GiNaC::ex value;
    try { value = Parser::Expr::parseExpression(expression, model); }
    catch (Exception &err) {
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Can not parse expression \"" << expression << "\": " << err.what();
      Utils::Logger::get().log(message);
      return -1;
    }

    // Get variable by identifier:
    if (! model->hasVariable(identifier)) {
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Can not set value of \"" << identifier << "\": Unknown identifier.";
      Utils::Logger::get().log(message);
      return -1;
    }

    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Set initial value of " << identifier << " to ";
    Parser::Expr::serializeExpression(value, message, model);
    message << std::endl;
    Utils::Logger::get().log(message);

    // Finally set the value
    model->getVariable(identifier)->setValue(value);
  }

  // Done.
  return 0;
}
