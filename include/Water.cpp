#include "Water.h"

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