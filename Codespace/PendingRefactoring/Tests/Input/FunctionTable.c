#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Input/Input.h"

int main(void) {
  /* ------------------------------------------------------------
   * 1. Main Input Package Function Table
   * ------------------------------------------------------------ */
  const struct Input_FunctionTable *inputFt = Input_GetFunctionTable();
  assert(inputFt != NULL);
  assert(inputFt->Update != NULL);
  assert(inputFt->SetCaptureMode != NULL);
  assert(inputFt->GetCapabilities != NULL);

  assert(inputFt->GetCapabilities() == Input_GetCapabilities());

  /* ------------------------------------------------------------
   * 2. Keyboard Package Function Table
   * ------------------------------------------------------------ */
  const struct Input_Keyboard_FunctionTable *kbFt =
      Input_Keyboard_GetFunctionTable();
  assert(kbFt != NULL);
  assert(kbFt->IsDown != NULL);
  assert(kbFt->IsPressed != NULL);
  assert(kbFt->IsReleased != NULL);
  assert(kbFt->IsRepeated != NULL);
  assert(kbFt->GetModifiers != NULL);
  assert(kbFt->IsPhysicalDown != NULL);
  assert(kbFt->IsPhysicalPressed != NULL);
  assert(kbFt->IsPhysicalReleased != NULL);

  assert(!kbFt->IsDown(INPUT_KEY_A));
  assert(kbFt->GetModifiers() == Input_Keyboard_GetModifiers());

  /* ------------------------------------------------------------
   * 3. Mouse Package Function Table
   * ------------------------------------------------------------ */
  const struct Input_Mouse_FunctionTable *mouseFt =
      Input_Mouse_GetFunctionTable();
  assert(mouseFt != NULL);
  assert(mouseFt->IsDown != NULL);
  assert(mouseFt->IsPressed != NULL);
  assert(mouseFt->IsReleased != NULL);
  assert(mouseFt->GetPosition != NULL);
  assert(mouseFt->GetDelta != NULL);
  assert(mouseFt->GetScroll != NULL);

  assert(!mouseFt->IsDown(INPUT_MOUSE_BUTTON_LEFT));

  /* ------------------------------------------------------------
   * 4. Event Package Function Table
   * ------------------------------------------------------------ */
  const struct Input_Event_FunctionTable *eventFt =
      Input_Event_GetFunctionTable();
  assert(eventFt != NULL);
  assert(eventFt->Poll != NULL);
  assert(eventFt->Clear != NULL);
  assert(eventFt->GetDroppedCount != NULL);
  assert(eventFt->Submit != NULL);

  assert(eventFt->GetDroppedCount() == Input_Event_GetDroppedCount());

  /* ------------------------------------------------------------
   * 5. Gamepad Package Function Table
   * ------------------------------------------------------------ */
  const struct Input_Gamepad_FunctionTable *gamepadFt =
      Input_Gamepad_GetFunctionTable();
  assert(gamepadFt != NULL);
  assert(gamepadFt->IsMapped != NULL);
  assert(gamepadFt->IsDown != NULL);
  assert(gamepadFt->IsPressed != NULL);
  assert(gamepadFt->IsReleased != NULL);
  assert(gamepadFt->GetAxis != NULL);
  assert(gamepadFt->MapButton != NULL);
  assert(gamepadFt->MapAxis != NULL);
  assert(gamepadFt->ClearMapping != NULL);

  /* ------------------------------------------------------------
   * 6. Controller Package Function Table
   * ------------------------------------------------------------ */
  const struct Input_Controller_FunctionTable *ctrlFt =
      Input_Controller_GetFunctionTable();
  assert(ctrlFt != NULL);
  assert(ctrlFt->Enumerate != NULL);
  assert(ctrlFt->IsConnected != NULL);
  assert(ctrlFt->GetName != NULL);
  assert(ctrlFt->GetButtonCount != NULL);
  assert(ctrlFt->GetAxisCount != NULL);
  assert(ctrlFt->GetHatCount != NULL);
  assert(ctrlFt->HasRumble != NULL);
  assert(ctrlFt->IsDown != NULL);
  assert(ctrlFt->IsPressed != NULL);
  assert(ctrlFt->IsReleased != NULL);
  assert(ctrlFt->GetAxis != NULL);
  assert(ctrlFt->GetHat != NULL);
  assert(ctrlFt->Rumble != NULL);
  assert(ctrlFt->StopRumble != NULL);
  assert(ctrlFt->GetVendor != NULL);
  assert(ctrlFt->GetProduct != NULL);

  assert(!ctrlFt->IsConnected(0));

  return 0;
}
