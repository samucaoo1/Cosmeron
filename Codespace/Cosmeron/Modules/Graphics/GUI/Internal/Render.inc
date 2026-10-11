#pragma once
static inline uint8_t GUI_INS(Coverage)(GUI_TYPE(TFont) f, uint32_t cp, uint32_t x, uint32_t y) {
  const char *p;
  uint32_t row = 0, col = 0;
  if (!f.asf)
    return f.coverage(f.user, cp, x, y);
  if (cp < 32 || cp > 126)
    cp = '?';
  p = f.asf->glyphs[cp - 32].art;
  if (!p)
    return 0;
  while (*p) {
    if (*p == '\n') {
      ++row;
      col = 0;
      ++p;
      continue;
    }
    if (row == y && col == x)
      return *p != ' ' && *p != '\t' ? 255 : 0;
    if (row > y)
      return 0;
    ++col;
    ++p;
  }
  return 0;
}
static inline void GUI_INS(Pixel)(GRAPHICS_TYPE(TSurface) * s, uint32_t x, uint32_t y,
                                  uint32_t color) {
  uint32_t *p = &s->pixels[(size_t)y * s->stride + x];
  *p = GRAPHICS_FUNC(Surface_Blend)(*p, color);
}
/* Clamp floating bounds before integer conversion, including off-screen geometry. */
static inline uint32_t GUI_INS(Limit)(float value, uint32_t max) {
  if (value <= 0)
    return 0;
  if (value >= (float)max)
    return max;
  return (uint32_t)value;
}
static inline void GUI_INS(RenderRect)(GRAPHICS_TYPE(TSurface) * s, const GUI_TYPE(TCommand) * cmd,
                                       float scale) {
  TQUAD_TYPE(float) clip = GUI_INS(Intersect)(cmd->region, cmd->clip);
  uint32_t x, y, x0 = GUI_INS(Limit)(clip.value1 * scale, s->size.x),
                 y0 = GUI_INS(Limit)(clip.value2 * scale, s->size.y);
  uint32_t x1 = GUI_INS(Limit)((clip.value1 + clip.value3) * scale + 1, s->size.x),
           y1 = GUI_INS(Limit)((clip.value2 + clip.value4) * scale + 1, s->size.y);
  float r = cmd->radius;
  if (r > cmd->region.value3 / 2)
    r = cmd->region.value3 / 2;
  if (r > cmd->region.value4 / 2)
    r = cmd->region.value4 / 2;
  for (y = y0; y < y1; ++y)
    for (x = x0; x < x1; ++x) {
      TDUAL_TYPE(float) p;
      float dx = 0, dy = 0;
      p.x = ((float)x + 0.5f) / scale;
      p.y = ((float)y + 0.5f) / scale;
      if (!GUI_INS(Contains)(clip, p))
        continue;
      if (r > 0) {
        float lx = p.x - cmd->region.value1, ly = p.y - cmd->region.value2;
        if (lx < r)
          dx = r - lx;
        else if (lx > cmd->region.value3 - r)
          dx = lx - (cmd->region.value3 - r);
        if (ly < r)
          dy = r - ly;
        else if (ly > cmd->region.value4 - r)
          dy = ly - (cmd->region.value4 - r);
        if (dx * dx + dy * dy > r * r)
          continue;
      }
      GUI_INS(Pixel)(s, x, y, cmd->color);
    }
}
static inline void GUI_INS(RenderText)(GRAPHICS_TYPE(TSurface) * s, const GUI_TYPE(TCommand) * cmd,
                                       float scale) {
  size_t offset = 0;
  float cursor = cmd->region.value1, top = cmd->region.value2;
  float glyphScale = scale * cmd->scale;
  while (offset < cmd->length) {
    TEXT_TYPE(TChar32) cp;
    size_t n;
    float advance;
    TQUAD_TYPE(float) box, clip;
    uint32_t x, y, x0, x1, y0, y1;
    if (TEXT_UTF8_FUNC(Decode)((const unsigned char *)cmd->text + offset, cmd->length - offset, &cp,
                               &n) != STATUS_CONST(SUCCESS))
      return;
    offset += n;
    if (cp == '\n') {
      cursor = cmd->region.value1;
      top += (cmd->font.height + 1) * cmd->scale;
      continue;
    }
    advance = GUI_INS(Advance)(cmd->font, cp);
    box = GUI_INS(Region)(cursor, top, advance * cmd->scale, cmd->font.height * cmd->scale);
    clip = GUI_INS(Intersect)(box, cmd->clip);
    x0 = GUI_INS(Limit)(clip.value1 * scale, s->size.x);
    y0 = GUI_INS(Limit)(clip.value2 * scale, s->size.y);
    x1 = GUI_INS(Limit)((clip.value1 + clip.value3) * scale + 1, s->size.x);
    y1 = GUI_INS(Limit)((clip.value2 + clip.value4) * scale + 1, s->size.y);
    for (y = y0; y < y1; ++y)
      for (x = x0; x < x1; ++x) {
        TDUAL_TYPE(float) p;
        uint32_t gx, gy;
        uint8_t coverage;
        uint32_t alpha, color;
        p.x = ((float)x + 0.5f) / scale;
        p.y = ((float)y + 0.5f) / scale;
        if (!GUI_INS(Contains)(clip, p))
          continue;
        gx = (uint32_t)(((float)x + 0.5f - cursor * scale) / glyphScale);
        gy = (uint32_t)(((float)y + 0.5f - top * scale) / glyphScale);
        coverage = GUI_INS(Coverage)(cmd->font, cp, gx, gy);
        alpha = ((cmd->color >> 24) * coverage + 127U) / 255U;
        color = (cmd->color & UINT32_C(0xFFFFFF)) | (alpha << 24);
        if (alpha)
          GUI_INS(Pixel)(s, x, y, color);
      }
    cursor += advance * cmd->scale;
  }
}
static inline void GUI_INS(RenderImage)(GRAPHICS_TYPE(TSurface) * s, const GUI_TYPE(TCommand) * cmd,
                                        float scale) {
  TQUAD_TYPE(float) clip = GUI_INS(Intersect)(cmd->region, cmd->clip);
  uint32_t x, y, x0 = GUI_INS(Limit)(clip.value1 * scale, s->size.x),
                 y0 = GUI_INS(Limit)(clip.value2 * scale, s->size.y);
  uint32_t x1 = GUI_INS(Limit)((clip.value1 + clip.value3) * scale + 1, s->size.x),
           y1 = GUI_INS(Limit)((clip.value2 + clip.value4) * scale + 1, s->size.y);
  for (y = y0; y < y1; ++y)
    for (x = x0; x < x1; ++x) {
      TDUAL_TYPE(float) p;
      uint32_t sx, sy;
      p.x = ((float)x + 0.5f) / scale;
      p.y = ((float)y + 0.5f) / scale;
      if (!GUI_INS(Contains)(clip, p))
        continue;
      sx = (uint32_t)((p.x - cmd->region.value1) / cmd->region.value3 * (float)cmd->image->size.x);
      sy = (uint32_t)((p.y - cmd->region.value2) / cmd->region.value4 * (float)cmd->image->size.y);
      if (sx >= cmd->image->size.x)
        sx = cmd->image->size.x - 1;
      if (sy >= cmd->image->size.y)
        sy = cmd->image->size.y - 1;
      GUI_INS(Pixel)(s, x, y, cmd->image->pixels[(size_t)sy * cmd->image->stride + sx]);
    }
}
GUI_RENDER_SOFTWARE_PROTOTYPE {
  size_t i;
  if (!GRAPHICS_FUNC(Surface_IsValid)(surface) || (!commands && count) ||
      !(scale > 0 && scale <= 1024))
    return STATUS_CONST(INVALID_ARGUMENT);
  /* Validate all commands before touching the surface. */
  for (i = 0; i < count; ++i) {
    const GUI_TYPE(TCommand) *cmd = &commands[i];
    if (!GUI_INS(RegionValid)(cmd->region) || !GUI_INS(Finite)(cmd->clip.value1) ||
        !GUI_INS(Finite)(cmd->clip.value2) ||
        !(cmd->clip.value3 >= 0 && cmd->clip.value3 <= 1000000) ||
        !(cmd->clip.value4 >= 0 && cmd->clip.value4 <= 1000000))
      return STATUS_CONST(INVALID_ARGUMENT);
    switch (cmd->type) {
    case GUI_CONST(COMMAND_RECTANGLE):
      if (!(cmd->radius >= 0 && cmd->radius <= 1000000))
        return STATUS_CONST(INVALID_ARGUMENT);
      break;
    case GUI_CONST(COMMAND_TEXT): {
      TDUAL_TYPE(float) size;
      OPSTATUS status;
      if (!cmd->text)
        return STATUS_CONST(INVALID_ARGUMENT);
      status = GUI_INS(Measure)(cmd->font, cmd->text, cmd->length, cmd->scale, &size);
      if (status != STATUS_CONST(SUCCESS))
        return status;
      break;
    }
    case GUI_CONST(COMMAND_IMAGE):
      if (!GRAPHICS_FUNC(Surface_IsValid)(cmd->image) ||
          GRAPHICS_FUNC(Surface_Overlaps)(cmd->image, surface))
        return STATUS_CONST(INVALID_ARGUMENT);
      break;
    default:
      return STATUS_CONST(INVALID_ARGUMENT);
    }
  }
  for (i = 0; i < count; ++i) {
    const GUI_TYPE(TCommand) *cmd = &commands[i];
    if (cmd->type == GUI_CONST(COMMAND_RECTANGLE))
      GUI_INS(RenderRect)(surface, cmd, scale);
    else if (cmd->type == GUI_CONST(COMMAND_TEXT))
      GUI_INS(RenderText)(surface, cmd, scale);
    else
      GUI_INS(RenderImage)(surface, cmd, scale);
  }
  return STATUS_CONST(SUCCESS);
}
