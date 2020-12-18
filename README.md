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

## TODO:
    . Better collisiom detection
    . Lighting configuration options
    . Work better with textures
    . Have an options for a normals vertex attribute
