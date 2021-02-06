#include "Cubemap.h"
#include "../vender/stb_image/stb_image.h"
#include <iostream>



Cubemap::Cubemap(GLenum type, int width, int height, std::vector<std::string> texture_faces,GLenum wrapOption, float* border)
: m_rendererID(0),m_LocalBuffer(nullptr),m_width(width),m_height(height),m_bpp(0)
{
        GLCall(glGenTextures(1,&m_rendererID));

        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,m_rendererID));

        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR)); 
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapOption);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapOption);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapOption);


        if(wrapOption == GL_CLAMP_TO_BORDER)
        {
            glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,border);
        }

        if(texture_faces.size() > 0)
        {
            for(unsigned int i=0;i<texture_faces.size();i++)
            {
                m_LocalBuffer = stbi_load(texture_faces[i].c_str(),&m_width,&m_height,&m_bpp,0);
                GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer));
            }
        }
        else{
            for(unsigned int i =0; i < 6;i++)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, type, m_width,m_height, 0, type, GL_FLOAT, NULL);
            }
        }

        if(m_LocalBuffer) 
        {
          stbi_image_free(m_LocalBuffer);
        }
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,0));

}



Cubemap::~Cubemap()
{
    GLCall(glDeleteTextures(1,&m_rendererID));
    glDeleteVertexArrays(1,&cubeMapVAO);
    glDeleteBuffers(1,&cubeMapVBO);
    glDeleteBuffers(1,&cubeMapIB);

}


void Cubemap::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0+slot));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,m_rendererID));
}

void Cubemap::UnBind() const
{
     GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,0)); 
}

void Cubemap::initBuffer()
{
    cubeMapShader.makeShader("res/shaders/cubemap.shader");
    float positions[] = {
    
	  -1.0f,-1.0f,-1.0f, //bottom left
        1.0f,-1.0f,-1.0f, //bottom right
        1.0f,1.0f,-1.0f,  //top right
       -1.0f,1.0f,-1.0f , //top left

        //Left square
        -1.0f,-1.0f,-1.0f, //bottom left
        -1.0f,-1.0f,1.0f,  //bottom right
        -1.0f,1.0f,1.0f,   //top right
        -1.0f,1.0f,-1.0f,   //top left

        //right square
        1.0f,-1.0f,1.0f, //bottom left
        1.0f,-1.0f,-1.0f, //bottom right
        1.0f, 1.0f, -1.0f, //top right
        1.0f, 1.0f, 1.0f, //top left

        //front square
        -1.0f,-1.0f,1.0f, //bottom left
        1.0f,-1.0f, 1.0f, //bottom right
        1.0f,1.0f,1.0f,   //top right
        -1.0f,1.0f,1.0f,  //top left

        //top square
        -1.0f,1.0f,1.0f, //bottom left
        1.0f,1.0f,1.0f, //bottom right
        1.0f,1.0f,-1.0f, //top right
        -1.0f,1.0f,-1.0f,

        //bottom square
        -1.0f,-1.0f,-1.0f, //bottom left
        1.0f,-1.0f,-1.0f, //bottom right
        1.0f,-1.0f,1.0f,  //top right
        -1.0f, -1.0f, 1.0f //top left
    };

    unsigned int skyboxIndices[] = {
        2,3,0,
        0,1,2,

        6,7,4,
        4,5,6,

        10,11,8,
        8,9,10,

        14,15,12,
        12,13,14,

        18,19,16,
        16,17,18,

        22,23,20,
        20,21,22
    }; 
    
    GLCall(glGenVertexArrays(1,&cubeMapVAO));
    GLCall(glGenBuffers(1,&cubeMapVBO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,cubeMapVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(positions),&positions[0],GL_STATIC_DRAW));
    GLCall(glBindVertexArray(cubeMapVAO));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0));

    GLCall(glGenBuffers(1,&cubeMapIB));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,cubeMapIB));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * 6 * sizeof(unsigned int),&skyboxIndices[0],GL_STATIC_DRAW));

    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));

}