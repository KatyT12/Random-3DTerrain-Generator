#include "sceneRenderer.h"

void sceneRenderer::DrawScene()
{
    if(terrain->usingFbo())
    {
        for(int i = 0; i < 3;i++)
        {
            float distance;
            if(i == 2)terrain->fboMode = false;
            else terrain->fboMode = true;
            if(terrain->fboMode){
                terrain->bindFrameBuffer(i);
                if(i == 0){
                    distance = camera->Position.g - terrain->getWaterHeight();
                    float temp = camera->Pitch;
                    camera->Position.g -= distance;
                    camera->Pitch * -1.0f;
                    view = glm::lookAt(camera->Position, camera->Position + camera->Front,camera->Up * -1.0f);
                }
            }
            else{
                terrain->unbindFrameBuffer();
            }

            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            proj_and_view->UpdateBufferPoint(0,&view[0][0],sizeof(glm::mat4),sizeof(glm::mat4));
            proj_and_view->UpdateBufferPoint(0,&proj[0][0],sizeof(glm::mat4),0);
    
            //Update proj and view matrix for all shaders not using the uniform buffer object 
            setNonUboShaders(terrain->notUboShaders,proj,view);

            if(playerModel != nullptr)
            {
                playerModelShader->Bind();
                playerModelShader->setUniformVec3f("u_viewPos",camera->Position);
                playerModel->Draw(*playerModelShader,true);
            }
            drawCubeMap();
        

            terrain->terrainShader.Bind();
        
            //Lighting
            terrain->terrainShader.setUniformVec3f("u_viewPos",cameraPosition);
    
            terrain->Draw();

            /*
            if(!terrain->fboMode)
            {
                terrain->bindWaterReflectionAttachment();
                sha.Bind();
                sha.setUniformMat4f("model",glm::mat4(1.0f));
                sha.setUniformMat4f("view",view);
                sha.setUniformMat4f("proj",proj);
                sha.setUniform1i("u_Texture",0);
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

            }*/


            if(i==0)
            {
                camera->Position.g += distance; 
                camera->Pitch * -1.0f;
                view = glm::lookAt(camera->Position, camera->Position + camera->Front,camera->Up);
            }

        }
   
    }
    else{
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        proj_and_view->UpdateBufferPoint(0,&view[0][0],sizeof(glm::mat4),sizeof(glm::mat4));
        proj_and_view->UpdateBufferPoint(0,&proj[0][0],sizeof(glm::mat4),0);

        //Update proj and view matrix for all shaders not using the uniform buffer object
        setNonUboShaders(terrain->notUboShaders,proj,view);

        drawCubeMap();

        if(playerModel != nullptr)
        {
            playerModelShader->Bind();
            playerModel->Draw(*playerModelShader,false);
        }

        terrain->terrainShader.Bind();
    
        //Lighting
        terrain->terrainShader.setUniformVec3f("u_viewPos",cameraPosition);

        terrain->Draw();
        }
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

void sceneRenderer::drawCubeMap()
{
    if(cubemap != nullptr)
    {
        glDepthFunc(GL_LEQUAL);
        GLCall(glBindVertexArray(cubemap->cubeMapVAO));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,cubemap->cubeMapIB));
        cubemap->Bind();
        cubemap->cubeMapShader.Bind();
        cubemap->cubeMapShader.setUniformMat4f("proj",proj);
        cubemap->cubeMapShader.setUniformMat4f("view",glm::mat4(glm::mat3(view))); 
        cubemap->cubeMapShader.setUniformMat4f("model",glm::mat4(1.0f)); 
        cubemap->cubeMapShader.setUniform1i("cubemap",0);
        GLCall(glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,nullptr));
        cubemap->cubeMapShader.UnBind();
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        cubemap->UnBind();
        glDepthFunc(GL_LESS);
    }
}
