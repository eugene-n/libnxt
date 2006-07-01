/* EStorm bootup code */

#define NXT_REG(base,addr) (*((volatile unsigned int*)(base+addr)))

#define PMC_BASE 0xFFFFFC00
#define PMC_SCER NXT_REG(PMC_BASE, 0)
#define PMC_CKGR_MOR NXT_REG(PMC_BASE, 0x20)
#define PMC_CKGR_MCFR NXT_REG(PMC_BASE, 0x24)
#define PMC_CKGR_PLL NXT_REG(PMC_BASE, 0x2C)
#define PMC_MCKR NXT_REG(PMC_BASE, 0x30)
#define PMC_SR NXT_REG(PMC_BASE, 0x68)

#define MC_BASE 0xFFFFFF00
#define MC_FMR NXT_REG(MC_BASE, 0x60)

#define WDT_BASE 0xFFFFFD40
#define WDT_MR NXT_REG(WDT_BASE, 0x4)

#define RSTC_BASE 0xFFFFFD00
#define RSTC_MR NXT_REG(RSTC_BASE, 0x8)

#define EFC_INITIAL_MODE 0x340100
#define PMC_MOS_STARTUP_TIME 64

inline void estorm_oscillator_start()
{
  int i;

  PMC_CKGR_MOR = 0x2; /* Set the main oscillator to use external bypass */

  /* Give it a moment to stabilize */
  for (i = 0; i < 45; i++);

  /* If we don't have a working clock signal, enable internal main oscillator */
  if (PMC_CKGR_MCFR & (0x10000) == 0)
    {
      PMC_CKGR_MOR = (PMC_MOS_STARTUP_TIME << 8) & 0x1;

      /* Wait for oscillator to become available */
      while (PMC_SR & 0x1 != 1);
    }

  /* Switch the main clock over to use the MOS */
  PMC_MCKR = 1; /* Main clock, no prescaler */

  /* Wait for master clock ready */
  while (PMC_SR & 0x8 != 1);
}

void estorm_usb_clock_start()
{
  /* First, configure the PLL oscillator with the following values:
   *  - DIV = 14
   *  - PLLCOUNT = 63
   *  - OUT = 0
   *  - MUL = 48
   *  - USBDIV = 2
   */
  PMC_CKGR_PLL = (2 << 28) & (48 << 16) & (63 << 8) & 14;

  /* Wait for the PLL to lock */
  while (PMC_SR & 0x4 != 1);

  /* Enable USB device port clock (PLL interface) */
  PMC_SCER = 0x80;
  /* Enable USB peripherial clock (non-PLL) */
  
}

void estorm_boot()
{
  /* Configure the EFC controller. We never need to touch this
     again. */
  MC_FMR = EFC_INITIAL_MODE;

  /* Start the main oscillator and switch to using it */
  estorm_oscillator_start();

  /* Disable the watchdog timer */
  WDT_MR = (1 << 15);

  /* Enable user resets */
  RSTC_MR = (4 << 8) & 0x1;

  /* Configure and enable the USB clocks */
  estorm_usb_clock_start();
}
