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

#define POINTER_SPEED 700
#define POINTER_SNIPE_SPEED 200
#define SLAVE_SYNC_TIME_MS 60

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
    NEXT_WORD,
    PREV_WORD,
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
    numpad_layer = _COL6;
}

enum my_combos {
  SD_SHOS,
  KL_SHOS,
  ER_CAPS,
  UI_CAPS,
  CV_NUM,
  MCOM_NUM,
  WE_ESC,
  JK_BS,
  COMBO_COUNT
};

const uint16_t COMBO_LEN = COMBO_COUNT;

#define COMBO_KEYS(name, keycodes...) const uint16_t PROGMEM name ## _combo[] = { keycodes, COMBO_END }
#define MAKE_COMBO(name, keycode) [name] = COMBO(name ## _combo, keycode)
COMBO_KEYS(SD_SHOS,      KC_S, LT(_MOUSE, KC_D));
COMBO_KEYS(KL_SHOS,      KC_K, KC_L);
COMBO_KEYS(ER_CAPS,      KC_E, KC_R);
COMBO_KEYS(UI_CAPS,      KC_U, KC_I);
COMBO_KEYS(JK_BS,        KC_J, KC_K);
COMBO_KEYS(CV_NUM,       LALT_T(KC_C), LCTL_T(KC_V));
COMBO_KEYS(MCOM_NUM,     RCTL_T(KC_M), RALT_T(KC_COMMA));
COMBO_KEYS(WE_ESC,       KC_W, KC_E);

combo_t key_combos[COMBO_COUNT] = {
    MAKE_COMBO(SD_SHOS,      SH_OS),
    MAKE_COMBO(KL_SHOS,      SH_OS),
    MAKE_COMBO(ER_CAPS,      KC_CAPS),
    MAKE_COMBO(UI_CAPS,      KC_CAPS),
    MAKE_COMBO(CV_NUM,       OSL(_COL6)),
    MAKE_COMBO(MCOM_NUM,     OSL(_COL6)),
    MAKE_COMBO(JK_BS,        KC_BSPC),
    MAKE_COMBO(WE_ESC,       KC_ESC),
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
    } else if (get_mods() & MOD_MASK_GUI) {
        RGB_NEXT_PREV(keycode, rgb_matrix_increase_speed(), rgb_matrix_decrease_speed());
    } else {
        RGB_NEXT_PREV(keycode, rgb_matrix_step(), rgb_matrix_step_reverse());
    }
}

void process_word(uint16_t keycode) {
    os_variant_t os = detected_host_os();
    uint16_t kc = keycode == NEXT_WORD ? KC_RGHT : KC_LEFT;
    uint16_t mod = os == OS_MACOS || os == OS_IOS ? KC_LALT : KC_LCTL;
    register_code(mod);
    tap_code(kc);
    unregister_code(mod);
}

void process_window(uint16_t keycode) {
    uint16_t mod_code = KC_LALT;
    os_variant_t os = detected_host_os();
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
#           ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
            cirque_pinnacle_enable_cursor_glide(is_scrolling);
#           endif
        }
        return false;
    case KC_MS_BTN1 ... KC_MS_BTN8:
        if (is_scrolling) {
            is_scrolling = false;
#           ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
            cirque_pinnacle_enable_cursor_glide(is_scrolling);
#           endif
        }
        break;
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
    case NEXT_WORD ... PREV_WORD:
        if (record->event.pressed) {
            process_word(keycode);
        }
        return false;
    case SNIPE:
        if (record->event.pressed) {
            is_sniping = !is_sniping;
            pointing_device_set_cpi(is_sniping ? POINTER_SNIPE_SPEED : POINTER_SPEED);
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
    [_QWERTY]  = { ENCODER_CCW_CW(KC_VOLD,  KC_VOLU)  },
    [_COLEMAK] = { ENCODER_CCW_CW(KC_VOLD,  KC_VOLU)  },
    [_NUM]     = { ENCODER_CCW_CW(PREV_WIN, NEXT_WIN) },
    [_SYM]     = { ENCODER_CCW_CW(PREV_APP, NEXT_APP) },
    [_NAV]     = { ENCODER_CCW_CW(PREV_TAB, NEXT_TAB) },
    [_MOUSE]   = { ENCODER_CCW_CW(KC_WH_L,  KC_WH_R)  },
    [_MEDIA]   = { ENCODER_CCW_CW(KC_WH_U,  KC_WH_D)  },
    [_COL6]    = { ENCODER_CCW_CW(PREV_RGB, NEXT_RGB) },
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
uint32_t mouse_timer = 0;
void pointing_device_init_user(void) {
#   ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
    cirque_pinnacle_enable_cursor_glide(false);
#   endif
    if (is_keyboard_master() && is_keyboard_left()) {
        set_auto_mouse_timeout(SLAVE_SYNC_TIME_MS + 5);
    }
    set_auto_mouse_layer(_MOUSE);
    set_auto_mouse_enable(true);
    pointing_device_set_cpi(POINTER_SPEED);
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
    const float divisor = 48;

    static float scroll_h = 0;
    static float scroll_v = 0;
    if (is_scrolling) {
        #if !CIRQUE_PINNACLE_POSITION_MODE
        if (mouse.v || mouse.h) return mouse; // already have scrolling
        #endif
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
    if (auto_mouse_activation(mouse))
        mouse_timer = timer_read32();
    return mouse;

}
bool is_swaphands = false;
#ifdef RGB_MATRIX_ENABLE
#define _ASSIGN_RGB(red, green, blue) r=red, g=green, b=blue
#define ASSIGN_RGB(...) _ASSIGN_RGB(__VA_ARGS__)
bool indicate_osm_led(uint8_t mod_mask) {
    bool indicated = false;
    if (!mod_mask) {
        return indicated;
    }
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint16_t kc = keymap_key_to_keycode(_NUM, (keypos_t){col,row});
            if (IS_QK_ONE_SHOT_MOD(kc)) {
                if (QK_ONE_SHOT_MOD_GET_MODS(kc) & mod_mask) {
                    rgb_matrix_set_color(g_led_config.matrix_co[row][col], RGB_WHITE);
                    indicated = true;
                }
            }
        }
    }
    return indicated;
}

os_variant_t slave_detected_os = OS_UNSURE;
bool rgb_matrix_indicators_user() {//uint8_t min, uint8_t max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    uint8_t r=0, g=0, b=0;
    static bool indicated = false;
    if (is_scrolling) {
        ASSIGN_RGB(RGB_CHARTREUSE);
        indicated = true;
    } else if (is_sniping) {
        ASSIGN_RGB(RGB_RED);
        indicated = true;
    } else if (layer == _MOUSE) {
        ASSIGN_RGB(RGB_TEAL);
        indicated = true;
    } else if (host_keyboard_led_state().num_lock) {
        ASSIGN_RGB(RGB_PURPLE);
        indicated = true;
    } else if (host_keyboard_led_state().caps_lock) {
        ASSIGN_RGB(RGB_GOLDENROD);
        indicated = true;
    } else if (is_swaphands || is_swap_hands_on()) {
        ASSIGN_RGB(RGB_TURQUOISE);
        indicated = true;
    }
    indicated = indicated || indicate_osm_led(get_oneshot_mods());
    // fast reset colors when the layer is gone by checking `indicated`
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT && (r || g || b || indicated); i++) {
        if (g_led_config.flags[i] & LED_FLAG_INDICATOR) {
            rgb_matrix_set_color(i, r, g, b);
        }
    }
    if (!r && !g && !b && indicated) {
        indicated = false;
    }
    // indicate OS detection
    r = g = b = 0;
    for (uint8_t i = 0, found = 0; i < RGB_MATRIX_LED_COUNT && !found; i++) {
        if (g_led_config.flags[i] & LED_FLAG_INDICATOR) {
            os_variant_t os = detected_host_os();
            if (os == OS_MACOS || os == OS_IOS || slave_detected_os == OS_MACOS || slave_detected_os == OS_IOS) {
                // finder blue
                r = 0x1c; g = 0x9f; b = 0xf9;
            } else if (os == OS_WINDOWS || slave_detected_os == OS_WINDOWS) {
                // microsoft orange
                r = 0xff; g = 0xbb; b = 0;
            } else {
                ASSIGN_RGB(RGB_WHITE);
            }
            // if (r || g || b) {
                rgb_matrix_set_color(i, r, g, b);
            // }
            found = 1;
        }
    }
    return false;
}
#endif
#endif

#include "transactions.h"
typedef struct {
    bool is_scrolling : 1;
    bool is_sniping : 1;
    bool is_swaphands : 1;
    uint8_t detected_os : 2;
    uint8_t unused : 3;
} __attribute__((packed)) split_transport_data_user;

void sync_slave_state(void) {
    split_transport_data_user state;
    state.is_sniping = is_sniping;
    state.is_scrolling = is_scrolling;
    state.is_swaphands = is_swap_hands_on();
    switch (detected_host_os()) {
        case OS_WINDOWS:
            state.detected_os = 1;
            break;
        case OS_MACOS:
        case OS_IOS:
            state.detected_os = 2;
            break;
        default:
            state.detected_os = 0;
            break;
    }
    transaction_rpc_send(SYNC_STATE_USER, sizeof(split_transport_data_user), &state);
}

void slave_transport_handler_user(uint8_t in_len, const void* in_data, uint8_t out_len, void *out_data) {
    const split_transport_data_user *data = (const split_transport_data_user *) in_data;
    is_scrolling = data->is_scrolling;
    is_sniping = data->is_sniping;
    is_swaphands = data->is_swaphands;
    switch (data->detected_os) {
        case 1:
            slave_detected_os = OS_WINDOWS;
            break;
        case 2:
            slave_detected_os = OS_MACOS;
            break;
    }
    rgb_matrix_indicators();
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(SYNC_STATE_USER, slave_transport_handler_user);
}

void housekeeping_task_kb() {
    if (is_keyboard_master()) {
        static uint32_t last_sync = 0;
        if (timer_elapsed32(last_sync) > SLAVE_SYNC_TIME_MS) {
            last_sync = timer_read32();
            sync_slave_state();
        }

        if ((is_sniping || is_scrolling) && timer_elapsed32(mouse_timer) > 2000 && get_highest_layer(layer_state) != _MOUSE) {
            if (is_sniping) {
                pointing_device_set_cpi(POINTER_SPEED);
            }
            is_sniping = false;
            is_scrolling = false;
#           ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
            cirque_pinnacle_enable_cursor_glide(false);
#           endif
        }
    }
}