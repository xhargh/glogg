# Klogg-io

Klogg-io is a fork of Klogg that adds serial port support.

## Overview

Klogg is a multi-platform GUI application that helps browse and search
through long and complex log files. It is designed with programmers and
system administrators in mind and can be seen as a graphical, interactive
combination of grep, less, and tail.

Please refer to the
[documentation](DOCUMENTATION.md)
page for how to use Klogg.

<!--

### Current stable release

[ ![Github](https://img.shields.io/github/v/release/variar/klogg?style=flat)](https://github.com/variar/klogg/releases/tag/v20.4)
[ ![Bintray](https://img.shields.io/bintray/v/variar/generic/klogg?style=flat)](https://bintray.com/variar/generic/klogg/_latestVersion)
[ ![Chocolatey](https://img.shields.io/chocolatey/v/klogg?style=flat)](https://chocolatey.org/packages/klogg)
[ ![homebrew cask](https://img.shields.io/homebrew/cask/v/klogg?style=flat)](https://formulae.brew.sh/cask/klogg)
-->
### Latest testing builds

![CI Build and Release](https://github.com/xhargh/klogg-io/workflows/CI%20Build%20and%20Release/badge.svg)

| Windows x64 | Windows x86 | Linux x64  | Mac x64 |
| ------------- |------------- | ------------- | ------------- |
| Build status | [![Win32 Build Status](https://ci.appveyor.com/api/projects/status/github/xhargh/klogg-io?svg=true)](https://ci.appveyor.com/project/xhargh/klogg-io) | [![Build Status](https://travis-ci.org/xhargh/klogg-io.svg?branch=master)](https://travis-ci.org/xhargh/klogg-io) | [![Build Status](https://travis-ci.org/xhargh/klogg-io.svg?branch=master)](https://travis-ci.org/xhargh/klogg-io) | ![GH Smoke](https://github.com/xhargh/klogg-io/workflows/CI:%20Build%20Test/badge.svg?branch=master)
| Artifacts | [continuous-win](https://github.com/xhargh/klogg-io/releases/tag/continuous-win) | [continuous-linux](https://github.com/xhargh/klogg-io/releases/tag/continuous-linux) | [continuous-osx](https://github.com/xhargh/klogg-io/releases/tag/continuous-osx) | n/a |

<!--
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f6db6ef0be3a4a5abff94111a5291c45)](https://www.codacy.com/manual/variar/klogg?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=variar/klogg&amp;utm_campaign=Badge_Grade)
-->

## Table of Contents

1. [About the Project](#about-the-project)
1. [Building](#building)
1. [How to Get Help](#how-to-get-help)
1. [Contributing](#contributing)
1. [License](#license)
1. [Authors](#authors)

## About the Project

klogg-io started as a fork of [klogg](https://github.com/variar/klogg).

Klogg started as a fork of [glogg](https://github.com/nickbnf/glogg) - the fast, smart log explorer in 2016.

Since then it has evolved from fixing small annoying bugs to rewriting core components to
make it faster and smarter that predecessor.

Development of klogg is driven by features my colleagues and I need
to stay productive as well as feature requests from users on Github and in glogg mailing list.

Latest news about klogg development can be found at https://klogg.filimonov.dev.

### Common features of klogg and glogg
* Runs on Unix-like systems, Windows and Mac thanks to Qt5
* Is fast and reads the file directly from disk, without loading it into memory
* Can operate on huge text files (10+ Gb is not a problem)
* Search results are displayed separately from original file
* Supports Perl-compatible regular expressions
* Colorizes the log and search results
* Displays a context view of where in the log the lines of interest are
* Watches for file changes on disk and reloads it (kind of like tail)
* Is open source, released under the GPL

### Features of klogg
* Multithreading support for file indexing and regular expression matching
* Log encoding detection using [uchardet](https://www.freedesktop.org/wiki/Software/uchardet/) library (support utf8, utf16, cp1251 and more)
* Limiting search to a part of open file
* In-memory cache of search results per search pattern
* Support for many common text encodings
* And lots of small features that make life easier (closing tabs, copying file paths, favorite files menu, etc.)

### Features of klogg-io
* Serial port support

**[Back to top](#table-of-contents)**

<!--

## Installation

This project uses [Calendar Versioning](https://calver.org/). For a list of available versions, see the [repository tag list](https://github.com/variar/klogg/tags).

### Current stable release builds

Current release is 20.4. Binaries for all platforms can be downloaded from GitHub releases or Bintray.

[ ![Release](https://img.shields.io/github/v/release/variar/klogg?style=flat)](https://github.com/variar/klogg/releases/tag/v20.4)
[ ![Bintray](https://img.shields.io/bintray/v/variar/generic/klogg?style=flat)](https://bintray.com/variar/generic/klogg/_latestVersion)

Windows installer is also available from Chocolatey:

[ ![Chocolatey](https://img.shields.io/chocolatey/v/klogg?style=flat)](https://chocolatey.org/packages/klogg)

Package for Mac can be installed from Homebrew

[ ![homebrew cask](https://img.shields.io/homebrew/cask/v/klogg?style=flat)](https://formulae.brew.sh/cask/klogg)

Linux packages are also available from DEB and RPM repositories. 

For DEB add Bintray GPG key and repository, then install from apt:
```
apt-key adv --keyserver hkps://keyserver.ubuntu.com --recv-keys 379CE192D401AB61
echo deb https://dl.bintray.com/variar/deb stable utils | sudo tee -a /etc/apt/sources.list
apt-get update
apt-get install klogg
```

For RPM:
```
curl https://bintray.com/variar/rpm/rpm | sudo tee /etc/yum.repos.d/bintray-variar-rpm.repo
yum update
yum install klogg
```

### Tesing builds

![CI Build and Release](https://github.com/variar/klogg/workflows/CI%20Build%20and%20Release/badge.svg)

| Windows x64 | Windows x86 | Linux x64  | Mac x64 |
| ------------- |------------- | ------------- | ------------- |
| [ci-win-x64](https://github.com/variar/klogg/releases/tag/continuous-windows-x64) | [ci-win-x86](https://github.com/variar/klogg/releases/tag/continuous-windows-x86) |  [ci-linux](https://github.com/variar/klogg/releases/tag/continuous-linux-x64) | [ci-mac](https://github.com/variar/klogg/releases/tag/continuous-macos-x64) |

**[Back to top](#table-of-contents)**

-->

## Building

Please review
[BUILD.md](BUILD.md)
for how to setup Klogg on your local machine for development and testing purposes.

## How to Get Help

First, please refer to the
[documentation](DOCUMENTATION.md)
page.

You can open issues using [klogg issues page](https://github.com/xhargh/klogg-io/issues)
or post questions to glogg development [mailing list](http://groups.google.co.uk/group/glogg-devel).

## Contributing

We encourage public contributions! Please review [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct and development process.

## License

This project is licensed under the GPLv3 or later - see [COPYING](COPYING) file for details.

## Authors

* **[Gustav Andersson](https://github.com/xhargh)**
* **[Anton Filimonov](https://github.com/variar)**
* *Initial work* - **[Nicolas Bonnefon](https://github.com/nickbnf)**

See also the list of [contributors](https://klogg.filimonov.dev/docs/getting_involved/#contributors) who participated in this project.

**[Back to top](#table-of-contents)**
