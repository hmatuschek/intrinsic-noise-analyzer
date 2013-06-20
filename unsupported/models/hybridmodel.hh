#ifndef __INA_HYBRIDMODEL_HH__
#define __INA_HYBRIDMODEL_HH__

#include "ast/ast.hh"
#include <ginac/ginac.h>

#include "models/basemodel.hh"
#include "models/constantstoichiometrymixin.hh"

#include "ast/modelcopyist.hh"


namespace iNA {
namespace Models {

class SignalOfInterest
    : public std::vector<GiNaC::ex>
{

public:

  std::set<Ast::Species *> exS;
  std::vector<std::string> copy;

  SignalOfInterest(const Ast::Model &model, const std::vector<std::string> &specList)
    : copy(specList)

  {

    GiNaC::parser reader;
    for(size_t i=0; i<specList.size(); i++)
      this->push_back(reader(specList[i]));

    GiNaC::symtab table = reader.get_syms();
    GiNaC::exmap substitutions;

    for(GiNaC::symtab::iterator it=table.begin(); it!=table.end(); it++)
    {
      GiNaC::ex exp;
      try
      {
         // Resolve symbol
         exp = model.getSpecies((*it).first)->getSymbol();
         // Add to list of external species
         exS.insert(model.getSpecies((*it).first));
      }
      catch(SymbolError &err)
      {
        try
        {
           // Fold parameter values immediately
           exp = model.getParameter((*it).first)->getValue();
        }
        catch(SymbolError &err)
        {
          SymbolError err;
          err << "Symbol " << (*it).first << " can neither be resolved as species or parameter.";
          throw err;
        }
      }

      // If found add to substitution table
      substitutions.insert(std::make_pair(table[(*it).first],exp));

    }

    // Carry out substitutions
    for(iterator it=this->begin(); it!=this->end(); it++)
      (*it)=(*it).subs(substitutions);

  }

  const std::set<Ast::Species *> &getSpeciesOfInterest() const
  {
    return this->exS;
  }



};


/**
 * Builds an hybrid model.
 *
 * @ingroup unsupported
 */
class HybridModel
        : public Ast::Model,
          private ConstantStoichiometryMixin

{

protected:

  Ast::Model external;
  SignalOfInterest soi;

public:

  /**
    Constructor.
    @param model Ast::Model to hybridize.
    @param specList Minimal list of external species.
  **/
  HybridModel(Ast::Model &model, const std::vector<std::string> &soiList);

  /**
   * Returns the external model
   */
  Model &getExternalModel();

  const SignalOfInterest &getSoI() const;

protected:

  /**
    Selects all reactions that change species along with those in list @param exS
  **/
  void selectReactions(std::set<Ast::Species *> &exS, std::set<Ast::Reaction *> &exR);

  /**
    Assembles the list of species changed in the reactions exR
  **/
  void assembleExternalSpeciesList(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS);

  /**
   Check exogeneity of external process used in distll.
  */
  bool checkConsistency(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS);

  /**
   Distills a model with list of species and reactions. Returns empty exR.
   TODO: avoid unnecessary parameters and things.
  */
  void distill(Ast::Model * src, Ast::Model *external, std::set<Ast::Reaction *> exR, GiNaC::exmap &translation_table);

  /**
   Returns species definition as parameter.
  */
  Ast::Parameter * SpeciestoParameter(Ast::Species *node, GiNaC::exmap &translation_table,
                                  std::map<Ast::Species *, Ast::Species *> &species_table);

  void dump(std::ostream &str);

};


}
}

#endif
