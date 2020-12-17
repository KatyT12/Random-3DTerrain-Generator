#pragma once

#include "renderer.h"
#include <vector>
#include <iostream>


struct Point
{
    unsigned int offset;
    unsigned int point;
    unsigned int size;
};



class UniformBuffer
{
    private:
        unsigned int m_rendererID;
        unsigned int m_offset = 0;
        unsigned int m_size;
        std::vector<Point> m_points;
    public:
        UniformBuffer(unsigned int size)
        :m_size(size)
        {
            
            GLCall(glGenBuffers(1,&m_rendererID));
            GLCall(glBindBuffer(GL_UNIFORM_BUFFER,m_rendererID));

            GLCall(glBufferData(GL_UNIFORM_BUFFER,size,NULL,GL_STATIC_DRAW));
            GLCall(glBindBuffer(GL_UNIFORM_BUFFER,0));
        }

        ~UniformBuffer()
        {
            GLCall(glDeleteBuffers(1,&m_rendererID));
        }

        void BindRange(unsigned int point,unsigned int size, bool autoOffset=true,unsigned int offset=0)
        {
            if(!autoOffset)
            {
                GLCall(glBindBufferRange(GL_UNIFORM_BUFFER,point,offset,m_rendererID,size));
                 m_points.push_back({offset,point,size});
            }
            else
            {
                GLCall(glBindBufferRange(GL_UNIFORM_BUFFER,point,m_rendererID,m_offset,size));
                
                m_points.push_back({m_offset,point,size});
            }
            m_offset += size;
           
        
        }

        //The name for the uniform block in the shaders must be the same for this function
        void BindShaders(uint32_t shaderid[],uint32_t shaderCount, unsigned int point, std::string& name,uint32_t size)
        {
                        
            for(uint32_t i=0; i<shaderCount;i++)
            {
                uint32_t shaderIndex = glGetUniformBlockIndex(shaderid[i], name.c_str());
                GLCall(glUniformBlockBinding(shaderid[i],shaderIndex,point));
            }
            
            BindRange(point,size);   
        }

        const Point GetPoint(const unsigned int bindingPoint) const
        {
            uint32_t index;
            for(uint32_t i=0; i<sizeof(m_points);i++)
            {
                if(m_points[i].point == bindingPoint)
                {
                    index = i;
                }
            }
            if(index)
            {
              return m_points[index];  
            }
            else
            {
                assert(false);
            }
            
        }


        void UpdateBufferPoint(uint32_t point,void* data,uint32_t dataSize,uint32_t suboffset=0)
        {
            GLCall(glBindBuffer(GL_UNIFORM_BUFFER,m_rendererID));
            const Point &block = GetPoint(point);
            GLCall(glBufferSubData(GL_UNIFORM_BUFFER,block.offset + suboffset,dataSize,data));
            GLCall(glBindBuffer(GL_UNIFORM_BUFFER,0));
        }
     



};