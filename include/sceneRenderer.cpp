#include "sceneRenderer.h"

void sceneRenderer::DrawScene()
{
	    proj_and_view->UpdateBufferPoint(0,&view[0][0],sizeof(glm::mat4),sizeof(glm::mat4));
        proj_and_view->UpdateBufferPoint(0,&proj[0][0],sizeof(glm::mat4),0);

        //Update proj and view matrix for all shaders not using the uniform buffer object
        setNonUboShaders(terrain->notUboShaders,proj,view);

        terrain->terrainShader.Bind();
     
        //Lighting
        terrain->terrainShader.setUniformVec3f("u_viewPos",cameraPosition);
   

        if(terrain->treesPresent())
        {
            terrain->treeShader.Bind();
        }
        terrain->Draw();
}

 void sceneRenderer::setNonUboShaders(std::vector<shader*>& shaders,glm::mat4& proj,glm::mat4& view)
 {
     for(int i=0;i<shaders.size();i++)
     {
        shaders[i]->Bind();
        shaders[i]->setUniformMat4f("proj",proj);
        shaders[i]->setUniformMat4f("view",view);
        shaders[i]->UnBind();
     }
 }