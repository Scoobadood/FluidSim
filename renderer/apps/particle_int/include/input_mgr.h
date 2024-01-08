//
// Created by Dave Durbin on 7/1/2024.
//

#ifndef FLUIDSIM_INPUT_MGR_H_
#define FLUIDSIM_INPUT_MGR_H_

#include <cstdint>
#include <GLHelpers/GLHelpers.h>
#include "common/arcball.h"
#include "particles/particle_system.h"

class InputManager {
 public:
  InputManager(uint32_t window_width, uint32_t window_height);

  void BeginViewDrag(uint32_t x, uint32_t y);
  void DragView(uint32_t x, uint32_t y);
  void EndViewDrag();

  bool BeginObjectSelection(uint32_t x, uint32_t y,
                            const std::shared_ptr<ParticleSystem> &ps,
                            glm::vec3 &drag_point);

  void DragSelectedObject(uint32_t x, uint32_t y, const std::shared_ptr<ParticleSystem> &ps);

  void EndObjectSelection(std::shared_ptr<ParticleSystem> &ps);

  void SetMatrices(const glm::mat4 &projection_matrix,
                   const glm::mat4 &view);

  int32_t IndexOfClosestPoint(const glm::vec3 &near, const glm::vec3 &far, const std::vector<float> &points);
  glm::vec3 ClosestPointOnRay(const glm::vec3 &P, const glm::vec3 &A, const glm::vec3 &B);
  float PointToLineDist(const glm::vec3 &P, const glm::vec3 &A, const glm::vec3 &B);

 private:
  glm::vec3 MouseToRay(uint32_t xpos, uint32_t ypos, glm::vec3 &ray_near, glm::vec3 &ray_far);

  uint32_t window_width_;
  uint32_t window_height_;
  std::shared_ptr<ArcBall> arcball_;
  bool is_view_dragging_;
  bool is_object_dragging_;
  glm::mat4 inv_projection_matrix_;
  glm::mat4 inv_view_matrix_;
};

#endif //FLUIDSIM_INPUT_MGR_H_
