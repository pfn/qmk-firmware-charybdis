#include QMK_KEYBOARD_H

#include "process_keymap.h"

enum my_layers {
    _QWERTY,
    _COLEMAK,
    _NUM,
    _SYM,
    _NAV,
    _MOUSE,
    _MEDIA,
    _COL6,
};

void matrix_init_user(void) {
    numpad_layer = _NUM;
    sym_layer = _SYM;
    #include "charybdis_rgb_matrix.inc"
}

enum my_combos {
  RF_SHOS,
  UJ_SHOS,
  FG_CAPS,
  YH_NUM,
  QA_ESC,
  AZ_TAB,
  PCLN_BS,
  CLNSLSH_QUOT,
  COMBO_COUNT
};

const uint16_t COMBO_LEN = COMBO_COUNT;

#define COMBO_KEYS(name, keycodes...) const uint16_t PROGMEM name ## _combo[] = { keycodes, COMBO_END }
#define MAKE_COMBO(name, keycode) [name] = COMBO(name ## _combo, keycode)
COMBO_KEYS(RF_SHOS,      KC_R, LT(_NAV, KC_F));
COMBO_KEYS(UJ_SHOS,      KC_U, KC_J);
COMBO_KEYS(YH_NUM,       KC_Y, KC_H);
COMBO_KEYS(FG_CAPS,      LT(_NAV, KC_F), LT(_MOUSE, KC_G));
COMBO_KEYS(QA_ESC,       KC_Q, LCTL_T(KC_A));
COMBO_KEYS(AZ_TAB,       LCTL_T(KC_A), LSFT_T(KC_Z));
COMBO_KEYS(PCLN_BS,      KC_P, RCTL_T(KC_SCLN));
COMBO_KEYS(CLNSLSH_QUOT, RSFT_T(KC_SLSH), RCTL_T(KC_SCLN));

combo_t key_combos[COMBO_COUNT] = {
    MAKE_COMBO(RF_SHOS,      SH_OS),
    MAKE_COMBO(UJ_SHOS,      SH_OS),
    MAKE_COMBO(FG_CAPS,      KC_CAPS),
    MAKE_COMBO(YH_NUM,       OSL(_NUM)),
    MAKE_COMBO(QA_ESC,       KC_ESC),
    MAKE_COMBO(AZ_TAB,       KC_TAB),
    MAKE_COMBO(PCLN_BS,      KC_BSPC),
    MAKE_COMBO(CLNSLSH_QUOT, KC_QUOT)
};

#ifdef ENCODER_ENABLE
#define MAKE_KC(mod, tru, els) (mod ? (tru) : (els))

#include <stdarg.h>
uint16_t extract_mod_kc(uint16_t mods, ...) {
    va_list ap;
    va_start(ap, mods);
    uint16_t kc;
    uint16_t result = KC_NO;
    while (result == KC_NO && (kc = va_arg(ap, int)) != KC_NO) {
        if (mods & MOD_BIT(kc))
            result = kc;
    }
    va_end(ap);
    return result;
}

void enforce_code(uint16_t kc) {
    if (mod_held != KC_NO && mod_held != kc) {
        unregister_code(mod_held);
        mod_held = KC_NO;
    }
    if (mod_held == KC_NO)
        register_code(kc);
    mod_held = kc;
}

bool encoder_update_user(uint8_t index, bool clockwise) {

    uint16_t kc = !index ? (clockwise ? KC_VOLU : KC_VOLD) : (clockwise ? KC_WH_D : KC_WH_U);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _MOUSE:
            //kc = clockwise ? KC_WH_D : KC_WH_U;
            kc = KC_NO;
            report_mouse_t mouse = pointing_device_get_report();
            if (get_mods() & MOD_MASK_SHIFT) {
                mouse.h = clockwise ? 4 : -4;
            } else {
                mouse.v = clockwise ? -4 : 4;
            }
            pointing_device_set_report(mouse);
            pointing_device_send();
            break;
        case _NAV:
            kc = clockwise ? KC_PGDN : KC_PGUP;
            break;
        // case _ADJUST:
            // kc = KC_NO;
            // if (clockwise)
                // rgb_matrix_step();
            // else
                // rgb_matrix_step_reverse();
            // break;
        case _MEDIA:
            enforce_code(KC_LCTL);
            kc = clockwise ? KC_TAB : LSFT(KC_TAB);
            break;
        case _NUM:
            enforce_code(KC_LALT);
            kc = clockwise ? KC_TAB : LSFT(KC_TAB);
            break;
        case _SYM:
            enforce_code(KC_LGUI);
            kc = clockwise ? KC_GRV : LSFT(KC_GRV);
            break;
    }

    if (kc != KC_NO) tap_code16(kc);

    return false;
}
#endif

// this only works on the master side  :(
#ifdef DIP_SWITCH_ENABLE
bool dip_switch_update_user(uint8_t index, bool active) {
    if (active) {
        tap_code16(KC_VOLU);
        return false;
    }
    return true;
}
#endif

#ifdef SWAP_HANDS_ENABLE
__attribute__((weak)) const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    // Left
    {{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}},
    {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}},
    {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}},
    {{0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}},
    // Right
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}},
    {{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}},
    {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}},
    {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}}
};
#endif

#ifdef POINTING_DEVICE_ENABLE
void pointing_device_init_user(void) {
    set_auto_mouse_layer(_MOUSE);
    set_auto_mouse_enable(true);
}
report_mouse_t pointing_device_task_user(report_mouse_t mouse) {
    const float divisor = 16;

    static float scroll_h = 0;
    static float scroll_v = 0;
    // can't seem to abort the drag scroll while a button is clicked?
    // mouse.buttons is also 0 regardless of click state?
    if (is_scrolling) {
        if (abs(mouse.x) > abs(mouse.y)) {
            scroll_h += -1 * (float) mouse.x / divisor;
            mouse.h = (int8_t) scroll_h;
            scroll_h -= mouse.h;
            scroll_v = 0;
        } else {
            scroll_v += (float) mouse.y / divisor;
            mouse.v = (int8_t) scroll_v;
            scroll_v -= mouse.v;
            scroll_h = 0;
        }
        mouse.x = 0;
        mouse.y = 0;
    }
    return mouse;

}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (get_highest_layer(state) != _MOUSE) {
        is_scrolling = false;
    }
    return state;
}
void keyboard_post_init_user(void) {
    pointing_device_set_cpi(800);
}
#endif