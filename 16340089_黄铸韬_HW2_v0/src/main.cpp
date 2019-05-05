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
const char* shader =  "#version 330 core\n\
						layout(location = 0) in vec3 pos;\n\
						layout(location = 1) in vec3 color;\n\
						out vec3 outColor;\n\
						void main() {\n\
							gl_Position = vec4(pos, 1.0);\n\
							outColor = color;\n\
						}\0";
// ����Ƭ����ɫ��
const char* fragment = "#version 330 core\n\
						in vec3 outColor;\n\
						out vec4 color;\n\
						void main() {\n\
							color = vec4(outColor, 1.0f);\n\
						}\0";

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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
	

	/*����������*/
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

	// ImGUI���Ʋ���(��ɫ)
	ImVec4 topColor0 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 leftColor0 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 rightColor0 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 topColor1 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 leftColor1 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 rightColor1 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 sameColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	unsigned int indices[] = { // ע��������0��ʼ
		0, 1, 2, // ��һ��������
		3, 4, 5  // �ڶ���������
	};

	// ��Ϸѭ��
	unsigned int VBO, VAO, EBO;
	bool show_normal_triangle = true, 
		 show_bonus_triangle = false, 
		 show_bonus_line = false,
		 show_bonus_point = false,
		 change_all_color = false;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	while (!glfwWindowShouldClose(window)) {
		// ����¼�
		glfwPollEvents();

		ImVec4 left, right, top;
		left = change_all_color ? sameColor : leftColor0;
		right = change_all_color ? sameColor : rightColor0;
		top = change_all_color ? sameColor : topColor0;
		// ��������
		GLfloat vertices[] = {
			// λ��(x,y,z)       // ��ɫ(r,g,b)
			-0.5f, -0.5f, 0.0f, left.x, left.y, left.z, // ����
			0.5f, -0.5f, 0.0f, right.x, right.y, right.z, // ����
			0.0f,  0.5f, 0.0f, top.x, top.y, top.z,  // ��
			-0.25f, -0.9f, 0.0f, leftColor1.x, leftColor1.y, leftColor1.z, // ����
			0.25f, -0.9f, 0.0f, rightColor1.x, rightColor1.y, rightColor1.z, // ����
			0.0f,   -0.5f, 0.0f, topColor1.x, topColor1.y, topColor1.z  // ��
		};

		/*������*/
		// ʹ��glGenBuffers������һ������ID����һ��VBO����
		glGenBuffers(1, &VBO);
		// ʹ��glGenBuffers������һ������ID����һ��EBO����
		glGenBuffers(1, &EBO);
		// ʹ��glGenBuffers������һ������ID����һ��VAO����
		glGenVertexArrays(1, &VAO);
		// ��VAO
		glBindVertexArray(VAO);
		// ��������:���㻺��
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// �������ݸ��Ƶ�������ڴ���
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// ��������:��������
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// �������ݸ��Ƶ�������ڴ���
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
		bool showWindow = true;
		if (showWindow) {
			if (!change_all_color) {
				ImGui::Begin("Color Choose");
				ImGui::BeginChild("triangle 1", ImVec2(380, 90));
				ImGui::Text("Normal triangle");
				ImGui::ColorEdit3("top color", (float*)&topColor0);
				ImGui::ColorEdit3("bottom left color", (float*)&leftColor0);
				ImGui::ColorEdit3("bottom right color", (float*)&rightColor0);
				ImGui::EndChild();
				ImGui::BeginChild("triangle 2", ImVec2(380, 90));
				ImGui::Text("Bonus triangle");
				ImGui::ColorEdit3("top color", (float*)&topColor1);
				ImGui::ColorEdit3("bottom left color", (float*)&leftColor1);
				ImGui::ColorEdit3("bottom right color", (float*)&rightColor1);
				ImGui::EndChild();
				ImGui::End();
			}

			ImGui::Begin("Display option", &showWindow);
			ImGui::Checkbox("change whole triangle", &change_all_color);
			ImGui::Text("(Single Choice)");
			ImGui::Checkbox("normal triangle", &show_normal_triangle);
			ImGui::Checkbox("bonus line", &show_bonus_line);
			ImGui::Checkbox("bonus point", &show_bonus_point);
			ImGui::Checkbox("bonus triangle", &show_bonus_triangle);
			ImGui::End();

			if (change_all_color) {
				ImGui::Begin("Whole triangle");
				ImGui::ColorEdit3("All color", (float*)&sameColor);
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
		// ʹ����ɫ������
		glUseProgram(shaderProgram);
		// ��������
		glBindVertexArray(VAO);
		if (show_normal_triangle) {
			show_bonus_triangle = false;
			show_bonus_line = false;
			show_bonus_point = false;
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);
		}
		if (show_bonus_triangle) {
			show_normal_triangle = false;
			show_bonus_line = false;
			show_bonus_point = false;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		}
		if (show_bonus_line) {
			show_normal_triangle = false;
			show_bonus_triangle = false;
			show_bonus_point = false;
			// bonus lineΪ���������εĵױ�
			glDrawArrays(GL_LINE_STRIP, 0, 2);
			glDrawArrays(GL_LINE_STRIP, 3, 2);
		}
		if (show_bonus_point) {
			show_normal_triangle = false;
			show_bonus_triangle = false;
			show_bonus_line = false;
			// bonus pointΪ���������εĶ���
			glDrawArrays(GL_POINTS, 0, 6);
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
