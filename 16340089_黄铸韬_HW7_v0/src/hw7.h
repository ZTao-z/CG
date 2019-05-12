#ifndef HW7_h
#define HW7_h

// ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// GLEW/GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
// OTHER
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

class HW7 {
private:
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
public:
	HW7(int w, int h) :WINDOW_WIDTH(w), WINDOW_HEIGHT(h) {};

	std::vector<GLfloat> getVertices() {
		std::vector<GLfloat> result{
			// 面1，红色，Normals
			 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,//
			-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,//
			// 面2，蓝色，Normals
			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f,  1.0f,//
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f,  1.0f,//
			// 面3，绿色，Normals
			-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,//
			-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,//
			// 面4，紫色，Normals
			 0.5f,  0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,//
			 0.5f, -0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,//
			// 面5，黄色，Normals
			 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,//
			-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,//
			// 面6，粉红色，Normals
			 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,//
			-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f //
		};
		return result;
	}

	std::vector<GLfloat> getVertices2() {
		std::vector<GLfloat> result{
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		return result;
	}

	std::vector<GLfloat> getPlaneVectices() {
		std::vector<GLfloat> result{
			// Positions          // Normals         // Texture Coords
			 25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
			-25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			 25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			 25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
		};
		return result;
	}

	std::vector<unsigned int> getIndices() {
		std::vector<unsigned int> indices{
			// 面1
			 0,  1,  3,
			 2,  1,  3,
			// 面2
			 4,  5,  7,
			 6,  5,  7,
			// 面3
			 8,  9, 11,
			10,  9, 11,
			// 面4
			12, 13, 15,
			14, 13, 15,
			// 面5
			16, 17, 19,
			18, 17, 19,
			// 面6
			20, 21, 23,
			22, 21, 23
		};
		return indices;
	}

	void Translation(glm::mat4& transform, glm::vec3 shaft) {
		transform = glm::translate(transform, shaft);
	}

	void Rotation(glm::mat4& transform, float speed, glm::vec3 shaft) {
		transform = glm::rotate(transform, speed, shaft);
	}

	void Scale(glm::mat4& transform, glm::vec3 shaft) {
		transform = glm::scale(transform, shaft);
	}
};

enum MOVE_DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Camera {
private:
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	float yaw;
	float pitch;
	float lastX;
	float lastY;
	float zoom;

	float MovementSpeed;
	bool constrainPitch;
public:

	Camera(int w = 600, int h = 600,
		glm::vec3 position = glm::vec3(0.0f,0.0f,3.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float YAW = 90.0f, float PITCH = 0.0f):WINDOW_WIDTH(w), WINDOW_HEIGHT(h){
		
		cameraPos = position;
		cameraUp = up;

		// yaw is initialized to -90.0 degrees since a yaw of 0.0 
		// results in a direction vector pointing to the right 
		// so we initially rotate a bit to the left.
		yaw = YAW;
		pitch = PITCH;
		zoom = 45.0f;

		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		MovementSpeed = 2.5f;
		constrainPitch = true;

		updateVector();
	};

	void moveForward(float cameraSpeed) {
		cameraPos += cameraSpeed * this->cameraFront;
	};

	void moveBack(float cameraSpeed) {
		cameraPos -= cameraSpeed * this->cameraFront;
	};

	void moveLeft(float cameraSpeed) {
		cameraPos -= this->cameraRight * cameraSpeed;
	};

	void moveRight(float cameraSpeed) {
		cameraPos += this->cameraRight * cameraSpeed;
	};

	void ProcessKeyboard(MOVE_DIRECTION direction, float deltaTime)
	{
		float cameraSpeed = this->MovementSpeed * deltaTime;
		if (direction == UP) {
			this->moveForward(cameraSpeed);
		}
		if (direction == DOWN) {
			this->moveBack(cameraSpeed);
		}
		if (direction == LEFT) {
			this->moveLeft(cameraSpeed);
		}
		if (direction == RIGHT) {
			this->moveRight(cameraSpeed);
		}
		cameraPos.y = 0.0f;
	};

	void updateVector() {
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		
		this->cameraFront = glm::normalize(front);
		this->cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
		this->cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	}

	void ProcessMouseMove(double xoffset, double yoffset) {
		
		// 设置敏感度
		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		// 改变俯仰角
		this->yaw += xoffset;
		this->pitch += yoffset;

		if (this->constrainPitch) {
			if (this->pitch > 89.0f) {
				this->pitch = 89.0f;
			}
			if (this->pitch < -89.0f) {
				this->pitch = -89.0f;
			}
		}

		updateVector();
	};

	void ProcessMouseScroll(float yoffset)
	{
		if (this->zoom >= 1.0f && this->zoom <= 45.0f)
			this->zoom -= yoffset;
		if (this->zoom <= 1.0f)
			this->zoom = 1.0f;
		if (this->zoom >= 45.0f)
			this->zoom = 45.0f;
	}

	glm::mat4 getView() const {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	glm::vec3 getPosition() const {
		return this->cameraPos;
	}

	float getZoom() const {
		return this->zoom;
	}
};

class Shader {
private:
	unsigned int shaderProgram;
public:
	Shader(const char* vertex, const char* fragment) {
		this->shaderProgram = setupShader(vertex, fragment);
	}

	void bindMat4(const char* name, glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void bindVec3(const char* name, glm::vec3& matrix) {
		glUniform3fv(glGetUniformLocation(this->shaderProgram, name), 1, glm::value_ptr(matrix));
	}

	void bindInt(const char* name, int value) {
		glUniform1i(glGetUniformLocation(this->shaderProgram, name), value);
	}

	void use() {
		glUseProgram(this->shaderProgram);
	}

	unsigned int getShader() const {
		return this->shaderProgram;
	}

	unsigned int setupShader(const char* vertex, const char* fragment) {
		// 编译顶点着色器
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertex, NULL);
		glCompileShader(vertexShader);
		// 检测是否成功编译
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR: " << infoLog << std::endl;
		}

		// 编译片段着色器
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragment, NULL);
		glCompileShader(fragmentShader);
		// 检测是否成功编译
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR: " << infoLog << std::endl;
		}

		// 创建着色器程序
		unsigned int shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR: " << infoLog << std::endl;
		}
		// 删除着色器对象
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return shaderProgram;
	}
};

#endif // !HW7_h