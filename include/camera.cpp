#include "camera.h"



Camera::Camera(glm::vec3 position,glm::vec3 up, float yaw, float pitch,float speed)

: Front(glm::vec3(0.0f,0.0f,-1.0f)), Speed(speed), Sensitivity(SENSITIVITY), fov(FOV)
{
	Yaw = yaw;
	Pitch = pitch;
	Position = position;
	world_up = up;
	updateVectors();
}



void Camera::updateVectors()
{
	glm::vec3 front;

	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	Front = glm::normalize(front);

	Right = glm::normalize(glm::cross(Front,world_up));
	Up = glm::normalize(glm::cross(Right,Front));
}

void Camera::processScroll(float y_offset)
{
	fov -= (float)y_offset;
        if (fov < 1.0f)
	{
		fov = 1.0f;
	}
	
	if (fov > 45.0f)
	{
		fov = 45.0f;
	}
}

void Camera::processMouse(float x_offset, float y_offset, bool constrain)
{
	x_offset *= Sensitivity;
	y_offset *= Sensitivity;

	Yaw += x_offset;
	Pitch += y_offset;

	if(constrain)
	{
		if(Pitch > 89.9)
		{
			Pitch = 89.9;
		}
		if(Pitch < -89.9)
		{
			Pitch = -89.9;
		}

	updateVectors();


	}



}


void Camera::processKeyboard(movement direction, float deltatime)
{
	//Some terrain collision bullshit
	if(terrain != nullptr)
	{
		float terrainheight = terrain->getTerrainHeight(Position.x,Position.z);
		if(terrainheight != 0)
		{
			if(Position.y < terrainheight + 0.5 && Position.y > terrainheight - 0.5 && Position.y > terrainheight && Front.y > 0.0 && direction == FORWARD)
			{
				direction = BACK;
			}
			else if(Position.y < terrainheight + 0.5 && Position.y > terrainheight - 0.5 && Position.y < terrainheight && Front.y < 0.0 && direction == FORWARD)
			{
				direction = BACK;
				
			}
		}

		
	}

	float vel = Speed*deltatime;
	if(direction == FORWARD)
	{
		Position += Front*vel;
	}
	if(direction == BACK)
	{
		Position -= Front*vel;
	}
	if(direction == LEFT)
	{
		Position -= Right*vel;
	}
	if(direction == RIGHT)
	{
		Position += Right*vel;
	}	



}


