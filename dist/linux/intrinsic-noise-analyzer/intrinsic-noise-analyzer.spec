Summary: An analysis tool for biological networks

%define version 0.2.0

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
BuildRequires: libqt4-devel >= 4.5, libginac-devel
Requires: libqt4 >= 4.5, libginac2
%endif
%if 0%{?fedora}
BuildRequires: qt4-devel >= 4.5, ginac-devel
BuildRequires: qt4 >= 4.5, ginac
%endif

%description
An analysis tool for biological networks.

%package -n libina
Summary: Runtime library for the Intrinsic Noise Analyzer
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
Provides the runtime-library for the Intrinsic Noise Analyzer.

%prep
%setup -q

%build
cmake -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr
make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

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
