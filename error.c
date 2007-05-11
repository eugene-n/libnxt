/**
 * NXT bootstrap interface; error handling code.
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

#include "lego-nxt.h"


/* A mapping of the error codes to nice strings for displaying. */
static const struct error_def {
  nxt_error_t errcode;
  const char *errstr;
} error_map[] = {
  { NXT_OK, "Success" },
  { NXT_INIT_FAILED, "Initialization failed" },
  { NXT_UNKNOWN_ERROR, "Unknown error" },
  { NXT_FILE_ERROR, "File open/handling error" },

  { NXT_USB_NOT_FOUND, "NXT not found on USB bus" },
  { NXT_CONFIGURATION_ERROR, "Error trying to configure the NXT USB link" },
  { NXT_IN_USE, "NXT USB interface is already claimed by another program" },
  { NXT_USB_WRITE_ERROR, "USB write error" },
  { NXT_USB_READ_ERROR, "USB read error" },

  { NXT_SAMBA_PROTOCOL_ERROR, "SAM-BA protocol error" },
  { NXT_SAMBA_HANDSHAKE_FAILED, "NXT handshake failed" },
  { NXT_SAMBA_INVALID_FIRMWARE, "Invalid firmware image" },

  { NXT_OK, NULL } /* Sentinel */
};


const char const *
nxt_strerror(nxt_error_t err)
{
  struct error_def *def;

  for (def = error_map; def->errstr != NULL; def++)
    if (def->errcode == err)
      return dev->errstr;

  return error_map[NXT_UNKNOWN_ERROR].errstr;
}
