#pragma once

/* Handedness. */
#define SPLIT_HAND_PIN GP13

#ifdef POINTING_DEVICE_ENABLE
#  define I2C_DRIVER I2CD1
#  define I2C1_SCL_PIN GP3
#  define I2C1_SDA_PIN GP2
#  define SPLIT_POINTING_ENABLE
#  define POINTING_DEVICE_RIGHT
#  define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#  define POINTING_DEVICE_GESTURES_SCROLL_ENABLE
#  define CIRQUE_PINNACLE_POSITION_MODE CIRQUE_PINNACLE_RELATIVE_MODE
#  define CIRQUE_PINNACLE_TAP_ENABLE
#  define CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE
#  define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_1X
#  define CIRQUE_PINNACLE_CURVED_OVERLAY
#endif

#define DIP_SWITCH_PINS { GP20 }

#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_MODS_ENABLE

/* Reset. */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP17
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 1000U

/* RGB matrix support. */
#ifdef RGB_MATRIX_ENABLE
#  define RGB_MATRIX_LED_COUNT 48
#  define RGB_MATRIX_SPLIT { 30, 18 }
#  define SPLIT_TRANSPORT_MIRROR
#  define RGB_MATRIX_MAXIMUM_BRIGHTNESS 255
#  define RGB_DISABLE_WHEN_USB_SUSPENDED
#  define RGB_MATRIX_KEYPRESSES

// Startup values.
#  define RGB_MATRIX_DEFAULT_VAL 64

// Rainbow swirl as startup mode.
#  define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#  define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CYCLE_LEFT_RIGHT

// Slow swirl at startup.
#  define RGB_MATRIX_DEFAULT_SPD 32
#  define ENABLE_RGB_MATRIX_ALPHAS_MODS
#  define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#  define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#  define ENABLE_RGB_MATRIX_BREATHING
#  define ENABLE_RGB_MATRIX_BAND_SAT
#  define ENABLE_RGB_MATRIX_BAND_VAL
#  define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#  define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#  define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#  define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#  define ENABLE_RGB_MATRIX_CYCLE_ALL
#  define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#  define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#  define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#  define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#  define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#  define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#  define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#  define ENABLE_RGB_MATRIX_DUAL_BEACON
#  define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#  define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#  define ENABLE_RGB_MATRIX_RAINDROPS
#  define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#  define ENABLE_RGB_MATRIX_HUE_BREATHING
#  define ENABLE_RGB_MATRIX_HUE_PENDULUM
#  define ENABLE_RGB_MATRIX_HUE_WAVE
#  define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#  define ENABLE_RGB_MATRIX_DIGITAL_RAIN
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#  define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#  define ENABLE_RGB_MATRIX_SPLASH
#  define ENABLE_RGB_MATRIX_MULTISPLASH
#  define ENABLE_RGB_MATRIX_SOLID_SPLASH
#  define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
#endif
