/**
 * Lego Mindstorms NXT interface library.
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

#ifndef __LEGO_NXT_H__
#define __LEGO_NXT_H__

#include <stdint.h>

/** @defgroup types Basic types
 *
 * @{
 */

/** Opaque handle to an NXT brick. */
typedef struct nxt_t nxt_t;


/** Errors that can be returned by the library. */
typedef enum
{
  /** No error. */
  NXT_OK = 0,
  /** Initialization failed. */
  NXT_INIT_FAILED = 1,
  /** Unknown error */
  NXT_UNKNOWN_ERROR = 2,
  /** File open/handling error. */
  NXT_FILE_ERROR = 3,

  /*
   * Low-level USB errors.
   */

  /** The NXT was not found on the USB bus. */
  NXT_USB_NOT_FOUND = 100,
  /** Error trying to configure the USB link. */
  NXT_USB_CONFIGURATION_ERROR = 101,
  /** The NXT is already reserved by another USB application. */
  NXT_USB_ALREADY_IN_USE = 102,
  /** USB write failed. */
  NXT_USB_WRITE_ERROR = 103,
  /** USB read failed. */
  NXT_USB_READ_ERROR = 104,

  /*
   * Errors related to the SAM Boot Assistant component.
   */

  /** SAM-BA protocol error. */
  NXT_SAMBA_PROTOCOL_ERROR = 200,
  /** SAM-BA handshake failed. */
  NXT_SAMBA_HANDSHAKE_FAILED = 201,
  /** Invalid firmware image provided for writing. */
  NXT_SAMBA_INVALID_FIRMWARE = 202,
} nxt_error_t;


/** A list of known NXT firmwares. */
typedef enum {
  /** Unknown firmware. */
  UNKNOWN = 0,
  /** Atmel SAM7 Boot Assistant. */
  SAMBA,
  /** The official Lego firmware. */
  LEGO,
  N_FIRMWARES,
} nxt_firmware_t;

/*@}*/



/** @defgroup core Core functions
 *
 * The core functions handle global initialization and error
 * reporting.
 *
 * @{
 */

/** Initialize the library.
 *
 * The initialization function can be called multiple times without
 * side-effects (all calls but the first are ignored). This function
 * should be called in a single-threaded context.
 *
 * @warning This function @b must be called before any other functions
 * in the library, and is @b not threadsafe.
 */
nxt_error_t nxt_init();


/** Return the descriptive string associated with an error.
 *
 * If the error code is not known, the string "Unknown error" is
 * returned.
 *
 * @param[in] err The error code.
 */
const char const *nxt_strerror(nxt_error_t err);


/** Macro that can be used for nice looking error propagation.
 *
 * If the given expression (usually a library call returning @a
 * nxt_error_t) evaluates to something other than NXT_OK, return the
 * resulting error code.
 */
#define NXT_ERR(expr)                   \
  do {                                  \
    nxt_error_t nxt__err_temp = (expr); \
    if (nxt__err_temp)                  \
      return nxt__err_temp;             \
  } while(0)

/*@}*/



/** @defgroup usb Low level USB communication
 *
 * This component contains low level USB functionality.  With it, you
 * can:
 *  - Scan the USB bus for NXT bricks running known firmwares.
 *  - Open a link to a NXT brick running a specific firmware. Either
 *    one of the known ones, or by passing the vendor/product ID of an
 *    unknown firmware.
 *  - Send and receive bulk data to an open brick.
 *
 * @{
 */

/** Scan the USB bus for a NXT brick.
 *
 * @param[out] firmware The firmware on the brick, if any.
 *
 * @return NXT_OK if a brick is found, with @a *firmware set to the
 * detected firmware. If no brick is found, returns NXT_USB_NOT_FOUND.
*/
nxt_error_t nxt_usb_scan(nxt_firmware_t *firmware);


/** Create a handle to a NXT brick over USB.
 *
 * @param[out] nxt The pointer to the created handle is placed here.
 * @param[in] firmware The firmware the brick should be running.
 *
 * @return NXT_OK if a brick with the specified firmware is found and
 * opened successfully, or an NXT_USB_* error otherwise.
 */
nxt_error_t nxt_usb_open(nxt_t **nxt, nxt_firmware_t firmware);


/** Create a handle to a NXT brick running an unknown firmware.
 *
 * This function is very similar to \ref nxt_usb_open, except that
 * instead of a "known firmware ID", it locates the brick based on the
 * given USB vendor ID, product ID and product revision. This function
 * can be used to communicate with firmwares that are not yet known to
 * the library.
 *
 * @param[out] nxt The pointer to the created handle is placed here.
 * @param[in] vendor_id The USB vendor ID that the brick's firmware reports.
 * @param[in] product_id The USB product ID that the brick's firmware reports.
 *
 * @return NXT_OK if a brick with the specified USB IDs is found and
 * opened successfully, or an NXT_USB_* error otherwise.
 */
nxt_error_t nxt_usb_open_full(nxt_t **nxt, uint16_t vendor_id,
                              uint16_t product_id);


/** Close the given USB link and free all associated resources.
 *
 * The handle passed to this function is no longer valid after
 * execution of the function, and should no longer be used.
 *
 * @param[in] nxt The handle to the brick.
 */
nxt_error_t nxt_usb_close(nxt_t *nxt);


/** Select the USB interface to use for communication with a brick.
 *
 * Different firmwares present different interfaces to the USB host,
 * and depending on the firmware the use of different interfaces are
 * required. This function lets the caller select the right interface
 * for the firmware, and/or switch interfaces during operation.
 *
 * @param[in] nxt The handle to the brick that is being reconfigured.
 * @param[in] interface_num The interface that should be selected.
 *
 * @return NXT_OK if the requested interface was successfully
 * obtained, or an NXT_USB_* error otherwise.
 */
nxt_error_t nxt_usb_select_interface(nxt_t *nxt, uint16_t interface_num);


/** Send data to a bulk endpoint of an open NXT brick.
 *
 * Note that this operation will block until all data is sent.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] endpoint The USB endpoint to send the data to.
 * @param[in] buf The data to send.
 * @param[in] len The amount of data to send.
 *
 * @return NXT_OK if the data was successfully sent, or an NXT_USB_*
 * error otherwise.
 */
nxt_error_t nxt_usb_bulk_send(nxt_t *nxt, uint16_t endpoint,
                              char *buf, unsigned int len);


/** Send a string to a bulk endpoint of an open NXT brick.
 *
 * This is just a convenience wrapper around \ref nxt_usb_bulk_send
 * when dealing with null-terminated strings. The trailing NULL byte
 * is not sent.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] endpoint The USB endpoint to send the data to.
 * @param[in] str The null terminated string to send.
 *
 * @return NXT_OK if the data was successfully sent, or an NXT_USB_*
 * error otherwise.
 */
nxt_error_t nxt_usb_bulk_send_str(nxt_t *nxt, uint16_t endpoint, char *str);


/** Receive data from a bulk endpoint of an open NXT brick.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] endpoint The USB endpoint to send the data to.
 * @param[in] buf The buffer that should receive the data.
 * @param[in] len The amount of data to read from the endpoint, in bytes.
 *
 * @return NXT_OK if the data was successfully read, or an NXT_USB_*
 * error otherwise.
 */
nxt_error_t nxt_usb_bulk_recv(nxt_t *nxt, uint16_t endpoint,
                              char *buf, unsigned int len);

/*@}*/



/** @defgroup samba Communication with the Atmel SAM Boot Assistant
 *
 * This component builds upon the low-level USB communication module,
 * and enables communication with the Atmel SAM7 Boot Assistant, which
 * is the firmware that runs the NXT when it is in reset mode
 * ("clicking brick mode").
 *
 * The entire SAM-BA commandset is implemented, and a few higher level
 * utility functions are provided to perform common operations, like
 * firmware flashing (which requires an additional in RAM driver on
 * the NXT, and a specific command sequence to control the flash
 * chip).
 */

/** Locate and open a NXT brick running the SAM-BA.
 * @ingroup samba
 *
 * A USB link to the brick is open, and the initial SAM-BA handshake
 * is performed.
 *
 * @param[out] nxt The pointer to the created handle is placed here.
 *
 * @return NXT_OK if the communication was successfully established,
 * or another error code otherwise.
 */
nxt_error_t nxt_samba_open(nxt_t **nxt);


/** @defgroup samba_commandset SAM-BA commandset
 * @{
 */


/** Write a byte into the NXT's memory.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The destination address in the NXT's memory.
 * @param[in] b The value to write.
 *
 * @return NXT_OK if the write succeeded, or an error code otherwise.
 */
nxt_error_t nxt_write_byte(nxt_t *nxt, uint32_t addr, unsigned char b);


/** Write a 2 byte halfword into the NXT's memory.
 *
 * @note The halfword should be provided in the native byte ordering
 * of the machine.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The destination address in the NXT's memory.
 * @param[in] hw The value to write.
 *
 * @return NXT_OK if the write succeeded, or an error code otherwise.
 */
nxt_error_t nxt_write_hword(nxt_t *nxt, uint32_t addr, uint16_t hw);


/** Write a 4 byte word into the NXT's memory.
 *
 * @note The word should be provided in the native byte ordering of
 * the machine.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The destination address in the NXT's memory.
 * @param[in] w The value to write.
 *
 * @return NXT_OK if the write succeeded, or an error code otherwise.
 */
nxt_error_t nxt_write_word(nxt_t *nxt, uint32_t addr, uint32_t w);


/** Read a byte from the NXT's memory.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The source address in the NXT's memory.
 * @param[in] b Storage for the read value.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_read_byte(nxt_t *nxt, uint32_t addr, unsigned char *b);


/** Read a 2 byte halfword from the NXT's memory.
 *
 * @note The value read will be converted to the machine's native byte
 * ordering.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The source address in the NXT's memory.
 * @param[in] hw Storage for the read value.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_read_hword(nxt_t *nxt, uint32_t addr, uint16_t *hw);


/** Read a 4 byte word from the NXT's memory.
 *
 * @note The value read will be converted to the machine's native byte
 * ordering.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The source address in the NXT's memory.
 * @param[in] w Storage for the read value.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_read_word(nxt_t *nxt, uint32_t addr, uint32_t *w);


/** Transfer the given buffer into the NXT's memory.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The destination address in the NXT's memory.
 * @param[in] buf The buffer containing the data to send.
 * @param[in] len The length of the buffer to send.
 *
 * @return NXT_OK if the write succeeded, or an error code otherwise.
 */
nxt_error_t nxt_send_buf(nxt_t *nxt, uint32_t addr,
                         char *buf, unsigned short len);


/** Transfer a memory region from the NXT into the given buffer.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The source address in the NXT's memory.
 * @param[in] buf The buffer that is to receive the read data.
 * @param[in] len The amount of data that should be transferred.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_recv_buf(nxt_t *nxt, uint32_t addr,
                         char *buf, unsigned short len);


/** Make SAM-BA jump call a routine in the NXT's memory.
 *
 * The routine itself should probably be transferred into the NXT's
 * ram first, using @ref nxt_send_buf.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The address of the routine to execute.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_jump(nxt_t *nxt, uint32_t addr);


/** Request the version of the SAM-BA firmware.
 *
 * @param[in] nxt The handle to the brick.
 * @param[out] version A pointer to a 4 byte buffer where the version
 * string will be placed.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_samba_version(nxt_t *nxt, char *version);

/*@}*/

/** @defgroup samba_flash Flash chip programming
 * @ingroup samba
 *
 * Programming the NXT's flash chip requires a precise sequence of
 * commands issued through SAM-BA. To help users of the library with
 * this relatively frequent task, the following functions provide a
 * high level API for writing data to the flash controller.
 *
 * @{
 */


/** Write the given buffer to flash memory.
 *
 * @param[in] nxt The handle to the brick.
 * @param[in] addr The address to write to in flash memory.
 * @param[in] buf The buffer containing the data to write.
 * @param[in] len The amount of data to write.
 *
 * @return NXT_OK if the read succeeded, or an error code otherwise.
 */
nxt_error_t nxt_samba_flash_buf(nxt_t *nxt, uint32_t addr, char *buf,
                                unsigned short int len);

/*@}*/

#endif /* __LEGO_NXT_H__ */
