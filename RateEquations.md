This content is also available from [Wikipedia](http://en.wikipedia.org/wiki/Rate_equation#Stoichiometric_reaction_networks).

## Stoichiometric reaction networks ##

The most general description of chemical reaction network considers a number
<img src='http://latex.codecogs.com/png.latex?N%.png' /> of distinct chemical species reacting via <img src='http://latex.codecogs.com/png.latex?R%.png' /> reactions. The chemical equation of the <img src='http://latex.codecogs.com/png.latex?j%.png' />-th reaction can then be written in the generic form

<img src='http://latex.codecogs.com/png.latex?s_{1j}X_1+s_{2j}X_2\ldots+s_{Nj}X_{N} \xrightarrow{k_j}r_{1j}X_{1}+r_{2j}X_{2}+\ldots+r_{Nj}X_{N},%.png' />

which is often written in the more compact form

<img src='http://latex.codecogs.com/png.latex?\sum_{i=1}^{N}s_{ij}X_i\xrightarrow{k_j} \sum_{i=1}^{N}\ r_{ij} X_{i}.%.png' />

Here <img src='http://latex.codecogs.com/png.latex?j%.png' /> is the reaction index running from 1 to <img src='http://latex.codecogs.com/png.latex?R%.png' />, <img src='http://latex.codecogs.com/png.latex?X_i%.png' /> denotes chemical species <img src='http://latex.codecogs.com/png.latex?i%.png' />, <img src='http://latex.codecogs.com/png.latex?k_j%.png' /> is the reaction rate of the <img src='http://latex.codecogs.com/png.latex?j%.png' />-th reaction and  <img src='http://latex.codecogs.com/png.latex?s_{ij}%.png' /> and <img src='http://latex.codecogs.com/png.latex?r_{ij}%.png' /> are the stoichiometric coefficients of reactants and products, respectively. Such networks are often referred to as **stoichiometric reaction networks**.
The rate of such reaction can be inferred by the [law of mass action](http://en.wikipedia.org/wiki/Law_of_mass_action)

<img src='http://latex.codecogs.com/png.latex?f_j([\vec{X}])= k_j \prod_{z=1}^N [X_z]^{s_{zj}}%.png' />

where <img src='http://latex.codecogs.com/png.latex?[\vec{X}]=([X_1], [X_2], ... ,[X_N])%.png' /> is the vector of concentrations.
Note that this definition includes the [elementary reactions](http://en.wikipedia.org/wiki/Elementary_reaction).
Each of which are discussed in detail below. By definition of the [stoichiometric matrix](http://en.wikipedia.org/wiki/Stoichiometric_Matrix#Stoichiometry_matrix)
<img src='http://latex.codecogs.com/png.latex?S_{ij}=r_{ij}-s_{ij},%.png' />
denoting the net extend of molecules of <img src='http://latex.codecogs.com/png.latex?i%.png' /> in reaction <img src='http://latex.codecogs.com/png.latex?j%.png' />. The reaction rate equations can then be written in the compact form

<img src='http://latex.codecogs.com/png.latex?\frac{d [X_i]}{dt} =\sum_{j=1}^{R} S_{ij} f_j([\vec{X}]).%.png' />

Particular simple solutions exist in equilibrium, <img src='http://latex.codecogs.com/png.latex?{d [X_i]}/{dt}=0.%.png' />, for systems composed of merely reversible reactions. In this case the rate of the forward and backward reactions are equal, a principle called [detailed balance](http://en.wikipedia.org/wiki/Detailed_balance). Note that detailed balance is a property of the stoichiometric matrix <img src='http://latex.codecogs.com/png.latex?S_{ij}%.png' /> alone and does not depend on the particular form of the rate functions <img src='http://latex.codecogs.com/png.latex?f_j%.png' />. All other cases where detailed balance is not of interest are commonly dealt with by [flux balance analysis](http://en.wikipedia.org/wiki/Flux_balance_analysis) intended to understand [metabolic pathways](http://en.wikipedia.org/wiki/Metabolic_pathway).

## Example ##

...