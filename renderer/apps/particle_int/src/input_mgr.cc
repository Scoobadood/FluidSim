#include "input_mgr.h"
#include "particles/particle_system.h"

InputManager::InputManager(uint32_t window_width, uint32_t window_height)
    : window_width_{window_width}//
    , window_height_{window_height}//
    , inv_projection_matrix_{1}//
    , inv_view_matrix_{1}//
{
  arcball_ = std::make_shared<ArcBall>(window_width, window_height);
}

void InputManager::BeginViewDrag(uint32_t x, uint32_t y) {
  if (is_view_dragging_) return;
  arcball_->DragStarted((float) x, (float) y);
  is_view_dragging_ = true;
}

void InputManager::DragView(uint32_t x, uint32_t y) {
  arcball_->Drag((float) x, (float) y);
}

void InputManager::EndViewDrag() {
  arcball_->DragEnded();
}

bool InputManager::BeginObjectSelection(uint32_t x, uint32_t y,
                                        const std::shared_ptr<ParticleSystem> &ps,
                                        glm::vec3 &drag_point) {
  glm::vec3 ray_near, ray_far;
  auto ray = MouseToRay(x, y, ray_near, ray_far);
  auto drag_idx = IndexOfClosestPoint(ray_near, ray_far, ps->Positions());
  if (drag_idx < 0) return false;
  is_object_dragging_ = true;

  drag_point = ClosestPointOnRay(ps->Particles()[drag_idx]->Position(), ray_near, ray_far);
  ps->CreateDragParticleAt(drag_idx, drag_point);
  return true;

}

void InputManager::SetMatrices(const glm::mat4 &projection_matrix,
                               const glm::mat4 &view) {
  inv_projection_matrix_ = glm::inverse(projection_matrix);
  inv_view_matrix_ = glm::inverse(view);
}

void InputManager::DragSelectedObject(uint32_t x, uint32_t y, const std::shared_ptr<ParticleSystem> &ps) {
  if (!is_object_dragging_) return;
  glm::vec3 ray_near, ray_far;
  auto ray = MouseToRay(x, y, ray_near, ray_far);
  auto drag_point_idx = ClosestPointOnRay(ps->Particles()[drag_point_idx]->Position(), ray_near, ray_far);
  drag_point->SetPosition(g_drag_point);
  return drag_point_idx;
}

void InputManager::EndObjectSelection(std::shared_ptr<ParticleSystem> &ps) {
  if (!is_object_dragging_)return;
  ps->StopDragging();
  is_object_dragging_ = false;
}

/*
 *
 * Convert a mouse click into a ray in world space coordinates
 * Convert Mouse Coordinates to Normalized Device Coordinates (NDC):
 *  Transform the mouse screen coordinates to NDC space (-1 to 1 range).
 * Create Ray in View Space:
 *  Transform the NDC coordinates to view space. For a perspective camera, this involves reversing the projection matrix.
 *  For an orthographic camera, it's a simpler transformation.
 *  Transform Ray to World Space:
 *    Transform the ray from view space to world space using the inverse of the camera's view matrix.
 */
glm::vec3 InputManager::MouseToRay(uint32_t xpos,
                                   uint32_t ypos,
                                   glm::vec3 &ray_near,
                                   glm::vec3 &ray_far) {
  auto ndc_x = (2.0f * ((float) xpos / (float) window_width_)) - 1.0f;
  auto ndc_y = 1.0f - (2.0f * ((float) ypos / (float) window_height_));
  glm::vec4 ray_start_ndc{ndc_x, ndc_y, -1.0f, 1.0f};
  glm::vec4 ray_end_ndc{ndc_x, ndc_y, 1.0f, 1.0f};

  auto ray_start_view = inv_projection_matrix_ * ray_start_ndc;
  auto ray_end_view = inv_projection_matrix_ * ray_end_ndc;
  ray_start_view /= ray_start_view.w;
  ray_end_view /= ray_end_view.w;

  auto ray_start_world = inv_view_matrix_ * ray_start_view;
  auto ray_end_world = inv_view_matrix_ * ray_end_view;
  ray_start_world /= ray_start_world.w;
  ray_end_world /= ray_end_world.w;
  glm::vec3 ray_dir = (ray_end_world - ray_start_world);
  ray_near = ray_start_world;
  ray_far = ray_end_world;
  return glm::normalize(ray_dir);
}

float InputManager::PointToLineDist(const glm::vec3 &P, const glm::vec3 &A, const glm::vec3 &B) {
  using namespace glm;
  // L1 to point
  vec3 AB = B - A;
  vec3 AP = P - A;

  vec3 cp = glm::cross(AP, AB);
  // Magnitude of the pll gram
  float area = glm::length(cp);
  // This is also perp dist * base (|AB|)
  float base = glm::length(AB);
  float perp_dist = area / base;
  return perp_dist;
}

glm::vec3 InputManager::ClosestPointOnRay(const glm::vec3 &P, const glm::vec3 &A, const glm::vec3 &B) {
  using namespace glm;
  // L1 to point
  vec3 AB = B - A;
  vec3 AP = P - A;

  auto dp1 = glm::dot(AP, AB);
  auto dp2 = glm::dot(AB, AB);

  return A + ((dp1 / dp2) * AB);
}

int32_t InputManager::IndexOfClosestPoint(const glm::vec3 &near,
                                          const glm::vec3 &far,
                                          const std::vector<float> &points) {
  auto num_points = points.size() / 3;
  auto index = -1;
  float min_dist = 10.0f;
  for (auto i = 0; i < num_points; i++) {
    auto dist =
        PointToLineDist({points[i * 3], points[i * 3 + 1], points[i * 3 + 2]}, near, far);
    if (dist < min_dist) {
      min_dist = dist;
      index = i;
    }
  }
  return index;
}
