# godot_midi_tools

This repository contains a Godot GDExtension plug-in that provides MIDI tools.
It is a work-in-progress, but will eventually provide the following Nodes:
- MTFluidSynthNode: Easy-to-use FluidSynth with InputEventMIDI response
- MTSequencerNode: Sequence FluidSynth & other synths, with MIDI In/Out support
- MTMidiFile and other helper classes to read, write and edit MIDI files

Only MTFluidSynthNode is fully functional, but development is quickly moving ahead.

## Building

The code has been successfully built and tested on Linux and Windows (10 & 11),
using GCC and SCons.  On Windows, the mingw-w64 toolchain provided by
[MSYS2](https://www.msys2.org/) was used.
The repository is set up for easy building in a Linux environment, and includes
the FluidSynth library for that platform.  Building on other platforms will require
some modifications.  Also be aware that linking the Godot GDExtension
bindings will take much longer on Windows when using mingw-w64.

### Tools Needed
- Git
- GCC
- [SCons (requires Python)](https://scons.org/doc/production/HTML/scons-user/ch01s02.html)

### Platform-specific changes
#### Linux
No changes should be necessary in a Linux environment.  If any link errors occur, check
the links in the 'fluidsynth/lib' directory are correct.

#### Windows
- Download a Windows 64-bit [binary release of FluidSynth.](https://github.com/FluidSynth/fluidsynth/releases) _(Tested with 2.4.6)_
- Copy 'libfluidsynth-3.lib' from the 'lib' folder of the archive to the 'fluidsynth/lib' directory of the repository
- Modify the 'SConstruct' file in the root directory of the repository to specify the exact name of the linkable library.
```python
# Replace:
env.Append(LIBS=["fluidsynth"])
# With:
env.Append(LIBS=[":libfluidsynth-3.lib"])
```

#### MacOS
Building on MacOS has not been tested, and could require extensive changes.
Pre-built packages of FluidSynth do not include the 
### Build Steps
- Clone this repository to a local folder
- Open a terminal and cd to the folder
- Populate the 'godot-cpp' submodule by running `git submodule update --init`
- Run the command `scons platform=<platform_name> template=<type>`
  - platforms: linux, windows, macos
  - templates: debug, release   _(Default: debug)_


