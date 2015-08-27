## iNA 0.4.2 ##

  * OpenMP support for MacOS X
  * New feature: View conserved quantities
  * New feature: Wizard for editing any plot

  * Change default behavior: import of substance units when all species have substance units
  * Allow TeX in species selection list
  * Switch automatically to new analyses results
  * Select current model in analysis wizard

  * Fix: Fano factor plot now available in parameter scans involving the volume parameter
  * Fix: Display of large integers
  * Fix: Task duration now measured in real time
  * Fix: Auto-completion caused GUI crash
  * Fix: Freeze in SSE wizard for large models
  * Fix: Handle MaxIterationsReached exception in SSE parameter scan
  * Fix: Exceptions for models defining zero volumes
  * Fix: Resize all table colums to content size
  * Fix: Parameters defined as Sqrt() cannot be correctly substituted
  * Fix: Missing error handling in redefinition of variables
  * Fix: Missing filename when downloading a model
  * Fix: Legend in plots
  * Fix: Axis origin in plots
  * Fix: Accept reaction names with white-spaces in reaction editor

## iNA 0.4.1 ##

  * SSA parameter scan
  * More efficient SSE parameter scan using bytecode interpreter or LLVM
  * Drag & drop models into side panel for import
  * Enabling Customized Plots for Windows using bytecode interpreter
  * Allow log messages to be saved to file or copied to clip board
  * Level filter for log messages
  * Lower threshold for autocompletion
  * Notification if identifier cannot be changed
  * Notification if expression cannot be parsed
  * Allow for moving local parameters into the global scope
  * Adjust Makefile for compilation with LLVM 3.2
  * Allow TeX in Plot labels
  * Center plot titles on axis not the figure

  * Fix: Segmentation fault when expanding reversible reactions
  * Fix: Export expressions using exp() to SBML
  * Fix: Steady state analysis, zero concentration wrongly declared unstable
  * Fix: Color of axis tick labels
  * Fix: Font size in SVG export
  * Fix: Remove Fano plot for volume parameter scan
  * Fix: Several memory leaks
  * Fix: Wrong parser handling of associative operators
  * Fix: Serialization of big integers abs()>1e6
  * Fix: Update kinetic law/chem eq view after reaction update
  * Fix: Exception handling for volume 0
  * Fix: Allow unknown TeX symbols in Customized Plot
  * Fix: Exception for empty model analysis
  * Fix: Update reaction view if parameter name or identifier was changed
  * Fix: Layout of TeX superscript
  * Fix: Function calls in expression writer
  * Fix: Vertical alignment of line and label in plot legends
  * Fix: Units for reaction parameters
  * Fix: Customized Plot missing plot legends
  * Fix: Handling of unicode filenames
  * Fix: Export of substance units to SBML
  * Fix: Parsing substance units in SBMLsh
  * Fix: Background of rendered expressions.
  * Fix: Adjusted mouse cursor hoovering for model units to indicate that they are editable. Also add a tooltip.
  * Fix: Window title in SBML-SH editor
  * Fix: Colors in steady state tables (being non-selectable)