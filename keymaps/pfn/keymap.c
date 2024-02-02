/**
 * Copyright 2024 Perry Nguyen (@pfn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "process_keymap.h"
#include "os_detection.h"

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
    SNIPE,
    EXIT_MOUSE,
    NEXT_APP,
    PREV_APP,
    NEXT_WIN,
    PREV_WIN,
    NEXT_TAB,
    PREV_TAB,
    NEXT_RGB,
    PREV_RGB,
    STORE_USBDET,
    PRINT_USBDET,
};
#include "keymap_configurator.inc"

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

bool is_scrolling;
bool is_sniping = false;

void process_scroll(uint16_t keycode) {
    report_mouse_t mouse = pointing_device_get_report();
    switch (keycode) {
        case KC_MS_WH_DOWN:  mouse.v = -4; break;
        case KC_MS_WH_UP:    mouse.v =  4; break;
        case KC_MS_WH_LEFT:  mouse.h = -4; break;
        case KC_MS_WH_RIGHT: mouse.h =  4; break;
    }
    pointing_device_set_report(mouse);
    pointing_device_send();
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

void process_tab(uint16_t keycode) {
    enforce_code(KC_LCTL);
    tap_code16(keycode == NEXT_TAB ? KC_TAB : LSFT(KC_TAB));
}

#define RGB_NEXT_PREV(kc, next, prev) if (kc == NEXT_RGB) { next; } else { prev; }
void process_kc_rgb(uint16_t keycode) {
    if (get_mods() & MOD_MASK_SHIFT) {
        RGB_NEXT_PREV(keycode, rgb_matrix_increase_sat(), rgb_matrix_decrease_sat());
    } else if (get_mods() & MOD_MASK_ALT) {
        RGB_NEXT_PREV(keycode, rgb_matrix_increase_val(), rgb_matrix_decrease_val());
    } else if (get_mods() & MOD_MASK_CTRL) {
        RGB_NEXT_PREV(keycode, rgb_matrix_increase_hue(), rgb_matrix_decrease_hue());
    } else {
        RGB_NEXT_PREV(keycode, rgb_matrix_step(), rgb_matrix_step_reverse());
    }
}

void process_window(uint16_t keycode) {
    uint16_t mod_code = KC_LALT;
    os_variant_t os = detected_host_os();
    #ifdef CONSOLE_ENABLE
    #include "print.h"
    uprintf("detected os %d\n", os);
    #endif
    bool is_mac = OS_MACOS == os || OS_IOS == os;
    if (is_mac) {
        mod_code = KC_LGUI;
    }
    enforce_code(mod_code);
    uint16_t kc = KC_NO;
    switch (keycode) {
        case NEXT_APP: kc = KC_TAB;                         break;
        case PREV_APP: kc = LSFT(KC_TAB);                   break;
        case NEXT_WIN: kc = is_mac ? KC_GRV : KC_TAB;       break;
        case PREV_WIN: kc = LSFT(is_mac ? KC_GRV : KC_TAB); break;
    }
    if (kc != KC_NO) {
        tap_code16(kc);
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case STORE_USBDET:
        if (record->event.pressed) {
            store_setups_in_eeprom();
        }
        return false;
    case PRINT_USBDET:
        if (record->event.pressed) {
            print_stored_setups();
        }
        return false;
    case DRAG_SCROLL:
        if (record->event.pressed) {
            is_scrolling = !is_scrolling;
        }
        return false;
    case KC_MS_WH_UP...KC_MS_WH_RIGHT:
        if (record->event.pressed) {
            process_scroll(keycode);
        }
        return false;
    case NEXT_APP...PREV_WIN:
        if (record->event.pressed) {
            process_window(keycode);
        }
        return false;
    case NEXT_TAB...PREV_TAB:
        if (record->event.pressed) {
            process_tab(keycode);
        }
        return false;
    case SNIPE:
        if (record->event.pressed) {
            is_sniping = !is_sniping;
            pointing_device_set_cpi(is_sniping ? 200 : 800);
        }
        return false;
    case NEXT_RGB...PREV_RGB:
        if (record->event.pressed) {
            process_kc_rgb(keycode);
        }
        return false;
    case EXIT_MOUSE:
        return false;
    }
    // need to return after for proper handling of mod_held from enforce_kc
    return process_record_user(keycode, record);
}

#ifdef ENCODER_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_QWERTY]  = { ENCODER_CCW_CW(KC_MS_WH_UP,   KC_MS_WH_DOWN)  },
    [_COLEMAK] = { ENCODER_CCW_CW(KC_MS_WH_UP,   KC_MS_WH_DOWN)  },
    [_NUM]     = { ENCODER_CCW_CW(PREV_WIN,      NEXT_WIN)       },
    [_SYM]     = { ENCODER_CCW_CW(PREV_APP,      NEXT_APP)       },
    [_NAV]     = { ENCODER_CCW_CW(PREV_TAB,      NEXT_TAB)       },
    [_MOUSE]   = { ENCODER_CCW_CW(KC_MS_WH_LEFT, KC_MS_WH_RIGHT) },
    [_MEDIA]   = { ENCODER_CCW_CW(KC_VOLD,       KC_VOLU)        },
    [_COL6]    = { ENCODER_CCW_CW(PREV_RGB,      NEXT_RGB)       },
};
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

#ifdef POINTING_DEVICE_ENABLE
void pointing_device_init_user(void) {
    set_auto_mouse_layer(_MOUSE);
    set_auto_mouse_enable(true);
    pointing_device_set_cpi(800);
}
bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case DRAG_SCROLL:
        case KC_MS_WH_UP...KC_MS_WH_RIGHT:
            return true;
    }
    return false;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse) {
    const float divisor = 16;

    static float scroll_h = 0;
    static float scroll_v = 0;
    // can't seem to abort the drag scroll while a button is clicked?
    // mouse.buttons is also 0 regardless of click state?
    if (is_scrolling) {
        if (mouse.v || mouse.h) return mouse; // already have scrolling
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
#ifdef RGB_MATRIX_ENABLE
#define _ASSIGN_RGB(red, green, blue) r=red, g=green, b=blue
#define ASSIGN_RGB(...) _ASSIGN_RGB(__VA_ARGS__)
bool rgb_matrix_indicators_user() {//uint8_t min, uint8_t max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    uint8_t r=0, g=0, b=0;
    static bool indicated = false;
    if (layer == _MOUSE) {
        ASSIGN_RGB(RGB_TEAL);
        indicated = true;
    } else if (layer == _NUM && is_oneshot_layer_active()) {
        ASSIGN_RGB(RGB_PURPLE);
        indicated = true;
    } else if (host_keyboard_led_state().caps_lock) {
        ASSIGN_RGB(RGB_GOLDENROD);
        indicated = true;
    } else if (is_swap_hands_on()) {
        ASSIGN_RGB(RGB_TURQUOISE);
        indicated = true;
    }
    // fast reset colors when the layer is gone by checking `indicated`
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT && (r || g || b || indicated); i++) {
        if (g_led_config.flags[i] & LED_FLAG_INDICATOR) {
            rgb_matrix_set_color(i, r, g, b);
        }
    }
    if (!r && !g && !b && indicated) {
        indicated = false;
    }
    return false;
}
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    if (get_highest_layer(state) != _MOUSE) {
        is_scrolling = false;
        if (is_sniping) {
            pointing_device_set_cpi(800);
        }
        is_sniping = false;
    }
    return state;
}
#endif