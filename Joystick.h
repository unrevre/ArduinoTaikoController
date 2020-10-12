#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <HID.h>

/*
 *  Adapted from
 *  https://github.com/progmem/Switch-Fightstick/blob/master/Joystick.h
 */

/* Enumeration for joystick buttons. */
enum JoystickButtons_t {
    SWITCH_BTN_Y       = 0x1,
    SWITCH_BTN_B       = 0x2,
    SWITCH_BTN_A       = 0x4,
    SWITCH_BTN_X       = 0x8,
    SWITCH_BTN_L       = 0x10,
    SWITCH_BTN_R       = 0x20,
    SWITCH_BTN_ZL      = 0x40,
    SWITCH_BTN_ZR      = 0x80,
    SWITCH_BTN_SELECT  = 0x100,
    SWITCH_BTN_START   = 0x200,
    SWITCH_BTN_LCLICK  = 0x400,
    SWITCH_BTN_RCLICK  = 0x800,
    SWITCH_BTN_HOME    = 0x1000,
    SWITCH_BTN_CAPTURE = 0x2000,
};

/* Enumeration for joystick hats. */
enum JoystickHats_t {
    SWITCH_HAT_U      = 0x0,
    SWITCH_HAT_UR     = 0x1,
    SWITCH_HAT_R      = 0x2,
    SWITCH_HAT_DR     = 0x3,
    SWITCH_HAT_D      = 0x4,
    SWITCH_HAT_DL     = 0x5,
    SWITCH_HAT_L      = 0x6,
    SWITCH_HAT_UL     = 0x7,
    SWITCH_HAT_CENTER = 0x8,
};

struct JoyReport {
    uint16_t BTN;
    uint8_t HAT;
    uint8_t LX;
    uint8_t LY;
    uint8_t RX;
    uint8_t RY;
    uint8_t VendorSpec;
};

class Joystick_ {
  public:
    Joystick_();

    void report(uint16_t state);

  private:
    struct JoyReport data;
};

extern Joystick_ Joystick;

#endif /* JOYSTICK_H */
