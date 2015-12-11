## Overview ##

LibNXT is an utility library for talking to the LEGO Mindstorms NXT intelligent brick at a relatively low level. It currently does:

  * Handling USB communication and locating the NXT in the USB tree.
  * Interaction with the Atmel AT91SAM boot assistant.
  * Flashing of a firmware image to the NXT.
  * Execution of code directly in RAM.

(If you have ideas of other stuff it should do, please suggest!)

LibNXT is targetted mainly at the platforms that the official Lego Minstorms NXT software overlooks, namely Linux and other unices. It will work on any POSIX-compliant operating system where libusb 0.1 (http://libusb.sf.net/) is supported. It might even work on Microsoft Windows, as there is a win32 port of libusb floating around!

The design of LibNXT is layered, meaning you can plug into it at any level of complexity or abstraction you desire, from the lowest level USB bulk bit-pushing interface, to an API exposing the SAM-BA commandset, right up to just calling nxt\_firmware\_flash() and having everything taken care of!

`fwflash` is the first utility program that uses LibNXT. As its name hints, its purpose is to take a NXT firmware image file and flash it to a connected NXT device.

`fwexec` is another cool utility, originally written by the folks of the Lejos project (http://lejos.sourceforge.net/). It takes a specially compiled firmware image, uploads it to the NXT's RAM, and executes it directly from there. While this firmware will only last
until the brick is powered down, it is a great tool for testing firmwares during development without wearing down the flash memory.

## News ##

_**22/04/2007**_: More than a year after the 0.2 release, there are a few changes, and it can't hurt to push them out in an actual release. This release includes a more intelligent build system that handles a missing flash driver elegantly, and the `fwexec` utility program. Unless people have specific itches that need scratching, or bugs that should be fixed, this will probably be the final release of libnxt. My personal itch is scratched, and I've moved on to using libnxt to write firmware code. :-)