#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "MyGlWindow.hh"

std::unique_ptr<MyGlWindow> win;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;
double m_lastMouseX;
double m_lastMouseY;
double cx, cy;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	win->resize(width, height);
	win->setAspect(width / (float)height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mbutton_down = true;
		else if (GLFW_RELEASE == action)
			mbutton_down = false;
	}
}

void mouseDragging(double width, double height)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		if (lbutton_down)
		{
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->getViewer()->rotate(fractionChangeX, fractionChangeY);
		}
		else if (mbutton_down)
		{
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->getViewer()->zoom(fractionChangeY);
		}
		else if (rbutton_down)
		{
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->getViewer()->translate(-fractionChangeX, -fractionChangeY, 1);
		}
	}

	m_lastMouseX = cx;
	m_lastMouseY = cy;
}

int main(int, char**)
{
	/* Initialize the library */
	if (!glfwInit())
	{
		// Initialization failed
		std::cerr << "Initialization failed !" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 800;
	int height = 800;
	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL FrameWork", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	const char* glsl_version = "#version 430";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	glfwMakeContextCurrent(window);
	/* Make the window's context current */
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		//Problem: glewInit failed, something is seriously wrong.
		std::cout << "glewInit failed, aborting." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSwapInterval(1);  //enable vsync

	std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	try {
		win = std::make_unique<MyGlWindow>(width, height);
	}
	catch (const std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
	}

	double previousTime = glfwGetTime();
	int frameCount = 0;
	double lastTime = 0;
	float lightPos[3] = { win->lightPosition.x, win->lightPosition.y, win->lightPosition.z };

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);

	while (!glfwWindowShouldClose(window) && win)
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Control Panel"), window_flags) {
			//ImGui::SetWindowSize(ImVec2(400, 200));

			ImGui::SliderFloat3("Light Position", lightPos, -10.0f, 10.0f, "v = %.1f");
			{
				win->lightPosition = glm::vec3(lightPos[0], lightPos[1], lightPos[2]);
			}

			ImGui::End();
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		win->draw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();

		mouseDragging(win->getWidth(), win->getHeight());
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}