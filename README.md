# CannonballX
This is a port of the Cannonball - OutRun Engine to the original XBox
## TO-DO
- Implement saving and loading from config.xml
- Better error handling in case of missing files
- Controller remapping support
- Figure out why triggers are seen as digital buttons
- Port custom music feature
- Code cleanup
## Port Info
- Based on the source code of the Cannonball - OutRun Engine by djyt
- Links:
  - [Original Source](https://github.com/djyt/cannonball)
  - [Reassembler Blog](http://reassembler.blogspot.co.uk/)
# Getting Started
- Download the latest precompiled binaries from the [release](https://github.com/kfkos/CannonballX/releases) section
- Copy the contents to your XBox
- Copy the OutRun revision B romset to the roms directory
- Launch the game!
# Building from source
## Dependencies
- [libSDL2x](https://github.com/marty28/libSDL2x) (ported by marty28)
- Boost v1.41.0
## Compiling
- Note: Visual Studio .NET 2003 is required
- Place dependencies in ./libs folder (check paths in project settings)
- After compiling, copy the res folder and your romset over to the XBox
- You can now run and debug the game as expected
