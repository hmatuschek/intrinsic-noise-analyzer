# Introduction #

This page describes the command line interface `ina-cli`, which is shipped along with iNAs GUI application. Currently, only the Linux distributions of iNA contain this command line tool.

The `ina-cli` tool provides a easy to use command line interface to iNA. This tool allows to perform analyses from command line, thus also on remote machines via SSH and `screen`.

This tool implements the following operations and analyses on models defined in SBML or SBML-sh:

  * Steady state analysis (SSE),
  * parameter scan in steady state (SSE),
  * time-course analysis with system size expansion (SSE)
  * time-course analysis with stochastic simulation algorithm (SSA),
  * setting of initial values for variables (parameters, compartments and species),
  * listing of variables and
  * export of (modified) models to SBML and SBML-sh.

The analyses results can be exported as either CSV into a file or printed to stdout.  Alternatively, the analyses results might be exported into a Matlat MAT file.


# Synopsis #

```

ina_cli --version
ina_cli --help
ina_cli [GLOBAL_OPTIONS] COMMAND [COMMAND_OPTIONS] MODEL OUTPUT```


# Global options #
Global parameters specifiy options, that apply globally on all analyses.

```
--loglevel=LEVEL```

The `--loglevel` specifies the log-level. The level is specified by name and must be one of `debug`, `info`, `warn` or `error`.


# Model specification #
This section describes how SBML or SBML-sh models are imported. The `--model` option allows to specify a model file of either SBML or SBLM-sh format. The file-type is determined by the file extension. Where files with the extensions `*.xml` and `*.sbml` are imported as SBML files and files with the extensions `*.mod` and `*.sbmlsh` are imported as SBML-sh files.

```
--model=FILE | --model-sbml=FILE | --model-sbmlsh=FILE [--set-value="ID=VALUE" ...]```

If the specified file has non of these extensions, you can use one of the explicit import options `--model-sbml` or `--model-sbmlsh`.

Optionally a the initial values of variables might by set prior to any analysis using the `--set-value` option.

  * `--model=FILE`, `-m FILE`
> > specifies the model to import. The type of the model is determined by the file extension.
  * `--model-sbml=FILE`
> > import the given file as a SBML file, ignoring the file extension.
  * `--model-sbmlsh=FILE`
> > import the given file as a SBML-sh file, ignoring the file extension.
  * `--set-value="ID=VALUE"`
> > sets the initial value of the variable (parameter, compartment or species) specified by `ID` to the given `VALUE`.


# Output specification #
This section describes how the results of analyses performed by iNA are exported. iNA can export results as comma separated values (CSV) and as Matlab MAT files.

```
--output=FILE | --output-csv=FILE | --output-mat=FILE | --output-stdout```

  * `--output=FILE`, `-o FILE`
> > specifies the file, the results are stored. The file type is determined by the file extension. If the file has the extension **.csv, the results are exported as CSV and if the file extension is**.mat, the results are exported as a Matlab MAT file.
  * `--output-csv=FILE`
> > exports the results as a CSV file, ignoring the file extension.
  * `--output-stdout`
> > prints the results as CSV into stdout.
  * `--output-mat=FILE`
> > exports the results as a Matlab MAT file, ignoring the file extension.


# Steady state analysis #
```
ina-cli [GLOBAL_OPTIONS] --steadystate [--max-iter=VALUE] [--eps=VALUE] [--max-df=VALUE] [--min-df=VALUE] MODEL OUTPUT```

The `--steadystate` command performs a steady state analysis on a model specified by `MODEL`. The result is exported as specified by `OUTPUT`. The steady state is determined in terms of the system size expansion with the accuracy of ![http://latex.codecogs.com/png.latex?\Omega^{-2}%.png](http://latex.codecogs.com/png.latex?\Omega^{-2}%.png) as described in [Grima, Thomas, Straube (2011)](http://link.aip.org/link/doi/10.1063/1.3625958) and [Thomas, Matuschek, Grima (2012)](http://dx.doi.org/10.1109/BIBM.2012.6392668).

  * `--max-iter=VALUE`
> > specifies the maximum number of iterations allowed to reach the steady state. (default: `VALUE`=100)
  * `--eps=VALUE`
> > specifies the absolute precision of the steady state. (default: `VALUE`=1e-9)
  * `--max-df=VALUE`
> > pecifies the maximum time-step size for the intermediate integration to reach the steady state. (default: `VALUE`=1e9)
  * `--min-df=VALUE`
> > specifies the minimum time-step size for the intermediate integration to reach the steady state. (default: `VALUE`=1e-1)


# Parameter scan #
```
ina-cli [GLOBAL_OPTIONS] --scan=PARAMETER --range=FROM:TO[:STEPS] MODEL OUTPUT```

The `--scan` command performs a parameter scan for the parameter specified by `PARAMETER`.  This type of analysis allows you to access parametric dependence of steady fluctuations of biochemical networks. Providing invaluable information that is difficult to obtain from stochastic simulations. The steady state is determined in terms of the system size expansion with the accuracy of ![http://latex.codecogs.com/png.latex?\Omega^{-2}%.png](http://latex.codecogs.com/png.latex?\Omega^{-2}%.png) as described in [Grima, Thomas, Straube (2011)](http://link.aip.org/link/doi/10.1063/1.3625958) and [Thomas, Matuschek, Grima (2012)](http://dx.doi.org/10.1109/BIBM.2012.6392668).

  * `--range=FROM:TO[:STEPS]`, `-R FROM:TO[:STEPS]`
> > specifies the value range for the scan. This performs a parameters scan from value `FROM` to value `TO` with `STEPS` steps. If `STEPS` is obmitted, 100 steps are taken.


# System size expansion (SSE) #
```
 ina-cli [GLOBAL_OPTIONS] (--re | --lna | --emre) --range=FROM:TO[:STEPS] MODEL OUTPUT```

These commands perform a time-course analysis using the system size expansion.

  * `--re`
> > performs a time-course analysis with the rate-equations. (Accuracy: ![http://latex.codecogs.com/png.latex?\Omega^{0}%.png](http://latex.codecogs.com/png.latex?\Omega^{0}%.png))
  * `--lna`
> > performs a time-course analysis with the linear noise approximation. (Accuracy ![http://latex.codecogs.com/png.latex?\Omega^{-1}%.png](http://latex.codecogs.com/png.latex?\Omega^{-1}%.png))
  * `--emre`
> > performs a time-course analysis with the effective mesoscopic rate-equations. (Accuracy: ![http://latex.codecogs.com/png.latex?\Omega^{-2}%.png](http://latex.codecogs.com/png.latex?\Omega^{-2}%.png))
  * `--range=FROM:TO[:STEPS]`, `-R FROM:TO[:STEPS]`
> > specifies the time range for the integration.


# Stochastic simulation algorithm (SSA) #
```
ina-cli [GLOBAL_OPTIONS] --ssa --range=FROM:TO[:STEPS] --ensemble=N MODEL OUTPUT```

The `--ssa` command performs a time-course analysis using the stochastic simulation algorithm.

  * `--range=FROM:TO[:STEPS]`, `-R FROM:TO[:STEPS]`
> > specifies the time range for the simulation.
  * `--ensemble=N`, `-N N`
> > specifies the ensemble size.


# Examples #
Performs a steady state analysis on the model `gene.sbmlsh` taken from [Tutorial 1](http://code.google.com/p/intrinsic-noise-analyzer/wiki/Tutorial1). The results are stored in the `gene_steadystate.csv` file as comma separated values (CSV).

```
ina-cli --steadystate -m gene.sbmlsh -o gene_steadystate.csv```

Perfroms a time-course analysis (LNA) on the model, the results are stored in the `gene_lna.csv` file.

```
ina-cli --lna -R 0:2:100 -m gene.sbmlsh -o gene_lna.csv```

Performs a time-course analysis (SSA) on the model, the results are stored in the `gene_ssa.csv` file.

```
ina-cli --ssa -R 0:2:100 -N 3000 -m gene.sbmlsh -o gene_ssa.csv```

<img src='http://i50.tinypic.com/16k2fqx.png' align='right' width='320' /> The following commands perform a RE time-course analysis and plots the results using `gnuplot`:

```

ina-cli --re -R 0:2:100 -m gene.sbmlsh -o gene_re.csv
gnuplot
gnuplot> set key autotitle columnhead
gnuplot> plot "gene_re.csv" using 1:2 with lines, \
"" using 1:3 with lines, \
"" using 1:4 with lines```

Producing the following figure.  The `set key autotitle columnhead` tells gnuplot to handle the first row of the data table as column names and to use these names for the legend. So gnuplot automatically selects the correct label for the selected columns.

`ina-cli` does not print any information about the progress of an analysis to the command line by default. If you want to be informed about the progress of long simulations, simply pass the `--loglevel=info` option to `ina-cli`. I.e.

```
 ina-cli --loglevel=info --ssa -R 0:2:100 -N 30000 -m gene.sbmlsh -o gene_ssa.csv```