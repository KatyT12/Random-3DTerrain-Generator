#pragma once
#include "json_parser.h"
#include "renderer.h"
#include <vector>
#include "Texture.h"
#include "Random.h"
#include "shader.h"

struct Config
{
    int octaves = 7;
    float bias = 1.8;
    int seed = 423;

    int x = 256;
    int y = 256;
    float height = 10.0f;
    
    float posX = 0.0f;
    float posY = 0.0f;
    float offset = 0.2f;

    bool staticColor = false;
    bool texture = false;
    std::string textureLocation = "";
    int textureSlot = 0;

    std::vector<float> color1 = {0.317,0.149,0.149};
    std::vector<float> color2 = {0.239,0.576,0};

    //This shader doesn't take in texture coordinates it just takes colors. for textures change to a shader such as basic.shader or your own shader
    std::string shaderLocation = "res/shader/color2.shader"; 
};  

class Terrain
{
    public:
        Terrain(std::string config_file = "");
        ~Terrain();
        
        void init(); 
        
        inline int getVao() const {return vao;}
        inline int getIb() const {return ib;}

        void Draw(GLenum primitive = GL_TRIANGLES);

        float getTerrainHeight(float x, float y);

        float length;
        float width;
        Texture terrainTexture;
        shader terrainShader;

    private:
        void read_config_file(std::string& name);
        float interpolateFloat(float color1, float color2, float fraction);

        unsigned int vao;
        unsigned int vbo;
        unsigned int ib;

        float barryCentric(std::vector<float> p1, std::vector<float> p2, std::vector<float> p3, std::vector<float> pos) const;
        

        void determineColAttrib(float *& buffer,int place);
        void determineTexAttrib(float *& buffer,int x, int y, int place);



        Json::Value* configuration;
        Config config_struct;
        float* height_map;

        


};