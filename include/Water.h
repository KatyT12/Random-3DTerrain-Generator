#pragma once

#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>


class Water{
    public:
        Water(int resolution,float length);        
        ~Water();

        void Draw();
        void setShader(std::string path);   
        void genBuffer();

        shader waterShader;


    private:
        int resolution;
        float length;
        float offsetBtwn;
        glm::vec3 pos;
        glm::mat4 model;

        float scaleComponent(float c);
        unsigned int vao;
        unsigned int vbo;



};