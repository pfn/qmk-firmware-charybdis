VIA_ENABLE = no

ENCODER_MAP_ENABLE = yes
OLED_ENABLE = no
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = cirque_pinnacle_i2c

COMBO_ENABLE = yes
ENCODER_ENABLE = yes
# CONSOLE_ENABLE = yes

MOUSEKEY_ENABLE = yes
DIP_SWITCH_ENABLE = yes
SWAP_HANDS_ENABLE = yes
OS_DETECTION_ENABLE = yes

INTROSPECTION_KEYMAP_C=$(INTERMEDIATE_OUTPUT)/src/keymap_configurator.inc

quantum/keymap_introspection.c: $(INTERMEDIATE_OUTPUT)/src/keymap_configurator.inc
.SECONDEXPANSION:
$(INTERMEDIATE_OUTPUT)/src/keymap_configurator.inc: $$(KEYMAP_PATH)/pfn.json
	$(QMK_BIN) json2c --quiet --output $@.tmp $<
	sed -e '/ENCODER_ENABLE/,+5d' $@.tmp > $@
	rm $@.tmp