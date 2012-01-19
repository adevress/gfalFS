Name:				gfalFS
Version:			1.0.0
Release:			2beta1%{?dist}
Summary:			Mount/unmount a GFAL file system
Group:				Applications/Internet
License:			ASL 2.0
URL:				https://svnweb.cern.ch/trac/lcgutil/wiki/gfal2
# svn export http://svn.cern.ch/guest/lcgutil/wlcggridfs/trunk gfalfs
Source0:			http://grid-deployment.web.cern.ch/grid-deployment/dms/lcgutil/tar/%{name}/%{name}-%{version}.tar.gz 
BuildRoot:			%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:		scons
BuildRequires:		glib2-devel
BuildRequires:		gfal2-devel
BuildRequires:		fuse-devel

Requires:			fuse%{?_isa}

%description
gfalFS provides a solution to mount any distributed file system managed \
by GFAL 2.0 permitting easy interactions with a large set of distributed file systems. 

%post

%clean
rm -rf "$RPM_BUILD_ROOT";
scons  %{?_smp_mflags} main_core=yes production=yes epel=yes -c build

%prep
%setup -q

%build
scons %{?_smp_mflags} main_core=yes production=yes epel=yes build

%postun

%install
rm -rf "$RPM_BUILD_ROOT"; 
scons  %{?_smp_mflags} main_core=yes production=yes epel=yes \
--install-sandbox="$RPM_BUILD_ROOT" install 

%files
%defattr (-,root,root)
%{_bindir}/gfalFS
%{_bindir}/gfalFS_umount


%changelog
* Mon Nov 14 2011 adevress at cern.ch 
 - Initial gfalFS 1.0 preview release
