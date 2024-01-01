#include "GLFW/glfw3.h"
#include "scene.h"
//
// Created by Dave Durbin on 1/1/2024.
//
void special_keyboard_handler(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
    auto o =(Scene *) glfwGetWindowUserPointer(window);
//    o->set_view_dist(o->view_dist() - 5.0f);
    return;
  }
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS && (mods & GLFW_MOD_SHIFT)) {
    auto o =(Scene *) glfwGetWindowUserPointer(window);
//    o->set_view_dist(o->view_dist() + 5.0f);
    return;
  }
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
//      ((Scene *) glfwGetWindowUserPointer(window))->set_view_dir(Scene::POS_Z);
    } else {
//      ((Scene *) glfwGetWindowUserPointer(window))->set_view_dir(Scene::NEG_Z);
    }
    return;
  }
  if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
//      ((Scene *) glfwGetWindowUserPointer(window))->set_view_dir(Scene::POS_Y);
    } else {
//      ((Scene *) glfwGetWindowUserPointer(window))->set_view_dir(Scene::NEG_Y);
    }
    return;
  }
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
//      ((Scene *) glfwGetWindowUserPointer(window))->set_view_dir(Scene::POS_X);
    } else {
//      ((Scene *) glfwGetWindowUserPointer(window))->set_view_dir(Scene::NEG_X);
    }
    return;
  }
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(0);
    return;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(1);
    return;
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(2);
    return;
  }
  if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(3);
    return;
  }
  if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(4);
    return;
  }
  if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(5);
    return;
  }
  if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(6);
    return;
  }
  if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
//    ((Scene *) glfwGetWindowUserPointer(window))->toggle_spot(7);
    return;
  }
  if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    if (mods & GLFW_MOD_SHIFT) {
//      ((Scene *) glfwGetWindowUserPointer(window))->big_light_on(true);
    } else {
//      ((Scene *) glfwGetWindowUserPointer(window))->big_light_on(false);
    }
  }
}