#pragma once

#include <GL/glew.h>
#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "random_terrain.h"

enum movement {
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};


//default
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;


class Camera
{
private:
    void updateVectors();
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 world_up;

	//Just stuff for cameraTest.cpp
	glm::mat4 boxModel = glm::mat4(1.0f);
	glm::vec4 boxPos = glm::vec4(0.0f,0.0f,0.1f,1.0f);

	Terrain* terrain;
	//Euler angles
	float Yaw;
	float Pitch;

	//options
	float Speed;
	float Sensitivity;
	float fov;

	//constructor
	Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f), float yaw = YAW, float pitch = PITCH, float Speed = SPEED);

	
	
	void processScroll(float y_offset);
	void processMouse(float x_offset,float y_offset,bool constrain = true);
	void processKeyboard(movement direction, float deltatime);

	void moveObj(movement direction, float deltatime);

	inline void setTerrain(Terrain* t)
	{
		terrain = t;
	}




};








