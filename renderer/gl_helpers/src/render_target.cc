#include "render_target.h"
#include "gl_common.h"
#include "spdlog/spdlog.h"

bool RenderTarget::validate_parms() {
  if (width_ < 1) {
    spdlog::error("Render target width must be greate than 0 ({})", width_);
    return false;
  }
  if (height_ < 1) {
    spdlog::error("Render target height must be greate than 0 ({})", height_);
    return false;
  }
  if (with_colour_ && num_channels_ != 1 && num_channels_ != 3 && num_channels_ != 4) {
    spdlog::error("Num channels must be 1, 3 or 4 ({})", num_channels_);
    return false;
  }
  if (!with_colour_ && !with_depth_) {
    spdlog::error("Must have at least one of colour and depthg attachment");
    return false;
  }
  return true;
}

RenderTarget::RenderTarget(int32_t width, int32_t height, int32_t num_channels,
                           bool with_colour, bool with_depth)//
        : width_{width} //
        , height_{height} //
        , num_channels_{num_channels} //
        , with_colour_{with_colour} //
        , with_depth_{with_depth} //
        , is_ready_{false} //
{
  if (!validate_parms()) return;

  glGenFramebuffers(1, &fbo_);
  if (!fbo_) return;

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
  if (with_colour) {
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Allocate texture
    GLenum format;
    switch (num_channels_) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);
  }

  if (with_depth_) {
    glGenRenderbuffers(1, &depth_buffer_);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_);
  }

  auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (err != GL_FRAMEBUFFER_COMPLETE) {
    spdlog::error("FBO not complete : {:x}", err);
    return;
  }
  is_ready_ = true;
}

RenderTarget::~RenderTarget() {
  if (depth_buffer_) glDeleteRenderbuffers(1, &depth_buffer_);
  if (texture_id_) glDeleteTextures(1, &texture_id_);
  if (fbo_) glDeleteFramebuffers(1, &fbo_);
}

void RenderTarget::Bind() {
  if(! is_ready_) return;
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
}

void RenderTarget::Unbind() {
  if(! is_ready_) return;
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
