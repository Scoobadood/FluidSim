#include "object.h"

#include "spdlog/spdlog-inl.h"

#include "main.h"

void special_keyboard_handler(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
    auto o =(Object *) glfwGetWindowUserPointer(window);
      o->set_view_dist(o->view_dist() - 5.0f);
    return;
  }
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS && (mods & GLFW_MOD_SHIFT)) {
    auto o =(Object *) glfwGetWindowUserPointer(window);
    o->set_view_dist(o->view_dist() + 5.0f);
    return;
  }
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
      ((Object *) glfwGetWindowUserPointer(window))->set_view_dir(Object::POS_Z);
    } else {
      ((Object *) glfwGetWindowUserPointer(window))->set_view_dir(Object::NEG_Z);
    }
    return;
  }
  if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
      ((Object *) glfwGetWindowUserPointer(window))->set_view_dir(Object::POS_Y);
    } else {
      ((Object *) glfwGetWindowUserPointer(window))->set_view_dir(Object::NEG_Y);
    }
    return;
  }
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
      ((Object *) glfwGetWindowUserPointer(window))->set_view_dir(Object::POS_X);
    } else {
      ((Object *) glfwGetWindowUserPointer(window))->set_view_dir(Object::NEG_X);
    }
    return;
  }
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
      ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(0);
      return;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(1);
    return;
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(2);
    return;
  }
  if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(3);
    return;
  }
  if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(4);
    return;
  }
  if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(5);
    return;
  }
  if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(6);
    return;
  }
  if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
    ((Object *) glfwGetWindowUserPointer(window))->toggle_spot(7);
    return;
  }
  if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
      ((Object *) glfwGetWindowUserPointer(window))->big_light_on(true);
    } else {
      ((Object *) glfwGetWindowUserPointer(window))->big_light_on(false);
    }
  }
}

void mouse_handler(GLFWwindow *window, int button, int action, int mods) {}

void drag_handler(int32_t x, int32_t y) {}

void mouse_motion_handler(int32_t x, int32_t y) {}

void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height) {}

void window_reshape_handler(GLFWwindow *window, int32_t width, int32_t height) {}

void idle_handler() {}

int main(int argc, char *argv[]) {
  if( argc == 1) {
    spdlog::error("Missing argument");
    return EXIT_FAILURE;
  }
  if (!glfwInit()) return EXIT_FAILURE;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);


  // Window creation
  auto window = glfwCreateWindow(800, 600,
                                 "My Window",
                                 nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Create window with graphics context
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


  Object obj{argv[1], true, true};

  glfwSetWindowUserPointer(window, &obj);

  while (!glfwWindowShouldClose(window)) {
    obj.main_loop();

    idle_handler();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;

}