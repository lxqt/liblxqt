# liblxqt

## Overview

`liblxqt` represents the core library of LXQt providing essential functionality
needed by nearly all of its components.

## Installation

### Sources

Its runtime dependencies are libxss, KWindowSystem, qtbase, qtx11extras and
[libqtxdg](https://github.com/lxqt/libqtxdg).
Additional build dependencies are CMake and optionally Git to pull latest VCS
checkouts. 

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX`
will normally have to be set to `/usr`, depending on the way library paths are
dealt with on 64bit systems variables like `CMAKE_INSTALL_LIBDIR` may have to be
set as well.

To build run `make`, to install `make install` which accepts variable `DESTDIR`
as usual.

### Binary packages

The library is provided by all major Linux distributions like Arch Linux, Debian,
Fedora and openSUSE. Just use your package manager to search for string `liblxqt`.


### Translation 

Translations can be done in [LXQt-Weblate](https://translate.lxqt-project.org/projects/lxqt-desktop/liblxqt/)

<a href="https://translate.lxqt-project.org/projects/lxqt-desktop/liblxqt/">
<img src="https://translate.lxqt-project.org/widgets/lxqt-desktop/-/liblxqt/multi-auto.svg" alt="Translation status"
</a>
