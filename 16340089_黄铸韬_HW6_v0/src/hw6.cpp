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
#include "hw6.h"



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void key_input(GLFWwindow* window, float deltaTime);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// ����������ɫ��
const char* Gouraud_light_vertex = "#version 330 core\n\
						layout(location = 0) in vec3 pos;\n\
						layout(location = 1) in vec3 color;\n\
						layout(location = 2) in vec3 aNormal;\n\
						struct Light {\n\
							vec3 ambient;\n\
							vec3 diffuse;\n\
							vec3 specular;\n\
						};\n\
						uniform mat4 projection;\n\
						uniform mat4 view;\n\
						uniform mat4 transform;\n\
						uniform vec3 lightPos;\n\
						uniform vec3 viewPos;\n\
						uniform vec3 lightColor;\n\
						uniform Light light;\n\
						out vec3 outColor;\n\
						out vec3 outLightColor;\n\
						void main() {\n\
							gl_Position = projection * view * transform * vec4(pos, 1.0);\n\
							outColor = color;\n\
							vec3 Position = vec3(transform * vec4(pos, 1.0));\n\
							vec3 Normal = mat3(transpose(inverse(transform))) * aNormal;\n\
							// ambient \n\
							vec3 ambient = light.ambient * lightColor; \n\
							// diffuse \n\
							vec3 norm = normalize(Normal);\n\
							vec3 lightDir = normalize(lightPos - Position); \n\
							float diff = max(dot(norm, lightDir), 0.0); \n\
							vec3 diffuse = light.diffuse * (diff * lightColor); \n\
							// specular \n\
							vec3 viewDir = normalize(viewPos - Position);\n\
							vec3 reflectDir = reflect(-lightDir, norm);\n\
							float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n\
							vec3 specular = light.specular * (spec * lightColor);\n\
							outLightColor = ambient + diffuse + specular; \n\
						}\0";
// ����Ƭ����ɫ��
const char* Gouraud_light_fragment = "#version 330 core\n\
						in vec3 outColor;\n\
						in vec3 outLightColor;\n\
						out vec4 color;\n\
						void main() {\n\
							color = vec4(outLightColor * outColor, 1.0f);\n\
						}\0";
// �������ն�����ɫ��
const char* Phong_light_vertex = "#version 330 core\n\
						layout(location = 0) in vec3 pos;\n\
						layout(location = 1) in vec3 color;\n\
						layout(location = 2) in vec3 aNormal;\n\
						out vec3 outColor;\n\
						out vec3 Normal;\n\
						out vec3 FragPos;\n\
						uniform mat4 projection;\n\
						uniform mat4 view;\n\
						uniform mat4 transform;\n\
						void main() {\n\
							gl_Position = projection * view * transform * vec4(pos, 1.0);\n\
							outColor = color;\n\
							FragPos = vec3(transform * vec4(pos,1.0));\n\
							Normal = mat3(transpose(inverse(transform))) * aNormal;\n\
						}\0";
// ��������Ƭ����ɫ��
const char* Phong_light_fragment = "#version 330 core\n\
						struct Light {\n\
							vec3 ambient;\n\
							vec3 diffuse;\n\
							vec3 specular;\n\
						};\n\
						in vec3 outColor;\n\
						in vec3 Normal;\n\
						in vec3 FragPos;\n\
						uniform vec3 lightPos;\n\
						uniform vec3 viewPos;\n\
						uniform vec3 lightColor;\n\
						uniform Light light;\n\
						out vec4 color;\n\
						void main() {\n\
							// ambient \n\
							vec3 ambient = light.ambient * lightColor;\n\
							// diffuse \n\
							vec3 norm = normalize(Normal);\n\
							vec3 lightDir = normalize(lightPos - FragPos);\n\
							float diff = max(dot(norm, lightDir), 0.0);\n\
							vec3 diffuse = light.diffuse * (diff * lightColor);\n\
							// specular \n\
							vec3 viewDir = normalize(viewPos - FragPos);\n\
							vec3 reflectDir = reflect(-lightDir, norm);\n\
							float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n\
							vec3 specular = light.specular * (spec * lightColor);\n\
							vec3 result = (ambient + diffuse + specular) * outColor;\n\
							color = vec4(result, 1.0f);\n\
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
	// Phong
	unsigned int vertexShader_Phong = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader_Phong, 1, &Phong_light_vertex, NULL);
	glCompileShader(vertexShader_Phong);
	// ����Ƿ�ɹ�����
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader_Phong, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader_Phong, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}
	// Gouraud
	unsigned int vertexShader_Gouraud = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader_Gouraud, 1, &Gouraud_light_vertex, NULL);
	glCompileShader(vertexShader_Gouraud);
	// ����Ƿ�ɹ�����
	glGetShaderiv(vertexShader_Gouraud, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader_Gouraud, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}

	// ����Ƭ����ɫ��
	// Phong
	unsigned int fragmentShader_Phong = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_Phong, 1, &Phong_light_fragment, NULL);
	glCompileShader(fragmentShader_Phong);
	// ����Ƿ�ɹ�����
	glGetShaderiv(fragmentShader_Phong, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader_Phong, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}
	// Gouraud
	unsigned int fragmentShader_Gouraud = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_Gouraud, 1, &Gouraud_light_fragment, NULL);
	glCompileShader(fragmentShader_Gouraud);
	// ����Ƿ�ɹ�����
	glGetShaderiv(fragmentShader_Gouraud, GL_COMPILE_STATUS, &success);
	if (!success) {
	glGetShaderInfoLog(fragmentShader_Gouraud, 512, NULL, infoLog);
	std::cout << "ERROR: " << infoLog << std::endl;
	}

	// ������ɫ������
	// Phong
	unsigned int shaderProgram_Phong = glCreateProgram();
	glAttachShader(shaderProgram_Phong, vertexShader_Phong);
	glAttachShader(shaderProgram_Phong, fragmentShader_Phong);
	glLinkProgram(shaderProgram_Phong);
	glGetProgramiv(shaderProgram_Phong, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_Phong, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}
	// ɾ����ɫ������
	glDeleteShader(vertexShader_Phong);
	glDeleteShader(fragmentShader_Phong);

	// Gouraud
	unsigned int shaderProgram_Gouraud = glCreateProgram();
	glAttachShader(shaderProgram_Gouraud, vertexShader_Gouraud);
	glAttachShader(shaderProgram_Gouraud, fragmentShader_Gouraud);
	glLinkProgram(shaderProgram_Gouraud);
	glGetProgramiv(shaderProgram_Gouraud, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_Gouraud, 512, NULL, infoLog);
		std::cout << "ERROR: " << infoLog << std::endl;
	}
	// ɾ����ɫ������
	glDeleteShader(vertexShader_Gouraud);
	glDeleteShader(fragmentShader_Gouraud);

	// ImGUI���Ʋ���
	unsigned int VBO, VAO, EBO;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	HW6 hw6(WINDOW_WIDTH, WINDOW_HEIGHT);
	// ��������
	GLfloat* vertices = NULL;
	unsigned int* indices = NULL;
	int total_v = 0, total_ind = 0;
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	// ��ͼ
	v = hw6.getVertices();
	ind = hw6.getIndices();
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
	float cameraRadius = 10.0f, cameraAngle = 0.0f, objectAngle = 0.0f;
	float lastFrame = 0.0f, deltaTime = 0.0f;

	glm::vec3 lightColor = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightPos = glm::vec3(2.0f, 0.0f, -2.0f);

	GLfloat ambient_Factor = 0.5f,
			  diffuse_Factor = 1.0f, 
			  specular_Factor = 0.9f;

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// ��ɫ
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ��λ������
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// �����
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ImGUI����
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ����GUI��������
		ImGui::Begin("Mode");
		ImGui::RadioButton("Phong", &modeChange, 1);
		ImGui::RadioButton("Gouraud", &modeChange, 2);
		ImGui::RadioButton("Bonus", &modeChange, 3);
		ImGui::End();

		ImGui::Begin("Object View");
		ImGui::SliderFloat("Angle", &objectAngle, 0.0f, 6.28f);
		ImGui::End();
		
		ImGui::Begin("Light");
		ImGui::ColorEdit3("Color", (float*)&lightColor);
		ImGui::BeginChild("Position");
		ImGui::SliderFloat("posX", &lightPos.x, -2.0f, 2.0f);
		ImGui::SliderFloat("posY", &lightPos.y, -2.0f, 2.0f);
		ImGui::SliderFloat("posZ", &lightPos.z, -2.0f, 2.0f);
		ImGui::EndChild();
		ImGui::End();

		ImGui::Begin("Effect");
		ImGui::SliderFloat("ambient", &ambient_Factor, 0.0f, 1.0f);
		ImGui::SliderFloat("diffuse", &diffuse_Factor, 0.0f, 1.0f);
		ImGui::SliderFloat("specular", &specular_Factor, 0.0f, 1.0f);
		ImGui::End();

		// ��Ⱦָ��
		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		// ������ڵ��ƶ��켣
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		// ������Ȳ���
		glEnable(GL_DEPTH_TEST);
		// ������Ȳ��ԣ������Ȳ��Ի���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// GLM�任
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		glm::vec3 lightView = camera.getPosition();

		if (modeChange == 3) {
			lightPos.x = sin(glfwGetTime()) * 3.0f;
			lightPos.z = cos(glfwGetTime()) * 3.0f;
			view = glm::lookAt(glm::vec3(3.0f, 3.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			transform = glm::rotate(transform, (float)objectAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			view = glm::lookAt(glm::vec3(3.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		glm::vec3 a_vec = ambient_Factor * glm::vec3(1.0f),
			d_vec = diffuse_Factor * glm::vec3(1.0f),
			s_vec = specular_Factor * glm::vec3(1.0f);

		if (modeChange == 1 || modeChange == 3) {
			// ʹ����ɫ������
			glUseProgram(shaderProgram_Phong);
			// ���任����Ӧ�õ�������
			// λ��
			unsigned int transformLoc = glGetUniformLocation(shaderProgram_Phong, "transform");
			unsigned int viewLoc = glGetUniformLocation(shaderProgram_Phong, "view");
			unsigned int projectionLoc = glGetUniformLocation(shaderProgram_Phong, "projection");

			// ����
			unsigned int lightLoc = glGetUniformLocation(shaderProgram_Phong, "lightColor");
			unsigned int lightViewLoc = glGetUniformLocation(shaderProgram_Phong, "viewPos");
			unsigned int lightPosLoc = glGetUniformLocation(shaderProgram_Phong, "lightPos");

			// ���ղ���
			unsigned int ambient = glGetUniformLocation(shaderProgram_Phong, "light.ambient");
			unsigned int diffuse = glGetUniformLocation(shaderProgram_Phong, "light.diffuse");
			unsigned int specular = glGetUniformLocation(shaderProgram_Phong, "light.specular");

			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
			glUniform3fv(lightLoc, 1, glm::value_ptr(lightColor));
			glUniform3fv(lightViewLoc, 1, &lightView[0]);
			glUniform3fv(lightPosLoc, 1, &lightPos[0]);
			glUniform3fv(ambient, 1, &a_vec[0]);
			glUniform3fv(diffuse, 1, &d_vec[0]);
			glUniform3fv(specular, 1, &s_vec[0]);
		}
		else {
			// ʹ����ɫ������
			glUseProgram(shaderProgram_Gouraud);
			// ���任����Ӧ�õ�������
			// λ��
			unsigned int transformLoc = glGetUniformLocation(shaderProgram_Gouraud, "transform");
			unsigned int viewLoc = glGetUniformLocation(shaderProgram_Gouraud, "view");
			unsigned int projectionLoc = glGetUniformLocation(shaderProgram_Gouraud, "projection");

			// ����
			unsigned int lightLoc = glGetUniformLocation(shaderProgram_Gouraud, "lightColor");
			unsigned int lightViewLoc = glGetUniformLocation(shaderProgram_Gouraud, "viewPos");
			unsigned int lightPosLoc = glGetUniformLocation(shaderProgram_Gouraud, "lightPos");

			// ���ղ���
			unsigned int ambient = glGetUniformLocation(shaderProgram_Gouraud, "light.ambient");
			unsigned int diffuse = glGetUniformLocation(shaderProgram_Gouraud, "light.diffuse");
			unsigned int specular = glGetUniformLocation(shaderProgram_Gouraud, "light.specular");

			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
			glUniform3fv(lightLoc, 1, glm::value_ptr(lightColor));
			glUniform3fv(lightViewLoc, 1, &lightView[0]);
			glUniform3fv(lightPosLoc, 1, &lightPos[0]);
			glUniform3fv(ambient, 1, &a_vec[0]);
			glUniform3fv(diffuse, 1, &d_vec[0]);
			glUniform3fv(specular, 1, &s_vec[0]);
		}
		
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