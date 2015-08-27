## July 2013 - iNA version 0.4.2 ##

  * **Parameter scans** (since 0.4) - Obtain statistical measures such as coefficient of variations, Fano factors as well as means and variances of concentrations from the linear noise approximation, higher-order corrections or time-averaged stochastic simulations.
  * **Conserved quantities** - Analyze the conservation laws of your model in iNA.
  * **Plot editor** - A new plot editor for high quality visualization allows to customize and edit all existing plots in iNA. You can add or delete any graph from simulation data without the hazzle of replotting.
  * **OpenMP support for MacOS X** - Make full use your resources and perform analysis and simulations on multiple cores automatically and in parallel.
  * Many minor bug fixes. See full [changelog](Changelog.md) for details.

## March 2013 - iNA version 0.4.1 ##
This release enables parameter scans from the SSA (Stochastic Simulation Algorithm) and provides minor bug fixes for the GUI.

  * **Parameter scans (SSA)**  - Obtain statistical such as coefficient of variations, Fano factors as well as means and variances of concentrations from time-averaged stochastic simulations in parallel. iNA's GUI delivers online statistics to monitor convergence and remove transients while you wait.
  * **Faster parameter scans** - Use the system size expansion to perform parameter scans even quicker than before! See our [preprint](http://grimagroup.bio.ed.ac.uk/documents/manuscript.pdf) for latest benchmarks of iNA's automatic bytecode interpreter and just-in-compiler.
  * **Customized Plots** - Now also available on Windows platform. Produce high quality visualization quickly from spreadsheet analysis data.
  * Minor bug fix release with **significantly reduced memory consumption**. See full [changelog](Changelog.md) for details.

An article describing how iNA 0.4.1 can be used to investigate the parametric dependence of noise in gene regulation has just been accepted for publication in **BMC Genomics**:

  * Thomas P, Matuschek H, Grima R (2013) <b>How reliable is the linear noise approximation of gene regulatory networks?</b> BMC Genomics (in press, <a href='http://grimagroup.bio.ed.ac.uk/documents/manuscript.pdf'>preprint</a>).

## November 2012 - iNA version 0.4 ##
This release of iNA focuses on the graphical user interface (GUI) and enhanced usability. The current release features:

  * **Parameter scans** - This new type of analysis allows you to access parametric dependence of steady fluctuations of biochemical networks. Providing invaluable information that is difficult to obtain from stochastic simulations.
  * **Model editing capabilities** - The current release introduces advanced stochastic model manipulation. It allows you to edit existing SBML models, convert them into a format suitable for stochastic analysis, or simply create a model from scratch.
  * **SBML-sh support** - iNA now supports the easy to read and write file format SBML shorthand allowing the rapid generation of the essential SBML model structure.
  * **Improved reaction & expression rendering** - This release introduces improved rendering of mathematical expressions and reaction equations improving the overall readability of the mathematical representation.
  * **Customized plots** - Beside predefined plots this release introduces customized plots allowing you to produce high quality visualization from spreadsheet analysis data quickly (Linux and MacOS builds only).
  * **Export vector graphics** - Export lossless vector graphics using the popular SVG file format.

## July 2012 - iNA version 0.3.1 ##
This version of iNA takes the system size expansion beyond the Linear Noise Approximation. The new release features:

  * The **Inverse Omega Square Analysis** (IOS) which yields more accurate noise estimates than the Linear Noise Approximation. At the same time it predicts the average concentrations more accurate than the EMREs.
  * **Just-in-time Compilation** based on the [LLVM](http://llvm.org) framework delivering high performance to iNA's analyses, see our latest [Benchmarks](Benchmarks.md).
  * Support for the **efficient ODE integrator** LSODA for our time course analysis.
  * A very robust steady state solver which uses a combination of the fast Newton-Raphson method and the reliable ODE integrator LSODA.
  * Support for even more SBML features such as constant species and automatic conversion of [reversible reactions](http://code.google.com/p/intrinsic-noise-analyzer/wiki/ReversibleReactions) into irreversible ones.

You can find a preprint of our conference proceeding including mathematical derivations and illustrating examples of the IOS analysis on [arXiv](http://arxiv.org/abs/1207.1631).

## April 2012 - iNA version 0.2 ##
This is the first public release of iNA together with an article introducing iNA to the scientific community published in [PLoS ONE](http://dx.doi.org/10.1371/journal.pone.0038518) (open access):

Thomas P, Matuschek H, Grima R (2012) intrinsic Noise Analyzer: **A Software Package for the Exploration of Stochastic Biochemical Kinetics Using the System Size Expansion**. PLoS ONE 7(6): e38518. [doi](http://dx.doi.org/10.1371/journal.pone.0038518):10.1371/journal.pone.0038518

  * **Linear Noise Approximation**
  * **Effective Mesoscopic Rate Equations** (EMREs)
  * Steady State Analysis, Time Course Analysis and Stochastic Simulation
  * **SBML support**