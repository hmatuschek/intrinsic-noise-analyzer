Summary: LibSBML with C++ namespace

%define version 5.0.0

License: LGPLv2
Group: Development/Libraries/C and C++
Name: libsbml-cppnamespace
Prefix: /usr
Release: 1
Source: libsbml-cppnamespace-%{version}.tar.gz
URL: https://sbml.org
Version: %{version}
Buildroot: /tmp/libsbmlcppnamespacerpm
BuildRequires: gcc-c++, cmake, libxml2-devel, zlib-devel, libbz2-devel, pkgconfig
Requires: libxml2, zlib
%if 0%{?fedora}
Requires: bzip2-libs
%endif
%if 0%{?suse}
Requires: libbz2-1
%endif
Conflicts: libsbml

%description
LibSBML is a free, open-source programming library to help you read, write, manipulate, translate, and validate SBML files and data streams. It is not an application itself (though it does come with example programs), but rather a library you can embed in your own applications.


%package devel
Summary: LibSBML header files
Group: Science
Requires: libxml2-devel, zlib-devel, libbz2-devel
Requires: %{name} = %{version}-%{release}, pkgconfig

%description devel
LibSBML is a free, open-source programming library to help you read, write, manipulate, translate, and validate SBML files and data streams. It is not an application itself (though it does come with example programs), but rather a library you can embed in your own applications.

This package contains the header files.

%prep
%setup -q 

%build
./configure --prefix=%{_prefix} \
            --libdir=%{_libdir} \
            --enable-cpp-namespace
make

%install
make DESTDIR=%buildroot install

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig


%files
%defattr(-, root, root, -)
%{_libdir}/libsbml.so

%files devel
%defattr(-, root, root, -)
%dir %{_prefix}/include/sbml/
%{_prefix}/include/sbml/*
%dir %{_prefix}/include/sbml/common/
%{_prefix}/include/sbml/common/*
%dir %{_prefix}/include/sbml/compress/
%{_prefix}/include/sbml/compress/*
%dir %{_prefix}/include/sbml/extension/
%{_prefix}/include/sbml/extension/*
%dir %{_prefix}/include/sbml/math/
%{_prefix}/include/sbml/math/*
%dir %{_prefix}/include/sbml/util/
%{_prefix}/include/sbml/util/*
%dir %{_prefix}/include/sbml/validator/
%{_prefix}/include/sbml/validator/*
%dir %{_prefix}/include/sbml/xml/
%{_prefix}/include/sbml/xml/*

%{_prefix}/lib/pkgconfig/libsbml.pc
%{_prefix}/share/pkgconfig/libsbml.pc
%{_libdir}/libsbml.a
%{_libdir}/libsbml.la
%{_libdir}/libsbml.so.5
%{_libdir}/libsbml.so.%{version}
