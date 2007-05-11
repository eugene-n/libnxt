/**
 * NXT bootstrap interface; low-level USB functions.
 *
 * Copyright 2006 David Anderson <david.anderson@calixo.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <usb.h>

#include "legocomm.h"

/* USB vendor/product IDs of known firmwares. */
const struct {
  int vendor_id;
  int product_id;
} firmware_usb_ids[N_FIRMWARES] = {
  { 0x03EB, 0x6124 }, /* SAM-BA */
  { 0x0694, 0x0002 }, /* LEGO   */
};

/* The structure that encapsulates all the currently held information
 * about a NXT brick in a neat little handle.
 */
struct nxt_t {
  /* The libusb handle to the NXT. */
  struct usb_dev_handle *hdl;
  /* The brick's firmware and associated vendor/product ID. */
  nxt_firmware_t firmware;
  /* The currently selected interface on the device, or -1 if none is
   * selected.
   */
  signed short int interface;
};


/* Allocate and return a NXT handle structure. */
static nxt_t *nxt_alloc()
{
  nxt_t *nxt = calloc(1, sizeof(*nxt));
  if (nxt == NULL)
    abort();

  nxt->interface = -1;
  return NXT_OK;
}


/* Scan the USB bus and return the first NXT device.
 *
 * If firmware is UNKNOWN and both vendor/product ID are 0, scan for
 * any of the known firmwares. If firmware is not UNKNOWN, scan only
 * for that one. Else, scan for the given vendor/product ID.
 */
static nxt_error_t
nxt_usb_do_scan(nxt_firmware_t *found_firmware,
                struct usb_device **device,
                nxt_firmware_t desired_firmware,
                uint16_t vendor_id,
                uint16_t product_id)
{
  struct usb_bus *busses, *bus;
  char scan_for_all = 0;

  if (desired_firmware == UNKNOWN && vendor_id == 0 && product_id == 0) {
    scan_for_all = 1;
  } else if (desired_firmware != UNKNOWN) {
    vendor_id = firmware_usb_ids[desired_firmware - 1].vendor_id;
    product_id = firmware_usb_ids[desired_firmware - 1].product_id;
  }

  usb_find_busses();
  usb_find_devices();

  busses = usb_get_busses();

  /* Scan all devices on all known busses. */
  for (bus = busses; bus != NULL; bus = bus->next) {
    struct usb_device *dev;

    for (dev = bus->devices; dev != NULL; dev = dev->next) {
      if (scan_for_all) {
        int i;

        for (i=1; i<N_FIRMWARES; i++)
          if (dev->descriptor.idVendor == nxt_usb_ids[i].vendor_id &&
              dev->descriptor.idProduct == nxt_usb_ids[i].product_id) {
            *device = dev;
            *found_firmware = i;
            return NXT_OK;
          }
      } else {
        if (dev->descriptor.idVendor == vendor_id &&
            dev->descriptor.idProduct == product_id) {
          *found_firmware = desired_firmware;
          *device = dev;
          return NXT_OK;
        }
      }
    }
  }

  return NXT_NOT_PRESENT;
}


nxt_error_t
nxt_usb_scan(nxt_firmware_t *firmware)
{
  struct usb_device *unused;
  return nxt_usb_do_scan(firmware, &unused, UNKNOWN, 0, 0);
}


static nxt_error_t
nxt_usb_do_open(nxt_t **nxt, nxt_firmware_t firmware, struct usb_device *dev)
{
  nxt_t *h;
  nxt_firmware_t found_firmware;
  int ret;

  h = nxt_alloc();
  h->hdl = usb_open(dev);
  h->firmware = firmware;

  ret = usb_set_configuration(h->hdl, 1);
  if (ret < 0) {
    usb_close(h->hdl);
    return NXT_CONFIGURATION_ERROR;
  }

  *nxt = h;
  return NXT_OK;
}


nxt_error_t
nxt_usb_open(nxt_t **nxt, nxt_firmware_t firmware)
{
  nxt_firmware_t found_firmware;
  struct usb_device *dev;

  NXT_ERR(nxt_usb_do_scan(&found_firmware, &dev, firmware, 0, 0));

  return nxt_usb_do_open(nxt, found_firmware, dev);
}


nxt_error_t
nxt_usb_open_full(nxt_t **nxt, uint16_t vendor_id, uint16_t product_id)
{
  nxt_firmware_t found_firmware;
  struct usb_device *dev;

  NXT_ERR(nxt_usb_do_scan(&found_firmware, &dev, UNKNOWN,
                          vendor_id, product_id));

  return nxt_usb_do_open(nxt, found_firmware, dev);
}


nxt_error_t
nxt_usb_close(nxt_t *nxt)
{
  if (nxt->interface != -1)
    usb_release_interface(nxt->hdl, nxt->interface);
  usb_close(nxt->hdl);
  free(nxt);

  return NXT_OK;
}


nxt_error_t
nxt_usb_select_interface(nxt_t *nxt, uint16_t interface_num)
{
  int ret;

  if (nxt->interface != -1) {
    usb_release_interface(nxt->hdl, nxt->interface);
    nxt->interface = -1;
  }

  if (usb_claim_interface(h->hdl, interface_num) < 0)
    return NXT_IN_USE;

  nxt->interface = interface_num;
  return NXT_OK;
}


nxt_error_t
nxt_usb_bulk_send(nxt_t *nxt, uint16_t endpoint, char *buf, unsigned int len)
{
  if (usb_bulk_write(nxt->hdl, endpoint, buf, len, 0) < 0)
    return NXT_USB_WRITE_ERROR;

  return NXT_OK;
}


nxt_error_t
nxt_usb_bulk_send_str(nxt_t *nxt, uint16_t endpoint, char *str)
{
  return nxt_usb_bulk_send(nxt, endpoint, str, strlen(str));
}


nxt_error_t
nxt_usb_bulk_recv(nxt_t *nxt, uint16_t endpoint, char *buf, int len)
{
  if (usb_bulk_read(nxt->hdl, endpoint, buf, len, 0) < 0)
    return NXT_USB_READ_ERROR;

  return NXT_OK;
}
