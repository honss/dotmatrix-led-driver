/***************************************************************************//**
 * @file spidrv.h
 * @brief SPIDRV API definition.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>(C) Copyright 2014 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_SPIDRV_H__
#define __SILICON_LABS_SPIDRV_H__

#include "em_device.h"
#include "em_cmu.h"

#include "ecode.h"
#include "spidrv_config.h"
#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
#include "rtcdriver.h"
#endif
#include "dmadrv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup emdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SPIDRV
 * @{
 ******************************************************************************/

#define ECODE_EMDRV_SPIDRV_OK                (ECODE_OK)                               ///< A successful return value.
#define ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE    (ECODE_EMDRV_SPIDRV_BASE | 0x00000001)   ///< An illegal SPI handle.
#define ECODE_EMDRV_SPIDRV_PARAM_ERROR       (ECODE_EMDRV_SPIDRV_BASE | 0x00000002)   ///< An illegal input parameter.
#define ECODE_EMDRV_SPIDRV_BUSY              (ECODE_EMDRV_SPIDRV_BASE | 0x00000003)   ///< The SPI port is busy.
#define ECODE_EMDRV_SPIDRV_TIMER_ALLOC_ERROR (ECODE_EMDRV_SPIDRV_BASE | 0x00000004)   ///< Unable to allocate timeout timer.
#define ECODE_EMDRV_SPIDRV_TIMEOUT           (ECODE_EMDRV_SPIDRV_BASE | 0x00000005)   ///< An SPI transfer timeout.
#define ECODE_EMDRV_SPIDRV_IDLE              (ECODE_EMDRV_SPIDRV_BASE | 0x00000006)   ///< No SPI transfer in progress.
#define ECODE_EMDRV_SPIDRV_ABORTED           (ECODE_EMDRV_SPIDRV_BASE | 0x00000007)   ///< An SPI transfer has been aborted.
#define ECODE_EMDRV_SPIDRV_MODE_ERROR        (ECODE_EMDRV_SPIDRV_BASE | 0x00000008)   ///< SPI master used slave API or vica versa.
#define ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR   (ECODE_EMDRV_SPIDRV_BASE | 0x00000009)   ///< Unable to allocate DMA channels.

/// SPI driver instance type.
typedef enum SPIDRV_Type{
  spidrvMaster = 0,               ///< Act as an SPI master.
  spidrvSlave  = 1                ///< Act as an SPI slave.
} SPIDRV_Type_t;

/// SPI bus bit order.
typedef enum SPIDRV_BitOrder{
  spidrvBitOrderLsbFirst = 0,     ///< LSB bit is transmitted first.
  spidrvBitOrderMsbFirst = 1      ///< MSB bit is transmitted first.
} SPIDRV_BitOrder_t;

/// SPI clock mode (clock polarity and phase).
typedef enum SPIDRV_ClockMode{
  spidrvClockMode0 = 0,           ///< SPI mode 0: CLKPOL=0, CLKPHA=0.
  spidrvClockMode1 = 1,           ///< SPI mode 1: CLKPOL=0, CLKPHA=1.
  spidrvClockMode2 = 2,           ///< SPI mode 2: CLKPOL=1, CLKPHA=0.
  spidrvClockMode3 = 3            ///< SPI mode 3: CLKPOL=1, CLKPHA=1.
} SPIDRV_ClockMode_t;

/// SPI master chip select (CS) control scheme.
typedef enum SPIDRV_CsControl{
  spidrvCsControlAuto = 0,        ///< CS controlled by the SPI driver.
  spidrvCsControlApplication = 1  ///< CS controlled by the application.
} SPIDRV_CsControl_t;

/// SPI slave transfer start scheme.
typedef enum SPIDRV_SlaveStart{
  spidrvSlaveStartImmediate = 0,  ///< Transfer starts immediately.
  spidrvSlaveStartDelayed = 1     ///< Transfer starts when the bus is idle (CS deasserted).
} SPIDRV_SlaveStart_t;

struct SPIDRV_HandleData;

/***************************************************************************//**
 * @brief
 *  SPIDRV transfer completion callback function.
 *
 * @details
 *  Called when a transfer is complete. An
 *  application should check the transferStatus and itemsTransferred values.
 *
 * @param[in] handle
 *   The SPIDRV device handle used to start the transfer.
 *
 * @param[in] transferStatus
 *   A number of bytes actually transferred.
 *
 * @param[in] itemsTransferred
 *   A number of bytes transferred.
 *
 * @return
 *   @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT
 *   on timeout. Timeouts are only relevant for slave mode transfers.
 ******************************************************************************/
typedef void (*SPIDRV_Callback_t)(struct SPIDRV_HandleData *handle,
                                  Ecode_t transferStatus,
                                  int itemsTransferred);

/// An SPI driver instance initialization structure.
/// Contains a number of SPIDRV configuration options.
/// This structure is passed to @ref SPIDRV_Init() when initializing a SPIDRV
/// instance. Some common initialization data sets are predefined in
/// @ref SPIDRV_MASTER_USART0 and friends.
typedef struct SPIDRV_Init{
  USART_TypeDef       *port;            ///< The USART used for SPI.
#if defined(_USART_ROUTELOC0_MASK)
  uint8_t             portLocationTx;   ///< A location number for the SPI Tx pin.
  uint8_t             portLocationRx;   ///< A location number for the SPI Rx pin.
  uint8_t             portLocationClk;  ///< A location number for the SPI Clk pin.
  uint8_t             portLocationCs;   ///< A location number for the SPI Cs pin.
#else
  uint8_t             portLocation;     ///< A location number for SPI pins.
#endif
  uint32_t            bitRate;          ///< An SPI bitrate.
  uint32_t            frameLength;      ///< An SPI framelength, valid numbers are 4..16
  uint32_t            dummyTxValue;     ///< The value to transmit when using SPI receive API functions.
  SPIDRV_Type_t       type;             ///< An SPI type, master or slave.
  SPIDRV_BitOrder_t   bitOrder;         ///< A bit order on the SPI bus, MSB or LSB first.
  SPIDRV_ClockMode_t  clockMode;        ///< SPI mode, CLKPOL/CLKPHASE setting.
  SPIDRV_CsControl_t  csControl;        ///< A select master mode chip select (CS) control scheme.
  SPIDRV_SlaveStart_t slaveStartMode;   ///< A slave mode transfer start scheme.
} SPIDRV_Init_t;

/// An SPI driver instance handle data structure.
/// The handle is allocated by the application using the SPIDRV.
/// Several concurrent driver instances can exist in an application. The application is
/// neither supposed to write or read the contents of the handle.
typedef struct SPIDRV_HandleData{
  /// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
  SPIDRV_Init_t       initData;
  unsigned int        txDMACh;
  unsigned int        rxDMACh;
  DMADRV_PeripheralSignal_t txDMASignal;
  DMADRV_PeripheralSignal_t rxDMASignal;
  SPIDRV_Callback_t   userCallback;
  uint32_t            dummyRx;
  int                 transferCount;
  int                 remaining;
  int                 csPort;
  int                 csPin;
  Ecode_t             transferStatus;
  volatile enum { spidrvStateIdle = 0, spidrvStateTransferring = 1 } state;
  CMU_Clock_TypeDef   usartClock;
  volatile bool       blockingCompleted;

  #if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  RTCDRV_TimerID_t timer;
  #endif
  /// @endcond
} SPIDRV_HandleData_t;

/// An SPI driver instance handle.
typedef SPIDRV_HandleData_t * SPIDRV_Handle_t;

#if defined(_USART_ROUTELOC0_MASK)   /* Series 1 devices */

#if defined(USART0)
/// Configuration data for SPI master using USART0.
#if defined(SLSTK3301A_EFM32TG11)
#define SPIDRV_MASTER_USART0                                           \
  {                                                                    \
    USART0,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC2, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC2, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC2, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC2, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPIDRV_MASTER_USART0                                           \
  {                                                                    \
    USART0,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC1, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC1, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif
#endif

#if defined(USART1)
/// Configuration data for SPI master using USART1.
#if defined(SLSTK3701A_EFM32GG11)
#define SPIDRV_MASTER_USART1                                           \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#elif defined(SLSTK3301A_EFM32TG11)
#define SPIDRV_MASTER_USART1                                           \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC2, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC2, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC1, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC4, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPIDRV_MASTER_USART1                                           \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC11, /* USART Tx pin location number   */ \
    _USART_ROUTELOC0_RXLOC_LOC11, /* USART Rx pin location number   */ \
    _USART_ROUTELOC0_CLKLOC_LOC11,/* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC11, /* USART Cs pin location number   */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif
#endif

#if defined(USART2)
/// Configuration data for SPI master using USART2.
#define SPIDRV_MASTER_USART2                                           \
  {                                                                    \
    USART2,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART3)
/// Configuration data for SPI master using USART3.
#define SPIDRV_MASTER_USART3                                           \
  {                                                                    \
    USART3,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART4)
/// Configuration data for SPI master using USART4.
#define SPIDRV_MASTER_USART4                                           \
  {                                                                    \
    USART4,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART5)
/// Configuration data for SPI master using USART5.
#define SPIDRV_MASTER_USART5                                           \
  {                                                                    \
    USART5,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART0)
/// Configuration data for SPI slave using USART0.
#if defined(SLSTK3301A_EFM32TG11)
#define SPIDRV_SLAVE_USART0                                            \
  {                                                                    \
    USART0,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC2, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC2, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC2, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC2, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPIDRV_SLAVE_USART0                                            \
  {                                                                    \
    USART0,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC1, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC1, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif
#endif

#if defined(USART1)
/// Configuration data for SPI slave using USART1.
#if defined(SLSTK3701A_EFM32GG11)
#define SPIDRV_SLAVE_USART1                                            \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#elif defined(SLSTK3301A_EFM32TG11)
#define SPIDRV_SLAVE_USART1                                            \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC2, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC2, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC1, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC4, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPIDRV_SLAVE_USART1                                            \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC11, /* USART Tx pin location number   */ \
    _USART_ROUTELOC0_RXLOC_LOC11, /* USART Rx pin location number   */ \
    _USART_ROUTELOC0_CLKLOC_LOC11,/* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC11, /* USART Cs pin location number   */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif
#endif

#if defined(USART2)
/// Configuration data for SPI slave using USART2.
#define SPIDRV_SLAVE_USART2                                            \
  {                                                                    \
    USART2,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART3)
/// Configuration data for SPI slave using USART3.
#define SPIDRV_SLAVE_USART3                                            \
  {                                                                    \
    USART3,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART4)
/// Configuration data for SPI slave using USART4.
#define SPIDRV_SLAVE_USART4                                            \
  {                                                                    \
    USART4,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#if defined(USART5)
/// Configuration data for SPI slave using USART5.
#define SPIDRV_SLAVE_USART5                                            \
  {                                                                    \
    USART5,                     /* USART port                       */ \
    _USART_ROUTELOC0_TXLOC_LOC0, /* USART Tx pin location number    */ \
    _USART_ROUTELOC0_RXLOC_LOC0, /* USART Rx pin location number    */ \
    _USART_ROUTELOC0_CLKLOC_LOC0, /* USART Clk pin location number  */ \
    _USART_ROUTELOC0_CSLOC_LOC0, /* USART Cs pin location number    */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#else /* Series 0 devices */

/// Configuration data for SPI master using USART0.
#define SPIDRV_MASTER_USART0                                           \
  {                                                                    \
    USART0,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }

/// Configuration data for SPI master using USART1.
#define SPIDRV_MASTER_USART1                                           \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }

/// Configuration data for SPI master using USART2.
#if defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY)
#define SPIDRV_MASTER_USART2                                           \
  {                                                                    \
    USART2,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPIDRV_MASTER_USART2                                           \
  {                                                                    \
    USART2,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC0, /* USART pins location number       */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

/// Configuration data for SPI master using USARTRF0.
#define SPIDRV_MASTER_USARTRF0                                         \
  {                                                                    \
    USARTRF0,                   /* USART port                       */ \
    RF_USARTRF_LOCATION,        /* USART pins location number       */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }

/// Configuration data for SPI master using USARTRF1.
#define SPIDRV_MASTER_USARTRF1                                         \
  {                                                                    \
    USARTRF1,                   /* USART port                       */ \
    RF_USARTRF_LOCATION,        /* USART pins location number       */ \
    1000000,                    /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }

/// Configuration data for SPI slave using USART0.
#define SPIDRV_SLAVE_USART0                                            \
  {                                                                    \
    USART0,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }

/// Configuration data for SPI slave using USART1.
#define SPIDRV_SLAVE_USART1                                            \
  {                                                                    \
    USART1,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }

/// Configuration data for SPI slave using USART2.
#if defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY)
#define SPIDRV_SLAVE_USART2                                            \
  {                                                                    \
    USART2,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPIDRV_SLAVE_USART2                                            \
  {                                                                    \
    USART2,                     /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC0, /* USART pins location number       */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

/// Configuration data for SPI slave using USARTRF0.
#define SPIDRV_SLAVE_USARTRF0                                          \
  {                                                                    \
    USARTRF0,                   /* USART port                       */ \
    _USART_ROUTE_LOCATION_LOC1, /* USART pins location number       */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0,                          /* Dummy Tx value for Rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif /* _USART_ROUTELOC0_MASK */

Ecode_t   SPIDRV_AbortTransfer(SPIDRV_Handle_t handle);

Ecode_t   SPIDRV_DeInit(SPIDRV_Handle_t handle);

Ecode_t   SPIDRV_GetBitrate(SPIDRV_Handle_t handle,
                            uint32_t *bitRate);

Ecode_t   SPIDRV_GetFramelength(SPIDRV_Handle_t handle,
                                uint32_t *frameLength);

Ecode_t   SPIDRV_GetTransferStatus(SPIDRV_Handle_t handle,
                                   int *itemsTransferred,
                                   int *itemsRemaining);

Ecode_t   SPIDRV_Init(SPIDRV_Handle_t handle,
                      SPIDRV_Init_t *initData);

Ecode_t   SPIDRV_MReceive(SPIDRV_Handle_t handle,
                          void *buffer,
                          int count,
                          SPIDRV_Callback_t callback);

Ecode_t   SPIDRV_MReceiveB(SPIDRV_Handle_t handle,
                           void *buffer,
                           int count);

Ecode_t   SPIDRV_MTransfer(SPIDRV_Handle_t handle,
                           const void *txBuffer,
                           void *rxBuffer,
                           int count,
                           SPIDRV_Callback_t callback);

Ecode_t   SPIDRV_MTransferB(SPIDRV_Handle_t handle,
                            const void *txBuffer,
                            void *rxBuffer,
                            int count);

Ecode_t   SPIDRV_MTransferSingleItemB(SPIDRV_Handle_t handle,
                                      uint32_t txValue,
                                      void *rxValue);

Ecode_t   SPIDRV_MTransmit(SPIDRV_Handle_t handle,
                           const void *buffer,
                           int count,
                           SPIDRV_Callback_t callback);

Ecode_t   SPIDRV_MTransmitB(SPIDRV_Handle_t handle,
                            const void *buffer,
                            int count);

Ecode_t   SPIDRV_SetBitrate(SPIDRV_Handle_t handle,
                            uint32_t bitRate);

Ecode_t   SPIDRV_SetFramelength(SPIDRV_Handle_t handle,
                                uint32_t frameLength);

Ecode_t   SPIDRV_SReceive(SPIDRV_Handle_t handle,
                          void *buffer,
                          int count,
                          SPIDRV_Callback_t callback,
                          int timeoutMs);

Ecode_t   SPIDRV_SReceiveB(SPIDRV_Handle_t handle,
                           void *buffer,
                           int count,
                           int timeoutMs);

Ecode_t   SPIDRV_STransfer(SPIDRV_Handle_t handle,
                           const void *txBuffer,
                           void *rxBuffer,
                           int count,
                           SPIDRV_Callback_t callback,
                           int timeoutMs);

Ecode_t   SPIDRV_STransferB(SPIDRV_Handle_t handle,
                            const void *txBuffer,
                            void *rxBuffer,
                            int count,
                            int timeoutMs);

Ecode_t   SPIDRV_STransmit(SPIDRV_Handle_t handle,
                           const void *buffer,
                           int count,
                           SPIDRV_Callback_t callback,
                           int timeoutMs);

Ecode_t   SPIDRV_STransmitB(SPIDRV_Handle_t handle,
                            const void *buffer,
                            int count,
                            int timeoutMs);

/** @} (end addtogroup SPIDRV) */
/** @} (end addtogroup emdrv) */

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_SPIDRV_H__ */
