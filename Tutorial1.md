We here give a six-step tutorial of how to use the Linear Noise Approximation in iNA and explore the noise properties arising from a simple gene expression. The tutorial contains also some "Tasks" that may be skipped.



This tutorial precedes our advanced tutorial that can be found [here](Tutorial2.md).

# Tutorial I: Gene Expression #
Over the past decades stochasticity in models of gene expression has received a lot of attention because genes are represented by only one or two copies in single cells. In this tutorial we consider a model describing transcription and translation which has become a standard model for single cell gene expression.

> <img src='http://latex.codecogs.com/png.latex?\text{Gene}\xrightarrow{k_0} \text{Gene} + \text{mRNA},%.png' />

> <img src='http://latex.codecogs.com/png.latex?\text{mRNA} \xrightarrow{k_\text{dM}} \varnothing,%.png' />

> <img src='http://latex.codecogs.com/png.latex?\text{mRNA} \xrightarrow{k_s} \text{mRNA} + \text{Protein}, %.png' />

> <img src='http://latex.codecogs.com/png.latex? \text{Protein} \xrightarrow{k_\text{dP}} \varnothing.%.png' />

Here the dynamics involve the mRNA and protein species which are produced by effective reactions modeling transcription and translation events, respectively. The relevant parameters for this reaction scheme are

| Parameter |  Value |
|:----------|:-------|
| Volume          | 10<sup>-16</sup> (l) |
| k<sub>0</sub>     |  135.5 (1/min) |
| k<sub>s</sub> |  4 (1/min) |
| k<sub>dM</sub> |  10 (1/min) |
| k<sub>dP</sub> |  5 (1/min) |

## Step 1: Construct a model with SBML shorthand ##

Unfortunately the SBML file format is very difficult to be read and written by humans. For this purpose Darren Wilkinson (Newcastle University) has developed a _shorthand_ version of SBML that contains the basic SBML model structure. A script describing the gene expression mechanism reads:

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

The first line of the above script
```
 @model:2.4.1=GeneExpression "Gene Expression"
```

declares a model with name "Gene Expression". Note that the first two numbers after the colon denote the SBML specification Level 2 Version 4 while the last number denotes the shorthand SMBL specification.

The definition of units used for the model definition reads

```

 @units
  substance = mole: s=-6
  volume = litre
  time = second: m=60

```

meaning that amount of substance is specified in "micromole" (a particle number approximately given by [Avogadro's number](http://en.wikipedia.org/wiki/Avogadro_constant)), time units in minutes (60 seconds) and volume in litres. Hence it follows that concentrations have units of "micromolar concentration" (M=10<sup>-6</sup> mole per litre).

Next we define a compartment of volume 0.1fl with identifier "cell"

```
 @compartments
 cell=1e-16
```

followed by a section defining the relevant species and their initial concentrations

```
 @species
 ...
 cell:[Gene]=0.0166
 ...
```

Here we declared the species "Gene" in compartment "cell" defined in units of concentrations which is indicated by the square brackets. The initial concentration of 0.0166 10<sup>-6</sup> M corresponds to a single gene copy number in a volume  0.1fl. You may verify this yourself using the definition of [micromolar](http://en.wikipedia.org/wiki/Molar_concentration) (10<sup>-6</sup> M) and femtolitre (fl=10<sup>-15</sup>l). Finally, a reaction describing the transcription of mRNA can be specified as

```
 @reactions
 @r=transcription "Transcription"
  Gene -> Gene + mRNA
  cell*k0*Gene
 ...
```

where the second line denotes the reaction identifier and name, the third line denotes the reaction scheme while the last one denotes the propensity of the reaction. The propensity denotes the probability per unit time for a reaction to occur. Its unit in this case is _mole/s_ which is a frequency. The latter is generally a function of the compartmental volumes, the concentrations of the species and globally or locally defined parameters. Note also that the involved species "Gene" and "mRNA" are defined in terms of concentrations.

### Task I: Generate the SMBL-sh file ###
Please copy the above model definition to a text file with the default file extension is ".sbmlsh". If you are interested in more information about SBML shorthand you can also a look at our [SBMLsh](SBMLsh.md) page.


## Step 2: Load the Model ##
Once you start iNA, you will see the main window with the menu bar at the top of it. A click on the entry ``File" will allow you to select the SBML-sh file from a local directory. For the purpose of this tutorial we will load the file generated in Task I into iNA. Alternatively you can drag [this link](http://intrinsic-noise-analyzer.googlecode.com/files/tut1_genemodel.sbmlsh) into the side panel of iNA (since version 0.4.1), iNA will download and import that model automatically.

> ![http://i46.tinypic.com/2i9l2d1.png](http://i46.tinypic.com/2i9l2d1.png)
> ![http://i45.tinypic.com/3445m4i.png](http://i45.tinypic.com/3445m4i.png)

> _The menu point_File_->_Open_enables you to load an SBML file. Simply go to the location of the file on your hard drive and select the desired file._

Once opened, the model will show up in the tree on the left. Of course, you will also be able to load multiple files into iNA's GUI.

## Step 3: View the Model ##
The tree on the left also allows you to inspect individual elements of the model definition. There are different tree nodes by which you can inspect the compartments, species, reactions and global quantities defined by the model. By selecting a node the corresponding information will be shown in the main window on the right side.

The node _Compartments_ lists all compartments together with their associated volume that have been defined in the SBML file.

> ![http://i47.tinypic.com/1exvll.png](http://i47.tinypic.com/1exvll.png)

> _This view lists all compartments that have been defined in the model together with their associated volumes and the units._

The next node _Species_ gives you a list of all defined species. The containing table also contains information about units in which each species have been defined, their compartments, and the amount in which the individual species are present at the beginning of a simulation.

> ![http://i45.tinypic.com/2urkvlu.png](http://i45.tinypic.com/2urkvlu.png)

> _This view list all species that comprise the reaction network together with their associated compartment, their initial value at the beginning of the simulation and the units in which it has been defined. Note that species can be defined in units of amount or concentrations._

Next the node _Reactions_ gives information about the reactions comprising the network. The view is divided in two panels.

> ![http://i45.tinypic.com/2v8mhdk.png](http://i45.tinypic.com/2v8mhdk.png)

> _This view gives you a list of the chemical equations and the propensities of all individual reactions. Note that symbols of species are represented in the unit in which the species has been defined. In the example shown the unit of all species is concentration._

The upper panel displays the reaction scheme containing information about the stoichiometry, the participating species and the associated propensities.
Note that locally defined parameters are listed in the table below while global parameters can be found by selecting the node _Global Parameters_.

> ![http://i45.tinypic.com/t7n2op.png](http://i45.tinypic.com/t7n2op.png)

> _This view collects all globally defined parameters together with their values._

## Step 4: Perform a Steady State Analysis ##
Under the menu point "Analyses" you can access specific wizards which guide you through the configuration of the analyses. The "Steady State Analysis" is the most basic analysis offered by the software iNA and allows you to perform a full noise analysis under steady state conditions using the system size expansion, i.e., after a time span when all transients in the average concentrations have decayed. This type of analysis is the quickest way by which you can perform a full noise analysis with iNA.

> ![http://i45.tinypic.com/9rqckl.png](http://i45.tinypic.com/9rqckl.png)

> _Accessing wizards from the menu bar._

The "Steady State Analysis" wizard is particularly simple to configure. First, we will need to select the model "Gene Expression" from the list which contains all opened models.

> ![http://i46.tinypic.com/9j0hl0.png](http://i46.tinypic.com/9j0hl0.png)

> _The first stage of an analysis wizard allows you to select a model from the list of open files._

The system size expansion relies on the knowledge of the concentrations of the [rate equations](RateEquations.md). The latter equations correspond to a macroscopic formulation of biochemical kinetics which is only valid in volumes of test-tube size or larger. Using the law of mass action, the deterministic rate equations for our model of gene expression can be formulated as

> <img src='http://latex.codecogs.com/png.latex?\frac{d}{dt} [M] = k_0 [G] - k_\text{dM} [M],%.png' />

> <img src='http://latex.codecogs.com/png.latex?\frac{d}{dt} [P] = k_s [M] - k_\text{dP} [P].%.png' />

where the deterministic concentrations are denoted by the square brackets. However, this formulation ignores the discreteness of molecules and hence also the fluctuations which are intrinsic to the kinetics. Under steady state conditions the solution of these equations is particularly simple since the present example considers only linear reactions. General reaction networks are represented by a set of coupled and nonlinear equations. Therefore iNA computes the roots of the REs using a robust steady state solver.

> ![http://i46.tinypic.com/346aid2.png](http://i46.tinypic.com/346aid2.png)

> _The second stage of the Steady State Analysis wizard allows one to customize the numerical procedure._

The method is an iterative procedure and therefore you will be asked to specify the precision and maximum number of iterations to be used by the solver.
Note that generally the number of iterations needed is proportional to the number of species in the network. Therefore you might need to increase this value when considering very large networks but the preset value is sufficient for the present example.

> ![http://i49.tinypic.com/nzgmzq.png](http://i49.tinypic.com/nzgmzq.png)

> _The final stage of a wizard gives a summary of the configuration._

Finally the wizard gives you a summary of your configuration and how much memory the results of the analysis will approximately use. The numeric values of the analysis are presented in two tables. The upper table lists the values of the concentrations that have been obtained as solutions of the deterministic rate equations for the selected. The second value has been computed by the EMREs and is supposed to give you a more accurate value for the true concentration as described by the CME. In the present case both results agree. This is been expected since the set of reactions describing the expression of the protein are unimolecular in nature. The more general case of gene expression involving also bimolecular reactions will be dealt with in the next tutorial. The lower table lists the values of the [covariance matrix](http://en.wikipedia.org/wiki/Covariance_matrix) as computed by the LNA.

> ![http://i49.tinypic.com/eb7vyc.png](http://i49.tinypic.com/eb7vyc.png)

> _The results of the Steady state analysis are summarized in a table view and is exportable to a data file._

You might want now want to visualize the numerical results. This can be done through the button "Plot steady state statistics" at the bottom of the window.

Second, we need to select the species we want to analyze. In this example there only two species of immediate interest, namely mRNA and protein, and we will select both of them.

> ![http://i47.tinypic.com/egr43p.png](http://i47.tinypic.com/egr43p.png)

> _Select a subset of species that you want to analyze._

You can find the result in the tree on the left under the node _Analyses_. The graph shows a column plot in the main window. The height of each column indicates the value of the steady state concentration for each species. Here the blue column denotes the results given by rate equations, the red one those by the EMREs, both which are agreement as mentioned above. The plot also contains error bars which denote the [standard deviation](http://en.wikipedia.org/wiki/Standard_deviation) of the concentration fluctuations which has been computed by the LNA.

> ![http://i49.tinypic.com/33o412a.png](http://i49.tinypic.com/33o412a.png)

> _Visualization of the Steady State Analysis result in terms of a column plot. The blue column denotes the prediction of the average concentration by the REs together with the standard deviation denoted by an error bar which has been calculated by means of the LNA. The red column denotes the average concentrations as predicted by the EMREs._

## Step 5: Perform a Time Course Analysis ##

The analysis performed in steady state conditions can also be performed as a function of time. This will provide you with information on how the fluctuations change over time. The time span we will consider here is the beginning of a simulation at which point the distribution of concentrations is sharply peaked about the deterministic initial conditions to the final time at which the steady state is reached. The wizard for this analysis can be accessed from the menu bar from "Analyses" -> "Time Course Analysis".

> ![http://i45.tinypic.com/116mic2.png](http://i45.tinypic.com/116mic2.png)

> _The Time Course Analysis wizard allows you to select the mode of analysis._

> ![http://i45.tinypic.com/2eyb7l0.png](http://i45.tinypic.com/2eyb7l0.png)

> _The third stage of the Time Course Analysis wizard guides you through the configuration of the ODE solver._

In order to obtain the time course, iNA provides a choice of different ODE solvers. For general purposes we recommend LSODA which automatically switches between stiff and non-stiff methods (stiffness arises from timescale separation in biochemical networks). The analysis is as simple to use as any conventional solver for deterministic REs as you find for instance in software like Copasi or CellDesigner. You only need to specify the final time of integration as well as the maximum relative and absolute errors. Therefore it is clear that all results obtained by the SSE methods should be checked whether they are consistent with integrations using smaller error estimates.

The result of the analysis is presented in a table view which you can access by the node of the analysis in the tree on the left. At the bottom of the table in the main window you will find a button to plot your result. The first plot should look like this:

> ![http://i48.tinypic.com/2djqoef.png](http://i48.tinypic.com/2djqoef.png)

> _Result of the time course analysis showing REs and LNA. The solid lines denote the concentrations as predicted by the REs while the colored areas denote the standard deviation calculated by the LNA._

This plot visualizes the time course according to the REs together with the prediction for the standard deviations as calculated by the LNA which is indicated by the colored areas.

### Task II: Coefficient of Variation ###
Please convince yourself that the results from the steady-state analysis are consistent with the value of the time course analysis at time _2s_.  You can do so by selecting the analysis in the tree on the left and using the numerical values in the table view of the main window. Check this for the mean concentrations as well as for the individual variances of mRNA and Protein. You may also calculate the [coefficient of variation](http://en.wikipedia.org/wiki/Coefficient_of_variation), defined as the ratio of standard deviation to mean concentration, for both species.

## Step 6: Verify by Stochastic Simulation ##

The results in the preceding steps have been obtained by means of the LNA which is based on the system size expansion. You can verify your results using the [Stochastic Simulation Algorithm](http://en.wikipedia.org/wiki/Gillespie_algorithm). The wizard can be accessed from "Analyses" in the menu bar, and besides species selection it also offers several more configuration options.

> ![http://i47.tinypic.com/qovfr6.png](http://i47.tinypic.com/qovfr6.png)

> _The third stage of the Stochastic Simulation wizard allows you to adjust the number of realizations used for the statistical average._

The SSA is a Monte Carlo method which generates independent realizations of the stochastic process which are distributed according to the exact probability distribution solution of the Chemical Master Equation. iNA offers you two options, the direct method and an optimized method. The former is the original formulation by Gillespie while the latter is an improved variant which has been proposed by [Cao](http://jcp.aip.org/resource/1/jcpsa6/v121/i9/p4059_s1) which is generally preferable. The merit of these methods is that they are exact. However in order to extract the necessary statistics, ensemble averaging over a large number of samples might be required. The number of realizations used can be adjusted by the option "Ensemble size". The option ""Plot points" simply refers to the desired number of points in the output plot. Using the option "Thread count" you make efficient use of multi-core architectures depending on the hardware you have available. The standard setting should make all cores of your system available. Setting the ensemble size
to _3000_ realizations and final time to _2s_ generates the following plot view:

> ![http://i50.tinypic.com/e5k1v6.png](http://i50.tinypic.com/e5k1v6.png)

> _Result of the SSA with statistical averaging using 3000 independent realizations._

The result is in excellent agreement with the predictions of Linear Noise Approximation which is expected since the reaction probabilities are at most linear in the concentrations according to the [law of mass action](http://en.wikipedia.org/wiki/Law_of_mass_action).

### Task III: Spreadsheet Analysis ###

Export the data obtained from stochastic simulation to a text file and analyze it within a spreadsheet application like Excel, LibreOffice or Octave. You can do so by selecting the analysis in the tree on the left and using the button _Save to data file_ at the bottom of the table view in the main window. Average the mean concentrations and variances over the time points from _1s_ to _2s_. _Note that, in principle, one could simply increase the number of realizations used for the ensemble average. This, however, is computationally very expensive mainly because of the use of the SSA. Additional time averaging is common to speed up convergence which is permitted in steady state conditions._ Also calculate the coefficients of variations. Compare your results with those obtained from the steady state analysis in Task I. What do you find?