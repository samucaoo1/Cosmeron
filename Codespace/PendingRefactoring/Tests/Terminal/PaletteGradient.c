#include <assert.h>
#include <math.h>
#include "../../Cosmeron/Modules/Graphics/Palette/Palette.h"
#include "../../Cosmeron/Modules/Graphics/Gradient/Gradient.h"

#define OK STATUS_CONST(SUCCESS)

int main(void) {
  /* Test TPalette */
  TPalette pal = {0};
  assert(TPalette_Init(&pal) == OK);
  assert(pal.api->Create(&pal, 256) == OK);

  assert(pal.api->LoadANSI16(&pal) == OK);
  assert(pal.api->Count(&pal) == 16);

  uint32_t color = 0;
  assert(pal.api->GetColor(&pal, 1, &color) == OK); /* Red */
  assert(color == 0xFF800000);

  /* FindNearest */
  size_t nearest = 0;
  assert(pal.api->FindNearest(&pal, 0xFF850000, &nearest) == OK);
  assert(nearest == 1); /* Should match red */

  assert(pal.api->LoadXTerm256(&pal) == OK);
  assert(pal.api->Count(&pal) == 256);

  pal.api->Destroy(&pal);

  /* Test LerpColor */
  uint32_t c1 = 0xFF000000;
  uint32_t c2 = 0xFFFFFFFF;
  uint32_t mid = GRADIENT_FUNC(LerpColor)(c1, c2, 0.5f);
  /* Mid should be approx 0xFF7F7F7F or 0xFF808080 */
  uint32_t r = (mid >> 16) & 0xFF;
  assert(r >= 127 && r <= 128);

  /* Test TGradient */
  TGradient grad = {0};
  assert(TGradient_Init(&grad) == OK);
  assert(grad.api->Create(&grad, 16) == OK);
  assert(grad.api->AddStop(&grad, 0.0f, 0xFF000000) == OK);
  assert(grad.api->AddStop(&grad, 1.0f, 0xFFFFFFFF) == OK);
  assert(grad.api->Count(&grad) == 2);

  assert(grad.api->Sample(&grad, 0.0f) == 0xFF000000);
  assert(grad.api->Sample(&grad, 1.0f) == 0xFFFFFFFF);

  uint32_t samples[5] = {0};
  assert(grad.api->SampleRange(&grad, samples, 5) == OK);
  assert(samples[0] == 0xFF000000);
  assert(samples[4] == 0xFFFFFFFF);

  grad.api->Destroy(&grad);
  return 0;
}
