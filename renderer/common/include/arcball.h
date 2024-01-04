//
// Created by Dave Durbin on 1/1/2024.
//

#ifndef FLUIDSIM_ARCBALL_H
#define FLUIDSIM_ARCBALL_H

#include "glm/detail/type_vec3.hpp"
#include "glm/fwd.hpp"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/detail/type_quat.hpp"

class ArcBall {
public:
  ArcBall(float width, float height);
  void DragStarted(float x, float y);
  void DragEnded();
  void Drag(float x, float y);
  glm::mat4 Rotation();
  void SetRotation(const glm::mat4& rotation);
private:
  glm::vec<3, float> PointToSphere(float x, float y) const;
  float radius_;
  float centre_x_;
  float centre_y_;
  bool is_dragging_;
  glm::vec3 drag_start_sphere_coord_;
  glm::quat drag_start_quat_;
  glm::mat4 rotation_;
};
#endif //FLUIDSIM_ARCBALL_H
