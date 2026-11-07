#include "../../Cosmeron/Modules/Graphics/Graphics.h"
int main(void) {
  GUI_TYPE(TContext) context = {0};
  return context.frame ? 1 : 0;
}
