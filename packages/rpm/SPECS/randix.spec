Name:           randix
Version:        1.0.0
Release:        1%{?dist}
Summary:        A simple terminal program that displays random characters.
License:        GPL3
BuildArch:      x86_64

%description
A simple terminal program that displays random characters.

%install
mkdir -p %{buildroot}/usr/bin
cp %{_sourcedir}/randix %{buildroot}/usr/bin/randix
chmod 755 %{buildroot}/usr/bin/randix

%files
/usr/bin/randix
