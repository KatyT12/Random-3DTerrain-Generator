#pragma once

#include "renderer.h"
#include <vector>


class Texture
{
    private:
        unsigned int m_rendererID;
        std::string m_FilePath;
        unsigned char* m_LocalBuffer;
        //bpp is bits per pixel
        int m_width, m_height, m_bpp;
    public:
        Texture(const std::string& path,GLenum wrapOption=GL_REPEAT,std::string type="",std::vector<std::string> texture_faces = {});
        Texture();
        void InitTexture(const std::string& path ="",GLenum wrapOption=GL_REPEAT,std::string type="",std::vector<std::string> texture_faces = {});
        
        ~Texture();

        void Bind(unsigned int slot=0) const;
        void UnBind() const;
        
        void BindCubeMap(unsigned int slot=0) const;
        void UnBindCubeMap() const;

        inline int getWidth() const {return m_width;}
        inline int getHeight() const {return m_height;}
        inline unsigned int getID() const {return m_rendererID;}

        inline const void* getBufferPointer() const {return &m_LocalBuffer; }


};