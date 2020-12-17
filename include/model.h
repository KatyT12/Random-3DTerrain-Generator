#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../vender/stb_image/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"
#include "Texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

/* There is a struct in mesh.h called Texturez which was changed from Texture because I have included a texture class*/


unsigned int TextureFromFile(std::string path, std::string &directory);



class Model
{
    public:
        Model(std::string const &path)
        {
            loadModel(path);
        }
        void Draw(shader &sha,bool simple=true)
        {
            for(unsigned int i=0;i<meshes.size();i++)
            {
                meshes[i].draw(sha,simple);
            }
        }

      std::vector<Mesh> meshes;
    std::vector<Texturez> textures_loaded; 
    private:
      
        std::string directory;




        //Load scene, gte directory and then call process node
        void loadModel(std::string const &path)
        {
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
            {
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                ASSERT(false);
            }
            directory = path.substr(0,path.find_last_of("/"));
            processNode(scene->mRootNode,scene);

        }
        
        /*  We first check each of the node's mesh indices and retrieve the corresponding mesh by indexing the scene's mMeshes array. 
        *The returned mesh is then passed to the processMesh function that returns a Mesh object that we can store in the meshes list/vector.
        *Once all the meshes have been processed, we iterate through all of the node's children and call the same processNode function for each its children. 
        Once a node no longer has any children, the recursion stops. Then process assimp data into mesh class we have made in mesh.h */
        
        void processNode(aiNode *node,const aiScene *scene)
        {   
            //Process all of the nodes meshes if any
            for(unsigned int i=0; i < node->mNumMeshes;i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh,scene));
            }
            for(unsigned int i=0;i<node->mNumChildren;i++)
            {
                processNode(node->mChildren[i],scene);
            }

        }  
        
        /*  Translating an aiMesh object to a mesh object of our own is not too difficult. All we need to do, is access each of the mesh's relevant properties and store them in our own object. 
        The general structure of the processMesh function then becomes:*/
        
        /* Processing a mesh is a 3-part process: retrieve all the vertex data, 
        *retrieve the mesh's indices, and finally retrieve the relevant material data. The processed data is stored in one of the 3 vectors and from those a 
        *Mesh is created and returned to the function's caller. */
        Mesh processMesh(aiMesh *mesh, const aiScene *scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texturez> textures;
            std::vector<Material> materials;

            for(unsigned int i=0;i<mesh->mNumVertices;i++)
            {
                //process vertex positions, normals and texture coordinates
                Vertex vertex;

                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;

                vertex.position = vector;

                //Normals
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;

                //Only care about first set of texture coordiantes        
                if(mesh->mTextureCoords[0]) //Does the mesh have texture coords?
                {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;   
                         
                }
                else 
                {
                    vertex.TexCoords = glm::vec2(0.0f,0.0f);
                }

                vertices.push_back(vertex);

            }
            /*Assimp's interface defines each mesh as having an array of faces, where each face represents 
            a single primitive, which in our case (due to the aiProcess_Triangulate option) are always triangles. 
            A face contains the indices of the vertices we need to draw in what order for its primitive*/
            
            for(unsigned int i=0;i<mesh->mNumFaces;i++)
            {
                aiFace f = mesh-> mFaces[i];
                for(unsigned int j=0;j<f.mNumIndices;j++)
                {
                    indices.push_back(f.mIndices[j]);
                }

            }

            /* We first retrieve the aiMaterial object from the scene's mMaterials array. Then we want to load the mesh's diffuse and/or 
            specular textures. A material object internally stores an array of texture locations for each texture type. The different texture types are all prefixed with aiTextureType_. We use a helper function called 
            loadMaterialTextures to retrieve, load, and initialize the textures from the material. The function returns a vector of Texture structs that we store at the end of the model's textures vector.  */
            if(mesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                
                Material mat = loadMaterial(material);
                materials.push_back(mat);
                
                std::vector<Texturez> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE,"texture_diffuse");

                textures.insert(textures.end(),diffuseMaps.begin(),diffuseMaps.end());

                std::vector<Texturez> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR,"texture_specular");
                textures.insert(textures.end(),specularMaps.begin(),specularMaps.end());

            }
           
            
            return Mesh(vertices, indices, textures,materials);

        }
        
        
        
        
        std::vector<Texturez> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
        {
            
            /* We first check the amount of textures stored in the material via its GetTextureCount function that expects one of the texture
             types we've given. We retrieve each of the texture's file locations via the GetTexture function that stores the result in an aiString. We then use another helper function called 
            TextureFromFile that loads a texture (with stb_image.h) for us and returns the texture's ID. You can check the complete code listing at the end for its content if 
            you're not sure how such a function is written. */
            
            std::vector<Texturez> textures;
            for(unsigned int i=0;i<mat->GetTextureCount(type);i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                bool skip = false;
                for(unsigned int j=0;j<textures_loaded.size();j++)
                {
                    if(std::strcmp(textures_loaded[j].path.data(),str.C_Str()) == 0) //If texture already in textures_loaded
                    {
                        textures.push_back(textures_loaded[j]);
                        skip = true;
                        break;
                    }
                }
                if(!skip)
                {
                
                Texturez texture;
                
                texture.id = TextureFromFile(str.C_Str(),directory);
              
               /* This did not work for some reason*/
               // Texture tex = Texture(directory + "/" + str.C_Str());
               // texture.id = tex.getID();

                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
                }
            }
            
            return textures;
            
        }

        Material loadMaterial(aiMaterial* mat)
        {
            Material material;
            aiColor3D color(0.0f,0.0f,0.0f);
            float shininess;

            mat->Get(AI_MATKEY_COLOR_DIFFUSE,color);
            material.Diffuse = glm::vec3(color.r,color.g,color.b);

            mat->Get(AI_MATKEY_COLOR_AMBIENT,color);
            material.Ambient = glm::vec3(color.r,color.g,color.b);

            mat->Get(AI_MATKEY_COLOR_SPECULAR,color);
            material.Specular = glm::vec3(color.r,color.g,color.b);

            mat->Get(AI_MATKEY_SHININESS,shininess);
            material.Shininess = shininess;

            return material;



        }



};

unsigned int TextureFromFile(std::string path, std::string &directory)
{
std::string filename = std::string(path.c_str());
    filename = directory + '/' + filename;

    //Remember to do this or it will look really wierd
    stbi_set_flip_vertically_on_load(1);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
           
        else if (nrComponents == 3)
            format = GL_RGB;
          
        else if (nrComponents == 4)
            format = GL_RGBA;

      

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}