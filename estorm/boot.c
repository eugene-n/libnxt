/* EStorm bootup code */

#include "at91sam7s256.h"

/* Configure the Flash controller with write speed settings. These
 * settings are valid for writing everywhere but the non-volatile bits
 * (lock, security, general-purpose NVM)
 */
inline void estorm_init_flash()
{
  /* The values are those provided by the LEGO OS. */
  *AT91C_MC_FMR = AT91C_MC_FWS_1FWS | (0x48 << 16);
}

/* Initialize the watchdog timer, by disabling it. We do not need it
 * in our application.
 */
inline void estorm_init_watchdog()
{
  *AT91C_WDTC_WDMR = AT91C_WDTC_WDDIS;
}

/* Boot the quartz oscillator, step the CPU up to 48MHz, and enable
 * USB clocks.
 */
inline void estorm_init_clocks()
{
  /* Enable internal main oscillator */
  *AT91C_CKGR_MOR = AT91C_CKGR_MOSCEN | (0x6 << 8);

  /* Wait for oscillator to stabilize */
  while ((*AT91C_PMC_SR & AT91C_PMC_MOSCS) == 0);

  /* Initialize the PLL clock. The quartz runs at 18.432MHz, we run
   * the PLL at ~96MHz.
   */
  *AT91C_CKGR_PLLR = (0xE | (0x1C << 8) | (0x48 << 16) | AT91C_CKGR_USBDIV_1);

  /* Wait for the PLL to lock */
  while ((*AT91C_PMC_SR & AT91C_PMC_LOCK) == 0);

  /* Wait for master clock ready (just in case) */
  while ((*AT91C_PMC_SR & AT91C_PMC_MCKRDY) == 0);

  /* Set the master clock prescaler to /2 (48MHz, the max cpu speed) */
  *AT91C_PMC_MCKR = AT91C_PMC_CSS_SLOW_CLK | AT91C_PMC_PRES_CLK_2;

  /* Wait for master clock ready */
  while ((*AT91C_PMC_SR & AT91C_PMC_MCKRDY) == 0);

  /* Switch the main clock over to the PLL */
  *AT91C_PMC_MCKR = AT91C_PMC_CSS_PLL_CLK | AT91C_PMC_PRES_CLK_2;

  /* Wait for master clock ready */
  while ((*AT91C_PMC_SR & AT91C_PMC_MCKRDY) == 0);
}

/* Initialize the reset controller, allowing hardware resets. */
inline void estorm_init_reset_control()
{
  *AT91C_RSTC_RMR = 0x1 | (0x4 << 8);
}

inline void estorm_init_ticker()
{
  /* Enable the PWM controller clock */
  *AT91C_PMC_PCER = (1 << AT91C_ID_PWMC);

  /* Set the PWM channel 0 to a 0-duty-cycle period of 0x7FFF, twice
   * faster than SAM-BA */
  *AT91C_PWMC_CH0_CPRDR = 0x7FFF;

  /* Activate PWM channel 0 */
  *AT91C_PWMC_ENA = 0x1;
}

void estorm_boot()
{
  estorm_init_flash(); /* Flash controller init */
  estorm_init_watchdog(); /* Silence the watchdog */
  estorm_init_clocks(); /* Start clocks */
  estorm_init_reset_control(); /* Allow hardware resets */
  estorm_init_ticker(); /* Start ticking audibly */

  while(1); /* Crash here for now. If we live through that init
               sequence, it's already a miracle. */
}
