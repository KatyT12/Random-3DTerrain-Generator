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
            unsigned int pointOffset;
            if(!autoOffset)
            {
                GLCall(glBindBufferRange(GL_UNIFORM_BUFFER,point,m_rendererID,offset,size));
                pointOffset = offset;
            }
            else
            {
                GLCall(glBindBufferRange(GL_UNIFORM_BUFFER,point,m_rendererID,m_offset,size));
                pointOffset = m_offset;
            }
            
            Point p;
            p.offset = pointOffset;
            p.point = point;
            p.size = size;
            m_points.push_back(p);
            m_offset += size;
        }

        //The name for the uniform block in the shaders must be the same for this function
        void BindShaders(std::vector<uint32_t> shaderid, unsigned int point, std::string& name,uint32_t size)
        {
            unsigned int shaderCount = shaderid.size();
            for(uint32_t i=0; i<shaderCount;i++)
            {
                uint32_t shaderIndex = glGetUniformBlockIndex(shaderid[i], name.c_str());
                GLCall(glUniformBlockBinding(shaderid[i],shaderIndex,point));
            }
            BindRange(point,size);   
        }

        const Point GetPoint(const unsigned int bindingPoint) const
        {
            uint32_t index = -1;
            for(uint32_t i=0; i<m_points.size();i++)
            {
                if(m_points[i].point == bindingPoint)
                {
                    index = i;
                }
            }
            if(index != -1)
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