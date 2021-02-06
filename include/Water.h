#pragma once

#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <vector>
#include "renderer.h"

#define FBODRAW false

struct FBO
{
    unsigned int frameBuffer;
    unsigned int colorAttachment;
    unsigned int depthAttachment;
};

class Water{
    public:
        inline Water(){}
        Water(int resolution,float length);        
        ~Water();

        void Draw();
        void setShader(std::string path);   
        void genBuffer();

        shader waterShader;
        glm::vec3 waterColor;

        inline void setHeight(float newHeight){height=newHeight;}
        inline float getHeight(){return height;}

        void createFrameBuffer(int width,int height,int index);
        void bindFrameBuffer(int index);
        void unbindFrameBuffer();
        void bindTextureAttachment(int index, int slot = 0);
        
    private:
        int fboWidth;
        int fboHeight;

        int resolution;
        float length;
        float offsetBtwn;
        glm::vec3 pos;
        glm::mat4 model;
        float height;

        void fillVertex(float*& buffer,int place,int x, int z);
        float scaleComponent(float c);
        unsigned int vao;
        unsigned int vbo;

        /*Depth buffer of reflection: color-texture depth-render buffer, refraction buffer: color-texture depth-texture*/

        void createAttachments(int width,int height,FBO& f);
        std::vector<FBO> framebuffers;

        glm::mat4 getModel();



};