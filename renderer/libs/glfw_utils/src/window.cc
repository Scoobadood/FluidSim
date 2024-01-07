
#include "spdlog/spdlog.h"
#include <GLHelpers/GLHelpers.h>
#include <glfw_utils/window.h>

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void mouse_move_callback(GLFWwindow *window, double xpos, double ypos);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

Window::Window(uint32_t width, uint32_t height, const std::string &title) {
/* ******************************************************************************************
 *  Initialise OpenGL, GLEW and GLFW
 * ******************************************************************************************/
  if (!glfwInit()) {
    spdlog::critical("Failed to initialise GLFW");
    throw std::runtime_error("Failed to initialise GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
  window_ = glfwCreateWindow((int) width, (int) height,
                             title.c_str(),
                             nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    spdlog::critical("Failed to create window");
    throw std::runtime_error("Failed to create window");
  }

  // Make current context live
  glfwMakeContextCurrent(window_);

#ifndef __APPLE__
  // Need context before we do this.
GLenum err = glewInit();
if( GLEW_OK != err) {
  glfwTerminate();
  auto msg = fmt("Error: {}", (const char *)glewGetErrorString(err));
  spdlog::critical(msg);
  throw std::runtime_error(msg);
}
#endif

  glfwSwapInterval(1); // Enable vsync


  double mx, my;
  glfwGetCursorPos(window_, &mx, &my);

  glfwSetWindowUserPointer(window_, this);
  glfwSetMouseButtonCallback(window_, mouse_button_callback);
  glfwSetCursorPosCallback(window_, mouse_move_callback);
  glfwSetKeyCallback(window_, key_callback);
}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Window::ShouldClose() { return glfwWindowShouldClose(window_); }

void Window::SwapBuffers() {
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

void Window::GetFrameBufferSize(int32_t &width, int32_t &height) {
  glfwGetFramebufferSize(window_, &width, &height);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  double mx, my;
  glfwGetCursorPos(window, &mx, &my);
  auto w = (Window *) glfwGetWindowUserPointer(window);
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && w->right_mouse_press_handler_) {
      w->right_mouse_press_handler_((float) mx, (float) my);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && w->left_mouse_press_handler_) {
      w->left_mouse_press_handler_((float) mx, (float) my);
    }
  } else if (action == GLFW_RELEASE) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && w->right_mouse_release_handler_) {
      w->right_mouse_release_handler_((float) mx, (float) my);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && w->left_mouse_release_handler_) {
      w->left_mouse_release_handler_((float) mx, (float) my);
    }
  }
}

void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
  // Get the mouse buttons
  auto w = (Window *) glfwGetWindowUserPointer(window);
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    if (w->left_mouse_drag_handler_) {
      w->left_mouse_drag_handler_((float) xpos, (float) ypos);
    }
  } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    if (w->right_mouse_drag_handler_) {
      w->right_mouse_drag_handler_((float) xpos, (float) ypos);
    }
  }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  auto w = (Window *) glfwGetWindowUserPointer(window);
  if (action == GLFW_RELEASE && w->key_release_handlers_.count(key) > 0) {
    w->key_release_handlers_[key]();
  } else if (action == GLFW_PRESS && w->key_press_handlers_.count(key) > 0) {
    w->key_press_handlers_[key]();
  }
}

void Window::RegisterKeyReleaseHandler(int32_t key, const std::function<void()> &hdlr) {
  key_release_handlers_[key] = hdlr;
}

void Window::RegisterKeyPressHandler(int32_t key, const std::function<void()> &hdlr) {
  key_press_handlers_[key] = hdlr;
}

void Window::SetLeftMousePressHandler(const std::function<void(float, float)> &hdlr) {
  left_mouse_press_handler_ = hdlr;
}

void Window::SetRightMousePressHandler(const std::function<void(float, float)> &hdlr) {
  right_mouse_press_handler_ = hdlr;
}

void Window::SetRightMouseReleaseHandler(const std::function<void(float, float)> &hdlr) {
  right_mouse_release_handler_ = hdlr;
}
void Window::SetLeftMouseReleaseHandler(const std::function<void(float, float)> &hdlr) {
  left_mouse_release_handler_ = hdlr;
}

void Window::SetLeftMouseDragHandler(const std::function<void(float, float)> &hdlr) {
  left_mouse_drag_handler_ = hdlr;
}
void Window::SetRightMouseDragHandler(const std::function<void(float, float)> &hdlr) {
  right_mouse_drag_handler_ = hdlr;
}
