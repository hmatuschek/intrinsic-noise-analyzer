# intrinsic Noise Analyzer #

iNA is an easy-to-use computational tool that puts the power of approximation methods at the fingertips of research groups involved in studying the role of noise in biochemical networks. The SBML-based software combines two complementary approaches to analyze the Chemical Master Equation:
  * the System Size Expansion - a systematic analytical approximation method,
  * the Stochastic Simulation Algorithm - a widely used Monte Carlo method.
iNA is based on the computer algebra system [GiNaC](http://www.ginac.de) and the compiler framework [LLVM](http://www.llvm.org).

This document gives a brief introduction to iNA, its user interface as well as model editing and fluctuation analysis.



# Introduction #

[Intrinsic noise](https://code.google.com/p/intrinsic-noise-analyzer/wiki/IntrinsicNoise) describes the concentration fluctuations in biochemical pathways due to the randomness of chemical reactions that are widely neglected by [deterministic approaches](http://en.wikipedia.org/wiki/Rate_equation). These fluctuations are most apparent when at least one of its molecular constituents is present in only very low copy numbers, a situation naturally encountered in living cells.

Most users might be puzzled by the large amount of acronyms used in the software. This section provides basic information on all acronyms and the employed methods in brief.


## Stochastic Simulation Algorithm (SSA) ##

iNA features two basic methods of exact stochastic simulation:

  * **Direct SSA** refers to [Gillespie's original formulation](http://en.wikipedia.org/wiki/Gillespie_algorithm) of the algorithm.
  * **Optimized SSA** is a variant of the direct method using a dependency graph for the propensity update, [Cao, Li, Petzold (2004)](http://dx.doi.org/10.1063/1.1778376). The dependencies are computed analytically and are safe to use for arbitrary reaction networks and complex-elementary reactions without explicit declaration of SBML modifiers.


## System Size Expansion (SSE) ##

The starting point of the [system size expansion](http://en.wikipedia.org/wiki/System_size_expansion) is given by the Chemical Master Equation which describes the probability distribution sampled by Stochastic Simulation Algorithm. The method as proposed by [van Kampen (1976)](http://dx.doi.org/10.1002/9780470142530.ch5) assumes an expansion of the Chemical Master Equation in terms of a large parameter Omega denoting the system size. For chemically open reaction networks the system size is given by the reaction volume while for closed reaction networks is often chosen to be the total molecule number. The expansion yields estimates for the first two moments of the probability distribution, i.e., means and variances, whose accuracy is improved by taking into account terms of increasing power of the inverse system size.

The system expansion performed by iNA yields means and variances of concentrations accurate up to -2. The acronyms describing estimates of increasing accuracy are described in the following:

  * **REs** [Rate Equations](RateEquations.md). REs refer to the deterministic formulation of chemical kinetics and arise as the average concentrations in the infinite system size limit from van Kampen's expansion. The order of the approximation is given by 0and hence the REs are independent of the system size.
  * **LNA** [Linear Noise Approximation](http://en.wikipedia.org/wiki/System_size_expansion#Linear_Noise_Approximation). The LNA has been derived by van Kampen is a popular method to estimate the variances about the concentrations obtained from the REs to leading order Omega<sup>-1</sup>; [Elf, Ehrenberg (2003)](http://link.aip.org/link/doi/10.1101/gr.1196503), [Hayot, Jayaprakash (2004)](http://dx.doi.org/10.1088/1478-3967/1/4/002), [Paulsson (2005)](http://dx.doi.org/10.1016/j.plrev.2005.03.003). iNA is the first standalone software which makes the technique available to a broad audience [Thomas, Matuschek, Grima (2012)](http://dx.doi.org/10.1371/journal.pone.0038518).
  * **EMREs** Effective Mesoscopic Rate Equations. These represent a set of effective differential equations that correct the REs by a term of order Omega<sup>-1</sup> [Grima (2010)](http://link.aip.org/link/doi/10.1063/1.3454685). Hence the EMREs more accurately describe the mean concentrations of the underlying stochastic process than the deterministic REs.
  * **IOS** Inverse Omega Squared method (featured since iNA 0.3). This method predicts the variances about the average concentrations given by EMREs. The accuracy of these estimates is given by Omega<sup>-2</sup> and hence is expected to be describe the fluctuations of the underlying stochastic process more accurately than the Linear Noise Approximation, [Grima, Thomas, Straube (2011)](http://link.aip.org/link/doi/10.1063/1.3625958). Moreover the method also allows to obtain a correction to the EMRE mean concentrations which is of order Omega<sup>-2</sup>.

The methods in order of increasing accuracy are summarized as follows.

| **Method**    | **Mean**    | **Std. Deviation** | **Variance** |
|:--------------|:------------|:-------------------|:-------------|
| REs	          | Omega<sup>0</sup>  | -                  | -            |
| REs & LNA     | Omega<sup>0</sup>  | Omega<sup>-1/2</sup>   | Omega<sup>-1</sup>  |
| EMREs & IOS   | Omega<sup>-1</sup> | Omega<sup>-1</sup> |  Omega<sup>-2</sup>  |
| IOS mean      | Omega<sup>-2</sup> | -                  | -            |


## Performance ##

At heart iNA is based on the powerful computer algebra system [GiNaC](http://www.ginac.de/). However, this mode of computation can be quite slow for numerical purposes due to arbitrary precision floating point arithmetic (as is also the case for the commercial software projects Mathematica and Maple). iNA provides two modes of execution delivering high performance:

  * a **byte code interpreter** can efficiently execute compact [numeric codes](http://en.wikipedia.org/wiki/Bytecode) of iNA's mathematical model representation (featured since iNA 0.2),
  * a state-of-the-art **JIT compiler** ([just-in-time](http://en.wikipedia.org/wiki/Just-in-time_compilation)) based on the [LLVM](http://llvm.org) framework resembling the performance of natively compiled code. It automatically compiles the mathematical model representation into executable code (featured since iNA 0.3).

You can find a detailed benchmark of all analysis methods in the section [Benchmarks](Benchmarks.md).



# Introduction into iNA’s GUI #

In this section we briefly outline the Graphical User Interface (GUI) of iNA and take you on a short tour through the exciting possibilities iNA makes available to you.

The GUI consists of two panels: The side panel, which lists all imported or created models, analyses and results (plots, data tables) in a tree structure and the main panel, which displays the elements selected in the side panel.


<p align='center'> <img src='http://i49.tinypic.com/33biuyp.png' width='320' /> </p>


## Import & Export Models ##

iNA imports models defined in [SBML](http://www.sbml.org) and [SBML-SH](SBMLsh.md). To import a model, select the `File` -> `Open Model...` menu entry or simply use the `Ctrl+O` (Windows, Linux) or `Cmd+O` (MacOS) keyboard shortcut. The file selection dialog will appear. Select a file from your hard drive and click on the `Open` button to import the  model file. iNA determines the file type by its extension where `*.xml` and `*.sbml` are recognized as SBML files and `*.mod` and `*.sbmlsh` are recognized as SBML-SH files. If the file format was not determined correctly, please rename your input file. If your input file has a different ending than listed here, iNA will ask you for the file type.

> Tip: If you start iNA from the command line, you may pass a model file name to the executable to import that model immediately: `$ ina MODELFILE`

> Tip: You can also drag SBML and SBML-SH files into the side panel to import a model. This also works with [links](https://intrinsic-noise-analyzer.googlecode.com/files/tut1_genemodel.sbmlsh) to models, iNA will download the file and import that model.

To close imported models, select the model and click on the `File` -> `Close`. Note that any unsaved changes made to that model are lost.


Once you made changes to the model (see [Model Editing](Help#Model_Editing.md) below), you may want to export the modified model into SBML or SBML-SH. To do so, select the modified model in the side panel and select the `File` -> `Export Model...` menu entry. The export dialog will appear and you can select a file to export the model into.

To export the model into SBML, choose a file name with either `*.xml` or `*.sbml` extension. To export the model in SBML-SH, choose a file name with either `*.mod` or `*.sbmlsh` extension. If the selected file already exists, you will be asked if you want to overwrite this file.


## Explore Models ##

Each model in the side panel has two sub elements. _Model_ gives an overview of the model and _Analyses_ collects running analyses and results (see [Perform Analyses](Help#Perform_Analyses.md)).

The first sub-element beneath _Model_ is _Compartment_. If you click on this element the list of compartments defined in the model is shown in the main panel.

<p align='center'><img src='http://i49.tinypic.com/10cnabt.png' width='320' /></p>

For each compartment, its identifier (_Id_), name, volume and unit is shown. The last column shows if the compartment is defined to be constant over time.

The second sub-element beneath the _Model_ element is _Species_, similar to _Compartments_ the list of species defined in the model are shown, if the _Species_ element is selected in the side panel.

<p align='center'><img src='http://i45.tinypic.com/el62xg.png' width='320' /></p>

Again, the identifier, name, initial value and unit are shown in the table for each species. The _Constant_ column shows if the concentration or amount of the species is defined to be constant over time. The last column (_Compartment_) specifies the compartment hosting it.

The _Reactions_ element beneath _Models_ shows an brief overview over all reactions defined in that model.

<p align='center'><img src='http://i46.tinypic.com/cqejl.png' width='320' /></p>

It displays name, reaction equation and propensity for each reaction defined in the model. The propensity may refer to local parameters defined within the propensity itself. These parameter definitions are not shown in this list but in the reaction view below.

Below the _Reactions_ element there is a sub-element for each reaction of the model. If you select a reaction from that list, a more detailed view of that reaction will be shown in the main panel.

<p align='center'><img src='http://i49.tinypic.com/bg4mkn.png' width='320' /></p>

The reaction view displays the reaction's chemical equation and the propensity (kinetic law) in the upper half. Below, you find the list of local parameters, i.e., parameters that are visible only within te context of this particular reaction.  This list shows the identifier, name, value and unit for each local parameter defined.

The last sub-element beneath _Model_ is shows the list of global parameters in the main panel.

<p align='center'><img src='http://i46.tinypic.com/35n4f8h.png' width='320' /></p>

Here, there are displayed the identifiers, names, initial values and units for each parameter. Again, the last column shows if the parameter is defined to be constant over time.


# Perform Analyses #
This section describes how to perform analyses provided by iNA and gives a brief introduction into the wizards that guides you through the setup of these analyses. Please have also a look into our tutorials ([1](Tutorial1.md) & [2](Tutorial2.md)) to get an step-by-step introduction into the analyses performed by iNA.

## Steady State Analysis (SSE) ##
This analysis allows you to determine the steady state of a model in terms of the [system size expansion (SSE)](http://en.wikipedia.org/wiki/System_size_expansion). This includes the steady state of the deterministic REs, the LNA and EMRE/IOS. The latter one allows for determination of the stochastic steady state with a precision in the order of magnitude of Omega<sup>-2</sup>, for the mean and variance which has been proven to be a reliable approximation of the CME ([Thomas (2012)](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6392668) & 2013 (in press, [preprint](http://grimagroup.bio.ed.ac.uk/documents/manuscript.pdf))). Please consider also the step-by-step introduction given in the tutorials [1](Tutorial1.md) & [2](Tutorial2.md).

<img src='http://i49.tinypic.com/dc3iuh.png' align='right' width='340' />
To configure a steady state analysis, select the _Steady State Analysis (SSE)_ item in the _Analysis_ menu. The configuration wizard will appear. On the first wizard page, you can select the model to be analyzed. Select the model and click on _Continue_.

The second wizard page configures the steady state solver. iNA uses a hybrid solver technique that incorporates the LSODA ODE integrator in cases, where the classic Newton solver fails to converge with the given precision (a) within the given number of steps (b). In these cases, the LSODA ODE solver is used to time-integrate the complete model up to the specified maximum integration time (c). The default values are reasonable for the most cases. If the solver fails to converge, try to increase the number of possible iterations (i.e. to 1000). To finish the configuration, click _Next_.

The final wizard page displays a short summary of the configured analysis showing the selected model and the approximative amount of memory to store the analysis results. Click on the Finish button to start the analysis. The analysis will appear in the side-panel under the Analyses node of the model item.

Once the analysis is finished, the results can be viewed by selecting the _Steady State Analysis (SSE)_ item beneath the _Analysis_ node of the model item in the side panel. The first table shows the mean concentrations of the steady state as determined by the deterministic REs, EMREs and IOS methods. The last two tables show the the covariance matrices as determined by the LNA or the IOS method. Save these results as a Comma Separated Values (CSV) text file, click on the _Save data to file_ button.

<img src='http://i49.tinypic.com/v8o5xw.png' align='right' width='340' />
To visualize these results you can click on the _Plot steady state statistics_ button. A small dialog will appear that allows you to select the species for the plot. The button above the list allows you to select either all or no species or to invert the selection of species.

The created plot can be viewed by selecting the “Steady state concentrations” item beneath the analysis node in the side panel (shown above). It shows a group of bars for each of the selected species, the first bar (blue) shows the RE steady state concentrations together with the LNA std. dev., the second bar (red) shows the EMRE concentrations together with the IOS std. dev. and the last bar (green) shows the IOS concentrations. The plot can be exported as a PNG or SVG image file by clicking on the “Save to file” button on the bottom of the plot window.

## Steady State Parameter Scan (SSE) ##
The steady state parameter scan allows for an investigation of the dependency of arbitrary measures of the steady state statistics on parameters like rate constants and volumes. Typically these are the mean concentrations and the fluctuations around them, the [coefficients of variation (CoV)](http://en.wikipedia.org/wiki/Coefficient_of_variation) and the [Fano factors](http://en.wikipedia.org/wiki/Fano_factor). iNA can perform this parameter scan in terms of the SSE for the REs, LNA and IOS steady states. The latter one has been introduced with iNA version 0.4 and was described in [Grima, Thomas, Straube (2011)](http://link.aip.org/link/doi/10.1063/1.3625958) and Thomas 2013 (in press, [preprint](http://grimagroup.bio.ed.ac.uk/documents/manuscript.pdf)).

To perform a parameter scan analysis, select the _Steady State Parameter Scan (SSE)_ item from the _Analysis_ menu of the main menu. A wizard will appear that will guide you through the configuration process. The first wizard page allows you to select the model to be analyzed and the order of the SSE to be used (see [Introduction](Help#Introduction.md) above for further information about the system size expansion methods). Please note that statistical quantities like CoV and Fano factors can only be determined by approximation methods of first order and higher (RE/LNA or EMRE/IOS) as the approximation method of 0th order (deterministic REs) do not yield an estimation for the covariance of the species concentrations.

The second wizard page allows you to select the execution engine for the analysis. Possible selections are _JIT compiler_ and _BCI Interpreter_. The former incorporates the just-in-time compilation scheme available in iNA since version 0.3. This scheme compiles the mathematical expressions derived internally by iNA for the selected approximation method into executable machine code. This increases the execution speed of the analysis significantly (see [Benchmarks](Benchmarks.md)). The latter execution engine is the byte-code interpreter present since the first release of iNA. We encourage you to use the _JIT compiler_ since it is the most performant execution engine in the majority of cases. If the flag _Optimize code_ is enabled, the generated byte- or machine code will be optimized during the compilation process. This allows to increase the execution speed further on the cost of an increased compilation time. This option does not provide a significant performance enhancement for a typical parameter scan task, therefore it can be left disabled.

<img src='http://i49.tinypic.com/19046a.png' align='right' width='340' />
The third wizard page allows to select the parameter and its value range for the analysis as well as the configuration of the steady state solver (see [Steady State Analysis (SSE)](Help#Steady_State_Analysis_(SSE).md) above for details about the configuration of the steady state solver). The parameter for the scan can be selected by the drop-down menu. Note that only parameters defined globally are selectable. If you like to perform a scan over a local parameter of a reaction, you could move that parameter into the scope of global parameters (see [Reaction Editing](Help#Reaction_Editing.md) below). The value range and number of steps for the parameter scan can be specified using the _Min. value_, _Max. value_ and _Steps_ fields. Once the scan configuration is complete, click the _Continue_ button.

The last wizard page shows a brief summary of the configured scan. Click the _Done_ button to start the analysis. The analysis task is shown as an item labeled _Parameter Scan (SSE)_ in the side panel below the _Analysis_ node of the model. If the analysis is selected in the side panel while the analysis is still performed, a progress bar is visible showing the current progress and the estimated time for the analysis. If the analysis is done, the results of the parameter scan are shown.

The results are collected within a table that shows the mean and covariance of the species concentrations in dependence of the scanned parameter. This table can be exported as a CSV text file by clicking on the _Save data to file_ button. The _Quick plot_ button allows for an easy plotting of selected species concentrations, CoV and Fano factors in dependence of the scanned parameter. Additionally, the _Customized plot_ button opens a dialog that allows to generate arbitrary plots from the analysis results (see [Customized Plots](Help#Customized_Plots.md) below for details).

## Time-averaged Parameter Scan (SSA) ##
This analysis was introduced with iNA 0.4.1 and allows for the first time a parameter scan using the SSA. This approach uses the fact that under assumption of [ergodicity](http://en.wikipedia.org/wiki/Ergodicity), the ensemble average is equal to the time average. This approach is rather semi-automatic as it is impossible for a software to determine efficiently if an ensemble is in a steady state, therefore this method needs an _transition time_ parameter that specifies the simulation time needed for the system to reach the steady state. An estimate for that transition time can be found using the [SSE Time-course Analysis](Help#Time_Course_Analysis_(SSE).md) described below.

To perform a parameter scan, select the _Time-Averaged Parameter Scan (SSA)_ item from the _Analysis_ menu of the main menu. A wizard will appear that guides you through the configuration process of the analysis.

On the first wizard page, the model is selected to be analyzed. Select the model from the drop-down menu and click on the _Continue_ button. On the second wizard page, the execution engine for the simulation is selected. The possible selections are the _JIT compiler_ and _Byte-code interpreter_. The former translates the mathematical expressions derived internally by iNA into executable machine code while the latter incorporates an efficient interpreter to evaluate these expressions. We encourage you to take the default (JIT compiler) as it is the most performant execution engine for the majority of cases. If the _Optimize code_ option is selected, the machine or byte-code will be optimized for execution speed on the cost of an increased compilation time. This option is only beneficial for large system.

On the third wizard page, the parameter and it range is selected and the time-averaging is configured. The parameter & range selection is identical to the one described in [Parameter Scan (SSE)](Help#Steady_State_Parameter_Scan_(SSE).md) above. The averaging is configured by specifying the transient time, the maximum simulation time and the interval at which samples are taken during the simulation for the time-averaging. The transient time specifies the simulation time needed by the system to reach the steady state. The maximum simulation time specifies the time (w/o transient time) at which time the analysis stops. This value can be left huge as the simulation can be stopped manually. The last option specifies the simulation time interval at which values are taken for the time-averaging. Additionally the number of parallel threads to use for the simulation is specified.

The last wizard page shows a brief summary of the configured analysis. Click _Done_ to start the analysis.

<img src='http://i48.tinypic.com/v4nhq0.png' align='right' width='340' />
The analysis task is then created and it is visible as the item labeled _Parameter Scan (SSA)_ beneath the _Analyses_ node in the side panel. During the simulation a preview of the parameter scan statistics is displayed. It shows either the mean concentrations and the fluctuations around them, the coefficients of variation or the Fano factors in dependence of the scanned parameter. The plot type shown, can be chosen by the drop-down menu _Select plot type_ and is updated continuously.  The statistics can be reset by clicking on the _Reset statistics_ button. This allows to reset the current statistics if the steady state was not reached during the transition time. On the right side of the preview, the species for the preview statistics can be selected. Once the time-averaging is sufficient, i.e. when the graphs are “smooth enough”, the simulation can be stopped by clicking on the _Done_ button.

If the simulation is finished, either by stopping it manually or by reaching the max. simulation time, the results of the parameter scan are shown as a table. This table can be exported as a CSV text file by clicking on the _Save data to file_ button. The _Quick plot_ button allows to create a plot for the mean & std. dev of the species concentrations, the CoV and the Fano factors in a convenient way. Additionally arbitrary plots can be created from the analysis data using the _Customized plot_ button as described below (see [Customized Plots](Help#Customized_Plots.md)).

## Time Course Analysis (SSE) ##
The time course analysis allows to investigate the dynamics of the mean concentrations of species and the fluctuations around them in time. iNA incorporates the SSE as a systematic approximation of the CME for the time course analysis including the deterministic REs, LNA and EMRE/IOS. The latter one has proven itself to be a reliable approximation technique over a wide variety of applications, while outperforming the exact SSA in terms of execution time by several orders of magnitude (see Benchmarks and Thomas 2013). Please also consider our tutorials (1 & 2) for a step-by-step introduction into the time course analysis of iNA.

To perform a time course analysis, select the “Time Course Analysis (SSE)” item from the “Analysis” menu of the main menu. A wizard will appear the will guide you through the configuration of the analysis.

On the first wizard page, the model as well as the approximation method is selected. On the second wizard page, the execution engine is selected. Possible options are “JIT compiler” and “Bytecode interpreter”. The former translates the mathematical expressions derived internally by iNA into executable machine code while the latter one incorporates an interpreter for the evaluation of these expressions. We recommend to utilize the JIT compiler as it provides the best performance. If the “Optimize code” option is selected, the byte or machine code is optimized on the cost of an increased compilation time. This option is particularly useful for long integrations. The byte code interpreter allows for a parallel evaluation of the derived expressions. This option may only be beneficial for large systems. This feature is not available for the JIT compiler.

On the third wizard page, the integration is configured by specifying the integration time, number of plot points, the integrator and the integration accuracy. The “Final time” specifies the integration/simulation time for the analysis. “Plot points” specifies the number of intervals for the time course plot, a higher number generates a smoother plot. The drop-down menu “Integrator” allows to specify the integrator to be used. Since version 0.3, iNA ships with the excellent general purpose integrator LSODA (Petzold 1983) that switches between explicit and implicit methods if needed. We strongly encourage you to use this integrator. The integration precision is finally specified in terms of the maximum absolute and relative error. The last wizard page summarizes the configured analysis. Click on “Done” to start the analysis.

The result of the time course analysis is presented as a table which can be exported to a CSV text file by the “Save data to file” button. The “Quick plot” button allows for the creation of plots of the mean and std. dev. of the selected species concentrations over time, alternatively arbitrary plots can be created using the “Customized plots” as described below (see “Customized Plots”).

## Stochastic Simulation (SSA) ##
The stochastic simulation algorithm (SSA) allows for an exact solution of the CME by the means of a monte carlo simulation. Although exact, this method is very slow compared to approximation methods as the SSE described above (see Benchmarks). However, we strongly encourage you to verify all (final) results obtained by the systematic approximation methods of the SSE using a stochastic simulation before publishing them. Please also consider our tutorials (1 & 2) for a step-by-step introduction into the stochastic simulation of iNA.

To perform a stochastic simulation, select the “Stochastic Simulation (SSA)” item in the “Analysis” menu. A wizard will appear that guides you through the configuration process.

On the first wizard page, the model for the simulation is selected. Select a model from the drop-down menu and click on “Continue”.

On the second wizard page, the execution engine for the simulation is selected. Possible options are “JIT compiler” and “Bytecode interpreter”. The former translates the mathematical expressions derived internally by iNA into executable machine code while the latter one incorporates an interpreter for the evaluation of these expressions. We recommend to utilize the JIT compiler as it provides the best performance. If the “Optimize code” option is selected, the byte or machine code is optimized for execution speed on the cost of an increased compilation time. The option usually does not have a significant effect on the simulation time since it is mainly governed by the random number generation.

The third wizard page configures the simulation, by specifying the ensemble size, final simulation time, plot points and the simulation algorithm. The ensemble size specifies basically how accurate the resulting statistics are, a larger ensemble increases the accuracy of the final statistics (leading to smoother graphs). The “Final time” specifies the final simulation time. “Plot points” specifies the number of intervals for the time course plots. The “SSA method” specifies the SSA algorithm to be used for the simulation. Possible options are “Optimized SSA” (Cao, Li, Petzold (2004)) or “Direct (SSA)” (Gillespie's original formulation). We encourage you to select “Optimized SSA” as it is as accurate as the direct method but significantly faster. Finally, the number of parallel threads can be specified to be used for the simulation.

The last wizard page shows a brief summary of the configuration made. Click “Done” to start the simulation.

During the simulation, the analysis task item in the side panel shows a progress bar and the estimated time the simulation will take to be done. Once the simulation is done, the results of the time course analysis are shown in a table, which can be exported as a CSV text file by the “Save data to file” button. The “Quick plot” button allows to easily create the time course plots for selected species. Alternatively, the “Customized plot” button allows to create arbitrary plots from the analysis data as described below.

## Customized Plots ##
<img src='http://i43.tinypic.com/2expm5c.png' align='right' width='340' />
Since iNA version 0.4 _customized plots_ allow for an quick & easy creation and editing of plots from within iNAs GUI. Since version 0.4.2, it is possible to edit all plots created by iNA, including the so called _quick plots_, allowing for the creation of publication quality plots even faster. This section gives a brief introduction into the plot editor.

When creating a new or editing an existing plot, the _Plot-o-mat_ dialog appears. This dialog shows on the left side a preview of the plot and on the right side the list of graphs, plot-range configuration or plot labels.


### Create/Edit Graphs ###
A new graph can be created by clicking on the “+” button below the graph list. A drop-down menu will appear that allows to choose either a _Line graph_ or a _Variance-line graph_. The former graph type displays a simple line while the latter one additionally shows a confidence region around the mean line. To edit an existing graph, simply double-click its label in the graph list.

<img src='http://i39.tinypic.com/15cm1d4.png' align='right' width='170' />
The configuration of a line graph is done through another dialog called _Graph-o-mat_, this dialog specifies the label of the graph, the line color and expressions for the X and Y coordinates of the graph. The expressions for the X & Y coordinates can incorporate arbitrary functions of the data columns, which are referred by `$COLUMN_INDEX` variables. For example the `$0` variable refers to the first column. The first column of the analyses data usually contains the independent variable, hence the time for time-course analyses and the parameter for parameter scans. The default expression for X is therefore usually `$0`. To assist you in assembling these expressions, the “+” button next to the expression field shows a list of all column names. Selecting a column will insert the associated column variable. The line color can be chosen by clicking on the color button next to the _Line color_ label. Click on the _Ok_ button to close the dialog and update the plot preview.

### Remove Graphs ###
To remove a graph from the list, select the graph in the list and click on the “-” button. This can not be reversed.

### Setting the Plot Range ###
<img src='http://i39.tinypic.com/2n7kt8o.png' width='340' />

The plot range can be specified by selecting the _Range_ tab on the right side of the _Plot-o-mat_ dialog. By default the plot ranges for X & Y are determined automatically, therefore the check boxes are all disabled. Enabling a check box allows to specify a minimum or maximum value for the X or Y plot range.

### Setting Labels ###
<img src='http://i42.tinypic.com/xe3ad4.png' width='340' />

The plot title as well as the axis labels can be set by clicking on the _Label_ tab on the right side of the _Plot-o-mat_ dialog. The preview of the plot is updated automatically after a few seconds.


# Model Editing #
Ina allows you to directly edit imported or newly created models. This section briefly describes how the model is manipulated.

<img src='http://i47.tinypic.com/ezl4oz.jpg' align='right' width='320' />
Click on the _Model_ element in the side panel. The main panel now shows an overview of the model, including its name and identifier as well as the global units. iNA does not allow to specify units for each species and compartment separately. Instead all compartments share the same unit (the volume unit) as well as all species share the same unit, defined here.

With the _Species in_ field, you select if the species are defined either in substance or concentration units. If _concentrations_ is selected, all species are measured in substance/volume units. If _amount_ is selected all species are measured in substance units only. If you change this field, the complete model will be updated to maintain consistency.

To change for example the substance unit, double click the unit. An editor will open and allows you to edit this unit directly. Note, that only _item_ and _mole_ are valid substance units. I.e., if you want the model to be defined in micromole, enter `1e-6 * mole` here.



## Edit Species, Compartments and Parameters ##

iNA allows you to add, remove and manipulate species, compartments and parameters of models. These model manipulations are very similar, therefore we describe them here exemplary for species.

To add a new species to a model, select the _Species_ item of the model in the side-panel to get the species list. Now, click on "`+`" button and a new species is added to the list. Note, that you can not create a new species unless there is at least one compartment defined in the model.

A species may be removed from a model by selecting the species identifier in the species list and click the “`-`” button on the top. If the species to be removed is referenced somewhere else in the model, for example in reactions, the species can not be removed and a message will appear.

A species, for example a newly created species, can be manipulated by double-clicking on the corresponding cell in the species list. This will open a specialized editor for that field. The _Name_ field is the simplest editor, you may enter any text string (see [Equation Rendering & TeX Names](Help#Equation_Rendering_&_TeX_Names.md)) here.

The _Initial Value_ field accepts any expression (see [Expression Syntax](Help#Expression_Syntax.md)). The syntax and the semantics of the expression is checked. If the syntax is valid and all referenced symbols can be resolved, the species is updated with the new initial value.

The unit fields of species and compartments can not be edited directly, as their units are defined implicitly by the model global units (see above). Only units of the parameters can be edited directly.

When you double-click on the compartment field of the species, a combo box is shown, allowing you to select the compartment the species is defined in.


## Edit Reactions ##

iNA provides an unified wizard assisting editing existing or creating new reactions. This wizard is also allows to create new species, compartments and parameters referenced in the reaction. This enables you to define complete models by simple definition of reactions.

To create a new reaction, click on the "+" button in the reactions overview (select "Reactions" in the side panel). You can edit an existing reaction by double clicking on the reaction in the reaction overview or by double clicking the reaction equation in the view of the reaction you want to edit.

<a href='Hidden comment: 
TODO Insert screenshot here.
'></a>

The first screen of the wizard defines the reaction. You can specify the the name of the reaction in the first text field from which a unique identifier will be generated. In the second text field, the chemical equation of the reaction is to be specified using the species' identifiers. This field provides auto-completion of species identifiers to ease the editing of the reaction equations. The following table shows some generic examples of the syntax:

| **Formula** | **Reaction** |
|:------------|:-------------|
| `2*A + B -> C` | <img src='http://latex.codecogs.com/png.latex?2\,A + B \rightarrow C%.png' /> |
| `2*A + B = C` | <img src='http://latex.codecogs.com/png.latex?2\,A + B \rightleftharpoons C%.png' /> |
| `-> A`      | <img src='http://latex.codecogs.com/png.latex?\emptyset \rightarrow A%.png' /> |
| `A ->`      | <img src='http://latex.codecogs.com/png.latex?A \rightarrow \emptyset %.png' /> |

If the syntax of the reaction equation is invalid, the background color of the text field turn red to indicate an error.

The _Type_ selection allows you to select the class of the reaction kinetics. The following options are available:

| **Type** | **Meaning** |
|:---------|:------------|
| _Mass action_ | The propensity of the reaction is completed using mass action kinetics. |
| _Multi-compartment_ | The propensity of the reaction is completed using macroscopic mass action fluxes as defined in concentrations. |
| _User defined_ | The propensity field becomes editable and allows you to specify the propensity directly. |

The _Propensity_ field shows either the automatically derived propensity (if _Type_ is not set to _User defined_) or allows you to enter the reaction kinetics directly as an equation.

<a href='Hidden comment: 
TODO Insert screenshot here.
'></a>

When you have completed the reaction definition, click _continue_. The following summary page renders the reaction equation and propensity and lists which newly created species along with the reaction. You can modify the reaction again by using the _Go back_ button.

If you click the _Done_ button, the reaction will be created (or modified if you edited an existing reaction) along with species and compartments if these were previously undefined.

### Reaction Local Parameters ###
[SBML](http://www.sbml.org), [SBML-sh](SBMLsh.md) as well as iNA allows to specify reaction-local parameters. These parameters are visible only within the scope of the reaction and they can only be referenced by the kinetic law of the reaction they are assigned to. In contrast to global parameters which might be referenced throughout the complete model.

Parameter scans (see [Parameter Scan (SSE)](Help#Steady_State_Parameter_Scan_(SSE).md) & [Parameter Scan (SSA)](Help#Time-averaged_Parameter_Scan_(SSA).md) above) can only be performed over global parameters. Therefore it is possible within iNAs GUI to move local parameters into the global scope. This is done by selecting the parameter in the list of local parameters of a reaction and click on the _Move to global parameters_ button above the parameter list. iNA now moves that parameter into the list of global parameters and also assigns a new identifier to that parameter if the identifier is already used by a global entity of the model. Once a parameter is declared globally, scans over that parameter can be performed.


## Combine and Expand Reactions ##

iNA allows you to expand a reversible reaction into two irreversible ones (if possible) and also to combine two irreversible reactions to a single reversible one.

To perform these transformations, select a model in the side panel and choose the `Edit -> Expand reversible reactions` menu item to expand reversible reactions or choose the `Edit -> Combine irreversible reactions` menu item to combine irreversible ones.

<a href='Hidden comment: 
Original,
<p align="center"><img src="http://i45.tinypic.com/2reh9g6.png" width="320"/>

Unknown end tag for &lt;/p&gt;



combined,
<p align="center"><img src="http://i48.tinypic.com/nzrm20.png" width="320"/>

Unknown end tag for &lt;/p&gt;



expanded again.
<p align="center"><img src="http://i46.tinypic.com/verfgj.png" width="320"/>

Unknown end tag for &lt;/p&gt;


'></a>

Note, that iNA searches in the list of reactions of the model for a set of irreversible reactions that can be combined to reversible ones. Therefore, iNA do not just recombine those irreversible reactions that has been expanded by iNA before and the other way around.


## Manipulate Models in SBML-SH ##

[SBML-SH](SBMLsh.md) was introduced to community by [Darren Wilkinson](http://www.staff.ncl.ac.uk/d.j.wilkinson/software/sbml-sh/) and allows to write a subset of SBML in a more human-readable form. It supports the most important features of SBML and allows to define units, species, compartment, parameters, reactions and events. Especially it allows to write expressions in a easy readable infix notation (the common arithmetic formula notation, see [Expression editing](Help#Expression_Editing.md) below). The major difference between SBML and SBML-SH are that the latter does not support to assign variable specific units, they all have the model default units. Furthermore, SBML-SH does not allow to attach any meta information (i.e., annotations and SBO terms).

<img src='http://i45.tinypic.com/1zpobgh.png' align='right' width='320' /> iNA allows you to import and export models in SBML-SH. But it also features the editing of a model in SBML-SH, directly within the GUI. The direct editing of models in the GUI is particularly useful if several modifications are made. Note that initial values can be changed directly in the parameter, compartment or species view.

To edit a model in SBML-SH, select it from the side-panel and choose the menu option `Edit -> Edit model`.

The editor window will appear presenting the model definition in SBML-SH. The editor features basic syntax highlighting to ease reading and editing of the model script.

Once you are done editing, click `open` on the bottom of the editor window. iNA will now check syntax and semantics of the script and highlight possible errors found.

<p align='center'><img src='http://i46.tinypic.com/2ars44.png' width='320' /></p>

If the syntax and semantics where correct, the model is re-imported into the GUI as a **new** model.

# Expression Editing #

iNA allows you to specify mathematical expressions in various places, from initial values of species, to the propensity (kinetic law) representations, to formulas in plots that allow to customize your plots of iNA's analyses. However, all these instances use the same expression parser and therefore have use same expression syntax (with minor modifications).

## Expression Syntax ##

The common arithmetic operations use the usual infix notation and precedence which means that `a+b*c` is evaluated as `a+(b*c)`. The power operation a<sup>b</sup> can be expressed either as `a^b` or  `a**b` and has higher precedence compared to the multiplication. But note that `a**b**c` is evaluated as `a**(b**c)` and not `(a**b)**c`.

Species, compartments or parameters must be referenced in expressions with their identifier and **not** with their name, as only identifiers are ensured to be unique. Note, that local parameters of the kinetic law may shadow global ones. We provide [autocompletion](Help#Autocompletion_for_expressions.md) for these identifiers to ease the editing of expressions.

The actual value/meaning of the referenced variable (species, parameter, etc.) depends on the type of the expression. For example, a species identifier in an initial value expression refers to the initial value of that species while a species identifier in a kinetic law expression refers to the current (during simulation) substance concentration or amount of that species. It is not possible to directly refer to an initial value of a species within a kinetic law expression, use parameters for that case.


## Autocompletion for expressions ##

To ease the editing of expressions, we provide some autocompletion of identifiers for compartments, parameters and species during expression editing.

<p align='center'><img src='http://i48.tinypic.com/2zhqsz4.png' width='640' /></p>

You can iterate through the possible completions using the up- and down-arrow keys. Press return or left-arrow key to accept the completion. Once the expression is ready it will be rendered into a more readable format for display.

## Equation Rendering & TeX Names ##

In order to increase readability of mathematical equations defined in the model, in particular propensities, iNA displays expressions  in a common mathematical representation rather than display them in the input form (see [Expression Editing](Help#Expression_Editing.md) for details).

Variables (species, compartments and parameters) are displayed with their name instead of their identifier if possible. To enhance your visual experience, iNA allows to use a subset of the TeX mathematical syntax in names including Greek letters and some symbols. In order to let iNA interpret names as TeX names, you must enclose them by the `$`-symbol, e.g., a compartment named "`$\Omega$`" will be rendered as http://latex.codecogs.com/png.latex?\Omega%.png" in all equations. It is also possible to use super- and subscripts in names such as "`$k_1$`" which is rendered as k<sub>1</sub> in all equations.

> _Note: We strongly discourage you to use superscripts in TeX names as they may be confused with powers in rendered expressions!_



### Uppercase & lowercase greek letters ###
| \Alpha   | http://latex.codecogs.com/png.latex?A%.png" |\alpha   |http://latex.codecogs.com/png.latex?\alpha%.png" |  \Beta    | http://latex.codecogs.com/png.latex?B%.png" | \beta    | http://latex.codecogs.com/png.latex?\beta%.png" |
|:---------|:--------------------------------------------|:--------|:------------------------------------------------|:----------|:--------------------------------------------|:---------|:------------------------------------------------|
| \Gamma   | http://latex.codecogs.com/png.latex?\Gamma%.png" |\gamma   | http://latex.codecogs.com/png.latex?\gamma%.png" | \Delta    | http://latex.codecogs.com/png.latex?\Delta%.png" | \delta   | http://latex.codecogs.com/png.latex?\delta%.png" |
| \Epsilon | http://latex.codecogs.com/png.latex?E%.png" |\epsilon | http://latex.codecogs.com/png.latex?\epsilon%.png" | \Zeta     | http://latex.codecogs.com/png.latex?Z%.png" | \zeta    | http://latex.codecogs.com/png.latex?\zeta%.png" |
| \Eta     |  http://latex.codecogs.com/png.latex?H%.png" |\eta     | http://latex.codecogs.com/png.latex?\eta%.png"  | \Theta    | http://latex.codecogs.com/png.latex?\Theta%.png" |\theta    | http://latex.codecogs.com/png.latex?\theta%.png" |
| \Iota    | http://latex.codecogs.com/png.latex?I%.png" |\iota    | http://latex.codecogs.com/png.latex?\iota%.png" | \Kappa    | http://latex.codecogs.com/png.latex?K%.png" |\kappa    | http://latex.codecogs.com/png.latex?\kappa%.png" |
| \Lambda  | http://latex.codecogs.com/png.latex?\nu%.png" |\lambda  | http://latex.codecogs.com/png.latex?\lambda%.png" | \Mu       | http://latex.codecogs.com/png.latex?M%.png" |\mu       | http://latex.codecogs.com/png.latex?\mu%.png"   |
| \Nu      | http://latex.codecogs.com/png.latex?N%.png" |\nu      | http://latex.codecogs.com/png.latex?\nu%.png"   | \Xi       | http://latex.codecogs.com/png.latex?\Xi%.png" | \xi      | http://latex.codecogs.com/png.latex?\xi%.png"   |
| \Pi      | http://latex.codecogs.com/png.latex?\Pi%.png" | \pi      | http://latex.codecogs.com/png.latex?\pi%.png"   | \Rho      | http://latex.codecogs.com/png.latex?P%.png" | \rho     | http://latex.codecogs.com/png.latex?\rho%.png"  |
| \Sigma   | http://latex.codecogs.com/png.latex?\Sigma%.png"|\sigma   | http://latex.codecogs.com/png.latex?\sigma%.png" | \Tau      | http://latex.codecogs.com/png.latex?T%.png" |\tau      | http://latex.codecogs.com/png.latex?\tau%.png"  |
| \Upsilon | http://latex.codecogs.com/png.latex?Y%.png" |\upsilon | http://latex.codecogs.com/png.latex?\upsilon%.png" | \Phi      | http://latex.codecogs.com/png.latex?\Phi%.png" | \phi     | http://latex.codecogs.com/png.latex?\phi%.png"  |
| \Chi     | http://latex.codecogs.com/png.latex?X%.png" | \chi    | http://latex.codecogs.com/png.latex?\chi%.png"  | \Psi      | http://latex.codecogs.com/png.latex?\Psi%.png" | \psi     | http://latex.codecogs.com/png.latex?\psi%.png"  |
| \Omega   | http://latex.codecogs.com/png.latex?\Omega%.png" | \omega  | http://latex.codecogs.com/png.latex?\omega%.png"|

### Arrows ###
|\leftarrow| http://latex.codecogs.com/png.latex?\leftarrow%.png"| \rightarrow | http://latex.codecogs.com/png.latex?\rightarrow%.png"| \leftrightarrow | http://latex.codecogs.com/png.latex?\leftrightarrow%.png"|
|:---------|:----------------------------------------------------|:------------|:-----------------------------------------------------|:----------------|:---------------------------------------------------------|
|\leftharnpoonup  | http://latex.codecogs.com/png.latex?\leftharpoonup%.png"| \rightharnpoondown  | http://latex.codecogs.com/png.latex?\rightharpoondown%.png"| \rightleftharpoons| http://latex.codecogs.com/png.latex?\rightleftharpoons%.png"|
|\Leftarrow | http://latex.codecogs.com/png.latex?\Leftarrow%.png"|\Rightarrow  | http://latex.codecogs.com/png.latex?\Rightarrow%.png"| \Leftrightarrow | http://latex.codecogs.com/png.latex?\Leftrightarrow%.png"|

### Operators ###
|\cdot   | http://latex.codecogs.com/png.latex?\cdot%.png"    | \times  | http://latex.codecogs.com/png.latex?\times%.png"   | \ast | http://latex.codecogs.com/png.latex?\ast%.png"  | \partial| http://latex.codecogs.com/png.latex?\partial%.png" |
|:-------|:---------------------------------------------------|:--------|:---------------------------------------------------|:-----|:------------------------------------------------|:--------|:---------------------------------------------------|



## Term Ordering in expressions ##

iNA uses the computer algebra system GiNaC for the internal representation of expressions to perform symbolic analysis. This may lead to a non-unique behavior of iNA when displaying equivalent mathematical expressions. For example, for the same model imported twice into iNA, the terms of the propensity of the reactions may be displayed in although equivalent but different order.

<p align='center'>
<blockquote><img src='http://i48.tinypic.com/110m255.png' width='320' />
<img src='http://i45.tinypic.com/2ho9vte.png' width='320' />
</p></blockquote>

Please note that in the example above, the propensities are equivalent but in different order. This behavior is due to GiNaC’s internal lexicographical ordering of mathematical terms. To do this GiNaC uses memory addresses of some terms. In effect two identical but independent copies of the same model may differ in the order in which terms are presented because they refer to different memory addresses. However, it is always guaranteed that these terms are equivalent.



# Feedback #
Help us to improve iNA further! We are constantly working on iNA to increase the reliability, speed, user experience and to add new features. However, we can not fix bugs we do not know and we can not implement features we have not thought of. So please, [tell us](https://code.google.com/p/intrinsic-noise-analyzer/issues/list) about any problem you encountered or about any feature that is missing in iNA. Your feedback is needed to turn iNA into a solution for your research.

# Miscellaneous #
iNA logs status and error messages from its core algorithms. These message are very helpful in understanding the cause of failures of analyses or model manipulations. Please consider to include this log into your [bug report](https://code.google.com/p/intrinsic-noise-analyzer/issues/list) as it helps us to understand the problem.  You can find the list of all log messages of the current iNA session under `Help -> Show log`.

If you are using one of the pre-build executables for Windows or MacOS X [provided](https://code.google.com/p/intrinsic-noise-analyzer/downloads/list) by us, iNA will check periodically (once per week) for a new release. This is done by downloading a [text file](https://intrinsic-noise-analyzer.googlecode.com/files/currentVersion.txt) that contains the version number of the current release. If you are using a personal firewall on your system it may prevent iNA from accessing this file. If you want to get notifications about new releases, please configure your firewall to grant iNA access to the internet. The pre-build packages for linux do not check for an update as new releases are delivered via the package management system of the linux distribution, read [install](Install.md) for more details.