

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "camera.h"
#include "shader.h"
#include "UniformBuffer.h"
#include "random_terrain.h"

class sceneRenderer 
{
	private:
		Terrain* terrain;
		glm::mat4 proj;
		glm::mat4 view;
		UniformBuffer* proj_and_view;
		glm::vec3 cameraPosition;
		void setNonUboShaders(std::vector<shader*>& shaders,glm::mat4& proj,glm::mat4& view);

	

	public:

		sceneRenderer(){}
		void DrawScene();

		inline void setTerrain(Terrain* t){terrain = t;}
		inline void setCameraPosition(glm::vec3 t){cameraPosition = t;}
		inline void setUniformBuffer(UniformBuffer* t){proj_and_view = t;}

		inline void setproj(glm::mat4 t){proj = t;}
		inline void setView(glm::mat4 t){view = t;}
		void Clear() const;



};