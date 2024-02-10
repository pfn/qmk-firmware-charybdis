# KB2040 controller charybdis 3x5+3 support

Drop this repo into qmk_firmware/keyboards/pfn/charybdis/

Supports:

* KB2040
* Cirque Glidepoint
* Encoder
* Other firmware niceties

### Layout

![Keymap Layout](layout.svg)

Generated using `keymap parse -q keymaps/pfn/pfn.json | keymap draw -j info.json - > layout.svg` from https://github.com/caksoylar/keymap-drawer

Notes:
* info.json matrix is slightly swapped to be compatible with crkbd layouts
  but swaps the inner and outer thumb keys because of positioning makes the
  inner-most thumb key uncomfortable for my primary space/enter usage
  
This is all used in conjunction with my userspace at https://github.com/pfn/qmk_userspace