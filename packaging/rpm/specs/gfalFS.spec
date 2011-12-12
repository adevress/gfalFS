Name:				gfalFS
Version:			1.0.0
Release:			1.11_preview
Summary:			GFAL 2.0 file system 
Group:				Applications/Internet
License:			ASL 2.0
URL:				https://svnweb.cern.ch/trac/lcgutil/wiki/gfal2
## source URL
# svn export http://svn.cern.ch/guest/lcgutil/wlcggridfs/trunk gfalfs
#
Source:				%{name}-%{version}.src.tar.gz
BuildRoot:			%{_tmppath}/%{name}-%{version}-%{release}

BuildRequires:		scons
BuildRequires:		glib2-devel%{?_isa}
BuildRequires:		gfal2-devel%{?_isa}
BuildRequires:		fuse-devel%{?_isa}
Requires:			glib2%{?_isa}
Requires:			gfal2-core%{?_isa}
Requires:			fuse%{?_isa}

%description
gfalFS provides a solution to mount any distributed file system managed \
GFAL 2.0. That allows standard POSIX access to remote distributed files.

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
%{_bindir}/gfalFS
%{_bindir}/gfal2_umount

%changelog
* Mon Nov 14 2011 adevress at cern.ch 
 - Initial gfalFS 1.0 preview release
