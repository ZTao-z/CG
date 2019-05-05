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

// homework
#include "hw3.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
	// 关闭应用程序
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// 创建顶点着色器
const char* shader = "#version 330 core\n\
						layout(location = 0) in vec3 pos;\n\
						layout(location = 1) in vec3 color;\n\
						out vec3 outColor;\n\
						void main() {\n\
							gl_Position = vec4(pos, 1.0);\n\
							outColor = color;\n\
						}\0";
// 创建片段着色器
const char* fragment = "#version 330 core\n\
						in vec3 outColor;\n\
						out vec4 color;\n\
						void main() {\n\
							color = vec4(outColor, 1.0f);\n\
						}\0";

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main() {
	// 实例化GLFW窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// 创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// OpenGL渲染窗口的尺寸大小
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 开始游戏循环之前注册函数至合适的回调(Esc退出)
	glfwSetKeyCallback(window, key_callback);

	// 初始化GLEW/GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 初始化ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 150");


	/*绘制图形*/
	//============================================
	// 编译顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &shader, NULL);
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

	// ImGUI控制部分(颜色)

	// 游戏循环
	unsigned int VBO, VAO;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	GLfloat Xa = 0.0f, Ya = 0.5f,
			Xb = -0.5f, Yb = -0.5f,
			Xc = 0.5f, Yc = -0.5f;
	GLfloat center_x = 0, center_y = 0;
	int radius = 100;
	bool isDrawLine = true,
		 fullfill = false;
	Bresenham bresenham(WINDOW_WIDTH, WINDOW_HEIGHT);
	while (!glfwWindowShouldClose(window)) {
		// 检查事件
		glfwPollEvents();

		// 顶点坐标
		GLfloat* vertices = NULL;
		int total = 0;
		std::vector<GLfloat> p;
		// 作图
		if (isDrawLine) {
			bresenham.drawTriangle(
				Xa, Ya,
				Xb, Yb,
				Xc, Yc,
				p,  fullfill
			);
		}
		else {
			bresenham.drawCircle(
				center_x, center_y,
				radius,   p
			);
		}
		total = p.size();
		vertices = new GLfloat[total];
		int pos = 0;
		for (unsigned int j = 0; j < p.size(); j++) {
			vertices[pos++] = p[j];
		}
		
		/*总流程*/
		// 使用glGenBuffers函数和一个缓冲ID生成一个VBO对象
		glGenBuffers(1, &VBO);
		// 使用glGenBuffers函数和一个缓冲ID生成一个VAO对象
		glGenVertexArrays(1, &VAO);
		// 绑定VAO
		glBindVertexArray(VAO);
		// 缓冲类型:顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 顶点数据复制到缓冲的内存中
		glBufferData(GL_ARRAY_BUFFER, total*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		// 顶点属性指针
		// 位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 颜色
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// 解除绑定
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		

		// ImGUI生成
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 设置GUI窗口内容
		ImGui::Begin("Display Choose");
		ImGui::Checkbox("Mode (default: triangle)", &isDrawLine);
		ImGui::End();
		if (isDrawLine) {
			ImGui::Begin("Line");
			ImGui::SliderFloat("Xa", &Xa, -1, 1);
			ImGui::SliderFloat("Ya", &Ya, -1, 1);
			ImGui::SliderFloat("Xb", &Xb, -1, 1);
			ImGui::SliderFloat("Yb", &Yb, -1, 1);
			ImGui::SliderFloat("Xc", &Xc, -1, 1);
			ImGui::SliderFloat("Yc", &Yc, -1, 1);
			ImGui::End();

			ImGui::Begin("Bonus");
			ImGui::Checkbox("Fullfill the triangle", &fullfill);
			ImGui::End();
		}
		else {
			ImGui::Begin("Circle");
			ImGui::SliderFloat("X", &center_x, -1, 1);
			ImGui::SliderFloat("Y", &center_y, -1, 1);
			ImGui::SliderInt("Radius", &radius, 2, 300);
			ImGui::End();
		}

		// 渲染指令
		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		// 清除窗口的移动轨迹
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// 使用着色器程序
		glUseProgram(shaderProgram);
		// 绘制物体
		glBindVertexArray(VAO);
		// 绘制点
		glDrawArrays(GL_POINTS, 0, total/6);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);

		// 交换缓冲
		glfwSwapBuffers(window);

		// 释放VBO，VAO，EBO
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		delete[] vertices;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	//============================================

	// 释放GLFW分配的内存
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
