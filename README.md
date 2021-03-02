This is a program that generates a random terrain. random_terrain.h and random_terrain.cpp are doing most of this.

You create a terrain in the main program by calling the constructor and providing the location of a json file or none if you would like the default settings

Then call .init() on the object and that will generate the vertex buffer and index buffer and do some other things depending on the configuration. To Draw just call .Draw(). You will have to set uniforms for the shader before drawing however (which is why the shader member in random_terrain.h is public) because I haven't implemented a way for random_terrain.cpp to set the uniforms itself. 

I have tried to make it quite easy to configure since for most changes you can simply edit the .json file without having to build the program again. Note that if you set the dimensions of the terrain to be small and give a large number of octaves you will get a floating point error

The largest I have made the terrain is 10000 * 10000 vertices which does work however it takes about half a minute for it to load everything in however it could be longer or shorter depending on how powerful your computer is.

Also note that when the bias is low (terrain is more spiky) collision detection does not work very well unless you lower the speed. You could instead increase collisionOffset in the primitive sub object in the json file. collisionOffset is the maximum offset from the terrains actual height (both above and below) that the camera needs to be at for the cameras position in world space to be clamped so it does not go through the terrain. For spikier terrains increasing this should improver collision detection.

## Building the program on linux

```
$ cmake .

$ make
```
Wherever you build it to make sure you run ./main in the directory the source file (main.cpp) is

The CMakeLists.txt file queries for the file /etc/arch-release to determine if the distrobution of the os is based on Arch and change some of the headers/libraries accordingly because some vary with distro

Of course make sure you have all the necessary libraries/headers.
This includes:

    . jsoncpp
    . glfw3 (or glfw)
    . glew
    . assimp 

And some other libraries but you are likely to already have them. All of the libraries used are listed in CMakeLists.txt

## Configuration

if you run gui.py in pyqt5-gui you can configure the program and run it from the gui


To give your own configuration for the terrain you need to provide your own .json file which you give the path of to the construcor
of Terrain.

test.json is an example of a configuration file and it includes all the options available but it is not necessary to set every option.

Some options have to be included in your json file for the program to work.
These options are:
    
    . bias
    . octaves
    . seed 
    . x (The width of the terrain)
    . y (The lenght of the terrain)
    . offset (space between vertices)

All the other options do not have to be set for the program to work as they will be given the defaults which you can see in random_terrain.h

You must make sure that your configuration matches for example if you set texture to true and provide a texure make sure the shader you give also accepts a texture.

The primitive option takes in a string. the string should be exactly like the openGL macros for primitives eg. for triangles "GL_TRIANGLES" also know that this primitive determines the default draw primitive (although you can set it as a parameter in Draw() and more importantly it determines what is in the index buffer so if you specify "GL_POINTS" there will be nothing in the index buffer since it is not necesesary if you are only drawing points, if you want to draw both points and triangles set this option to GL_TRIANGLES and pass GL_POINTS to .Draw() when you want to draw points.

There is an option for both the terrain shader and tree shader to have the projection and view matrix be provided with a uniform buffer, if you set these to true you must provide
shaders that allow this, see res/shaders/tree.shader and res/shaders/terrainDirectionalLighting.shader which are shaders that can work with the uniform buffer

## Examples
![alt text](https://github.com/open-source-KT/Random-3DTerrain-Generator/blob/master/res/examples/example8.png?raw=true)
![alt text](https://github.com/open-source-KT/Random-3DTerrain-Generator/blob/master/res/examples/example9.png?raw=true)
![alt text](https://github.com/open-source-KT/Random-3DTerrain-Generator/blob/master/res/examples/example10.png?raw=true)


## TODO:
    . Lighting configuration options



