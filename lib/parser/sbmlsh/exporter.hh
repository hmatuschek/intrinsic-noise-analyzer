#ifndef __INA_PARSER_SBMLSH_EXPORTER_HH__
#define __INA_PARSER_SBMLSH_EXPORTER_HH__

#include <ostream>
#include <ast/model.hh>
#include <ast/reaction.hh>


namespace Fluc {
namespace Parser {
namespace Sbmlsh {

void __process_model(Ast::Model &model, std::ostream &output);
void __process_model_header(Ast::Model &model, std::ostream &ouput);
std::string __get_base_unit_identifier(Ast::ScaledBaseUnit::BaseUnit);
void __process_unit_definitions(Ast::Model &model, std::ostream &output);
void __process_compartments(Ast::Model &model, std::ostream &output);
void __process_compartment(Ast::Compartment *comp, std::ostream &output);
void __process_species_list(Ast::Model &model, std::ostream &output);
void __process_species(Ast::Species *species, std::ostream &output);
void __process_parameter_list(Ast::Model &model, std::ostream &output);
void __process_parameter(Ast::Parameter *param, std::ostream &output);
void __process_rule_list(Ast::Model &model, std::ostream &output);
void __process_rule(Ast::Rule *rule, std::ostream &output);
void __process_reaction_list(Ast::Model &model, std::ostream &output);
void __process_reaction(Ast::Reaction *reac, std::ostream &output);
void __process_kinetic_law(Ast::KineticLaw *law, std::ostream &output);
void __process_event_list(Ast::Model &model, std::ostream &output);

}
}
}

#endif // __INA_PARSER_SBMLSH_EXPORTER_HH__
