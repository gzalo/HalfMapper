# HalfMapper
This project is a renderer designed specifically to explore the world of Half-Life.
It allows for realtime rendering of the Black Mesa Research Facility.

**The main aim is not to make it playable**, just to be able to navigable (noclip). Models, decals, and other objects won't be rendered.

## Download
The latest binary (for Windows) can be downloaded here: http://gzalo.com/halfmapper_en/
**It needs a Half Life installation** (at least the map folder and a couple of wads for textures)
For other platforms it can be compiled after installing the required libraries and doing the required changes

![Isometric support](http://i.imgur.com/ghh8OeT.jpg)
![Lambda Core](http://i.imgur.com/y5xzn7Q.png)

Video showing the program http://www.youtube.com/watch?v=Hl2HbV3UbMs

## Used libraries
SDL for window and event management

OpenGL and GLU for rendering

GLEW for easy extension access

## TODO 
Provide a better interface, allow for map position fine-tuning and disable certain maps

Try to add other Black Mesa maps to expand the universe: Opposing Force, Blue Shift and Decay might be possible  

Give a way to automatically take screenshots of the top of the maps, to do easier map joins.

Hard code every missing "info_landmark" matching entities to improve the full map (for instance elevator and "surface tension" pipe -after the dam- don't have a set height)
