#include "object.h"

#include "spdlog/spdlog-inl.h"

#include "main.h"

#include "keyboard_handler.h"
#include "scene.h"

void mouse_handler(GLFWwindow *window, int button, int action, int mods) {}

void drag_handler(int32_t x, int32_t y) {}

void mouse_motion_handler(int32_t x, int32_t y) {}

void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height) {}

void window_reshape_handler(GLFWwindow *window, int32_t width, int32_t height) {}

void idle_handler() {}

/*
 * Main
 */
int main(int argc, char *argv[]) {
  if (!glfwInit()) return EXIT_FAILURE;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

  auto window = glfwCreateWindow(800, 600,
                                 "Sim",
                                 nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    spdlog::critical("Failed to create window");
    return EXIT_FAILURE;
  }

  // Make current context live
  glfwMakeContextCurrent(window);

#ifndef __APPLE__
  // Need context before we do this.
  GLenum err = glewInit();
  if( GLEW_OK != err) {
    spdlog::critical("Error: {}", (const char *)glewGetErrorString(err));
  }
#endif

  glfwSwapInterval(1); // Enable vsync

  glfwSetWindowSizeCallback(window, window_reshape_handler);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, special_keyboard_handler);


  Scene scene{};
  glfwSetWindowUserPointer(window, &scene);

  while (!glfwWindowShouldClose(window)) {
    scene.Render();

    idle_handler();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;

}