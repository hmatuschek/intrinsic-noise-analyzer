# Intrinsic noise in cellular systems #

Intrinsic noise in biochemical systems stems from the fact that chemical reactions occur in random order and at random times. This randomness becomes apparent when some molecular components are present only in very low copy numbers as is the case in living cells. In effect the time course of molecular populations is subject to increasing fluctuations as the number of molecules in the system decreases. In this case a deterministic description in terms of [ODEs](http://en.wikipedia.org/wiki/rate_equation) becomes inadequate and has to be replaced by a stochastic description.

## Effects of intrinsic noise ##

Intrinsic noise is widely accepted to account for fluctuations in the expression levels of nearly all genes in prokaryotic and eukaryotic cells. The main reason being that gene regulation takes place at a single DNA locus within a cell leading to tremendous fluctuations in the copy numbers of mRNA and proteins of genetically identical cells. In E. coli, mean mRNA and protein copy numbers have been determined in the range of 10<sup>-2</sup>-10 and 1-10<sup>3</sup>, respectively
([Taniguchi et al. (2010)](http://dx.doi.org/10.1126/science.1188308)).

Furthermore, real time observations of single enzymes experiments have shown that enzymatic turnovers are entirely stochastic events, a fact which is not evident from in vitro experiments. In living, highly compartmentalized, cells holding only a few tens to hundreds copies of a particular enzyme, such fluctuations might result in a non-negligible physiological effect ([English et al. (2005)](http://dx.doi.org/10.1038/nchembio759)).

## Quantitative analysis ##

A popular measure to quantify the variations in biochemical pathways is given by the [coefficient of variation](http://en.wikipedia.org/wiki/Coefficient_of_variation):

<img src='http://latex.codecogs.com/png.latex?\eta=\frac{\sigma}{\mu}%.png' title='coefficient of variation' />

which is a non-dimensional quantity. The symbol <img src='http://latex.codecogs.com/png.latex?\mu%.png' alt='mu' /> refers to the [statistical mean](http://en.wikipedia.org/wiki/Mean) in concentration of a chemical species while <img src='http://latex.codecogs.com/png.latex?{\sigma}%.png' title='coefficient of variation' /> refers to its [standard deviation](http://en.wikipedia.org/wiki/Standard_deviation) and hence the above expression measures the inverse signal-to-noise ratio and is sometimes referred to as "size of the noise". Both quantities, <img src='http://latex.codecogs.com/png.latex?$\mu$%.png' alt='mu' align='bottom' /> and <img src='http://latex.codecogs.com/png.latex?\sigma%.png' alt='sigma' align='bottom' />, can be computed using iNA by complementary methods:

  * the Chemical Master Equation describing the time evolution of the probability distribution and
  * the Stochastic Simulation Algorithm, a Monte Carlo strategy sampling time traces with exactly equivalent statistics.

The Chemical Master Equation is solved in iNA using the system size expansion. Please refer to the [User Guide](Help.md) for the accuracy of the approximation.

## What intrinsic noise is not... ##

Intrinsic noise can be used to describe the variations in populations of identical cells. However, it does not account for

  * the effects of cellular aging or cell division,
  * the variations due environmental influences such as temperature and light fluctuations,
  * the variations of kinetic parameters and organelle distributions,
  * the effects of crowding in the intracellular environment.

The above contributions can be summarized by term extrinsic noise which is only loosely defined and to-date lacks a widely accepted theoretical foundation.

## Intrinsic noise also affects... ##

...almost all models whose constituents are discrete entities and for which stoichiometries and propensities can be formulated. Such models have been employed far beyond the context of biochemical kinetics on the basis of the Stochastic Simulation Algorithm or equivalently the Master Equation:

  * **Epidemics** [Simões, da Gama, Nunes (2008)](http://dx.doi.org/10.1098/rsif.2007.1206)
  * **Dynamics of honeybee swarms** [Galla (2010)](http://dx.doi.org/10.1016/j.jtbi.2009.09.007)
  * **Neural networks** [Ohira, Cowan (1993)](http://dx.doi.org/10.1103/PhysRevE.48.2259)
  * **Protein folding** [Zwanzig (1995)](http://www.pnas.org/content/92/21/9801)
  * **Peptide dynamics** [Buchete,Hummer (2008)](http://dx.doi.org/10.1021/jp0761665)
  * **Calcium oscillations** [Dupont, Abou-Lovergne, Combettes (2008)](http://dx.doi.org/10.1529/biophysj.108.133777)
  * **Gene expression** [McAdams, Arkin (1997)](http://www.pnas.org/content/94/3/814).