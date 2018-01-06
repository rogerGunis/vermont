Name:    vermont
Version:
Release:
License:
Summary: Vermont is an open-source software toolkit for the creation and processing of network flow data
Url:
Group: network, monitoring
Source:
Patch:
BuildRequires: libmysqlclient-dev
BuildRequires: libczmq-dev
BuildRequires: cmake
BuildRequires: libboost-filesystem-dev
BuildRequires: libboost-regex-dev
BuildRequires: libboost-test-dev
BuildRequires: libboost-thread-dev
BuildRequires: libxml2-dev
BuildRequires: libpcap-dev
BuildRequires: libsctp-dev
PreReq:
Provides:
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

%post

%postun

%files
%defattr(-,root,root)
%doc ChangeLog README COPYING

