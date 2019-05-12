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
#include "stb_image.h"
// homework
#include "hw7.h"


// 函数声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void key_input(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void renderScene(Shader &shader);
void renderCube();
unsigned int loadTexture(char const * path);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 阴影着色器
const char* simpleDept_vertex = "#version 330 core\n\
								layout(location = 0) in vec3 aPos;\n\
								uniform mat4 lightSpaceMatrix;\n\
								uniform mat4 model;\n\
								void main()\n\
								{\n\
									gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);\n\
								}";
const char* simpleDept_fragment = "#version 330 core\n\
									void main()\n\
									{\n\
										 gl_FragDepth = gl_FragCoord.z;\n\
									}\0";
// 创建光照顶点着色器
const char* Phong_light_vertex = "#version 330 core\n\
									layout(location = 0) in vec3 pos;\n\
									layout (location = 1) in vec3 aNormal;\n\
									layout(location = 2) in vec2 aTexCoords;\n\
									out VS_OUT {\n\
										vec3 FragPos;\n\
										vec3 Normal;\n\
										vec2 TexCoords;\n\
										vec4 FragPosLightSpace;\n\
									} vs_out;\n\
									uniform mat4 projection;\n\
									uniform mat4 view;\n\
									uniform mat4 model;\n\
									uniform mat4 lightSpaceMatrix;\n\
									void main() {\n\
										gl_Position = projection * view * model * vec4(pos, 1.0);\n\
										vs_out.FragPos = vec3(model * vec4(pos, 1.0));\n\
										vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;\n\
										vs_out.TexCoords = aTexCoords;\n\
										vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);\n\
									}\0";
// 创建光照片段着色器
const char* Phong_light_fragment = "#version 330 core\n\
									out vec4 FragColor;\n\
									in VS_OUT{\n\
										vec3 FragPos;\n\
										vec3 Normal;\n\
										vec2 TexCoords;\n\
										vec4 FragPosLightSpace;\n\
									} fs_in;\n\
									uniform sampler2D diffuseTexture;\n\
									uniform sampler2D shadowMap;\n\
									uniform vec3 lightPos;\n\
									uniform vec3 viewPos;\n\
									float ShadowCalculation(vec4 fragPosLightSpace)\n\
									{\n\
										// perform perspective divide\n\
										vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n\
										// transform to [0,1] range\n\
										projCoords = projCoords * 0.5 + 0.5;\n\
										// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)\n\
										float closestDepth = texture(shadowMap, projCoords.xy).r;\n\
										// get depth of current fragment from light's perspective\n\
										float currentDepth = projCoords.z;\n\
										// calculate bias (based on depth map resolution and slope)\n\
										vec3 normal = normalize(fs_in.Normal);\n\
										vec3 lightDir = normalize(lightPos - fs_in.FragPos);\n\
										float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);\n\
										// check whether current frag pos is in shadow\n\
										// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;\n\
										// PCF\n\
										float shadow = 0.0; \n\
										vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n\
										for (int x = -1; x <= 1; ++x)\n\
										{\n\
											for (int y = -1; y <= 1; ++y)\n\
											{\n\
												float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;\n\
												shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;\n\
											}\n\
										}\n\
										shadow /= 9.0;\n\
										// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.\n\
										if (projCoords.z > 1.0)\n\
											shadow = 0.0;\n\
										return shadow;\n\
									}\n\
									void main()\n\
									{\n\
										vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;\n\
										vec3 normal = normalize(fs_in.Normal);\n\
										vec3 lightColor = vec3(0.3);\n\
										// ambient\n\
										vec3 ambient = 0.3 * color; \n\
										// diffuse\n\
										vec3 lightDir = normalize(lightPos - fs_in.FragPos);\n\
										float diff = max(dot(lightDir, normal), 0.0);\n\
										vec3 diffuse = diff * lightColor;\n\
										// specular\n\
										vec3 viewDir = normalize(viewPos - fs_in.FragPos);\n\
										vec3 reflectDir = reflect(-lightDir, normal);\n\
										float spec = 0.0;\n\
										vec3 halfwayDir = normalize(lightDir + viewDir);\n\
										spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);\n\
										vec3 specular = spec * lightColor;\n\
										// calculate shadow\n\
										float shadow = ShadowCalculation(fs_in.FragPosLightSpace); \n\
										shadow = min(shadow, 0.75); \n\
										vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; \n\
										FragColor = vec4(lighting, 1.0); \n\
									};";

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
HW7 hw7(WINDOW_WIDTH, WINDOW_HEIGHT);

Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, -3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstTimeUsingMouse = true;

// meshes
unsigned int planeVAO;

GLfloat *cubeVertices = NULL;
unsigned int* cubeIndices = NULL;
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
unsigned int cubeEBO = 0;

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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	// 着色器
	Shader normalShader(Phong_light_vertex, Phong_light_fragment);
	Shader simpleDepthShader(simpleDept_vertex, simpleDept_fragment);

	// 贴图
	unsigned int woodTexture = loadTexture("./wood.png");

	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	// 顶点数组
	
	GLfloat* vertices = NULL;
	GLfloat* planeVertices = NULL;

	int total_p_v = 0;
	std::vector<GLfloat> p_v;
	// 作图
	// plane
	p_v = hw7.getPlaneVectices();
	total_p_v = p_v.size();
	planeVertices = new GLfloat[total_p_v];
	for (int i = 0; i < total_p_v; i++) {
		planeVertices[i] = p_v[i];
	}
	
	// 基础参数
	float cameraRadius = 10.0f, cameraAngle = 0.0f, objectAngle = 0.0f;
	float lastFrame = 0.0f, deltaTime = 0.0f;
	glm::vec3 lightColor(0.5f, 0.5f, 0.5f);
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	// 阴影
	// Setup plane VAO
	GLuint planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * total_p_v, planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// Configure depth map FBO
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	normalShader.use();
	normalShader.bindInt("diffuseTexture", 0);
	normalShader.bindInt("shadowMap", 1);

	// 模式选择
	int modeChange = 1, orth_pers = 1;
	bool isAutoMove = false;

	// 游戏循环
	while (!glfwWindowShouldClose(window)) {
		// 检查事件
		glfwPollEvents();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		key_input(window, deltaTime);

		// ImGUI控制部分
		// ImGUI生成
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 设置GUI窗口内容
		ImGui::Begin("Mode");
		ImGui::RadioButton("Normal", &modeChange, 1);
		ImGui::RadioButton("Bonus", &modeChange, 2);
		ImGui::End();

		ImGui::Begin("Light move mode");
		ImGui::Checkbox("Auto move", &isAutoMove);
		ImGui::SliderFloat("posX", &lightPos.x, -4.0f, 4.0f);
		ImGui::SliderFloat("posY", &lightPos.y, -4.0f, 4.0f);
		ImGui::SliderFloat("posZ", &lightPos.z, -4.0f, 4.0f);
		if (isAutoMove) {
			lightPos.x = sin(glfwGetTime()) * 4.0f;
			lightPos.z = cos(glfwGetTime()) * 4.0f;
			lightPos.y = 4.0f;
		}
		
		if (modeChange == 2) {
			ImGui::Begin("Light mode");
			ImGui::RadioButton("Orth", &orth_pers, 1);
			ImGui::RadioButton("Pers", &orth_pers, 2);
			ImGui::End();
		}
		ImGui::End();
		// 渲染指令
		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		// 清除窗口的移动轨迹
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		// 开启深度测试，清空深度测试缓存
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Render depth of scene to texture (from ligth's perspective)
		// - Get light projection/view matrix.
		glm::mat4 lightProjection = glm::mat4(1.0f), 
				lightView = glm::mat4(1.0f),
				lightSpaceMatrix = glm::mat4(1.0f);
		GLfloat near_plane = 1.0f, far_plane = 7.5f;
		if (orth_pers == 1) {
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		}
		else {
			lightProjection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane/2, far_plane*2);
		}
		
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// - now render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.bindMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, woodTexture);
			glCullFace(GL_FRONT);
			renderScene(simpleDepthShader);
			glCullFace(GL_BACK); // 不要忘记设回原先的culling face
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// GLM变换
		// reset viewport
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		normalShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.getView();
		glm::vec3 camPos = camera.getPosition();
		normalShader.bindMat4("projection", projection);
		normalShader.bindMat4("view", view);
		// set light uniforms
		normalShader.bindVec3("viewPos", camPos);
		normalShader.bindVec3("lightPos", lightPos);
		normalShader.bindMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(normalShader);


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);

		// 交换缓冲
		glfwSwapBuffers(window);
	}
	
	delete[] planeVertices;
	delete[] cubeVertices;

	// 释放VBO，VAO，EBO
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
	// 关闭应用程序
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
		yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMove(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void renderScene(Shader &shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.bindMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.bindMat4("model", model);
	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.bindMat4("model", model);
	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.bindMat4("model", model);
	renderCube();
}

void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		std::vector<GLfloat> v = hw7.getVertices2();
		cubeVertices = new GLfloat[v.size()];
		for (unsigned int i = 0; i < v.size(); i++) {
			cubeVertices[i] = v[i];
		}
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * v.size(), cubeVertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderSceneEBO() {
	int indSize = 0;
	if (cubeVAO == 0)
	{
		std::vector<GLfloat> v = hw7.getVertices();
		std::vector<unsigned int> ind = hw7.getIndices();
		cubeVertices = new GLfloat[v.size()];
		cubeIndices = new unsigned int[ind.size()];
		indSize = ind.size();
		for (unsigned int i = 0; i < v.size(); i++) {
			cubeVertices[i] = v[i];
		}
		for (unsigned int i = 0; i < ind.size(); i++) {
			cubeIndices[i] = ind[i];
		}
		/*总流程*/
		// 使用glGenBuffers函数和一个缓冲ID生成一个VBO对象
		glGenBuffers(1, &cubeVBO);
		// 使用glGenBuffers函数和一个缓冲ID生成一个VAO对象
		glGenVertexArrays(1, &cubeVAO);
		// 使用glGenBuffers函数和一个缓冲ID生成一个EBO对象
		glGenBuffers(1, &cubeEBO);
		// 绑定VAO
		glBindVertexArray(cubeVAO);
		// 缓冲类型:顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		// 顶点数据复制到缓冲的内存中
		glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GLfloat), cubeVertices, GL_STATIC_DRAW);
		// 缓冲类型:索引缓冲
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		// 索引数据复制到缓冲的内存中
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(unsigned int), cubeIndices, GL_STATIC_DRAW);
		// 顶点属性指针
		// 位置
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 颜色
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// 单位法向量
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// 解除绑定
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	// 绘制物体
	glBindVertexArray(cubeVAO);
	// 绘制立方体
	glDrawElements(GL_TRIANGLES, indSize * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}