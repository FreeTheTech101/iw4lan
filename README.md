# IW4LAN

[![Build status](https://ci.appveyor.com/api/projects/status/qfcle1nxvwmpbq9i?svg=true)](https://ci.appveyor.com/project/win32re/iw4lan)

Open source IW4 modification project which brings an offline mode to the game.

## Build instructions

> Building with Visual Studio versions older than Visual Studio 2013 is not supported.

### Requirements

* Git
* Microsoft Visual Studio 2013

### Clone the repository

```
git clone https://github.com/twizre/iw4lan.git
cd iw4lan
```

### Generate project files

```
cd iw4lan\deps\tools
premake5 vs2013
```
Or use `createSLN.bat`.

### Build the project

1. Open the project folder and open `iw4cli.sln`.
2. Build the project using Visual Studio 2013.

