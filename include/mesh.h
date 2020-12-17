#pragma once


#include <GL/glew.h>
#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


#include <iostream>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 TexCoords;

};


struct Texturez
{
    unsigned int id;
    std::string type;
    std::string path;

};

struct Material {
glm::vec3 Diffuse;
glm::vec3 Specular;
glm::vec3 Ambient;
float Shininess;
};


class Mesh
{
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texturez> textures;
        std::vector<Material> materials;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texturez> textures,std::vector<Material> materials)
        {
            this->vertices = vertices;
            this->textures = textures;
            this->indices = indices;
            this->materials = materials;
            setUpMesh();

        }
        void draw(shader &sha,bool simple=true)
        {
            //GLCall(glBindVertexArray(VAO));
            //GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
            //GLCall(glBindVertexArray(0));
            
            if(simple)
            {
                GLCall(glBindVertexArray(VAO));
                GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
                GLCall(glBindVertexArray(0));
            }
            else 
            {
                unsigned int diffuseNr = 1;
                unsigned int specularNr = 1;


                for(unsigned int i=0; i < textures.size();i++)
                {
                    GLCall(glActiveTexture(GL_TEXTURE0 + i));
                    std::string number;
                    std::string name = textures[i].type;
                    std::string n;
                    
                    if(name == "texture_diffuse")
                    {
                        n = "diffuse";
                        number = std::to_string(diffuseNr++);
                        
                    }

                    else if(name == "texture_specular")
                    {
                        n = "specular";
                        number = std::to_string(specularNr++);
                    }

                
                    
                    //GLCall(glUniform1i(glGetUniformLocation(sha.getShaderID(), (name + number).c_str()), i ));
                    GLCall(glActiveTexture(GL_TEXTURE0+i));
                    GLCall(glBindTexture(GL_TEXTURE_2D, textures[i].id));
                    
                    sha.setUniform1i(("myTexture." + n),i);
                    sha.setUniform1f("myTexture.shininess",32);
                    sha.setUniform1i("tex",1);
                    
                    GLCall(glActiveTexture(GL_TEXTURE0));
                    //sha.setUniform1f(("material." + name + number).c_str(), i);
                    //GLCall(glBindTexture(GL_TEXTURE_2D,textures[i].id));
                    
                }

                //Set the materials if there are no textures
                if(materials.size() > 0 && textures.size() < 1)
                {

                    sha.setUniform1i("tex",0);
                    sha.setUniformVec3f("material.diffuse",materials[0].Diffuse); /** glm::vec3(0.5,0.5,0.5)*/
                    sha.setUniformVec3f("material.specular",materials[0].Specular);
                    sha.setUniform1f("material.shininess",materials[0].Shininess);

                }
                
            // GLCall(glActiveTexture(GL_TEXTURE0));
                GLCall(glBindVertexArray(VAO));
            
                

                GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
                GLCall(glBindVertexArray(0));
                

                }
            }
            
        unsigned int VAO;
    private:
        
        
        /*Structs are very useful for this because in memory they are stored like an array of bytes and can use useful macro called offsetof */
        unsigned int VBO, EBO;
        void setUpMesh()
        {
            GLCall(glGenVertexArrays(1,&VAO));
            
            GLCall(glGenBuffers(1,&VBO));
            GLCall(glGenBuffers(1,&EBO));

            GLCall(glBindVertexArray(VAO));

            GLCall(glBindBuffer(GL_ARRAY_BUFFER,VBO));

            GLCall(glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),&vertices[0],GL_STATIC_DRAW));

            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO));
            GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),&indices[0],GL_STATIC_DRAW));

            
            GLCall(glEnableVertexAttribArray(0));
            GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
            
            GLCall(glEnableVertexAttribArray(1));
            GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            GLCall(glEnableVertexAttribArray(2));
            GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)));

            GLCall(glBindVertexArray(0));
   );








        }




};