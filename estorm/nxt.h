/*
 * Definition of the control registers for the subsystems of the NXT
 */

/* Register base type */
typedef unsigned long int nxt_reg;

/* Base address of the Power Management Controller */
#define NXT_PMC_BASE  0xFFFFFC00
#define NXT_RSTC_BASE 0xFFFFFD00
#define NXT_WDT_BASE  0xFFFFFD40
#define NXT_MC_BASE   0xFFFFFF00

/********************************************************************
 * PMC - Power Management Controller                                *
 ********************************************************************/
volatile struct
{
  nxt_reg PMC_SCER;   /* System Clock Enable Register */
  nxt_reg PMC_SCER;   /* System Clock Disable Register */
  nxt_reg PMC_SCSR;   /* System Clock Status Register */

  nxt_reg _r1;        /* Reserved */

  nxt_reg PMC_PCER;   /* Peripherial Clock Enable Register */
  nxt_reg PMC_PCDR;   /* Peripherial Clock Disable Register */
  nxt_reg PMC_PCSR;   /* Peripherial Clock Status Register */

  nxt_reg _r2;        /* Reserved */

  nxt_reg CKGR_MOR;   /* Main Oscillator Register */
  nxt_reg CKGR_MCFR   /* Main Clock Frequency Register */

  nxt_reg _r3;        /* Reserved */

  nxt_reg CKGR_PLLR;  /* PLL Register */
  nxt_reg PMC_MCKR;   /* Master Clock Register */

  nxt_reg _r4[2];     /* Reserved */

  nxt_reg PMC_PCK0;   /* Programmable Clock Register */
  nxt_reg PMC_PCK1;   /* Programmable Clock Register */
  nxt_reg PMC_PCK2;   /* Programmable Clock Register */

  nxt_reg _r6[5];     /* Reserved */

  nxt_reg PMC_IER;    /* Interrupt Enable Register */
  nxt_reg PMC_IDR;    /* Interrupt Disable Register */
  nxt_reg PMC_SR;     /* Status Register */
  nxt_reg PMC_IMR;    /* Interrupt Mask Register */
} *NXT_PMC = NXT_PMC_BASE;

/* Flags fields and values for CKGR_MOR */
enum
  {
    CKGR_MOR_ENABLE = 0x1,
    CKGR_MOR_BYPASS = 0x2,
    CKGR_MOR_OSCOUNT = 0xFF00,
  };
#define CKGR_MOS_START_TIME (0x6 << 8)

/* Flag, fields and values for CKGR_MCFR */
enum
  {
    CKGR_MCFR_MAINRDY = (1 << 16),
    CKGR_MCFR_MAINF = 0xFFFF,
  };
/* TODO: think about the way of reading/writing multibit values in
   registers. */

/* Flags, fields and values for PMC_SR */
enum
  {
    PMC_SR_MOSCS = 0x1,
    PMC_SR_PLL_LOCKED = 0x2,
    PMC_SR_MCKRDY = 0x4,
    PMC_SR_PCKRDY0 = (1 << 8),
    PMC_SR_PCKRDY1 = (1 << 8),
    PMC_SR_PCKRDY2 = (1 << 8),
  };

/* Flags, fields and values for PMC_MCKR */
enum
  {
    PMC_MCKR_CSS = 0x3,
    PMC_MCKR_PRES_2 = (0x1 << 2),
    PMC_MCKR_PRES_4 = (0x2 << 2),
    PMC_MCKR_PRES_8 = (0x3 << 2),
    PMC_MCKR_PRES_16 = (0x4 << 2),
    PMC_MCKR_PRES_32 = (0x5 << 2),
    PMC_MCKR_PRES_64 = (0x6 << 2),
  };
#define PMC_MCKR_CSS_SLOW 0x0
#define PMC_MCKR_CSS_MAIN 0x1
#define PMC_MCKR_CSS_PLL 0x3

/* Flags, fields and values for CKGR_PLLR */
enum
  {
    CKGR_PLL_DIV_0 = 0x0,
    CKGR_PLL_DIV_BYPASS = 0x1,
    CKGR_PLL_DIV = 0xFF,
    CKGR_PLL_COUNT = (0x3F << 8),
    CKGR_PLL_OUT = (0xC0 << 8),
    CKGR_PLL_MUL = (0x7FF << 16),
    CKGR_PLL_USBDIV_2 = (0x1 << 28),
    CKGR_PLL_USBDIV_4 = (0x2 << 28),
  };
#define CKGR_PLL_DIV_V(div) (((nxt_reg)div) & CKGR_PLL_DIV)
#define CKGR_PLL_COUNT_V(cnt) ((((nxt_reg)cnt) << 8) & CKGR_PLL_COUNT)
#define CKGR_PLL_MUL_V(val) ((((nxt_reg)val) << 16) & CKGR_PLL_MUL)


/********************************************************************
 * RSTC - Reset Controller                                          *
 ********************************************************************/
volatile struct
{
  nxt_reg RSTC_CR; /* Control Register */
  nxt_reg RSTC_SR; /* Status Register */
  nxt_reg RSTC_MR; /* Mode Register */
} *NXT_RSTC = NXT_RSTC_BASE;

/* Flags, fields and values for RSTC_CR */
enum
  {
    RSTC_CR_PROCRST = 0x1,          /* Processor Reset  */
    RSTC_CR_PERRST = 0x4,           /* Peripheral Reset */
    RSTC_CR_EXTRST = 0x8,           /* External Reset   */
    RSTC_CR_KEY = (0xFF << 24),     /* Security key */
  };
#define RSTC_CR_KEY_VAL (0xA5 << 24)

/* Flags, fields and values for RSTC_SR */
enum
  {
    RSTC_SR_URSTS = 0x1,            /* User Reset Status */
    RSTC_SR_BODSTS = 0x2,           /* Brownout Detection Status */
    RSTC_SR_RSTTYP = (0x7 << 8),    /* Reset Type */
    RSTC_SR_NRSTL = (0x1 << 16),    /* NRST Pin Level */
    RSTC_SR_SRCMP = (0x1 << 17),    /* Reset Controller Busy */
  };
#define RSTC_SR_RSTTYP_POWERUP 0x0  /* Cold boot */
#define RSTC_SR_RSTTYP_WATCHDOG 0x2 /* Watchdog reset */
#define RSTC_SR_RSTTYP_SOFT 0x3     /* Software-requested reset */
#define RSTC_SR_RSTTYP_USER 0x4     /* NRST reset */
#define RSTC_SR_RSTTYP_BROWNOUT 0x5 /* Brownout scram */

/* Flags, fields and values for RSTC_MR */
enum
  {
    RSTC_MR_URSTEN = 0x1,           /* Enable NRST reset */
    RSTC_MR_URSTIEN = (0x1 << 4),   /* Interrupt on NRST */
    RSTC_MR_ERSTL = (0xF << 8),     /* External Reset Length */
    RSTC_MR_BODIEN = (0x1 << 16),   /* Interrupt on brownout */
    RSTC_MR_KEY = (0xF << 24),      /* Security key */
  };
#define RSTC_MR_ERSTL_V(cnt) ((((nxt_reg)cnt) << 8) & RSTC_MR_ERSTL)
#define RSTC_MR_KEY_VAL (0xA5 << 24)


/********************************************************************
 * WDT - Watchdog Timer                                             *
 ********************************************************************/
volatile struct
{
  nxt_reg WDT_CR; /* Control Register */
  nxt_reg WDT_MR; /* Mode Register */
  nxt_reg WDT_SR; /* Status Register */
} *NXT_WDT = NXT_WDT_BASE;

/* Flags, fields and values for WDT_CR */
#define WDT_CR_RESTART 0xA5000001 /* Only valid value for WDT_CR */

/* Flags, fields and values for WDT_MR */
enum
  {
    WDT_MR_WDV = 0xFFF,             /* Watchdog Value */
    WDT_MR_WDFIEN = (0x1 << 12),    /* Watchdog Full Interrupt Enable */
    WDT_MR_WDRSTEN = (0x1 << 13),   /* Watchdog Reset Enable */
    WDT_MR_WDRPROC = (0x1 << 14),   /* Watchdog Reset Processor */
    WDT_MR_WDDIS = (0x1 << 15),     /* Watchdog Disable */
    WDT_MR_WDD = (0xFFF << 16),     /* Watchdog Delta Value */
    WDT_MR_WDDBGHLT = (0x1 << 28),  /* Watchdog Debug Halt */
    WDT_MR_WDIDLEHLT = (0x1 << 29), /* Watchdog Idle Halt */
  };
#define WDT_MR_WDV_V(val) (((nxt_reg)val) & WDT_MR_WDV)
#define WDT_MR_WDD_V(val) ((((nxt_reg)val) << 12) & WDT_MR_WDV)

/* Flags, fields and values for WDT_SR */
enum
  {
    WDT_WDUNF = 0x1,                /* Watchdog Underflow */
    WDT_WDERR = 0x2,                /* Watchdog Error */
  };


/********************************************************************
 * MC - Memory Controller                                           *
 ********************************************************************/
volatile struct
{
  nxt_reg MC_RCR;  /* Remap Control Register */
  nxt_reg MC_ASR;  /* Abort Status Register */
  nxt_reg MC_AASR; /* Abort Address Status Register */

  nxt_reg _r[22];  /* Reserved */

  nxt_reg MC_FMR;  /* Flash Mode Register */
  nxt_reg MC_FCR;  /* Flash Command Register */
  nxt_reg MC_FSR;  /* Flash Status Register */
} *NXT_MC = NXT_MC_BASE;

/* Flags, fields and values for MC_RCR */
enum
  {
    MC_RCR_RCB = 0x1,               /* Remap Command Bit */
  };

/* Flags, fields and values for MC_ASR */
enum
  {
    MC_ASR_UNDADD = 0x1,            /* Abort due to undefined address */
    MC_ASR_MISADD = 0x2,            /* Abort due to address misalign */
    MC_ASR_ABTSZ = (0x3 << 8),      /* Abort size */
    MC_ASR_ABTTYP = (0x3 << 10),    /* Abort type */
    MC_ASR_MST0 = (0x1 << 16),      /* Abort due to PDC (DMA) */
    MC_ASR_MST1 = (0x1 << 17),      /* Abort due to CPU */
    MC_ASR_SVMST0 = (0x1 << 24),    /* Saved PDC (DMA) abort */
    MC_ASR_SVMST1 = (0x1 << 25)     /* Saved CPU abort */
  };
#define MC_ASR_ABTSZ_BYTE 0         /* Byte abort size */
#define MC_ASR_ABTSZ_HWORD 0x1      /* Half-word abort size */
#define MC_ASR_ABTSZ_WORD 0x2       /* Word abort size */
#define MC_ASR_ABTTYP_READ 0        /* Data read op */
#define MC_ASR_ABTTYP_WRITE 0x1     /* Data write op */
#define MC_ASR_ABTTYP_CODE 0x2      /* Code fetch op */

/* Flags, fields and values for MC_FMR */
enum
  {
    MC_FMR_FRDY = 0x1,              /* Flash Ready Interrupt Enable */
    MC_FMR_LOCKE = 0x4,             /* Lock Error Interrupt Enable  */
    MC_FMR_PROGE = 0x8,             /* Prog Error Interrupt Enable  */
    MC_FMR_NEBP = (0x1 << 7),       /* No Erase Before Programming  */
    MC_FMR_FWS = (0x3 << 8),        /* Flash Wait State             */
    MC_FMR_FMCN = (0xFF << 16),     /* Flash Microsecond Cycle Num  */
  };
#define MC_FMR_FWS_V(val) ((((nxt_reg)val) << 8) & MC_FMR_FWS)
#define MC_FMR_FMCN_NVM (0x5 << 16) /* Given value for NXT */
#define MC_FMR_FMCN_FLASH (0x48 << 16) /* Given value for NXT */

/* Flags, fields and values for MC_FCR */
enum
  {
    MC_FCR_FCMD = 0xF,              /* Flash Command                */
    MC_FCR_FCMD_WP = 0x1,           /* Write Page                   */
    MC_FCR_FCMD_SLB = 0x2,          /* Set Lock Bit                 */
    MC_FCR_FCMD_WPL = 0x3,          /* Write Page and Lock          */
    MC_FCR_FCMD_CLB = 0x4,          /* Clear Lock Bit               */
    MC_FCR_FCMD_EA = 0x8,           /* Erase All                    */
    MC_FCR_FCMD_SGPB = 0xB,         /* Set General Purpose NVM      */
    MC_FCR_FCMD_CGPB = 0xD,         /* Clear General Purpose NVM    */
    MC_FCR_FCMD_SSB = 0xF,          /* Set Security Bit             */
    MC_FCR_PAGEN = (0x3FF << 8),    /* Flash Page Number            */
    MC_FCR_KEY = (0xFF << 24),      /* Flash Command Key            */
  };
#define MC_FCR_PAGEN(n) = ((((nxt_reg)cmd) << 8) & MC_FCR_PAGEN)
#define MC_FCR_KEY_VAL (0xA5 << 24)

/* Flags, fields and values for MC_FSR */
enum
  {
    MC_FSR_FRDY = 0x1,              /* Flash Ready */
    MC_FSR_LOCKE = 0x4,             /* Lock Error */
    MC_FSR_PROGE = 0x8,             /* Programming Error */
    MC_FSR_SECURITY = (0x1 << 4),   /* Security Bit Enabled */
  };
#define MC_FSR_GPNVM(n) (0x1 << ((n % 2)+8)) /* NVM Bit n Status */
#define MC_FSR_LOCKS(n) (0x1 << ((n % 16)+16)) /* Lock Bit n Status */
