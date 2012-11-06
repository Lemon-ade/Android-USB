#include "ch9.h"

static USB_DEVICE_DESCRIPTOR
device_desc = {
    .bLength = 0x12,               // Length of this descriptor.
    .bDescriptorType = 0x01,  // DEVICE descriptor type (USB_DESCRIPTOR_DEVICE).
    .bcdUSB = 0x0200,      // USB Spec Release Number (BCD).
    .bDeviceClass = 0x00,          // Class code (assigned by the USB-IF). 0xFF-Vendor specific.
    .bDeviceSubClass = 0x00,       // Subclass code (assigned by the USB-IF).
    .bDeviceProtocol = 0x00,       // Protocol code (assigned by the USB-IF). 0xFF-Vendor specific.
    .bMaxPacketSize0 = 0x08,       // Maximum packet size for endpoint 0.
    .idVendor = 0x0461,      // Vendor ID (assigned by the USB-IF).
    .idProduct = 0x4d16,     // Product ID (assigned by the manufacturer).
    .bcdDevice = 0x0200,      // Device release number (BCD).
    .iManufacturer = 0x00,         // Index of String Descriptor describing the manufacturer.
    .iProduct = 0x02,              // Index of String Descriptor describing the product.
    .iSerialNumber = 0x00,         // Index of String Descriptor with the device's serial number.
    .bNumConfigurations = 0x01    // Number of possible configurations.
};

/* Configuration descriptor structure */
static USB_CONFIGURATION_DESCRIPTOR 
config_desc = {
    .bLength = 0x09,               // Length of this descriptor.
    .bDescriptorType = 0x02,       // CONFIGURATION descriptor type (USB_DESCRIPTOR_CONFIGURATION).
    .wTotalLength = 0x0022,          // Total length of all descriptors for this configuration.
    .bNumInterfaces = 0x01,        // Number of interfaces in this configuration.
    .bConfigurationValue = 0x01,   // Value of this configuration (1 based).
    .iConfiguration = 0x00,        // Index of String Descriptor describing the configuration.
    .bmAttributes = 0xa0,          // Configuration characteristics.
    .bMaxPower = 0x32             // Maximum power consumed by this configuration.
};

/* Interface descriptor structure */
static USB_INTERFACE_DESCRIPTOR 
intf_desc = {
    .bLength = 0x09,               // Length of this descriptor.
    .bDescriptorType = 0x04,       // INTERFACE descriptor type (USB_DESCRIPTOR_INTERFACE).
    .bInterfaceNumber = 0x00,      // Number of this interface (0 based).
    .bAlternateSetting = 0x01,     // Value of this alternate interface setting.
    .bNumEndpoints = 0x03,         // Number of endpoints in this interface.
    .bInterfaceClass = 0x03,       // Class code (assigned by the USB-IF).  0xFF-Vendor specific.
    .bInterfaceSubClass = 0x01,    // Subclass code (assigned by the USB-IF).
    .bInterfaceProtocol = 0x02,    // Protocol code (assigned by the USB-IF).  0xFF-Vendor specific.
    .iInterface = 0x00            // Index of String Descriptor describing the interface.
};

/* Endpoint descriptor structure */
static USB_ENDPOINT_DESCRIPTOR
endpoint_desc = {
    .bLength = 0x07,               // Length of this descriptor.
    .bDescriptorType = 0x05,       // ENDPOINT descriptor type (USB_DESCRIPTOR_ENDPOINT).
    .bEndpointAddress = 0x81,      // Endpoint address. Bit 7 indicates direction (0=OUT, 1=IN).
    .bmAttributes = 0x03,          // Endpoint transfer type.
    .wMaxPacketSize = 0x0004,        // Maximum packet size.
    .bInterval = 0x0a             // Polling interval in frames.
};

/* HID descriptor */
static USB_HID_DESCRIPTOR 
hid_desc = {
    .bLength = 0x09,                       
    .bDescriptorType = 0x021,   
    .bcdHID = 0x0111,                    
    .bCountryCode = 0x00,          
    .bNumDescriptors = 0x01,
    .bDescrType = 0x22,                        
    .wDescriptorLength = 0x34
};
