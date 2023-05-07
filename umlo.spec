Name:           umlo
Summary:        umlo for Hikvision
Version:        1.0.0

%if 0%{?mageia}
Release:        %mkrel 1
%else
Release:        1.surf.mlo
%endif

License:        GPLv3
Group:          Video/Players
URL:            https://github.com/surfzoid/umlo
Source0:        https://github.com/surfzoid/umlo/archive/%{version}/umlo-%{version}.tar.gz
# List of additional build dependencies
%if 0%{?mageia}
BuildRequires:  qtbase5-common-devel
BuildRequires:  pkgconfig(Qt5Widgets)
%else
%if 0%{?suse_version}
BuildRequires:  libqt5-qtbase-devel
%else
BuildRequires:  pkgconfig(Qt5)
%endif
%endif
BuildRequires:  wget
BuildRequires:  unzip

Requires: qtbase5-common
Requires:	%{name}-python = %{version}
 
%description
A wizard to configure and install umlo https://github.com/surfzoid/umlo


%package	python
Summary:	Extract periodically record event from Hikvion camera or NVR.
Group:		Networking/Other
Requires:	%{name} = %{version}
Requires:	python3

%description	python
Provides functionality to extract periodically record event from 
Hikvion camera or NVR with ISAPI and HTTPDigestAuth enable.
Add this script to an schedule task and you will keep records 
during the number of day you put in the config or 
use the GUI wizard /usr/bin/umlo.
 
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

mkdir -p %{buildroot}%{_datadir}/umlo/template/

wget https://github.com/surfzoid/umlo/archive/refs/heads/main.zip
unzip -o ./main.zip
cd ./umlo-main/
cp -fv ./*.py %{buildroot}%{_datadir}/umlo/template/
cp -fv ./LICENSE %{buildroot}%{_datadir}/umlo/template/
cp -fv ./README.md %{buildroot}%{_datadir}/umlo/template/

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


%files python
%{_datadir}/umlo/template/

%changelog
* Sat May 6 2023 surfzoid@gmail.com
+ Split in bin and python-umlo rpms.

* Sat Apr 22 2023 surfzoid@gmail.com
+ when start, test and display phyton version.
+ check if umlo-python is Here.
+ sugest deb, rpm repo or for windoze download.

* Sat Apr 15 2023 surfzoid@gmail.com
+ Windoze schedle task.
+ Cosmetics.

* Mon Feb 06 2023 surfzoid@gmail.com
+ First release.