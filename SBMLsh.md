# Introduction #
SBMLsh was proposed by [Darren Wilkinson](http://www.staff.ncl.ac.uk/d.j.wilkinson/software/sbml-sh/) allowing to write the essential SBML construct in a more human-readable form. It is also contained in the [semantic SBML](http://semanticsbml.org/semanticSBML/default/shorthand_sbml) tool and supports the basic SBML structure including the definition of units, species, compartment, parameters, reactions and events. In particular, it allows to write mathematical expressions in an comprehensible form. The major discrepancy between SBML and its shorthand notation is that the latter does not assign units as those are defined by the model's default ones. Furthermore, SBMLsh does not include any meta information (i.e., annotations and SBO terms) that hinder you from reading the essential model definition.

Here we give a brief step-by-step introduction to SBMLsh. Assume we want to describe the following set of reactions which constitute a common model of gene expression discussed in the [Tutorial](Tutorial1.md).

> <img src='http://latex.codecogs.com/png.latex?\text{Gene}\xrightarrow{k_0} \text{Gene} + \text{mRNA},%.png' />

> <img src='http://latex.codecogs.com/png.latex?\text{mRNA} \xrightarrow{k_\text{dM}} \varnothing,%.png' />

> <img src='http://latex.codecogs.com/png.latex?\text{mRNA} \xrightarrow{k_s} \text{mRNA} + \text{Protein}, %.png' />

> <img src='http://latex.codecogs.com/png.latex? \text{Protein} \xrightarrow{k_\text{dP}} \varnothing.%.png' />

The SBMLsh code reads as follows:

```
 @model:3.1.1=GeneExpression "Gene Expression"
 @units
  substance = mole: s=-6
  volume = litre
  time = second: m=60

 @compartments
  cell=1e-16 

 @species
  cell:[mRNA]=0
  cell:[Protein]=0
  cell:[Gene]=0.0166

 @parameters
  k0=135.542
  ks=4
  kdp=5
  kdm=10

 @reactions
  @r=transcription "Transcription"
   Gene -> Gene + mRNA
   cell*k0*Gene
  @r=translation "Translation"
   mRNA -> mRNA + Protein
   cell*ks*mRNA
  @r=degrade_Protein "Protein degradation"
   Protein ->
   cell*kdp*Protein
  @r=degrade_mRNA "mRNA degradation"
   mRNA -> 
   cell*kdm*mRNA
```

The first line is the model header.

```
@model:3.1.1=GeneExpression "Gene Expression" 
```

It specifies the basic information about the model as the SBMLsh version, model identifier (here `GeneExpression`) and model name (here _Gene Expression_). The SBMLsh version consists of 3 numbers. The first two specify the SBML version and level and the last specifies the SBMLsh version for that SBML version and level.

The remaining model is separated into blocks or sections for units, parameters, compartments, species etc. In the following, we give a brief overview over these sections.

The first block is the unit section, which allows for the specification of the model default units. In contrast to SBML, where user defined units can be specified to any variable (species, parameter or compartment), SBMLsh allows only to re-specify the default units for substance, volumes etc.

```
 @units
  substance = mole: s=-6
  volume = litre
  time = second: m=60
```

This section defines that the default substance unit is micron (1e-6 mole), the default volume unit is litre and the default time units is minute (60s).

The second section defines compartments of the model.

```
 @compartments
  cell=1e-16 
```

Here a compartment with the identifier `cell` is defined to have a constant volume of 1e-16 litre, as the volume unit was defined above to be _litre_.

The next section specifies the species defined in the model and also associates each species with the compartments it lives in.

```
 @species
  cell:[mRNA]=0
  cell:[Protein]=0
  cell:[Gene]=0.0166
```

Here 3 species (`mRNA`, `Protein` and `Gene`) are defined within the compartment `cell`. The species identifiers are enclosed in brackets, which means that the initial values (`0`, `0` and `0.0166`) are the initial concentrations of these species.

The next section defines the model global parameters (reactions can have their own, local parameter). The format is quiet similar to the compartment definition above.

```
 @parameters
  k0=135.542
  ks=4
  kdp=5
  kdm=10
```

The last section specifies the reactions of the model. Each definition consists of at least 3 lines. The first line specifies the reaction identifier and (optionally) the reaction name. The 2nd line specifies the reaction equation and the 3rd line specifies the kinetic law of the reaction.
```
 @reactions
  @r=transcription "Transcription"
   Gene -> Gene + mRNA
   cell*k0*Gene
  @r=translation "Translation"
   mRNA -> mRNA + Protein
   cell*ks*mRNA
  @r=degrade_Protein "Protein degradation"
   Protein ->
   cell*kdp*Protein
  @r=degrade_mRNA "mRNA degradation"
   mRNA -> 
   cell*kdm*mRNA
```

The first reaction named "Transcription" is defined  in a few lines
```
  @r=transcription "Transcription"
   Gene -> Gene + mRNA
   cell*k0*Gene
```
meaning

<img src='http://latex.codecogs.com/png.latex?\text{Gene}\xrightarrow{k_0}%20\text{Gene}%20+%20\text{mRNA}.%.png' />

Similarly the reaction

<img src='http://latex.codecogs.com/png.latex?%20\text{mRNA}%20\xrightarrow{k_\text{dM}}%20\varnothing,%.png' />

is represented by

```
  @r=degrade_mRNA "mRNA degradation"
   mRNA -> 
   cell*kdm*mRNA
```


# Formal SBMLsh Grammar #
Unfortunately, there exists no "official" precise definition of SBMLsh in terms of a grammar definition, but a detailed explanation is given by [Darren Wilkinson](http://www.staff.ncl.ac.uk/d.j.wilkinson/software/sbml-sh/). We therefore give the [EBNF](http://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_Form) grammar of SBMLsh of its implementation in iNA.

The general structure of a SBMLsh file consists of a mandatory model header and followed by optional sections for unit, compartment, species, parameter, reaction and event definitions.

```
<SBMLsh> ::= <ModelHeader> [<EOL> <UnitSec>] [<EOL> <CompartmentSec>] [<EOL> <SpeciesSec>] [<EOL> <ParameterSec>]
              [<EOL> <RuleSec>] [<EOL> <ReactionSec>] [<EOL> <EventSec>]
```

Where `<EOL>` represents one or more line endings (i.e. `\n` or `\r`). As only the `ModelHeader` production is mandatory, also an empty model (without species etc.) is a valid SBMLsh model.

The model header defines some basic information about the model. It specifies the SBMLsh version, model identifier, an optional model name and optionally the default units for the model.

```
<ModelHeader> ::= '@model' ':' <VersionString> '=' <Identifier> [<QuotedString>] [<EOL> <DefaultUnitDefinitions>]

<VersionString> ::= <Integer> '.' <Integer> '.' <Integer>
```

The version number consists of 3 integers separated by dots, i.e. `x.y.z`, where `x` denotes the addressed SBML level, `y` the SBML version of that level and `z` the version of SBMLsh for the SBML level and version. The mandatory `Identifier` production specifies the model ID, an unique identifier for model. The model identifier may be followed by a descriptive (non-unique) name of the SBML model as a quoted string (i.e. "model name").

```
<DefaultUnitDefinitions> ::= <DefaultUnitDefinition> <DefaultUnitDefinitions>
  | <DefaultUnitDefinition>

<DefaultUnitDefinition> ::= ('s' | 't' | 'v' | 'a' | 'l' | 'e' | 'c') '=' <GenericUnit>

<GenericUnit> ::= 'ampere' | 'becquerel' | 'candela' | 'coulomb' | 'dimensionless' | 'farad' | 'gram' | 'katal' | 'gray' 
  | 'kelvin' | 'henry' | 'kilogram' | 'hertz' | 'litre' | 'item' | 'lumen' | 'joule' | 'lux' | 'metre' | 'mole' 
  | 'newton' | 'ohm' | 'pascal' | 'radian' | 'second' | 'watt' | 'siemens' | 'weber' | 'sievert' | 'steradian' 
  | 'tesla' | 'volt'
```

After the mandatory part of the model header, it is possible to define the global default units using the optional `DefaultUnitDefinition` production. This production is a series of assignments defining the default unit for substance (`s`), time (`t`), volume (`v`), area (`a`), length (`l`), ??? (`e`) and ??? (`c`). The identifier of the right side of the assignment must be one of the pre-defined basis units of SBML.


## Units Section ##
Within the section _Units_ it is possible to define new units. A unit definition starts with the keyword `@units` followed by a list of at least one unit-definition.

```
<UnitSec> ::= '@units' <UnitDefinitionList>

<UnitDefinitionList> ::= <EOL> <UnitDefinition> <UnitDefinitionList>
  | <EOL> <UnitDefinition>
  
<UnitDefinition> ::= <Identifier> '=' <ScaledUnitList> [<QuotedString>]

<ScaledUnitList> ::= <ScaledUnit>
  | <ScaledUnit> ';' <ScaledUnitList>

<ScaledUnit> ::= <GenericUnit> [':' <ScaledUnitModifiers>]

<ScaledUnitModifiers> ::= ('m' | 's' | 'e' | 'o') '=' <Number> [',' <ScaledUnitModifiers>]
```

Each unit definition consists of an `Identifier` specifying the unique identifier of the unit and a list of scaled generic units (`ScaledUnit` production). A `ScaledUnit` production consists of a generic SBML unit followed by an optional modifier specifying the exponent (`e`), multiplier (`m`) scale (`s`) or offset (`o`). For the definition of the `GenericUnit` production, please see above.


## Compartments Section ##
The section _Compartments_ assigns compartments different species may reside in. This section starts with the keyword `@compartments` followed by a list of at least one compartment definition separated by line breaks.

```
<CompartmentSec> ::= '@compartments' <CompartmentList>

<CompartmentList> ::= <EOL> <Compartment> <CompartmentList>
  | <EOL> <Compartment> 
  
<Compartment> ::= <Identifier> '=' <Expression> [<QuotedString>]
```

A compartment definition consists of an identifier specifying its unique id followed by the initial value expression and finally the optional name of the compartment.

> _Note:_ Modifiers specifying whether the compartment is variable or constant in size are not defined. Therefore all compartments are assumed to be constant. If this is assumed, the initial value for the compartment must be mandatory.


## Species Section ##
The section _Species_ consists of the keyword `@species` followed by a list of at least one of its definitions seperated by line breaks. The species definition (`Species` production) consists of the compartment identifier the species is associated with, the species identifier, an initial value, an optional list of species modifiers and an optional (descriptive) name of the species.

```
<SpeciesSec> ::= '@species' <SpeciesList>

<SpeciesList> ::= <EOL> <Species> <SepeciesList> 
 | <EOL> <Species> 
 
<Species> ::= <Identifier> ':' (('[' <Identifier>']') | <Identifier>) 
              '=' <Expression> [<SpeciesModifierList>] [<QuotedString>]

<SpeciesModifierList> ::= <SpeciesModifier> <SpeciesModifierList>
  | <SpecieModifier>
  
<SpeciesModifier> ::= 's' | 'b' | 'c'
```

The optional species modifier list allows to specify whether the species has substance units (`s`), has a boundary condition (`b`) or is constant (`c`). Any combination of these modifiers are allowed. If the identifier of the species is given in rectangular brackets the initial value is assumed to be given in concentration units rather then substance units.


## Parameters Section ##
The section _Parameters_ consists of the keyword `@parameters` followed by a list of at least one parameter definition separated by line breaks. The parameter definition consists of an unique identifier and an initial value followed by an optional `v` modifier specifying that the parameter is variable meaning that it is non-constant as well as an optional name.

```
<ParameterSec> ::= '@parameters' <ParameterList>

<ParameterList> ::= <EOL> <Parameter> <ParameterList>
  | <EOL> <Parameter>
  
<Parameter> ::= <Identifier> '=' <Expression> ['v'] [QuotedString]
```


## Rules Section ##

SBMLsh allows to define rate and assignment rules to a model within the section _Rules_. This section consists of the keyword `@rules` and a list of at least one rule separated by line breaks.

```
<RuleSec> ::= '@rules' <RuleList>

<RuleList> ::= <EOL> <Rule> <RuleList>
  | <EOL> <Rule>
  
<Rule> ::= '@assign' ':' <Identifier> '=' <Expression>
  | '@rate' ':' <Identifier> '=' <Expression>
```

A definition of a rule consists of an species identifier (or parameter) for which the rule is defined followed by an expression. This defines the assignment rule for a given species (parameter). Alternatively the rule can be assigned a prefix '@rate' to define a rate rule.


## Reactions Section ##
Reactions can be defined within the section _Reactions_. This section consists of the keyword `@reactions` followed by a list of at least on reaction definition separated by line breaks.

```
<ReactionSec> ::= '@reactions' <ReactionList>

<ReactionList> ::= <EOL> <Reaction> <ReactionList>
  | <EOL> <Reaction>
  
<Reaction> ::= ('@r' | '@rr') '=' <Identifier> [<QuotedString>] <EOL> 
               <ReactionEquation> [':' <ReactionModifierList>] <EOL>
               <KineticLaw>

<ReactionModifierList> ::= <Identifier> [',' <ReactionModifierList>]
```

A reaction definition consists of at least 3 lines. The first line starts with either the `@r` or with the `@rr` keyword. The former defines an irreversible reaction while the latter defines a reversible one. The keyword is followed by an unique reaction identifier and an optional descriptive name. The second line defines the chemical equation followed by an optional identifier specifying reaction modifiers. The third line specifies the kinetic law expression.

```
<ReactionEquation> ::= <Stoichiometry> '->' [<Stoichiometry>]
  | '->' <Stoichiometry>
  
<Stoichiometry> ::= [<Integer>] <Identifier> '+' <Stoichiometry>
  | [<Integer>] <Identifier>
```

A reaction equation consists of either a stoichiometry expression for the reactants and an optional stoichiometry expression for the products, or by defining only the stoichiometry for the products. The stoichiometry expression consists of a sum of identifiers of the reactants or products prefixed with an optional integer, specifying the multiplicity for the species. If the multiplicity is left out 1 is assumed.

```
<KineticLaw> ::= <Expression> [':' <LocalParameterList>]

<LocalParameterList> ::= <LocalParameter> ',' <LocalParameterList>
  | <LocalParameter>
  
<LocalParameter> ::= <Identifier> '=' <Expression>
```

The kinetic law definition consist of an expression for the kinetic law followed by an optional list of local parameters. The list of local parameters is just a semicolon separated list of assignments, where the identifier specifies the unique identifier of the local parameter and the expression specifies its value.


## Events Section ##
Events can be described within the section _Events_. This section consist of the `@events` keyword followed by a list of at least one event definition.

```
<EventSec> ::= '@events' <EventList>

<EventList> ::= <EOL> <Event> <EOL> <EventList>
  | <EOL> <Event>
  
<Event> ::= <Identifier> '=' <Condition> [';' <Expression>] ':' <AssignmentList> [<QuotedString>]

<Condition> ::= <Identifier> ('"=="' | '"!="' | '">"' | '">="' | '"<"' | '"<="') <Expression>;

<AssignmentList> ::= <Assignment> ';' <AssignmentList>
  | <Assignment>
  
<Assignment> ::= <Identifier> '=' <Expression>
```