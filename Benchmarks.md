# Benchmarks #

(See our [preprint](http://grimagroup.bio.ed.ac.uk/documents/manuscript.pdf) for latest benchmarks)

We have carried out a detailed comparison of the performance of iNA's methods.
We have used the SBML model [coremodel2.xml](http://code.google.com/p/intrinsic-noise-analyzer/downloads/detail?name=sbmlmodels-0.2-plosone.zip&can=2&q) which describes an ensemble of independent circadian oscillators exhibiting damped oscillations. The model is described in detail in [Thomas, Matuschek, Grima (2012)](http://dx.doi.org/10.1371/journal.pone.0038518).

The result of both the system size expansion and the average of 50,000 stochastic simulation produces the following graphs:

![http://oi50.tinypic.com/zvpmk0.jpg](http://oi50.tinypic.com/zvpmk0.jpg) ![http://oi49.tinypic.com/15o7y1y.jpg](http://oi49.tinypic.com/15o7y1y.jpg)

The average concentration levels as well as the standard deviation (indicated by the shaded areas) are in very good agreement.

Below you find benchmarks which discriminate different methods of expression evaluation available through iNA's GUI:

  * **GiNaC** is the computer algebra system used by iNA which relies on arbitrary precision floating point arithmetic (not contained in the GUI for obvious reasons),
  * **BCI** a basic [bytecode interpreter](http://en.wikipedia.org/wiki/Bytecode) executing compact numeric codes of iNA's mathematical model representation (featured since iNA 0.2),
  * **JIT** the [just-in-time](http://en.wikipedia.org/wiki/Just-in-time_compilation) compiler based on the [LLVM](http://www.llvm.org) framework. It automatically compiles the mathematical model representation into executable code (featured in iNA 0.3).

The use of the bytecode interpreter and JIT compiler enables one to also use instruction as well as compiler optimizations.

### System Size Expansion ###

A detailed benchmark of iNA's IOS analysis is performed for the ODE integrators Rosenbrock and LSODA in combination with different techniques of expression evaluation available through iNA's GUI.

|        | **GiNaC**  | **BCI**     | **BCI** (opt) | **JIT** | **JIT** (opt)|
|:-------|:-----------|:------------|:--------------|:--------|:-------------|
|**Rosenbrock** | >120s         | 12.97s	     | 15.90s        | 6.04s   | 5.96s        |
|**LSODA**      | >80s       | 7.59s	      | 5.60s         | 0.47s   | 0.45s        |

### Stochastic Simulation ###

We have also compared the execution times for Direct and Optimized SSA using an ensemble average of 50,000 stochastic simulations. The first value in the table denotes the simulation time in hours while the second value denotes the average simulation time of a single realization in seconds.

|                | **GiNaC**         | **BCI**         | **BCI** (opt)   | **JIT**          | **JIT** (opt)   |
|:---------------|:------------------|:----------------|:----------------|:-----------------|:----------------|
|**Direct SSA**    | 136.93h / 9.86s   | 2.28h / 0.16s   | 2.00h / 0.14s   | 0.71h / 0.05s    | 0.71h / 0.05s   |
|**Optimized SSA** | 58.79h / 4.23s    | 1.18h / 0.08s   | 1.08h / 0.08s   | 0.70h / 0.05s    | 0.70h / 0.05s   |

All benchmarks were performed on MacPro, MacOS 10.6, 2 x Quad Core Intel Xeon @ 2.4Ghz, 16GB RAM using a single core.

### Conclusion ###

The analysis of stochastic biochemical kinetics can greatly benefit from approximate methods. In present case the iNA's system size expansion reduces the time to obtain accurate statistical information of the dynamics to only a fraction of seconds. This high performance is particular conspicuous compared to execution times of about 40min for the exact stochastic simulation methods. This discrepancy mainly stems from the fact that the stochastic simulation algorithm needs to simulate each reaction explicitly and requires a tremendous amount of ensemble average to obtain accurate statistical information.

The performance of iNA's analysis methods greatly benefit from the use of dynamic bytecode intepreters and modern just-in-time compilation techniques. At heart iNA is based on the powerful computer algebra system [GiNaC](http://www.ginac.de). Such frameworks however are limited by the speed of their arbitrary precision arithmetic units and hence are not well suited for numerical tasks. Using bytecode interpreters improves the performance of expression evaluation by at least one order of magnitude.
For the system size method the use of just-in-time compilation in combination with the economic ODE integrator LSODA reduces execution times by another order of magnitude. For the stochastic simulation algorithm lesser reductions are observed mainly because its performance is limited by other factors as the random number generator and the reaction selection step.
