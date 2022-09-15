# ARPG Automation Tool
Uses opencv to identify game state then simulates input to automate rolling for desired modifiers.

![Thumbnail](./thumbnail.jpg)

## Info
Simple tool to help with automating modifier rolling for a popular ARPG. It gives the user a GUI with which they can configure the modifiers to search for and which item to roll onto. Once a desired modifier is found, the tool stores the modifier into a compass for use later. The tool uses opencv to detect game state and win32 api to send simulated inputs. The tool does not read the games memory. The tool currently only works for windows platforms as the simulated inputs and screenshots are done through the windows api.

### Features include
* Load custom modlist to use when rolling and storing modifiers. Use copy paste on the item with the modifier you want to get exact wording.
* Select which item gets its modifiers rolled.
* Automatically handles inventory.
* Adjust the speed at which the bot simulates inputs. (Experimental)

## How to use

### Using prebuilt
 1. Download the build from the tag, and run the executable. 
 2. If you have a custom modlist, then load that in using the GUI and select which item you want to roll the modifiers on. 
 3. Once everything is set, Start the roller by pressing Cntrl+R or start on the GUI. Tab into the game and press numpad 0 to start. The bot will roll modifiers until it runs out of resources to roll with. User can manually stop by pressing and holding numpad 1.

### Building from source
* Requirement: ` CMake 3.6 or higher `

If you are using MSVC and have visual studio, simply open the root project folder and ninja build configuration will automatically run cmake.
This is the CMakeSettings.json used by visual studio: 
```json
{
  "configurations": [
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "inheritEnvironments": [ "msvc_x86_x64" ],
      "buildRoot": "${projectDir}\\build\\${name}",
      "installRoot": "${projectDir}\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "-v",
      "ctestCommandArgs": ""
    }
  ]
}
```

If you do not have visual studio, then you will need to run cmake manually. 
```console
cmake -B./build/ -S. -DCMAKE_INSTALL_PREFIX=./build/bin
```

* Note: Building from source takes a considerable amount of time. Also, be aware that the locations of the images and default modifier list is relative to my build configuration. If the executable is moved, then the path to the images and defaultModList.txt need to be adjusted in `sextant_roller_constants.h`.

## Disclaimer
I do not condone breaking the terms of service for any games. This is purely for educational purposes and for me to learn about opencv, and automation tools. Use it at your own risk as the developers may flag the executable's signature.
