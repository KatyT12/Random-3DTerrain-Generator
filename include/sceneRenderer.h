

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "camera.h"
#include "shader.h"
#include "UniformBuffer.h"
#include "random_terrain.h"
#include "Cubemap.h"
#include "camera.h"

class sceneRenderer 
{
	private:
		Terrain* terrain;
		glm::mat4 proj;
		glm::mat4 view;
		UniformBuffer* proj_and_view;
		glm::vec3 cameraPosition;
		void setNonUboShaders(std::vector<shader*>& shaders,glm::mat4& proj,glm::mat4& view);
		Cubemap* cubemap;
		Camera* camera;

		

	public:

		sceneRenderer(){
			/*
			float positions[] = {
				-1.0f,-1.0f,-1.0f, 0.0f,0.0f,
				 1.0f,-1.0f,-1.0f, 1.0f,0.0f,
				 1.0f, 1.0f,-1.0f, 1.0f,1.0f,
				 -1.0f,1.0f,-1.0f, 0.0f,1.0f
			};

			unsigned int indices[] ={
				0,1,2,
				2,3,0
			};
			glGenVertexArrays(1,&vao);
			glBindVertexArray(vao);

			glGenBuffers(1,&vbo);
			glBindBuffer(GL_ARRAY_BUFFER,vbo);
			glBufferData(GL_ARRAY_BUFFER,5*4*sizeof(float),positions,GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof(float),(void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5 * sizeof(float),(void*)(3 * sizeof(float)));

			glGenBuffers(1,&ib);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(unsigned int),indices,GL_STATIC_DRAW);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


			sha.makeShader("res/shaders/test.shader");*/

		}
		void DrawScene();

		inline void setTerrain(Terrain* t){terrain = t;}
		inline void setCameraPosition(glm::vec3 t){cameraPosition = t;}
		inline void setUniformBuffer(UniformBuffer* t){proj_and_view = t;}

		inline void setproj(glm::mat4 t){proj = t;}
		inline void setView(glm::mat4 t){view = t;}
		inline void setCubeMap(Cubemap* c){cubemap =c;}
		inline void setCamera(Camera* c){camera=c;}
		void Clear() const;
		void drawCubeMap();

/*
		shader sha;
		unsigned int vao;
		unsigned int vbo;
		unsigned int ib;*/
};