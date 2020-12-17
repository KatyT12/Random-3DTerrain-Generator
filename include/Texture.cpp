#include "Texture.h"
#include "../vender/stb_image/stb_image.h"
#include <iostream>


Texture::Texture(const std::string& path,GLenum wrapOption, std::string type,std::vector<std::string> texture_faces)
: m_rendererID(0),m_FilePath(path),m_LocalBuffer(nullptr),m_width(0),m_height(0),m_bpp(0)
{
    InitTexture(path,wrapOption,type,texture_faces);
}

Texture::Texture()
: m_rendererID(0),m_LocalBuffer(nullptr),m_width(0),m_height(0),m_bpp(0)
{

}

void Texture::InitTexture(const std::string& path,GLenum wrapOption, std::string type,std::vector<std::string> texture_faces)
{
    m_FilePath = path;

    if(type == "cube_map")
    {   
        stbi_set_flip_vertically_on_load(0);

        GLCall(glGenTextures(1,&m_rendererID));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,m_rendererID));

        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));  

        for(unsigned int i=0;i<texture_faces.size();i++)
        {
            m_LocalBuffer = stbi_load(texture_faces[i].c_str(),&m_width,&m_height,&m_bpp,0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer);

        }
       
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,0)); //unbind texture
        
        if(m_LocalBuffer) 
        {
        stbi_image_free(m_LocalBuffer);
        }

        
        return;

    }


    if(path == "")
    { 

        GLCall(glGenTextures(1,&m_rendererID));
         GLCall(glBindTexture(GL_TEXTURE_2D,m_rendererID));

        GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);
        if(type == "color")
        {
           
            GLCall(glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,960,540,0,GL_RGB,GL_UNSIGNED_BYTE,NULL));
        
        }
       if(type == "depth-stencil")
       {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,960,540,0,GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8,NULL));
       }

        GLCall(glBindTexture(GL_TEXTURE_2D,0)); //unbind texture
       return;
    }


    stbi_set_flip_vertically_on_load(1);
    //Loads file with given file name. Will put correct details in the given pointers. The last argument is channels which is 4 because we want rgba
    m_LocalBuffer = stbi_load(path.c_str(),&m_width,&m_height,&m_bpp,4);

    GLCall(glGenTextures(1,&m_rendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D,m_rendererID));

    //set texture parameters.
    
    
    GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
    /* Specify magnification filter which is if we actually kind of render our texture on an area that is larger than the texture*/
    GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
    /* Specify the wrap mode. We want it to clamp and not extent the area. NOTE s and t is like x and y but for textures*/
    /*NOTE this was changed from clamp to edge */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);

    GLCall(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_width,m_height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_LocalBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D,0)); //unbind texture

    if(m_LocalBuffer) 
    {
        stbi_image_free(m_LocalBuffer);
    }
    
}




Texture::~Texture()
{
    GLCall(glDeleteTextures(1,&m_rendererID));

}


void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0+slot));
    GLCall(glBindTexture(GL_TEXTURE_2D,m_rendererID));
}

void Texture::UnBind() const
{
     GLCall(glBindTexture(GL_TEXTURE_2D,0)); 
}


void Texture::BindCubeMap(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0+slot));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,m_rendererID));
}

void Texture::UnBindCubeMap() const
{
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP,0));
}

