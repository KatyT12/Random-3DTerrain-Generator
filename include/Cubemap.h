#pragma once

#include "renderer.h"
#include <vector>
#include "shader.h"

class Cubemap
{
    private:
        unsigned int m_rendererID;
        std::string m_FilePath;
        unsigned char* m_LocalBuffer;
        //bpp is bits per pixel
        int m_width, m_height, m_bpp;
    public:
        Cubemap(GLenum type,int width,int height,std::vector<std::string> texture_faces={},GLenum wrapOption=GL_REPEAT,float* border = nullptr);
        ~Cubemap();
        void initBuffer();

        void Bind(unsigned int slot=0) const;
        void UnBind() const;

        inline int getWidth() const {return m_width;}
        inline int getHeight() const {return m_height;}
        inline unsigned int getID() const {return m_rendererID;}

        inline const void* getBufferPointer() const {return &m_LocalBuffer; }

        shader cubeMapShader;
        unsigned int cubeMapIB;
        unsigned int cubeMapVBO;
        unsigned int cubeMapVAO;



};