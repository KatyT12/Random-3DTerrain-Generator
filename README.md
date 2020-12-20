This is a program that generates a random terrain. random_terrain.h and random_terrain.cpp are doing most of this.

You create a terrain in the main program by calling the constructor and providing the location of a json file or none if you would like the default settings

Then call .init() on the object and that will generate the vertex buffer and index buffer and do some other things depending on the configuration. To Draw just call .Draw(). You will have to set uniforms for the shader before drawing however (which is why the shader member in random_terrain.h is public) because I haven't implemented a way for random_terrain.cpp to set the uniforms itself. 

I have tried to make it quite easy to configure since for most changes you can simply edit the .json file without having to build the program again. Note that if you set the dimensions of the terrain to be small and give a large number of octaves you will get a floating point error

The largest I have made the terrain is 10000 * 10000 vertices which does work however it takes about half a minuete for it to load everything in

Also note that when vertices are very cramped together in the terrain the collision detection does not work very well unless you lower the speed

## Building the program

```
$ cmake .

$ make
```
Wherever you build it to make sure you run ./main in the directory the source file (main.cpp) is

However note that if you are using Ubuntu you will likely have to remove some of the libraries included or change the name of some of them such as glfw3 to glfw.

There may also be some errors depending on your compiler. There were a few errors I encountered when building this program on a different device where I had to change some of the source code in randome_terrain.cpp to be more complient with the compiler

You may have to change the include path in random_terrain.h from json/json.h to jsoncpp/json/json.h

If you are using something other than the GNU g++ compiler you will have to change the ASSERT macro to whatever the assert function is with your compiler 


Of course make sure you have all the necessary libraries/headers.
This includes:


    . jsoncpp
    . glfw3 (or glfw)
    . glew
    . assimp (possibly in the future for model loading)

And likely some other libraries I am forgetting


## Configuration

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

the trees options should be true or false (default is false) and if set to true it will also draw trees on the terrain randomly. This can also
be configured in the grid option.


You must make sure that your configuration matches for example if you set texture to true and provide a texure make sure the shader you give also accepts a texture.

When making the vertex buffer for the terrain it sets the texture coordinates or colors (depending on configuration) as the second vertex attribute so the shaders you use must accept
them as index 1. I plan to also have these as options in the config. There is an option in the shaderConfig called textureUniformName which you should set the uniform name to match the one in the shader


Note that the main.cpp file is responsible for setting most of the uniforms with the exception of the texture of the terrain (if there is one),
textures and model matrices of the trees if trees is set to true.



## TODO:
    . Lighting configuration options
    . Work better with textures
    . Have an options for a normals vertex attribute



