Name: show
Version: 1.0
Release: alt1

Summary: Simple ncurses text file viewer
License: Unspecified
Group: Text tools
Packager: Tumen Tsybikov <tumen.tsybikov04@mail.ru>
Url: https://github.com/TsybikovTumen/linux2026
Source: %name-%version.tar.gz

BuildRequires: gcc make libncurses-devel

%description
Show displays a text file in a terminal window using ncurses.
Use the arrow keys, Space, Page Up and Page Down to scroll, and Esc to exit.

%prep
%setup -q

%build
%make_build CC=gcc CFLAGS="%optflags -Wall -Wextra"

%install
%makeinstall_std PREFIX=%_prefix BINDIR=%_bindir

%files
%_bindir/Show
