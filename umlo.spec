Name:           umlo
Summary:        umlo for French MLO Repository
Version:        1.0.0

%if 0%{?mageia}
Release:        %mkrel 4
%else
Release:        4.surf.mlo
%endif

License:        GPLv3
Group:          Networking/File transfer
URL:            https://github.com/surfzoid/umlo
Source0:        https://github.com/surfzoid/umlo/archive/%{version}/umlo-%{version}.tar.gz
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
A client for rpm builders to manage MLO Repository

%prep
%autosetup -n umlo-%{version}
 
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
* Sun May 7 2023 surfzoid@gmail.com
+ First beta release.