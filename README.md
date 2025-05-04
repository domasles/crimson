# Crimson
A 2D top-down action game about saving the planet from a deadly virus that plagues both people and environment surrounding them, resulting in visual artifacts as you go along.
This game comes with a simple to use and expand 2D game engine made using primarily [SDL3]("https://github.com/libsdl-org/SDL") and [LDTk]("https://github.com/deepnight/ldtk") as a level editor for extra moddability and simplicity creating new games in mind.
The engine comes in a format of a ready to use C++ library and is still considered a WIP project.

## Installation:
### Clone the repository:
Providing you have ```git``` installed, you can clone this repository through **HTTPS**:
```bash
git clone --recursive https://github.com/domasles/crimson.git
```
**IMPORTANT**: This project relies on git's submodules, so in case you forgot the ```--recursive``` flag, you can run the following:
```bash
git submodule update --init
```
to re-download or update required submodules.

## Building:
Currently, this project *officially* supports only Windows and Linux on x86_64 processors, so any other platform or architecture might require additional tinkering as they're not guaranteed to work out of the box. This may include subtle changes in game's code and replacement of pre-compiled libraries coming alongside this engine to work. As this project grows, it will support more and more platforms and architectures, but for now you'll have to wait until it's in a state ready for game development.

For project generation [Premake](https://premake.github.io) is being used, but editing the configuration isn't recommended unless you know what you're doing.

### Windows:
This Project is under active development using [Visual Studio 2022](https://visualstudio.microsoft.com). Other versions aren't tested, so proceed at your own risk. Some of the modern ```C++``` features may not be available using older compilers with a different set of the standard library.

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
All builds wll be under ```bin/configuration-platform-architecture```, where you'll find the built binary of the launcher alongside with the libraries copied over and both game data and assets in a separate directory under ```games/```. In the future this engine will support several different games to use the same launcher binary and a select screen, allowing to change installed games, making the possibility of addons for existing games and/or new ones using the same launcher and libraries to save space.

## Dependencies:
[json](https://github.com/nlohmann/json)

[Premake](https://github.com/premake/premake-core)

[SDL3](https://github.com/libsdl-org/SDL)

[SDL3_image](https://github.com/libsdl-org/SDL_image)

**Happy playing!**
