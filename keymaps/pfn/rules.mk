VIA_ENABLE = no

ENCODER_MAP_ENABLE = yes

COMBO_ENABLE = yes
# CONSOLE_ENABLE = yes

SPLIT_WATCHDOG_ENABLE = yes

DIP_SWITCH_ENABLE = yes
OS_DETECTION_ENABLE = yes

# this is required still, even though INTROSPECTION_KEYMAP_C doesn't need to be set
quantum/keymap_introspection.c: $(INTERMEDIATE_OUTPUT)/src/keymap_configurator.inc

.SECONDEXPANSION:
$(INTERMEDIATE_OUTPUT)/src/keymap_configurator.inc: $$(KEYMAP_PATH)/pfn.json
	$(QMK_BIN) json2c --quiet --output $@.tmp $<
	sed -e '/ENCODER_ENABLE/,+5d' $@.tmp > $@
	rm $@.tmp