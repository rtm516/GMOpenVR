# Garry's Mod OpenVR
#### Garrysmod Virtual Reality
###### Ryan Milner

Uses a C++ module to connect OpenVR methods with Garry's Mod LUA.

## Features

### Complete
* __(C++)__ Connect with SteamVR and tell if a headset is present
* __(C++)__ Get the position/angle of the HMD

### In Progress
* __(GLUA / C++)__ Rendering in VR without relying on VorpX/Virtual Desktop

### Todo
* __(C++)__ Get the position/angle of the controllers
* __(C++)__ Get the trigger/input state on the controllers
* __(GLUA)__ Mapping buttons to controls
* __(GLUA)__ Teleporting?

## Requirements for usage

* Latest [openvr_api.dll](https://github.com/ValveSoftware/openvr/raw/master/bin/win32/openvr_api.dll)

* [SteamVR](http://store.steampowered.com/steamvr) on steam

## Usage

Download the latest `openvr_api.dll` and place it directly in your Garry's Mod root directory, the same directory that has `hl2.exe` in it. If you dont know where this is you can find it by right clicking on Garry's Mod in Steam, clicking "properties" selecting the "Local Files" tab and clicking "Browse Local Files". This will take you to the Garry's Mod root directory.

## Requirements for development
* Windows (Tested with Windows 10)
* [Visual Studio](https://www.visualstudio.com) With Windows SDK v8.1 or higher (Tested with version 2017)
* [Valve OpenVR SDK](https://github.com/ValveSoftware/openvr) (read instructions before cloning)
* [Premake 5.0+](https://github.com/premake/premake-core/releases) extracted to your _C:\Windows_ folder, path, or the project directory (it will be gitignored)

## Installation for development
1. Clone Repo with a [git CLI](https://git-scm.com/downloads)
	```bash
	$ git clone https://github.com/bizzclaw/gmVR.git
	```

2. Clone the Valve OpenVR sdk to a folder and take note of that folder's location.
3. Create a new file called `"buildconfig.lua"` in the project's main directory with your favorite text editor, fill it with the following:

	*NOTE:* You cannot use backslashes for the path! You'll have to replace any "\\"s with "/"s		

	```LUA
	sdkPaths = "Set this as the path to the folder that contains your cloned OpenVR SDK EG: 'G:/GitHub' where the openvr folder is inside there"
	```

4. Now, simply run `BuildProjects.bat`, the batch file will generate a project folder with a Visual Studio project that can be loaded.

## Compiling and Testing

* When you're ready to compile, build from Visual Studio and move the compiled gmcl_openvr_win32.dll from the `build` folder into your `steam/steamapps/common/garrysmod/garrysmod/lua/bin` folder

___

###### This README and project is a work in progress. Any issues and/or bug fixes are welcome

## Credits
__Datamats__ - Created the original [gmcl_openvr](https://github.com/Datamats/gmcl_openvr) that this is largely based off of and used as reference.

__Joseph (Bizz) Tomlinson__ - Created the [gmVR](https://github.com/bizzclaw/gmVR) that this is based from.

__Ryan (rtm516) Milner__ - Current development, LUA scripting, C++ API.
