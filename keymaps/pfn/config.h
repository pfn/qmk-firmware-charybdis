#pragma once

#ifdef VIA_ENABLE
/* VIA configuration. */
#    define DYNAMIC_KEYMAP_LAYER_COUNT 16
#endif // VIA_ENABLE

// For https://github.com/qmk/qmk_firmware/pull/21777
#define OS_DETECTION_KEYBOARD_RESET
#define OS_DETECTION_DEBUG_ENABLE

#define SPLIT_TRANSACTION_IDS_USER SYNC_STATE_USER
#define SPLIT_WATCHDOG_TIMEOUT 1000

#ifdef POINTING_DEVICE_ENABLE
#  define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
#  define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#  undef POINTING_DEVICE_GESTURES_SCROLL_ENABLE
#  undef CIRQUE_PINNACLE_POSITION_MODE
#  define CIRQUE_PINNACLE_POSITION_MODE CIRQUE_PINNACLE_ABSOLUTE_MODE
#  define CIRQUE_PINNACLE_TAP_ENABLE
#  define CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE
#  undef CIRQUE_PINNACLE_ATTENUATION
#  define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_2X
#  define CIRQUE_PINNACLE_CURVED_OVERLAY
#  if CIRQUE_PINNACLE_POSITION_MODE == CIRQUE_PINNACLE_ABSOLUTE_MODE
#    define AUTO_MOUSE_TIME 20
#  else
#    define AUTO_MOUSE_TIME 650
#  endif
#endif // POINTING_DEVICE_ENABLE

#define COMBO_TERM 12