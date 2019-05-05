// ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// GLEW/GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// OTHER
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
// homework
#include "hw4.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// ���û�����ESC��,��������window���ڵ�WindowShouldClose����Ϊtrue
	// �ر�Ӧ�ó���
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// ����������ɫ��
const char* shader = "#version 330 core\n\
						layout(location = 0) in vec3 pos;\n\
						layout(location = 1) in vec3 color;\n\
						uniform mat4 view;\n\
						uniform mat4 transform;\n\
						out vec3 outColor;\n\
						void main() {\n\
							gl_Position = view * transform * vec4(pos, 1.0);\n\
							outColor = color;\n\
						}\0";
// ����Ƭ����ɫ��
const char* fragment = "#version 330 core\n\
						in vec3 outColor;\n\
						out vec4 color;\n\
						void main() {\n\
							color = vec4(outColor, 1.0f);\n\
						}\0";

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
int static_open = false;

int main() {
	// ʵ����GLFW����
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// ����һ�����ڶ���
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// OpenGL��Ⱦ���ڵĳߴ��С
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��ʼ��Ϸѭ��֮ǰע�ắ�������ʵĻص�(Esc�˳�)
	glfwSetKeyCallback(window, key_callback);

	// ��ʼ��GLEW/GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ��ʼ��ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	/*����ͼ��*/
	//============================================
	// ���붥����ɫ��
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &shader, NULL);
	glCompileShader(vertexShader);
	// ����Ƿ�ɹ�����
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}

	// ����Ƭ����ɫ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);
	// ����Ƿ�ɹ�����
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}
	// ������ɫ������
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}
	// ɾ����ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// ImGUI���Ʋ���
	unsigned int VBO, VAO, EBO;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	
	HW4 hw4(WINDOW_WIDTH, WINDOW_HEIGHT);
	// ��������
	GLfloat* vertices = NULL;
	unsigned int* indices = NULL;
	int total_v = 0, total_ind = 0;
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	// ��ͼ
	v = hw4.getVertices();
	ind = hw4.getIndices();
	total_v = v.size();
	total_ind = ind.size();

	vertices = new GLfloat[total_v];
	indices = new unsigned int[total_ind];

	for (int i = 0; i < total_v; i++) {
		vertices[i] = v[i];
	}
	for (int i = 0; i < total_ind; i++) {
		indices[i] = ind[i];
	}

	// ģʽѡ��
	bool Translation_func = true, Rotation_func = false, Scale_func = false, enableDT = true, Bonus_func = false;
	GLfloat t_x = 0, t_y = 0, speed = 0.5, scale_size = 1.0;
	bool leftright = true;
	// ��Ϸѭ��
	while (!glfwWindowShouldClose(window)) {
		// ����¼�
		glfwPollEvents();

;		/*������*/
		// ʹ��glGenBuffers������һ������ID����һ��VBO����
		glGenBuffers(1, &VBO);
		// ʹ��glGenBuffers������һ������ID����һ��VAO����
		glGenVertexArrays(1, &VAO);
		// ʹ��glGenBuffers������һ������ID����һ��EBO����
		glGenBuffers(1, &EBO);
		// ��VAO
		glBindVertexArray(VAO);
		// ��������:���㻺��
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// �������ݸ��Ƶ�������ڴ���
		glBufferData(GL_ARRAY_BUFFER, total_v * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		// ��������:��������
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// �������ݸ��Ƶ�������ڴ���
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_ind * sizeof(unsigned int), indices, GL_STATIC_DRAW);
		// ��������ָ��
		// ��������ָ��
		// λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// ��ɫ
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// �����
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ImGUI����
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ����GUI��������
		ImGui::Begin("function choose");
		ImGui::Checkbox("Open DEPTH_TEST", &enableDT);
		ImGui::Checkbox("Translation", &Translation_func);
		ImGui::Checkbox("Rotation", &Rotation_func);
		ImGui::Checkbox("Scale", &Scale_func);
		ImGui::Checkbox("Bonus", &Bonus_func);
		ImGui::End();
		if (Bonus_func) {
			Rotation_func = true;
		}
		if (Translation_func) {
			ImGui::Begin("Translation");
			ImGui::Checkbox("Left right / Up down", &leftright);
			ImGui::End();
		}
		if (Rotation_func) {
			ImGui::Begin("Rotation");
			ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
			ImGui::End();
		}

		// ��Ⱦָ��
		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		// ������ڵ��ƶ��켣
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		if (enableDT) {
			// ������Ȳ���
			glEnable(GL_DEPTH_TEST);
			// ������Ȳ��ԣ������Ȳ��Ի���
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		else {
			// �ر���Ȳ���
			glDisable(GL_DEPTH_TEST);
		}
		// ʹ����ɫ������
		glUseProgram(shaderProgram);
		// GLM�任
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::scale(view, glm::vec3(0.25f, 0.25f, 0.25f));

		if (Translation_func) {
			if (leftright) {
				t_x = 2 * sin(glfwGetTime());
				hw4.Translation(transform, glm::vec3(t_x, 0.0f, 0.0f));
			}
			else {
				t_y = 2 * sin(glfwGetTime());
				hw4.Translation(transform, glm::vec3(0.0f, t_y, 0.0f));
			}
		}
		if (Rotation_func) {
			if (Bonus_func) {
				hw4.Scale(transform, glm::vec3(0.3, 0.3, 0.3));
			}
			hw4.Rotation(transform, speed * (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));
		}
		if (Scale_func) {
			scale_size = (sin(glfwGetTime()) + 1.5) * 0.5;
			hw4.Scale(transform, glm::vec3(scale_size, scale_size, scale_size));
		}
		
		// ���任����Ӧ�õ�������
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		// ��������
		glBindVertexArray(VAO);
		// ����������
		glDrawElements(GL_TRIANGLES, total_ind * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

		if (Bonus_func) {
			transform = glm::mat4(1.0f);
			hw4.Scale(transform, glm::vec3(0.2, 0.2, 0.2));
			t_y = 16 * sin((float)glfwGetTime());
			t_x = 16 * cos((float)glfwGetTime());
			hw4.Translation(transform, glm::vec3(t_x, t_y, 0.0f));
			hw4.Rotation(transform, 5 * (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]);
			// ��������
			glBindVertexArray(VAO);
			// ����������
			glDrawElements(GL_TRIANGLES, total_ind * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);

		// ��������
		glfwSwapBuffers(window);

		// �ͷ�VBO��VAO��EBO
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	delete[] vertices;
	delete[] indices;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	//============================================

	// �ͷ�GLFW������ڴ�
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
