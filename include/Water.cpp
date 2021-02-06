#include "Water.h"
#include <iostream>

Water::Water(int resolution, float length)
: resolution(resolution), length(length), vbo(-1),vao(-1),waterColor(glm::vec3(0.0f,0.0f,1.0f))
{
    if(resolution < 2)
    {
        resolution = 2;
    }

    offsetBtwn = 1/((float)resolution-1);

   

}

void Water::genBuffer()
{
    int place = 0;
    int stride = 6;

    float* vertices = new float[(resolution-1) * (resolution-1) * 6 * stride];
    for(int x=0; x < resolution-1; x++)
    {
        for(int z=0; z < resolution-1; z++)
        {
            fillVertex(vertices,place,x,z);
            place += stride;
            fillVertex(vertices,place,x,z+1);
            place += stride;
            fillVertex(vertices,place,x+1,z);
            place += stride;
            fillVertex(vertices,place,x,z+1);
            place += stride;
            fillVertex(vertices,place,x+1,z+1);
            place += stride;
            fillVertex(vertices,place,x+1,z);
            place += stride;
        }

    }

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,(resolution-1) * (resolution-1) * 6*stride*sizeof(float),vertices,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,stride * sizeof(float),(void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,stride * sizeof(float),(void*)(3 * sizeof(float)));


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    delete vertices;
    

    createFrameBuffer(DISPLAY_WIDTH,DISPLAY_HEIGHT,0);
    createFrameBuffer(DISPLAY_WIDTH,DISPLAY_HEIGHT,1);
}

void Water::fillVertex(float*& buffer,int place, int x, int z)
{
    buffer[place] = offsetBtwn*x; 
    buffer[place+1] = 0.0f;
    buffer[place+2] = offsetBtwn * z;

    buffer[place+3] = waterColor.x; 
    buffer[place+4] = waterColor.y;
    buffer[place+5] = waterColor.z;
}

void Water::setShader(std::string path)
{
    waterShader.makeShader(path);
    waterShader.proj_and_view_ubo = true;
}

Water::~Water()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(1,&vbo);

    for(int i =0;i<framebuffers.size();i++)
    {
        glDeleteFramebuffers(1,&framebuffers[i].frameBuffer);
        glDeleteTextures(1,&framebuffers[i].colorAttachment);
        if(i == 0)
        {
            glDeleteRenderbuffers(1,&framebuffers[i].depthAttachment);
        }
        else{
            glDeleteTextures(1,&framebuffers[i].depthAttachment);
        }
    }

}


void Water::Draw()
{
    glBindVertexArray(vao);
    waterShader.Bind();
    waterShader.setUniformMat4f("model",getModel());
    glDrawArrays(GL_TRIANGLES,0,(resolution-1) * (resolution-1) * 6 );
    glBindVertexArray(0);
}

glm::mat4 Water::getModel()
{
    glm::mat4 matrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,height,0.0f));
    matrix = glm::scale(matrix,glm::vec3(length,0.0f,-length));
    return matrix;
}

void Water::createFrameBuffer(int width,int height,int index)
{   
    fboWidth = width;
    fboHeight = height;

    FBO f;
    unsigned int fboId;


    GLCall(glGenFramebuffers(1,&f.frameBuffer));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER,f.frameBuffer));
    createAttachments(width,height,f);
    GLCall(glDrawBuffer(GL_COLOR_ATTACHMENT0));

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
         std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    framebuffers.push_back(f);

}

void Water::createAttachments(int width,int height,FBO& f)
{
    GLCall(glGenTextures(1,&f.colorAttachment));
    GLCall(glBindTexture(GL_TEXTURE_2D,f.colorAttachment));
    GLCall(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr));
    GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,f.colorAttachment,0));


    if(framebuffers.size() == 0)
    {
        GLCall(glGenRenderbuffers(1,&f.depthAttachment));
        GLCall(glBindRenderbuffer(GL_RENDERBUFFER,f.depthAttachment));
        GLCall(glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,width,height));
        GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,f.depthAttachment));
        GLCall(glBindRenderbuffer(GL_RENDERBUFFER,0))
    }
    else{
        GLCall(glGenTextures(1,&f.depthAttachment));
        GLCall(glBindTexture(GL_TEXTURE_2D,f.depthAttachment));
        GLCall(glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,width,height,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,nullptr));
        GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,f.depthAttachment,0));
    }
   
    GLCall(glBindTexture(GL_TEXTURE_2D,0));
}
        


void Water::bindFrameBuffer(int index)
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER,framebuffers[index].frameBuffer));
    GLCall(glViewport(0,0,fboWidth,fboHeight));
}

void Water::unbindFrameBuffer()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER,0));
    GLCall(glViewport(0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT));
}


void Water::bindTextureAttachment(int index,int slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0+slot));
    GLCall(glBindTexture(GL_TEXTURE_2D,framebuffers[index].colorAttachment));
}

