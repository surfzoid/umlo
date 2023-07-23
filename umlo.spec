Name:           umlo
Summary:        umlo for French MLO Repository
Version:        1.0.8

%global Rel 10
%if 0%{?mageia}
Release:        %mkrel %{Rel}
%elif 0%{?fedora} > 36 || 0%{?rhel} > 6
Release:        %{Rel}.surf%{?dist}
%else
Release:        %{Rel}.surf.mlo
%endif

License:        GPLv3
Group:          Networking/File transfer
URL:            https://github.com/surfzoid/umlo
Source0:        https://github.com/surfzoid/umlo/archive/%{version}-%{Rel}/umlo-%{version}-%{Rel}.tar.gz
# List of additional build dependencies
%if 0%{?mageia}
BuildRequires:  qtbase5-common-devel
BuildRequires:  lib64qt5concurrent-devel
BuildRequires:  pkgconfig(Qt5Widgets)
%else
%if 0%{?suse_version}
BuildRequires:  libqt5-qtbase-devel
%else
BuildRequires:  pkgconfig(Qt5)
%endif
%endif

Requires: qtbase5-common
 
%description
A client for rpm builders to manage MLO Repository.
It increase productivity and reduce stress by freeing mind of where to put rpm at good place.

%prep
%autosetup -n umlo-%{version}-%{Rel}
sed -i 's/\(VERSION = \).*/\1         %{version}/' %{_builddir}/%{name}-%{version}-%{Rel}/QtVsPlayer.pro
sed -i 's/\(RELEASE = \).*/\1         %{Rel}/' %{_builddir}/%{name}-%{version}-%{Rel}/QtVsPlayer.pro
 
 
%build
%if 0%{?suse_version}
#find /|grep -i qmake
/usr/bin/qmake-qt5
%else
%qmake_qt5
%endif
%make_build
 
%install
%make_install INSTALL_ROOT=%{buildroot}%{_datadir}
 
%find_lang %{name} --with-qt --all-name

%post

%clean
rm -rf %buildroot
chmod -R ug+rw %{_rpmdir}
chmod -R ug+rw %{_srcrpmdir}
 
%files
%if 0%{?mageia}
%license LICENSE
%doc README.md
%else
%{_datadir}/doc/umlo/README.md
%{_datadir}/licenses/umlo/LICENSE
%endif
%{_bindir}/umlo
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/umlo.png
%{_datadir}/umlo/translations/umlo_fr_FR.qm

%changelog
* Fri Jul 14 2023 surfzoid@gmail.com
+ manage multiple build directory.

* Wed Jun 14 2023 surfzoid@gmail.com
+ Avoid conflict between user prefix and rpm name.

* Wed May 24 2023 surfzoid@gmail.com
+ Best perf.

* Wed May 10 2023 surfzoid@gmail.com
+ Ask to conect manualy.
+ Refresh dir before scan.
+ Check if mounted before operations.
+ Custom ssh root path in settings.

* Tue May 9 2023 surfzoid@gmail.com
- Bug line number.

* Mon May 8 2023 surfzoid@gmail.com
+ Use a QtableWidget.
+ Filter rpm list.

* Sun May 7 2023 surfzoid@gmail.com
+ First beta release.
