#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include "common/tusb_common.h"
#include "device/usbd.h"
#include "piuio.h"

// Gamepad Report Descriptor Template
// with PIUIO_HID_BTN_NUM buttons and 2 joysticks with following layout
// | Button Map (2 bytes) |  X | Y | Z | Rz
// Logical max/min set such that 0 is a centered joystick.
#define GAMECON_REPORT_DESC_GAMEPAD(...)                   \
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                \
        HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),              \
        HID_COLLECTION(HID_COLLECTION_APPLICATION),        \
        __VA_ARGS__                                        \
        HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),             \
        HID_USAGE_MIN(1),                                  \
        HID_USAGE_MAX(PIUIO_HID_BTN_NUM),                  \
        HID_LOGICAL_MIN(0),                                \
        HID_LOGICAL_MAX(1),                                \
        HID_REPORT_COUNT(PIUIO_HID_BTN_NUM),               \
        HID_REPORT_SIZE(1),                                \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), \
        HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),            \
        HID_LOGICAL_MIN(0x81),                             \
        HID_LOGICAL_MAX(0x7f),                             \
        HID_USAGE(HID_USAGE_DESKTOP_Z),                    \
        HID_USAGE(HID_USAGE_DESKTOP_RX),                   \
        HID_USAGE(HID_USAGE_DESKTOP_RY),                   \
        HID_USAGE(HID_USAGE_DESKTOP_RZ),                   \
        HID_REPORT_COUNT(4),                               \
        HID_REPORT_SIZE(8),                                \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), \
        HID_COLLECTION_END

#define GAMECON_REPORT_DESC_LIGHTS(...)                        \
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                    \
        HID_USAGE(0x00),                                       \
        HID_COLLECTION(HID_COLLECTION_APPLICATION),            \
        __VA_ARGS__                                            \
        HID_REPORT_COUNT(PIUIO_HID_NUM_LIGHTS),                \
        HID_REPORT_SIZE(8),                                    \
        HID_LOGICAL_MIN(0x00),                                 \
        HID_LOGICAL_MAX_N(0x00ff, 2),                          \
        HID_USAGE_PAGE(HID_USAGE_PAGE_ORDINAL),                \
        0x89, 0x04, /* 4 is the start of the string index*/    \
        0x99, 0x04 + PIUIO_HID_NUM_LIGHTS,                     \
        HID_USAGE_MIN(1),                                      \
        HID_USAGE_MAX(PIUIO_HID_NUM_LIGHTS),                   \
        HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),    \
        HID_USAGE_MIN(1),                                      \
        HID_USAGE_MAX(1),                                      \
        HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE), \
        HID_COLLECTION_END

#endif