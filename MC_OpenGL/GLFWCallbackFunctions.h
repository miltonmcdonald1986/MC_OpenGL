#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace MC_OpenGL {


auto GLFWCallbackFramebufferSize (GLFWwindow *window, int width, int height) -> void;
auto GlfwCallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods) -> void;
auto GlfwCallbackCursorEnter (GLFWwindow *window, int entered) -> void;
auto GlfwCallbackCursorPos (GLFWwindow *window, double xpos, double ypos) -> void;
auto GlfwCallbackMouseButton (GLFWwindow* window, int button, int action, int mods) -> void;
auto GlfwCallbackScroll(GLFWwindow* window, double xoffset, double yoffset) -> void;


}
