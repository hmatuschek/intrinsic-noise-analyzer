Summary: An analysis tool for biochemical reaction networks

%define version 0.4.2

License: GPL-2.0+
Group: Productivity/Scientific/Chemistry
Name: intrinsic-noise-analyzer
Prefix: /usr
Release: 1
Source: intrinsic-noise-analyzer-%{version}.tar.gz
URL: https://googlecode.com/p/intrinsic-noise-analyzer
Version: %{version}
Buildroot: /tmp/intrinsicnoiseanalyzerrpm
BuildRequires: gcc-c++, cmake, libsbml-devel >= 5.0
Requires: libsbml >= 5.0, libina0_4_2 = %{version}-%{release}
%if 0%{?suse_version}
BuildRequires: libqt4-devel >= 4.5, libginac-devel, llvm-devel >= 2.9, libeigen3-devel >= 3.0
Requires: libqt4 >= 4.5, libginac2, llvm >= 2.9
%endif
%if 0%{?fedora}
BuildRequires: qt4-devel >= 4.5, ginac-devel, llvm-devel >= 2.9, llvm-static >= 2.9, eigen3 >= 3.0
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


%package -n libina0_4_2
Summary: Runtime library for the intrinsic Noise Analyzer
Group: Development/Libraries/C and C++
BuildRequires: gcc-c++, cmake, libsbml-devel >= 5.0
Requires: libsbml >= 5.0
%if 0%{?suse_version}
BuildRequires: libginac-devel, libeigen3-devel >= 3.0
Requires: libginac2
%endif
%if 0%{?fedora}
BuildRequires: ginac-devel, eigen3 >= 3.0
BuildRequires: ginac
%endif

%description -n libina0_4_2
Provides the runtime library for the intrinsic Noise Analyzer.

%post -n libina0_4_2
/sbin/ldconfig

%postun -n libina0_4_2
/sbin/ldconfig


%package -n libina-devel
Summary: Runtime library for the intrinsic Noise Analyzer
Group: Development/Libraries/C and C++
BuildRequires: gcc-c++, cmake, libsbml-devel >= 5.0
Requires: libsbml >= 5.0, libina0_4_2 = %{version}-%{release}
%if 0%{?suse_version}
BuildRequires: libginac-devel, libeigen3-devel >= 3.0
Requires: libginac2
%endif
%if 0%{?fedora}
BuildRequires: ginac-devel, eigen3 >= 3.0
BuildRequires: ginac
%endif

%description -n libina-devel
Provides the runtime library header files for the intrinsic Noise Analyzer.

%prep
%setup -q

%build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DINA_ENABLE_NEW_VERSION_CHECK=OFF -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr
make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%post -n libina-devel
/sbin/ldconfig

%postun -n libina-devel
/sbin/ldconfig


%files
%defattr(-, root, root, -)
%attr(755, root, root) %{_prefix}/bin/ina
%{_prefix}/share/applications/IntrinsicNoiseAnalyzer.desktop
%{_prefix}/share/icons/IntrinsicNoiseAnalyzer.png

%files -n libina0_4_2
%defattr(-, root, root, -)
%{_libdir}/libina.so.*

%files -n libina-devel
%defattr(-, root, root, -)
%{_libdir}/libina.so
%{_includedir}/libina/
