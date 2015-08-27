iNA is distributed as executable binaries for all popular platforms like [Linux](Install#Linux.md), [MacOS X](Install#MacOS_X.md) and Microsoft [Windows](Install#Windows.md). If we do not provide a binary package for your operating system, you can build iNA from the [sources](Install#Build_iNA_from_Sources.md).


# Linux #
We provide binary-packages of iNA and its dependencies for popular Linux distributions using the openSuSE build service. You can add our repository to your package management system to install iNA comfortably and to receive automatic updates as we release new versions of iNA.

## Ubuntu ##
Under Ubuntu 13.04 ("Raring Ringtail") add the following line to your source list in `/etc/apt/sources.lst`:

```

deb http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/xUbuntu_13.04/ 	./ ```

for Ubuntu 12.10 ("Quantal Quetzal"):

```

deb http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/xUbuntu_12.10/ 	./ ```

and for Ubuntu 12.04 ("Precise Pangolin"):

```

deb http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/xUbuntu_12.04/ 	./ ```

You need to add the repository key to your key ring to install software from this repository.  Download the [key file](http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/xUbuntu_13.04/Release.key) and add it to your key ring by calling
```

sudo apt-key add Release.key```
on your command line. Finally, update the package database by calling
```

sudo apt-get update```
from command line. Now, you can install iNA by calling
```

sudo apt-get install intrinsic-noise-analyzer```

## Debian ##
Since version 0.4.2, iNA also supports the current Debian distribution (7.0) and we also provide pre-build packages for it. To add your Debian repository to your list of software sources, add the following line to the file `/etc/apt/sources.lst`
```

deb http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/Debian_7.0/ ./ ```

Like under Ubuntu, you must also add the public key of the repository to your package management. Download the [key file](http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/Debian_7.0/Release.key) and add it to your key ring by calling
```

sudo apt-key add Release.key```
on your command line. Finally, update the package database by calling
```

sudo apt-get update```
from command line. Now, you can install iNA by calling
```

sudo apt-get install intrinsic-noise-analyzer```


## openSuSE ##
We provide binary packages of iNA for openSuSE 12.1, 12.2  and 12.3. To add our repository to your list of software sources, run the following command as root (for openSuSE 12.1):
```

zypper addrepo --check --name "Intrinsic Noise Analyzer" \
http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/openSUSE_12.1/ \
IntrinsicNoiseAnalyzer
```

for openSuSE 12.2:
```

zypper addrepo --check --name "Intrinsic Noise Analyzer"  \
http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/openSUSE_12.2/ \
IntrinsicNoiseAnalyzer
```

and for openSuSE 12.3:
```

zypper addrepo --check --name "Intrinsic Noise Analyzer"  \
http://download.opensuse.org/repositories/home:/hannesmatuschek:/IntrinsicNoiseAnalyzer/openSUSE_12.3/ \
IntrinsicNoiseAnalyzer
```

finally you can install the intrinsic Noise Analyzer with
```

zypper install intrinsic-noise-analyzer```


## Fedora ##
To add our repository for Fedora (17, 18, 19) to your list of software sources, execute
```

su -c 'curl http://intrinsic-noise-analyzer.googlecode.com/files/intrinsic-noise-analyzer.fedora.repo >> \
/etc/yum.repos.d/intrinsic-noise-analyzer.fedora.repo'```
on the command line. The above downloads our [repository file](http://intrinsic-noise-analyzer.googlecode.com/files/intrinsic-noise-analyzer.fedora.repo) and copies it to `/etc/yum.repos.d/` (needs root access). Alternatively you can do this manually.

iNA can now be installed using `yum`, for example on the command-line:

```

su -c 'yum install intrinsic-noise-analyzer'```


# MacOS X #
We provide an application-bundle (dmg), that runs without installation under MacOS 10.6 and newer versions under 64bit systems. You can download this package from the [Downloads](https://code.google.com/p/intrinsic-noise-analyzer/downloads/list) section.


# Windows #
We provide an installer for the Microsoft Windows platform, you can download the installer from the [Downloads](https://code.google.com/p/intrinsic-noise-analyzer/downloads/list) section.

# Build iNA from Sources #
This section gives a brief introduction how to compile iNA from sources. We have not reinvented the wheel, therefore iNA depends on other open source libraries which are mostly available through the package management system of common Linux distributions. iNA uses the CMake build system for the configuration, compilation and installation. Therefore the build steps described below are basically the same for all platforms. However, this section describes the build on a Linux system.

## Dependencies ##
  * A **C++ compiler** (usually GCC or clang), shipped along with almost all Linux distributions.
  * **[cmake](http://www.cmake.org)**: iNA uses CMake as its build system.
  * **[GiNaC](http://www.ginac.de)**: This is the computer algebra system iNA uses for all mathematical manipulations. To compile iNA from sources the libraries as well as the development files are needed. The most Linux distributions provide these files as pre-build packages usually called _libginac_ and _libginac-devel_ or similar.
  * **[libsbml](http://www.sbml.org)**: A library to read and write SBML models. Unfortunately only a few Linux distributions deliver pre-build packages for this library. You may also need to compile them from sources.
  * **[Qt4](http://www.qt-project.org)**: A graphical user interface library. iNA uses Qt for its user interface, you also need the development files (headers etc.). These files are distributed by almost all Linux distributions as pre-build packages usually called _libqt4_ and _libqt4-devel_ or similar.
  * **[LLVM](http://www.llvm.org)**: The low level virtual machine, a collection of runtime libraries to build a JIT compiler and optimizer. iNA uses LLVM for the JIT compilation of mathematical expressions for fast evaluation. You also need the development files for LLVM to compile iNA usually called _llvm_ and _llvm-devel_ or similar.
  * **[Eigen 3](http://eigen.tuxfamily.org)**: Eigen 3 a C++ linear algebra template library. Some Linux distributions also provide this library as a package usually called _libeigen3-devel_ or similar.

## Compile iNA ##
Download the iNA [sources](https://code.google.com/p/intrinsic-noise-analyzer/downloads/list) and unpack them. You will find a directory called “intrinsic-noise-analyzer-x.x.x”. Change into that directory and create a build directory (i.e. “build”, the actual name doesn’t matter) and change into that build directory.  Now, configure the build of iNA with cmake. Execute

```
$ cmake .. -DCMAKE_BUILD_TYPE=RELEASE```

This step configures the build of iNA and searches for the required libraries. If this step fails, please make sure that all the dependencies listed above are installed and can be found by CMake. By default on Unix systems, iNA will be installed into the `/usr/local` directory. This can be changed by appending `-DCMAKE_INSTALL_PREFIX=PREFIXPATH` to the configuration step above. This will install iNA into the directory `PREFIXPATH`. iNA also supports to notify you about a new release. This feature is disabled by default. If you want to get notified if a new version of iNA is released, append `-DINA_ENABLE_VERSIONCHECK=ON` to the configuration above.  If the configuration succeeds, compile iNA by executing

```
$ make```

This will compile iNA. This step may take some time but should succeed as the configuration step above ensures that all requirements are met. If not, please contact us. Finally iNA is installed by executing

```
$ sudo make install```

iNA should now be installed into your system and can be started with

```
$ ina```

On Linux systems, iNA installs also a “.desktop“ file into the directory `CMAKE_INSTALL_PREFIX/share/application/` so your desktop may list iNA in its application list.