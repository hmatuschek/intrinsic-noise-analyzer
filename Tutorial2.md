We here give a four-step tutorial of how noise properties of nonlinear reactions can analyzed using iNA. The tutorial concludes with two "Tasks".



This tutorial assumes knowlegde of the introductory tutorial that can be found [here](Tutorial1.md).

# Tutorial II: Gene expression with functional enzyme #

In this tutorial we extend the simple gene expression model of [Tutorial I](Tutorial1.md) by modelling protein degradation reaction to occur via a functional enzyme.
The reactions describing transcription, translation and mRNA degradation are

> <img src='http://latex.codecogs.com/png.latex?\text{Gene}\xrightarrow{k_0} \text{Gene} + \text{mRNA},%.png' />

> <img src='http://latex.codecogs.com/png.latex?\text{mRNA} \xrightarrow{k_\text{dM}} \varnothing,%.png' />

> <img src='http://latex.codecogs.com/png.latex?\text{mRNA} \xrightarrow{k_s} \text{mRNA} + \text{Protein}. %.png' />

The reactions describing the degradation of protein occurring via a catalytic enzyme-protein complex with [Michaelis-Menten](http://en.wikipedia.org/wiki/Michaelis%E2%80%93Menten_kinetics) kinetics can be written as

> <img src='http://latex.codecogs.com/png.latex? \text{Protein} + \text{Enzyme}  \mathop{\rightleftharpoons}^{k_1}_{k_{-1}} \text{Complex} \xrightarrow{k_2} \text{Enzyme} + \varnothing.%.png' />

Such models also occur frequently when considering protein consumption by metabolic pathways or post-translational modifications. The key tool for the analysis are the EMREs whose application is the purpose of this tutorial.

## Step 1: Model Preparation ##

We extend the model in Tutorial I by including the enzymatic degradation step in the above reaction scheme. In order to do this simply select _Reactions_ from the tree view on the left hand side which will show the reaction list. You can now select the protein degradation reaction and remove using the button labeled "-" on top of the list. With a click on the button "+" the reaction editor will appear and you are asked to enter the details on the reaction. For the enzyme-complex binding reaction your input should look like this:

> ![http://i49.tinypic.com/t8t65y.png](http://i49.tinypic.com/t8t65y.png)

> _The reaction editor allows you to specify a chemical equation from which the propensity will be generated automatically. Note also that previously undefined species will be automatically added to your model._

Note that species _Complex_ has not yet been defined but iNA takes care of this issue automatically. However, you may want to set its initial values (_0_) afterwards by selecting the species _Complex_ from the species list. By selecting this new reaction from the tree view on the left hand side you will see an overview showing the chemical equation and the propensity of the reaction. You will also be able to edit it as well as setting the rate constant as shown below.

> ![http://i47.tinypic.com/30cvhxz.png](http://i47.tinypic.com/30cvhxz.png)

> _Reaction view._

In this example we used a values of <img src='http://latex.codecogs.com/png.latex?k_1=100 {(\mu M s)}^{-1}%.png' />, where the unit is implicitly specified through the model units. The remaining two reactions can be added similarly using the rate constants in the table below. Please do not forget to save your file once you are finished. You can do so by selecting "File" -> "Export model".

| Parameter |  Value |
|:----------|:-------|
| V         | 10<sup>-16</sup> l |
| [[G](G.md)]       | 0.0166 x 10<sup>-6</sup> mol/l |
| [E<sub>T</sub>]       | 1 x 10<sup>-6</sup> mol/l |
| k<sub>0</sub>    |  135.5 (1/s) |
| k<sub>s</sub> |  4 (1/s) |
| k<sub>dM</sub> |  10 1/s |
| k<sub>1</sub> |  10<sup>8</sup> 1/(Ms) |
| k<sub>-1</sub> | 1 (1/s) |
| k<sub>2</sub> | 1 (1/s) |

> _Kinetic parameters used in the model description._

## Step 2: Generate a Single Realization ##

The second step in this tutorial is to obtain a single realization of the stochastic process. You can do that by selecting _Analyses_ -> _Stochastic Simulation Algorithm_ from the menu bar. The wizard popping up will allow you to select the number of simulation runs which you want to perform.

> ![http://i45.tinypic.com/15ebmo2.png](http://i45.tinypic.com/15ebmo2.png)

> _Configuration of the Stochastic Simulation wizard for an individual trajectory of the stochastic process._

Since we are interested in a single realization you should choose a value of _1_ in the field _Ensemble_, see Figure above, meaning that no ensemble average is performed. A final time of _300s_ together with _300_ plot points should be sufficient to visualize your results in this particular example.
The resulting stochastic trajectory for mRNA and protein concentrations should look similar to this:

> ![http://i48.tinypic.com/71l3td.png](http://i48.tinypic.com/71l3td.png)

> _Visualization of the stochastic trajectories of mRNA and protein concentrations. Note that the expression of protein occurs in random bursts._

Of course, the SSA generates different trajectories every time you run a simulation which means you will not be able to obtain the same result twice. However, the features of each trajectory should generally be typical for the underlying stochastic process, namely that the expression of the protein occurs in bursts. The height of these bursts is random as are the individual times of their occurrence. This phenomenon has also been observed experimentally and is called [transcriptional bursting](http://en.wikipedia.org/wiki/Transcriptional_bursting).

## Step 3: Time Course Analysis using the EMREs ##

In a next step we will explore the temporal dynamics by means of the EMREs. You can select the time course wizard from the menu bar via _Analyses_ -> _Time Course Analysis_. The wizard will allow you to select the type of analysis. We select _Higher Order Approximations_ to analyze the system using EMREs.

> ![http://i49.tinypic.com/14wdfmo.png](http://i49.tinypic.com/14wdfmo.png)

On the pages you may set the final time to _40s_, this will give a satisfying result. After the analysis terminates you can click the button _Quick plot_ at the top of the result table and select the species that you want to visualize. This will make the plots available in the tree on the left under the node _Analyses_. Click on the third plot, the result should look like this:

> ![http://i47.tinypic.com/a0xd1w.png](http://i47.tinypic.com/a0xd1w.png)

> _Visualization of the Time Course Analysis showing REs versus EMREs (higher order corrections using IOS are also shown). Note that while the REs predict the protein concentrations to be smaller than those of the mRNA the EMREs predict the opposite. This effect has been termed discreteness-induced concentration inversion._

The plot generated by iNA allows us to compare the predictions of the deterministic REs with those of EMREs. The outcome can be summarized as follows: the RE concentrations predict the average mRNA concentration to be greater than the protein concentration while the EMREs predict the opposite. Note that since the EMREs take into account stochastic effects their results are supposed to be closer to the prediction of the SSA and the CME. This noise-induced concentration inversion effect has been described by [Ramaswamy et al.](http://grimagroup.bio.ed.ac.uk/documents/ncomms1775.pdf).

You may also note that mRNA and protein concentrations reach steady state on vastly different timescales. Under such conditions the ratio of translation rate and mRNA lifetime has been related to the "burst size". ([Swain 2008](http://www.pnas.org/content/105/45/17256.short), [Paulsson 2005](http://166.111.93.130/~jzlei/teaching/sysbio2009/Paulsson_PhyLifeRev_2005.pdf))

> ## Step 4: Verify by Stochastic Simulation ##

You can verify this effect using the SSA in iNA. Choose a final time of _40s_ and repeat the simulation for an ensemble size of _3_, _30_ and _3000_ realizations. The result after _3000_ simulations should look like this:

> ![http://i46.tinypic.com/1dzv5s.png](http://i46.tinypic.com/1dzv5s.png)

> _Visualization of the results of the SSA. Note that the protein concentrations are larger than those of the mRNA species and hence confirm the discreteness-induced concentration inversion predicted by iNA using the EMREs in Figure above._

Export the data obtained from _3000_ simulations to a data file and import it into a spreadsheet application. Average the mean concentrations and variances from time _20s_ to _40s_ and calculate the coefficients of variation.

## Task I: Steady State Analysis ##
Please perform a steady state analysis using iNA. Extract the average concentrations according to the REs and EMREs for mRNA and protein species. Obtain also the variances according to the LNA and IOS analysis and calculate the coefficients of variation. What can you say about the accuracy of the estimates for variance and coefficients of variation for both approximations?

![http://i47.tinypic.com/23uv4w7.png](http://i47.tinypic.com/23uv4w7.png)

## Task II: Variation of the Translation Rate ##

Reduce the rate of translation rate to k<sub>s</sub>=_1s_ and repeat the simulation of a single realization and perform either steady state or time course analysis. Describe your observations. Is the expression of protein still bursty? Can you relate your observation to the burst size? Do you observe a concentration inversion?