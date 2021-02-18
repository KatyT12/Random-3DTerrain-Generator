#include "camera.h"

#include <iostream>


Camera::Camera(glm::vec3 position,glm::vec3 up, float yaw, float pitch, float speed)

: Front(glm::vec3(0.0f,0.0f,-1.0f)), Speed(speed), Sensitivity(0.01), fov(60)
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

	front.x = cos(glm::radians(Yaw)) * cos((Pitch));
	front.y = sin(Pitch);
	front.z = sin(glm::radians(Yaw)) * cos((Pitch));

	//Front = glm::normalize(front);

    //centre = 0,0,0

	Position.x = ((sin(Yaw) * cos(Pitch) * 7));
	Position.z = ((cos(Yaw) * cos(Pitch) * 7));
	Position.y =((sin(Pitch) * 7));



	Position = Position + glm::vec3(boxPos);


	//Position = Position + glm::vec3(boxPos);
	

	//std::cout << "Yaw: " << Yaw << "Pitch: " << Pitch << std::endl;

	//Front = glm::vec3(0.0f,0.0f,-0.1f);
	Front = glm::normalize(glm::vec3(boxPos) - Position);
	Right = glm::normalize(glm::cross(Front,world_up));
	Up = glm::normalize(glm::cross(Right,Front));
	//Position.y = sin(Pitch);

	//Right = glm::normalize(glm::cross(Front,world_up));
	//Up = glm::normalize(glm::cross(Right,Front));
}

void Camera::processScroll(float y_offset)
{
	fov -= (float)y_offset;
        if (fov < 1.0f)
	{
		fov = 1.0f;
	}
	
	if (fov > 60.0f)
	{
		fov = 60.0f;
	}
}

void Camera::processMouse(float x_offset, float y_offset, bool constrain)
{
	x_offset *= Sensitivity;
	y_offset *= Sensitivity;

	Yaw += x_offset;
	Pitch += y_offset;




	constrain = false;
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
	
	}

	updateVectors();

}


void Camera::processKeyboard(movement direction, float deltatime)
{
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

void Camera::moveObj(movement direction, float deltatime)
{
	float vel = Speed* deltatime;
	
	if(direction == FORWARD)
	{
		
		boxModel = glm::translate(boxModel,Front*vel);
	}
	if(direction == BACK)
	{
		boxModel = glm::translate(boxModel, Front * vel * -1.0f);
	}
	if(direction == LEFT)
	{
		boxModel = glm::translate(boxModel,Right*vel * -1.0f);
	}
	if(direction == RIGHT)
	{
		boxModel = glm::translate(boxModel,Right*vel);
	}	
	if(direction == UP)
	{
		boxModel = glm::translate(boxModel,Up*vel);
	}
	if(direction == DOWN)
	{
		boxModel = glm::translate(boxModel,Up*vel*-1.0f);
	}	

	boxPos = boxModel * glm::vec4(0.0f,0.0f,0.1f,1.0f);

	if(terrain)
	{
			float collisionOffset = terrain->getCollisionOffset();
		    float terrainWorldHeight = (terrain->getTerrainModelMatrix()*glm::vec4(0,0,0,1)).y;
		    float terrainLocalHeight = terrain->getTerrainHeight(boxPos.x,boxPos.z);
		    float terrainheight = terrainWorldHeight + terrainLocalHeight;
			if(terrainheight != 0)
			{
				if(boxPos.y < terrainheight + collisionOffset && boxPos.y > terrainheight - collisionOffset && boxPos.y > terrainheight)
				{
					glm::vec4 tmpBoxPos = boxPos;
					boxPos.y = terrainheight + collisionOffset;
					boxModel = glm::translate(boxModel,glm::vec3(boxPos - tmpBoxPos));
				}
				else if(boxPos.y < terrainheight + collisionOffset && boxPos.y > terrainheight - collisionOffset && boxPos.y < terrainheight)
				{
					glm::vec4 tmpBoxPos = boxPos;

					boxPos.y = terrainheight - collisionOffset;
					boxModel = glm::translate(boxModel,glm::vec3(boxPos - tmpBoxPos));



				}
			}
	}
	

	updateVectors();
	
	//Position = glm::vec3(boxPos) + Position;
	//Front = glm::vec3(boxPos) - Position;
	

}


