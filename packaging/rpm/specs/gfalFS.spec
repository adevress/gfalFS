%define projectname gfalFS
%define version 1.0
%define release 1.11_preview


%define debug_package %{nil}

Name: %{projectname}
License: Apache-2.0
Summary: File system for lcg storage system
Version: %{version}
Release: %{release}
Group: Grid/lcg
BuildRoot: %{_tmppath}/%{projectname}-%{version}-%{release}
Source: %{projectname}-%{version}-%{release}.src.tar.gz
BuildRequires: gfal2-devel, glib2-devel, fuse-devel
Requires: gfal2, glib2, fuse
%description
User space file system able to mount \
any distributed storage system managed by GFAL 2.0 \
( LFC lfn:// , SRMv1/v2 srm://, rfio rfio://, dcap gsidcap://, local file://, http(s) http://, webdav dav://)
Allowing the usage of the remote grid files with \
non-grid applications.









%post 

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT";
scons  main_core=yes production=yes -c build

%prep
%setup -q

%build
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
scons -j $NUMCPU main_core=yes production=yes build

%postun


%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT"; 
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
scons  -j $NUMCPU  main_core=yes production=yes --install-sandbox="$RPM_BUILD_ROOT" install 


 
%files

%changelog
* Mon Nov 14 2011 adevress at cern.ch 
 - Initial gfalFS 1.0 preview release
