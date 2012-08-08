#ifndef __INA_TRAFO_PARTIALMODEL_HH__
#define __INA_TRAFO_PARTIALMODEL_HH__

namespace Fluc {
namespace Trafo {


/** This class manages a permutation and link matrix, such that all species defined in a model
 * are separated into a set of independent species and a set of (dependent) species that
 * are defined as linear combinations of the independent species (the link matrix).
 *
 * This class tries to unify constraints defined as @c Ast::AlgebraicConstraint,
 * @c Ast::AssignementRule or defined by a @c Model::ConservationAnalysis for species variables,
 * into a single layer. This is achieved by first resolving all initially defined
 * @c Ast::AssignmentRule instances attaced to @c Ast::Species instance using the
 * @c Trafo::AssginementRuleInliner. Then the system of linear constraints, initially defined by
 * all @c Ast::AlgebraicConstraint instances is solved and turned into a set of
 * @c Ast::AssignementRule for some species. These rules are then again inlined using the
 * @c Trafo::AssignementRuleInliner. Finally and optionally the @c Trafo::ConservartionAnalysis
 * can be used to find conserved cycles in the remaining independent species. The conservation
 * analysis may create again some @c Ast::AssignmentRule instances, that are inlined using the
 * @c Trafo::AssignmentRuleInliner operator.
 *
 * To achieve this, certain assertions must be made on the @c Ast::Model. First, all initially
 * defined @c Ast::AssignmentRule instances must be linear in independent species (species w/o an
 * @c Ast::AssignmentRule or @c Ast::RateRule). Furthermore, the set of @a Ast::AlgebraicConstraint
 * instances must form a linear system of equations in the independent variables.
 *
 * Once all @c Ast::AlgebraicConstraints are resolved and the conservation analysis defined
 * @c Ast::AssignmentRule instances for (now) dependent species, the @c PartialModel assembles
 * a @c permutationM matrix and a @c linkMatrix matrix, that allows to map from the space of
 * independent species into the original space of (dependent and independent) species, where the
 * link patrix constist of an unit matrix part for the independent species and a part for the
 * dependent species (formed by the linear assignment rules). The permutation matrix just resorts
 * the (original) species such that the first @c numIndep elements of the permuted species vector
 * are independent species and the remaining elements are dependent species.
 *
 * @todo Implement: Traverse tree and assign new indices to species as they are dependent or
 *       independent. Then assemble link matrix for dependen species.
 *
 * @ingroup trafo */
class PartialModel
{
public:
  PartialModel();
};


}
}

#endif
