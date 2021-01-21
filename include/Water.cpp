#include "Water.h"

Water::Water(int resolution, float length)
: resolution(resolution), length(length), vbo(-1),vao(-1)
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
    int stride = 3;

    float* vertices = new float[(resolution-1) * (resolution-1) * 6 * stride];
    for(int x=0; x < resolution-1; x++)
    {
        for(int z=0; z < resolution-1; z++)
        {
            vertices[place] = offsetBtwn*x; 
            vertices[place+1] = 0.0f;
            vertices[place+2] = offsetBtwn * z;
            place += stride;
            vertices[place] = offsetBtwn*x; 
            vertices[place+1] = 0.0f;
            vertices[place+2] = offsetBtwn * (z+1);
            
            place += stride;
            vertices[place] = offsetBtwn*(x+1); 
            vertices[place+1] = 0.0f;
            vertices[place+2] = offsetBtwn * z;
            place += stride;
            

            vertices[place] = offsetBtwn*x; 
            vertices[place+1] = 0.0f;
            vertices[place+2] = offsetBtwn * (z+1);
            place += stride;
            vertices[place] = offsetBtwn*(x+1); 
            vertices[place+1] = 0.0f;
            vertices[place+2] = offsetBtwn * (z+1);
            
            place += stride;
            vertices[place] = offsetBtwn*(x+1); 
            vertices[place+1] = 0.0f;
            vertices[place+2] = offsetBtwn * z;

            place += stride;
            

        }

    }

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,(resolution-1) * (resolution-1) * 6*3*sizeof(float),vertices,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,stride * sizeof(float),(void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    delete vertices;
}

void Water::setShader(std::string path)
{
    waterShader.makeShader(path);
}

Water::~Water()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(1,&vbo);
}


void Water::Draw()
{
    glBindVertexArray(vao);
    waterShader.Bind();

    glDrawArrays(GL_TRIANGLES,0,(resolution-1) * (resolution-1) * 6 );
    glBindVertexArray(0);
}

