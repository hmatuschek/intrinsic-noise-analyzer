Summary: An analysis tool for biochemical reaction networks

%define version 0.2.99

License: GPLv2
Group: Productivity/Scientific/Chemistry
Name: intrinsic-noise-analyzer
Prefix: /usr
Release: 1
Source: intrinsic-noise-analyzer-%{version}.tar.gz
URL: https://googlecode.com/p/intrinsic-noise-analyzer
Version: %{version}
Buildroot: /tmp/intrinsicnoiseanalyzerrpm
BuildRequires: gcc-c++, cmake, libsbml-cppnamespace-devel = 5.0.0
Requires: libsbml-cppnamespace = 5.0.0, libina = %{version}-%{release}
%if 0%{?suse_version}
BuildRequires: libqt4-devel >= 4.5, libginac-devel, llvm-devel >= 2.9
Requires: libqt4 >= 4.5, libginac2, llvm >= 2.9
%endif
%if 0%{?fedora}
BuildRequires: qt4-devel >= 4.5, ginac-devel, llvm-devel >= 2.9
Requires: qt4 >= 4.5, ginac, llvm >= 2.9
%endif

%description
The intrinsic Noise Analyzer (iNA) is an easy-to-use computational tool for 
efficient analysis of intrinsic noise in biochemical reaction networks. The 
SBML-based software combines two complementary approaches to analyze the 
Chemical Master Equation:

 - the System Size Expansion - a systematic analytical approximation method,
 - the Stochastic Simulation Algorithm - a widely used Monte Carlo method.

iNA is based on the computer algebra system Ginac and facilitates multi-core
simulations.


%package -n libina
Summary: Runtime library for the intrinsic Noise Analyzer
Group: Science
BuildRequires: gcc-c++, cmake, libsbml-cppnamespace-devel = 5.0.0 
Requires: libsbml-cppnamespace = 5.0.0
%if 0%{?suse_version}
BuildRequires: libginac-devel
Requires: libginac2
%endif
%if 0%{?fedora}
BuildRequires: ginac-devel
BuildRequires: ginac
%endif

%description -n libina
Provides the runtime library for the intrinsic Noise Analyzer.

%prep
%setup -q

%build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr
make

%install
make install

%post
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%postun
/sbin/ldconfig


%files
%defattr(-, root, root, -)
%attr(755, root, root) %{_prefix}/bin/ina
%{_prefix}/share/applications/IntrinsicNoiseAnalyzer.desktop
%{_prefix}/share/icons/IntrinsicNoiseAnalyzer.png


%files -n libina
%defattr(-, root, root, -)
%{_libdir}/libina.so
