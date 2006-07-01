/* EStorm bootup code */

#include "nxt.h"

/* Configure the Flash controller with write speed settings. These
 * settings are valid for writing everywhere but the non-volatile bits
 * (lock, security, general-purpose NVM)
 */
inline void estorm_init_flash()
{
  /* The values are those provided by the LEGO OS. */
  NXT_MC->MC_FMR = MC_FMR_FMCN_FLASH | MC_FMR_FWS_V(1);
}

/* Initialize the watchdog timer, by disabling it. We do not need it
 * in our application.
 */
inline void estorm_init_watchdog()
{
  NXT_WDT->WDT_MR = WDT_MR_WDDIS;

}

/* Boot the quartz oscillator, step the CPU up to 48MHz, and enable
 * USB clocks.
 */
inline void estorm_init_clocks()
{
  /* Enable internal main oscillator */
  NXT_PMC->CKGR_MOR = (CKGR_MOS_START_TIME | CKGR_MOR_ENABLE);

  /* Wait for oscillator to stabilize */
  while (NXT_PMC->PMC_SR & PMC_SR_MOSCS == 0);

  /* Initialize the PLL clock. The quartz runs at 18.432MHz, we run
   * the PLL at ~96MHz.
   */
  NXT_PMC->CKGR_PLLR = (CKGR_PLL_DIV_V(0xE) | CKGR_PLL_COUNT_V(0x1C) |
                        CKGR_PLL_MUL_V(0x48) | CKGR_PLL_USBDIV_2);

  /* Wait for the PLL to lock */
  while (NXT_PMC->PMC_SR & PMC_SR_PLL_LOCKED == 0);

  /* Wait for master clock ready (just in case) */
  while (NXT_PMC->PMC_SR & PMC_SR_MCKRDY == 0);

  /* Set the master clock prescaler to /2 */
  NXT_PMC->PMC_MCKR = PMC_MCKR_PRES_2 | PMC_MCKR_CSS_SLOW;

  /* Wait for master clock ready */
  while (NXT_PMC->PMC_SR & PMC_SR_MCKRDY == 0);

  /* Switch the main clock over to the PLL */
  NXT_PMC->PMC_MCKR = PMC_MCKR_PRES_2 | PMC_MCKR_CSS_PLL;

  /* Wait for master clock ready */
  while (NXT_PMC->PMC_SR & PMC_SR_MCKRDY == 0);
}

/* Initialize the reset controller
 *
 */
inline void estorm_init_reset_control()
{
  NXT_RSTC->RSTC_MR = (RSTC_MR_ERSTL_V(4) | RSTC_MR_URSTEN
                       | RSTC_MR_KEY_VAL);
}

void estorm_boot()
{
  estorm_init_flash();    /* Flash controller init */
  estorm_init_watchdog(); /* Silence the watchdog  */
  estorm_init_clocks();   /* Start clocks          */
  estorm_init_reset_control(); /* Allow hardware resets */
}
