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
#include "hw5.h"



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void key_input(GLFWwindow* window, float deltaTime);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// ����������ɫ��
const char* shader = "#version 330 core\n\
						layout(location = 0) in vec3 pos;\n\
						layout(location = 1) in vec3 color;\n\
						uniform mat4 projection;\n\
						uniform mat4 view;\n\
						uniform mat4 transform;\n\
						out vec3 outColor;\n\
						void main() {\n\
							gl_Position = projection * view * transform * vec4(pos, 1.0);\n\
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

Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, -3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstTimeUsingMouse = true;

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
	glfwSetCursorPosCallback(window, mouse_callback);

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

	HW5 hw5(WINDOW_WIDTH, WINDOW_HEIGHT);
	// ��������
	GLfloat* vertices = NULL;
	unsigned int* indices = NULL;
	int total_v = 0, total_ind = 0;
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	// ��ͼ
	v = hw5.getVertices();
	ind = hw5.getIndices();
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
	int modeChange = 1;
	bool enableDT = true, Orth_or_Pers = true;
	float O_left = -2.0f, O_right = 5.0f, O_bottom = -3.0f, O_top = 5.0f, O_near = 0.1f, O_far = 10.0f;
	float P_radians = 45.0f, P_near = 0.1f, P_far = 20.0f;
	float cameraRadius = 10.0f, cameraAngle = 0.0f;

	float lastFrame = 0.0f, deltaTime = 0.0f;

	// ��Ϸѭ��
	while (!glfwWindowShouldClose(window)) {
		// ����¼�
		glfwPollEvents();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		key_input(window, deltaTime);
		/*������*/
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
		ImGui::RadioButton("FPS Camera", &modeChange, 3);
		ImGui::RadioButton("O/P mode", &modeChange, 1);
		ImGui::RadioButton("View Change", &modeChange, 2);
		if (modeChange < 2) {
			ImGui::Checkbox("Orthographic/Perspective", &Orth_or_Pers);
		}
		ImGui::End();

		if (modeChange < 3) {
			ImGui::Begin("Camera");
			ImGui::SliderFloat("Radius", &cameraRadius, (float)0.0f, (float)50.0f);
			ImGui::End();
		}

		if (modeChange < 2) {
			if (Orth_or_Pers) {
				ImGui::Begin("Ortho");
				ImGui::SliderFloat("Left", &O_left, (float)-10.0f, (float)10.0f);
				ImGui::SliderFloat("Right", &O_right, (float)-10.0f, (float)10.0f);
				ImGui::SliderFloat("Bottom", &O_bottom, (float)-10.0f, (float)10.0f);
				ImGui::SliderFloat("Top", &O_top, (float)-10.0f, (float)10.0f);
				ImGui::SliderFloat("Near", &O_near, (float)0.0f, (float)10.0f);
				ImGui::SliderFloat("Far", &O_far, (float)0.0f, (float)20.0f);
				ImGui::End();
			}
			else {
				ImGui::Begin("Pers");
				ImGui::SliderFloat("Radians", &P_radians, (float)0.0f, (float)180.0f);
				ImGui::SliderFloat("Near", &P_near, (float)0.0f, (float)10.0f);
				ImGui::SliderFloat("Far", &P_far, (float)0.0f, (float)20.0f);
				ImGui::End();
			}
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
		glm::mat4 projection = glm::mat4(1.0f);
		
		if (modeChange == 1) {
			transform = glm::translate(transform, glm::vec3(-1.5f, 0.5f, -1.5f));
			view = glm::lookAt(glm::vec3((cameraRadius - 25.0f) / 5 , 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			if (Orth_or_Pers) {
				projection = glm::ortho(O_left, O_right, O_bottom, O_top, O_near, O_far);
			}
			else {
				projection = glm::perspective(glm::radians(P_radians), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, P_near, P_far);
			}
		}
		else if (modeChange == 2) {
			float camPosX = sin(glfwGetTime()) * cameraRadius;
			float camPosZ = cos(glfwGetTime()) * cameraRadius;
			view = glm::lookAt(glm::vec3(camPosX, 0.0f, camPosZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		}
		else{
			view = camera.getView();
			projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		}
		// ���任����Ӧ�õ�������
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		// ��������
		glBindVertexArray(VAO);
		// ����������
		glDrawElements(GL_TRIANGLES, total_ind * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// ���û�����ESC��,��������window���ڵ�WindowShouldClose����Ϊtrue
	// �ر�Ӧ�ó���
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void key_input(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstTimeUsingMouse) {
		lastX = xpos;
		lastY = ypos;
		firstTimeUsingMouse = false;
	}

	float xoffset = xpos - lastX,
		yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMove(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}