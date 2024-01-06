#ifndef GLHELPERS_RENDER_TARGET_H
#define GLHELPERS_RENDER_TARGET_H

#include <cstdint>

class RenderTarget {
public:
  RenderTarget(int32_t width, int32_t height, int32_t num_channels,
               bool with_colour = true,
               bool with_depth = false
               );
  ~RenderTarget();
  void Bind();
  void Unbind();


    private:
  bool validate_parms( );
  int32_t width_;
  int32_t height_;
  int32_t num_channels_;
  bool with_colour_;
  bool with_depth_;
  uint32_t texture_id_;
  uint32_t depth_buffer_;
  uint32_t fbo_;
  bool is_ready_;
};

#endif
