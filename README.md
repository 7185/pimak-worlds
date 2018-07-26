# Pimak Worlds
Pimak Worlds is a project about creating a 3D virtual world and stuff.

[![Build Status](https://travis-ci.org/7185/pimak-worlds.png?branch=master)](https://travis-ci.org/7185/pimak-worlds)

## Dependencies
* qt
* ogre
* boost

## Building
### Linux
* Install qbs
* Run the following commands
```
qbs setup-toolchains --detect
qbs setup-qt --detect
qbs config defaultProfile <your-qt-version>
qbs build -d build/
```
* For the client, you'll have to move the `pwclient` executable from `build/default/install-root` to the `client` directory

### Windows
Sadly windows building isn't maintained anymore.


## Scripting
This project includes a python library to access the PW protocol without GUI. An example can be found in `bobinot.py`.

