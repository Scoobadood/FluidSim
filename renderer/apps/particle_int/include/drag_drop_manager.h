#ifndef FLUIDSIM_RENDERER_APPS_PARTICLE_INT_INCLUDE_DRAG_DROP_MANAGER_H_
#define FLUIDSIM_RENDERER_APPS_PARTICLE_INT_INCLUDE_DRAG_DROP_MANAGER_H_

#include "renderer.h"
#include "phys_model.h"
#include <memory>

class DragDropManager{
 public DragDropManager(std::shared_ptr<PhysicalModel>& model,
                        std::shared_ptr<Renderer>& renderer);

};
#endif //FLUIDSIM_RENDERER_APPS_PARTICLE_INT_INCLUDE_DRAG_DROP_MANAGER_H_
