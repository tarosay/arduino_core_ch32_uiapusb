#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

// EP0 + EP1(IN)
#define ENDPOINTS 2

// UIAPduino fixed USB pins
#define USB_PORT D
#define USB_PIN_DP 3
#define USB_PIN_DM 4
// D- pull-up is external/fixed, so no USB_PIN_DPU

#define RV003USB_OPTIMIZE_FLASH    1
#define RV003USB_DEBUG_TIMING      0
#define RV003USB_EVENT_DEBUGGING   0

// Raw HID send/recv
#define RV003USB_HANDLE_IN_REQUEST 1
#define RV003USB_HANDLE_USER_DATA  1
#define RV003USB_HID_FEATURES      1
#define RV003USB_OTHER_CONTROL     0
#define RV003USB_USB_TERMINAL      0
#define RV003USB_SUPPORT_CONTROL_OUT 0
#define RV003USB_USE_REBOOT_FEATURE_REPORT 0

#ifndef __ASSEMBLER__

#include <tinyusb_hid.h>

#ifdef INSTANCE_DESCRIPTORS

#define UIAPUSB_VID  0x1209
#define UIAPUSB_PID  0xD100
#define UIAPUSB_EP_SIZE 8

// Report IDs
#define UIAPUSB_REPORT_ID_IN       0x01
#define UIAPUSB_REPORT_ID_OUT      0x02
#define UIAPUSB_REPORT_ID_FEATURE  0x03

// 8-byte raw reports
static const uint8_t uiapusb_hid_report_desc[] = {
  HID_USAGE_PAGE_N ( 0xFF00, 2 ),
  HID_USAGE        ( 0x01 ),
  HID_COLLECTION   ( HID_COLLECTION_APPLICATION ),

    // Input: device -> host
    HID_REPORT_ID    ( UIAPUSB_REPORT_ID_IN )
    HID_REPORT_SIZE  ( 8 ),
    HID_REPORT_COUNT ( UIAPUSB_EP_SIZE ),
    HID_USAGE        ( 0x01 ),
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

    // Output: host -> device
    HID_REPORT_ID    ( UIAPUSB_REPORT_ID_OUT )
    HID_REPORT_SIZE  ( 8 ),
    HID_REPORT_COUNT ( UIAPUSB_EP_SIZE ),
    HID_USAGE        ( 0x02 ),
    HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

    // Feature: host -> device / device -> host
    HID_REPORT_ID    ( UIAPUSB_REPORT_ID_FEATURE )
    HID_REPORT_SIZE  ( 8 ),
    HID_REPORT_COUNT ( UIAPUSB_EP_SIZE ),
    HID_USAGE        ( 0x03 ),
    HID_FEATURE      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

  HID_COLLECTION_END
};

static const uint8_t device_descriptor[] = {
  18,        // bLength
  1,         // bDescriptorType = Device
  0x10, 0x01,// bcdUSB = 1.10 (LS)
  0x00,      // bDeviceClass
  0x00,      // bDeviceSubClass
  0x00,      // bDeviceProtocol
  0x08,      // bMaxPacketSize0
  (uint8_t)(UIAPUSB_VID & 0xff), (uint8_t)(UIAPUSB_VID >> 8),
  (uint8_t)(UIAPUSB_PID & 0xff), (uint8_t)(UIAPUSB_PID >> 8),
  0x00, 0x01,// bcdDevice
  1,         // iManufacturer
  2,         // iProduct
  3,         // iSerialNumber
  1          // bNumConfigurations
};

static const uint8_t config_descriptor[] = {
  // Configuration
  9,         // bLength
  2,         // bDescriptorType = Configuration
  34, 0x00,  // wTotalLength
  0x01,      // bNumInterfaces
  0x01,      // bConfigurationValue
  0x00,      // iConfiguration
  0x80,      // bmAttributes
  50,        // bMaxPower = 100mA

  // Interface
  9,         // bLength
  4,         // bDescriptorType = Interface
  0x00,      // bInterfaceNumber
  0x00,      // bAlternateSetting
  0x01,      // bNumEndpoints
  0x03,      // bInterfaceClass = HID
  0x00,      // bInterfaceSubClass
  0x00,      // bInterfaceProtocol
  0x00,      // iInterface

  // HID descriptor
  9,         // bLength
  0x21,      // bDescriptorType = HID
  0x11, 0x01,// bcdHID = 1.11
  0x00,      // bCountryCode
  0x01,      // bNumDescriptors
  0x22,      // bDescriptorType[0] = Report
  sizeof(uiapusb_hid_report_desc), 0x00,

  // Endpoint IN (interrupt)
  7,         // bLength
  0x05,      // bDescriptorType = Endpoint
  0x81,      // bEndpointAddress = EP1 IN
  0x03,      // bmAttributes = Interrupt
  UIAPUSB_EP_SIZE, 0x00, // wMaxPacketSize
  1          // bInterval
};

#define STR_MANUFACTURER u"UIAPUSB"
#define STR_PRODUCT      u"CH32V003 Raw HID"
#define STR_SERIAL       u"0001"

struct usb_string_descriptor_struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  const uint16_t wString[];
};

const static struct usb_string_descriptor_struct string0 = {
  4, 3, {0x0409}
};
const static struct usb_string_descriptor_struct string1 = {
  sizeof(STR_MANUFACTURER), 3, STR_MANUFACTURER
};
const static struct usb_string_descriptor_struct string2 = {
  sizeof(STR_PRODUCT), 3, STR_PRODUCT
};
const static struct usb_string_descriptor_struct string3 = {
  sizeof(STR_SERIAL), 3, STR_SERIAL
};

const static struct descriptor_list_struct {
  uint32_t lIndexValue;
  const uint8_t *addr;
  uint8_t length;
} descriptor_list[] = {
  {0x00000100, device_descriptor, sizeof(device_descriptor)},
  {0x00000200, config_descriptor, sizeof(config_descriptor)},
  {0x00002100, config_descriptor + 18, 9},
  {0x00002200, uiapusb_hid_report_desc, sizeof(uiapusb_hid_report_desc)},
  {0x00000300, (const uint8_t *)&string0, 4},
  {0x04090301, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
  {0x04090302, (const uint8_t *)&string2, sizeof(STR_PRODUCT)},
  {0x04090303, (const uint8_t *)&string3, sizeof(STR_SERIAL)}
};

#define DESCRIPTOR_LIST_ENTRIES \
  ((sizeof(descriptor_list)) / (sizeof(struct descriptor_list_struct)))

#endif // INSTANCE_DESCRIPTORS
#endif // __ASSEMBLER__
#endif // _USB_CONFIG_H