#norootforbuild

Summary: C++ library for symbolic calculations
Name: ginac
Version: 1.6.2
Release: 0
License: GPL
Group: Development/Libraries/C and C++
Source0: ftp://ftpthep.physik.uni-mainz.de/pub/GiNaC/%{name}-%{version}.tar.bz2
Url: http://www.ginac.de/
BuildRoot: %{_tmppath}/%{name}-%{version}-build
BuildRequires: cln-devel gcc-c++ readline-devel gmp-devel pkgconfig

%description
GiNaC (which stands for "GiNaC is Not a CAS (Computer Algebra System)") is an
open framework for symbolic computation within the C++ programming language.

%package -n libginac2
Summary: C++ library for symbolic calculations
Group: Development/Libraries/C and C++

%description -n libginac2

GiNaC (which stands for "GiNaC is Not a CAS (Computer Algebra System)") is an
open framework for symbolic computation within the C++ programming language.

%package -n libginac-devel
Summary: GiNaC development libraries and header files
Group: Development/Libraries/C and C++
Requires: libginac2 = %{version} cln-devel
PreReq:   %install_info_prereq

%description -n libginac-devel
GiNaC (which stands for "GiNaC is Not a CAS (Computer Algebra System)") is an
open framework for symbolic computation within the C++ programming language.

This package contains the libraries, include files and other resources you
use to develop GiNaC applications.

%prep
%setup -q

%build
autoreconf -fiv
%configure --disable-static --with-pic --disable-rpath

%{__make} %{?jobs:-j%jobs}

%check

export MALLOC_CHECK_=2

%{__make} check

unset MALLOC_CHECK_

%install
%makeinstall

rm -f %{buildroot}%{_libdir}/*.la

%clean
rm -rf ${RPM_BUILD_ROOT}

%post -n libginac2 -p /sbin/ldconfig

%postun -n libginac2 -p /sbin/ldconfig

%post -n libginac-devel
%install_info --info-dir=%{_infodir} %{_infodir}/ginac.info.gz

%postun -n libginac-devel
%install_info_delete  --info-dir=%{_infodir} %{_infodir}/ginac.info.gz


%files -n libginac2
%defattr(-,root,root)
%{_libdir}/libginac.so.2*

%files -n libginac-devel
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog NEWS README
%{_libdir}/*.so
%{_libdir}/pkgconfig/ginac.pc
%dir %{_includedir}/ginac
%{_includedir}/ginac/*.h
%{_infodir}/*.info*

%files
%defattr(-,root,root)
%{_bindir}/ginsh
%{_bindir}/viewgar
%{_bindir}/ginac-excompiler
%{_mandir}/man1/ginsh.1*
%{_mandir}/man1/viewgar.1*

%changelog
