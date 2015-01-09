# HalfMapper
This project is a renderer designed specifically to explore the world of Half-Life.
It allows for realtime rendering of the Black Mesa Research Facility.

**The main aim is not to make it playable**, just to be able to navigable (noclip). Models, decals, and other objects won't be rendered.

## Download
The latest binary (for Windows) can be downloaded here: http://gzalo.com/halfmapper_en/
**It needs a Half Life installation** (at least the map folder and a couple of wads for textures)
For other platforms it can be compiled easily after installing the required libraries

![Overlapping maps](http://i.imgur.com/BPebM.jpg)

## Used libraries
SDL for window and event management

OpenGL and GLU for rendering

GLEW for easy extension access

## TODO 
Provide a better configuration file and interface

Provide a better interface, allow for map position fine-tuning and disable certain maps

Solving overlaps: c0a0c folds into itself

Solving overlaps: elevator and "surface tension" pipe (after the dam) don't have a set height

Hard code every missing "info_landmark" entities to improve the full map
