Summary: An analysis tool for biochemical reaction networks

%define version 0.4.1

License: GPLv2
Group: Productivity/Scientific/Chemistry
Name: intrinsic-noise-analyzer
Prefix: /usr
Release: 1
Source: intrinsic-noise-analyzer-%{version}.tar.gz
URL: https://googlecode.com/p/intrinsic-noise-analyzer
Version: %{version}
Buildroot: /tmp/intrinsicnoiseanalyzerrpm
BuildRequires: gcc-c++, cmake, libsbml-devel >= 5.0
Requires: libsbml >= 5.0, libina = %{version}-%{release}
%if 0%{?suse_version}
BuildRequires: libqt4-devel >= 4.5, libginac-devel, llvm-devel >= 2.9
Requires: libqt4 >= 4.5, libginac2, llvm >= 2.9
%endif
%if 0%{?fedora}
BuildRequires: qt4-devel >= 4.5, ginac-devel, llvm-devel >= 2.9
Requires: qt4 >= 4.5, ginac, llvm >= 2.9
%endif

%description
iNA is a computational tool for quantitative analysis of fluctuations in biochemical reaction
networks. Such fluctuations, also known as intrinsic noise, arise due to the stochastic nature of
chemical reactions and cannot be ignored when some molecules are present in very low copy numbers
only, as is the case in living cells. The SBML-based software computes statistical measures such
as means and standard deviations of concentrations within a given accuracy. This is carried out
automatically using the system size expansion whose leading order term is the popular Linear Noise
Approximation. The results of the analysis can be tested against the computationally much more
expensive stochastic simulation algorithm.


%package -n libina
Summary: Runtime library for the intrinsic Noise Analyzer
Group: Science
BuildRequires: gcc-c++, cmake, libsbml-devel >= 5.0
Requires: libsbml >= 5.0
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
cmake -DCMAKE_BUILD_TYPE=RELEASE -DINA_ENABLE_NEW_VERSION_CHECK=OFF -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr
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
