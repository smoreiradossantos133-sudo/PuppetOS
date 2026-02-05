/*
 * PCI Bus Enumeration & Device Management
 * Enables hardware device detection and driver loading
 */

#ifndef PCI_H
#define PCI_H

#include <kernel/kernel.h>

/* ===== PCI VENDOR/DEVICE IDS ===== */
#define PCI_VENDOR_INTEL        0x8086
#define PCI_VENDOR_AMD          0x1022
#define PCI_VENDOR_VMware       0x15ad
#define PCI_VENDOR_VirtualBox   0x80EE

/* ===== PCI DEVICE CLASSES ===== */
#define PCI_CLASS_STORAGE       0x01
#define PCI_CLASS_NETWORK       0x02
#define PCI_CLASS_DISPLAY       0x03
#define PCI_CLASS_MULTIMEDIA    0x04
#define PCI_CLASS_MEMORY        0x05
#define PCI_CLASS_BRIDGE        0x06
#define PCI_CLASS_COMMUNICATION 0x07

/* ===== PCI DEVICE STRUCTURE ===== */
typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    
    uint16_t vendor_id;
    uint16_t device_id;
    
    uint8_t class_code;
    uint8_t subclass_code;
    uint8_t prog_if;
    
    uint32_t bar[6];           /* Base Address Registers */
    uint32_t rom_base;         /* ROM base address */
    
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    
    uint8_t revision_id;
    bool is_bridge;
    
    void *driver_data;
} pci_device_t;

/* ===== PCI DRIVER INTERFACE ===== */
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    
    int (*probe)(pci_device_t *device);
    void (*remove)(pci_device_t *device);
    void (*suspend)(pci_device_t *device);
    void (*resume)(pci_device_t *device);
} pci_driver_t;

/* ===== PCI FUNCTIONS ===== */
void pci_init(void);
void pci_enumerate_devices(void);
pci_device_t *pci_find_device(uint16_t vendor_id, uint16_t device_id);
void pci_register_driver(pci_driver_t *driver);
void pci_load_drivers(void);

/* ===== PCI CONFIG I/O ===== */
uint8_t pci_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

void pci_write_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t value);
void pci_write_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t value);
void pci_write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);

#endif /* PCI_H */
