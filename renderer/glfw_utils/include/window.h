//
// Created by Dave Durbin on 4/1/2024.
//

#ifndef FLUIDSIM_WINDOW_H
#define FLUIDSIM_WINDOW_H

#include "GLFW/glfw3.h"
#include <string>
#include <map>

class Window {
public:
  Window(uint32_t width, uint32_t height, const std::string &title);

  ~Window();

  bool ShouldClose();

  void GetFrameBufferSize(int32_t &width, int32_t &height);

  void SwapBuffers();

  void SetRightMousePressHandler(const std::function<void(float, float)> &hdlr);

  void SetRightMouseReleaseHandler(const std::function<void(float, float)> &hdlr);

  void RegisterKeyReleaseHandler(int32_t key, const std::function<void()>&);
  void RegisterKeyPressHandler(int32_t key, const std::function<void()>&);

  void SetMouseMoveHandler(const std::function<void(float, float)> &hdlr);

private:
  GLFWwindow *window_;
  std::function<void(float, float)> right_mouse_press_handler_;
  std::function<void(float, float)> right_mouse_release_handler_;
  std::function<void(float, float)> left_mouse_press_handler_;
  std::function<void(float, float)> left_mouse_release_handler_;
  std::function<void(float, float)> mouse_move_handler_;
  std::map<int32_t, std::function<void()>> key_release_handlers_;
  std::map<int32_t, std::function<void()>> key_press_handlers_;


  friend void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

  friend void mouse_move_callback(GLFWwindow *window, double xpos, double ypos);

  friend void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

};

#endif //FLUIDSIM_WINDOW_H
