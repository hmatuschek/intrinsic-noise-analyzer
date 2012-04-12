#include "fspbase.hh"
#include "openmp.hh"

namespace Fluc {
namespace Models {

FSPBase::FSPBase(libsbml::Model *model) :
  BaseModel(model),
  ConservationAnalysisMixin((BaseModel &)(*this)),
  ParticleNumbersMixin((BaseModel &)(*this)),
  sparseStoichiometry(numIndSpecies(),numReactions()),
  dependencyGraph(numReactions(),numReactions()),
  conservationConstants(numDepSpecies()),
  rates(numReactions())
{

    size_t opt_level=0;

    // initalize symbols as placeholders for constants arising from conservation laws
    for(size_t i=0;i<numDepSpecies();i++)
        conservationConstants(i) = GiNaC::symbol();

    Eigen::VectorXex spec(this->numSpecies());
    Eigen::VectorXex dep_species(this->numDepSpecies());
    Eigen::VectorXex ind_species(this->numIndSpecies());

    for(size_t j=0; j<this->species.size(); j++)
        spec(j) = this->species[j];

    ind_species = (this->PermutationM.cast<GiNaC::ex>() * spec).head(this->numIndSpecies());
    dep_species = (this->PermutationM.cast<GiNaC::ex>() * spec).tail(this->numDepSpecies());

    // reconstruct dependencies due to conservation laws
    // and substititute conservation constants as symbols
    Eigen::VectorXex dependence = conservationConstants + this->link_zero_matrix.cast<GiNaC::ex>()*ind_species;

    // generate substitution table to remove dependent species
    GiNaC::exmap subs_table;
    for (size_t s=0; s<this->numDepSpecies(); s++)
        subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( dep_species(s), dependence(s) ) );

    // ... and substitute
    for (size_t i=0; i<this->numReactions(); i++)
        this->rates[i] = foldConstants(this->propensities[i].subs(subs_table));


    // setup the interpreter from a dependency graph
    int d;
    for (size_t j=0;j<this->numReactions();j++)
    {
        this->dependencyGraph.startVec(j);
        for (size_t i=0;i<this->numReactions();i++)
        {
            d=0;
            for(size_t k=0;k<this->numIndSpecies();k++)
                // ask if species k is changed by reaction j
                // which affects the propensities of reaction i in which species k is a modifier or a reactant
                d = d || ( (this->reactants_stoichiometry(k,i)!=0 || this->propensities[i].has(this->species[k]) ) && this->stoichiometry(k,j)!=0 ) ;

            if (d!=0)
            {
                this->dependencyGraph.insertBack(i,j) = d;
            }
        }
    }
    this->dependencyGraph.finalize();

    // fill sparse stoichiometry
    for(size_t j=0; j<this->numReactions(); ++j)
    {
      this->sparseStoichiometry.startVec(j);
      for(size_t i=0; i<this->numIndSpecies(); i++)
          if (this->reduced_stoichiometry(i,j)!=0) this->sparseStoichiometry.insertBack(i,j) = this->reduced_stoichiometry(i,j);
    }
    this->sparseStoichiometry.finalize();

    // Evaluate initial concentrations and evaluate volumes:
    Ast::EvaluateInitialValue evICs(*this);
    Eigen::VectorXd ICs(species.size());
    for(size_t i=0; i<species.size();i++){
       ICs(i)=evICs.evaluate(this->species[i]);
       //this->Omega(i)=evICs.evaluate(this->volumes(i));
    }
    // and permute
    this->ICsPermuted = this->PermutationM*ICs;

    // generate substitution table
    if(numDepSpecies()>0){


        GiNaC::exmap subs_table;
        // compute conserved cycles in permutated base and store in conserved_cycles
        this->conserved_cycles = this->conservation_matrix*this->ICsPermuted;

        // now fold coverservation constants for all coefficients

        for (size_t s=0; s<this->numDepSpecies(); s++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( this->conservationConstants(s), conserved_cycles(s) ) );


        // ... and fold all constants due to conservation laws
        for (size_t i=0; i<this->numReactions(); i++)
            this->rates[i] = this->rates[i].subs(subs_table);

    }

    // make index table of independent species
    for(size_t i=0; i<this->numSpecies(); i++)
       //for(size_t j=0; j<this->numSpecies(); j++)
          // if(this->PermutationM(i,j))
        this->stateIndex.insert(std::make_pair(this->getSpecies(i)->getSymbol(),i));

    Intprt::Compiler<Eigen::VectorXd> compiler(this->stateIndex);

    compiler.compileVector(rates);

    // optimize and store
    compiler.optimize(opt_level);
    this->all_bytecode = compiler.getCode();

    // set interpreter code
    interpreter.setCode(&all_bytecode);

}

}
}
