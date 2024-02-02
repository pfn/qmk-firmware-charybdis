#pragma once

#ifdef VIA_ENABLE
/* VIA configuration. */
#    define DYNAMIC_KEYMAP_LAYER_COUNT 16
#endif // VIA_ENABLE

// For https://github.com/qmk/qmk_firmware/pull/21777
#define OS_DETECTION_KEYBOARD_RESET
#define OS_DETECTION_DEBUG_ENABLE

#ifdef POINTING_DEVICE_ENABLE
#  define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#endif // POINTING_DEVICE_ENABLE
