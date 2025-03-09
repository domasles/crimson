# Crimson
A 2D top-down action game about saving the planet.

## Installation:
### Clone the repository:
Providing you have ```git``` installed, you can clone this repository through either **HTTPS**:
```bash
git clone --recursive https://github.com/domasles/crimson.git
```

or **SSH**:
```bash
git clone --recursive git@github.com:domasles/crimson.git
```

Either way will clone everything you need, and you'll be able to build the game.

**IMPORTANT**: This project relies on submodules, so in case you forgot the ```--recursive``` flag, you can run the following:
```bash
git submodule update --init
```

## Building:
Currently, this project *officially* supports only Windows and Linux on x86_64 processors, so any other platforms might require additional tinkering as they're not guaranteed to work out of the box.

For project generation [Premake](https://premake.github.io) is being used, but editing the configuration isn't recommended unless you know what you're doing.

### Windows:
This Project is in active development using [Visual Studio 2022](https://visualstudio.microsoft.com). Other versions aren't tested, so proceed at your own risk. Some of the modern ```C++``` features may not be available using older compilers.

Upon cloning the repository, you should see ```GenerateProjects-Windows.bat``` at the root of the project. Run it to see ```crimson.sln``` appear.

Open it up and hit ```F5``` to build. A new window with the built executable will pop up and you'll be able to play the latest commit of this game.

### Linux:
Linux requires a little more setup in order to link this project properly. The only platform put to the test was [Ubuntu](https://ubuntu.com) and its ```apt``` package manager. On different package managers you might need to look up some of the packages' names.

Start by updating and upgrading your system:
```bash
sudo apt update && sudo apt upgrade -y
```

Next, install these packages:
```bash
sudo apt install build-essential libasound2t64 libxss1 libpulse0 libdecor-0-0 libwebpdemux2
```

Lastly, navigate to the root of the project, run ```GenerateProjects-Linux.sh``` and ```make```.

## Finding the builds:
All builds wll be under ```bin/configuration-platform-architecture```, where you'll find built binaries alongside with the game data and assets.

## Dependencies:
[json](https://github.com/nlohmann/json)

[Premake](https://github.com/premake/premake-core)

[SDL3](https://github.com/libsdl-org/SDL)

[SDL3_image](https://github.com/libsdl-org/SDL_image)

**Happy playing!**
