Concerns iNA 0.3.1 and above.

# Background #

Many reaction models defined in SBML models make use of reversible reactions. An example is the reaction

<img src='http://latex.codecogs.com/png.latex?A \rightleftharpoons B%.png' title='revreaction' />

with rate

<img src='http://latex.codecogs.com/png.latex?r(\vec{n})=k_f\, n_A - k_r\,n_B%.png' title='rate' />

where <img src='http://latex.codecogs.com/png.latex?k_f%.png' /> and <img src='http://latex.codecogs.com/png.latex?k_r%.png' /> are the forward and backward rate constants, respectively.
Unfortunately such rate laws can be justified only for deterministic rate equations but cannot be interpreted as a reaction propensity; i.e. the probability per unit time for a reaction to occur. In fact such propensities would not be guaranteed to be positive. Therefore iNA automatically converts such reactions into pairs of irreversible ones:

<img src='http://latex.codecogs.com/png.latex?A \rightarrow B, \qquad B \rightarrow A%.png' title='forwreaction' />

with propensities for the forward and backward reactions:

<img src='http://latex.codecogs.com/png.latex?a_f(\vec{n})=k_f\, n_A, \qquad a_r(\vec{n})=k_r\, n_B%.png' title='forwardrate' />

which is the legitimate description both for the stochastic simulation algorithm and the Chemical Master Equation.

# Details #

Given a reversible reaction

<img src='http://latex.codecogs.com/png.latex?\sum_i s_{ij} R_i \rightleftharpoons \sum_i r_{ij} P_i%.png' title='' />

with reactants <img src='http://latex.codecogs.com/png.latex?R_i%.png' title='' />  and non-exclusive products <img src='http://latex.codecogs.com/png.latex?P_i%.png' title='' /> and rate <img src='http://latex.codecogs.com/png.latex?r(\vec{n}_R,\vec{n}_P)%.png' title='' />.

iNA attempts to write the latter as rational expression in the form

<img src='http://latex.codecogs.com/png.latex?r(\vec{n}_R,\vec{n}_P)=\frac{f(\vec{n}_R)+g(\vec{n}_P)}{D(\vec{n}_R,\vec{n}_P)}+k%.png' title='' />

where the functions f and g are related to the forward and reverse reaction propensity

<img src='http://latex.codecogs.com/png.latex?a_f(\vec{n})=\frac{f(\vec{n}_R)}{D(\vec{n}_R,\vec{n}_P)} \qquad a_r(\vec{n})=\frac{g(\vec{n}_R)}{D(\vec{n}_R,\vec{n}_P)}%.png' />

whenever <img src='http://latex.codecogs.com/png.latex?k=0%.png' title='' />. In the case <img src='http://latex.codecogs.com/png.latex?k\neq 0%.png' title='' /> this constant accounts for constant fluxes whenever either <img src='http://latex.codecogs.com/png.latex?s_{ij}=0%.png' title='' /> or
<img src='http://latex.codecogs.com/png.latex?r_{ij}=0\,\forall i%.png' title='' />.