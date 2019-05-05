#ifndef HW4_h
#define HW4_h

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

class HW4 {
private:
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
public:
	HW4(int w, int h) :WINDOW_WIDTH(w), WINDOW_HEIGHT(h) {};

	std::vector<GLfloat> getVertices() {
		std::vector<GLfloat> result{
			// ��1����ɫ
			 2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
			 2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 0.0f,//
			-2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
			-2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f,//
			// ��2����ɫ
			 2.0f, -2.0f,  2.0f, 0.0f, 0.0f, 1.0f,
			 2.0f,  2.0f,  2.0f, 0.0f, 0.0f, 1.0f,//
			-2.0f,  2.0f,  2.0f, 0.0f, 0.0f, 1.0f,
			-2.0f, -2.0f,  2.0f, 0.0f, 0.0f, 1.0f,//
			// ��3����ɫ
			-2.0f,  2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
			-2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f,//
			-2.0f, -2.0f,  2.0f, 0.0f, 1.0f, 0.0f,
			-2.0f,  2.0f,  2.0f, 0.0f, 1.0f, 0.0f,//
			// ��4����ɫ
			 2.0f,  2.0f, -2.0f, 0.5f, 0.0f, 1.0f,
			 2.0f, -2.0f, -2.0f, 0.5f, 0.0f, 1.0f,//
			 2.0f, -2.0f,  2.0f, 0.5f, 0.0f, 1.0f,
			 2.0f,  2.0f,  2.0f, 0.5f, 0.0f, 1.0f,//
			// ��5����ɫ
			 2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
			 2.0f, -2.0f,  2.0f, 1.0f, 1.0f, 0.0f,//
			-2.0f, -2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
			-2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,//
			// ��6���ۺ�ɫ
			 2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
			 2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,//
			-2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,
			-2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 1.0f //
		};
		return result;
	}

	std::vector<unsigned int> getIndices() {
		std::vector<unsigned int> indices{
			// ��1
			 0,  1,  3,
			 2,  1,  3,
			// ��2
			 4,  5,  7,
			 6,  5,  7,
			// ��3
			 8,  9, 11,
			10,  9, 11,
			// ��4
			12, 13, 15,
			14, 13, 15,
			// ��5
			16, 17, 19,
			18, 17, 19,
			// ��6
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

#endif // !HW4_h