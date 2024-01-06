#include "arcball.h"
#include "glm/gtc/quaternion.hpp"

ArcBall::ArcBall(float width, float height) //
        : is_dragging_{false}//
        , drag_start_sphere_coord_{glm::vec3(0, 0, 1)}//;


{
  centre_x_ = (float) width / 2.0f;
  centre_y_ = (float) height / 2.0f;
  radius_ = std::fminf(centre_y_, centre_x_);
  rotation_ = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
}

/*
 * Given a point on the screen, map it onto the sphere which projects
 * onto the screen.
 */
glm::vec<3, float> ArcBall::PointToSphere(float x, float y) const {
  auto px = (x - centre_x_) / radius_;
  auto py = (centre_y_ - y) / radius_;
  double pz = 0.0;
  auto r2 = px * px + py * py;
  if (r2 > 1) {
    auto scale = 1.0f / std::sqrtf(r2);
    px *= scale;
    py *= scale;
  } else {
    pz = std::sqrt(1.0 - r2);
  }
  return glm::normalize(glm::vec3(px, py, pz));
}

void ArcBall::DragStarted(float x, float y) {
  is_dragging_ = true;
  drag_start_sphere_coord_ = PointToSphere(x, y);
  drag_start_quat_ = glm::quat_cast(rotation_);
}

void ArcBall::DragEnded() {
  is_dragging_ = false;
}

void ArcBall::Drag(float x, float y) {
  if (!is_dragging_) return;

  auto p = PointToSphere(x, y);
  auto q_xyz = glm::cross(drag_start_sphere_coord_, p);
  auto q_w = glm::dot(drag_start_sphere_coord_, p);

  auto s = std::sqrtf((1.0f + q_w) * 2.0f);
  glm::quat q_xform(0.5f * s, q_xyz / s);

  auto q_now = drag_start_quat_ * q_xform;
  q_now /= length(q_now);

  rotation_ = glm::mat4_cast(q_now);
}

glm::mat4 ArcBall::Rotation() { return rotation_; }

void ArcBall::SetRotation(const glm::mat4 &rotation) { rotation_ = rotation; }

