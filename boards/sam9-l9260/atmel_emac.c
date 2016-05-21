/*---------------------------------------------------------------------------*/
/* Akalon RTOS                                                               */
/* Copyright (c) 2011-2016, Dasa Dahanayaka                                  */
/* All rights reserved.                                                      */
/*                                                                           */
/* Usage of the works is permitted provided that this instrument is retained */
/* with the works, so that any entity that uses the works is notified of     */
/* this instrument.                                                          */
/*                                                                           */	
/* DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* File Name       : atmel_emac.c                                            */
/* Description     : The Atmel EMAC network driver                           */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <akalon.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Registers - starts at 0xfffc_4000 */
#define  EMAC_NCR      0xfffc4000
#define  EMAC_NCFG     0xfffc4004
#define  EMAC_RBQP     0xfffc4018
#define  EMAC_TBQP     0xfffc401c 
#define  EMAC_RSR      0xfffc4020
#define  EMAC_ISR      0xfffc4024
#define  EMAC_IER      0xfffc4028
#define  EMAC_IDR      0xfffc402c
#define  EMAC_IMR      0xfffc4030
#define  EMAC_SA1B     0xfffc4098
#define  EMAC_SA1U     0xfffc409c

#define  RX_TASK_PRIORITY    100
#define  RX_TASK_STACK_SIZE  0x1000
#define  NUM_BUFS            1024
#define  RX_BUF_SIZE         128
#define  TX_BUF_SIZE         2048


typedef struct buf_desc_t
{
    u32  buf_addr ;
    u32  buf_stat ;
} buf_desc_t ;

typedef struct dev_t
{
    u8             mac[6]     ;

    buf_desc_t     *rx_desc   ;
    u8             *rx_buf    ;
    usys           rx_p_index ;
    usys           rx_c_index ;
    usys           rx_sem     ;

    buf_desc_t     *tx_desc   ;
    u8             *tx_buf    ;
    usys           tx_index   ;

    usys           rx_task    ;

} dev_t ;


static   dev_t        dev ;
link_t   emac_link ;



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : buf_init                                                */
/* Description     : Initialize the Tx and Rx buffers                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static usys   buf_init (dev_t *dev)
{
    usys i ;

    /* *** RX ***/
    /* malloc the rx buffer */
    if ((dev->rx_buf = malloc (RX_BUF_SIZE * NUM_BUFS)) == NULL)
    {  printf ("ERR: buf_init() malloc failed on rx_buf\n") ;
       goto err ;
    }

    if ((dev->rx_desc = malloc (NUM_BUFS * sizeof (buf_desc_t))) == NULL)
    {  printf ("ERR: buf_init() malloc failed on rx_buf_desc\n") ;
       goto err ;
    }

    /* Initialize the rx desciptor */
    for (i = 0; i < NUM_BUFS; i++)
    {
       dev->rx_desc[i].buf_addr  = (u32) &dev->rx_buf [RX_BUF_SIZE * i] ;
       dev->rx_desc[i].buf_addr &= 0xfffffffe ;

       dev->rx_desc[i].buf_stat  = 0 ;
    }

    dev->rx_p_index = dev->rx_c_index = 0 ; /* Init Index */

    /* *** TX ***/
    /* malloc the tx buffer */
    if ((dev->tx_buf = malloc (TX_BUF_SIZE * NUM_BUFS)) == NULL)
    {  printf ("ERR: buf_init() malloc failed on tx_buf\n") ;
       goto err ;
    }

    if ((dev->tx_desc = malloc (NUM_BUFS * sizeof (buf_desc_t))) == NULL)
    {  printf ("ERR: buf_init() malloc failed on tx_buf_desc\n") ;
       goto err ;
    }

    /* Initialize the tx desciptor */
    for (i = 0; i < NUM_BUFS; i++)
    {
       dev->tx_desc[i].buf_addr  = (u32) &dev->tx_buf [TX_BUF_SIZE * i] ;
       dev->tx_desc[i].buf_stat  = 0x80000000 ;
    }

    dev->tx_index = 0 ; /* Init Index */

    return GOOD ;

err:
    if (dev->tx_desc != NULL)
       free (dev->tx_desc) ;

    if (dev->rx_desc != NULL)
       free (dev->rx_desc) ;

    if (dev->tx_buf != NULL)
       free (dev->tx_buf) ;

    if (dev->rx_buf != NULL)
       free (dev->rx_buf) ;

    return E_MEM ;

} /* End of function buf_init() ; */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dev_init                                                */
/* Description     : Initialize the Device                                   */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      dev_init(dev_t *dev)
{
    u32 *reg, val  ;

    /* Initialize the needed tx and rx buffers */ 
    if (buf_init (dev) != GOOD)
       return BAD ;

    /* Shutdown the Tx and Rx Circuits */
    reg  = (u32 *) EMAC_NCR   ;
    val  = *reg & 0xfffffff3  ;
    *reg = val ;

    /* Shutdown the Loopback Mode */
    val  = *reg & 0xfffffffc  ;
    *reg = val ;

    /* Write the Rx Buffer Queue Pointer */
    reg  = (u32 *) EMAC_RBQP  ;
    *reg = (u32) dev->rx_desc ;

    /* Write the Tx Buffer Queue Pointer */
    reg  = (u32 *) EMAC_TBQP  ;
    *reg = (u32) dev->tx_desc ;

    /* Write the MAC address */
    reg  = (u32 *) EMAC_SA1B  ;
    *reg = (dev->mac [3] << 24) | (dev->mac [2] << 16) | 
           (dev->mac [1] << 8)  |  dev->mac [0] ;

    reg  = (u32 *) EMAC_SA1U  ;
    *reg = (dev->mac [5] << 8)  |  dev->mac [4] ;

    return GOOD ;
} /* End of function dev_init() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : emac_rx_task                                            */
/* Description     : The EMAC RX task                                        */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     emac_rx_task (usys arg0)
{
    volatile u32   *reg, val ;
    dev_t *dev ;

    /* Enable the Rx Interrupt <-- DO */
    reg = (volatile u32 *) EMAC_IER ;
    *reg = 0x2 ;

    /* Enable the Receiver */
    reg = (volatile u32 *) EMAC_NCR ;
    val  = *reg ;
    *reg = val  | 0x4 ;

    dev = (dev_t *) arg0 ;

    /* Loop forever and sleep until an interrupt */
    for (;;)
    {
       /* Await the next Rx Interrupt */
       task_sleep() ;  

       /* Parse through all of the new frames that have been received */
       while (dev->rx_desc[dev->rx_p_index].buf_addr & 0x1)
       {
	  /* Release the Semaphore */
	  sem_give (dev->rx_sem) ;

          /* Increment the index */
          dev->rx_p_index++ ;
          if (dev->rx_p_index == NUM_BUFS)
             dev->rx_p_index = 0 ;

       }

    } /* Forever loop */

} /* End of function emac_rx_task() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : emac_isr                                                */
/* Description     : The EMAC ISR                                            */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void     emac_isr (void)
{
    volatile u32 *reg, val ;

    /* Clear the ISR (interrupt Status) by reading it */
    reg  = (volatile u32 *) EMAC_ISR ;
    val  = *reg ;

    /* Check if a packet was received and if the rx task needs to be woken */
    reg  = (volatile u32 *) EMAC_RSR ;
    val  = *reg ;
    *reg = val  ;

    if ((val & 0x7) == 0x2)
       task_wake (dev.rx_task) ;

} /* End of function emac_isr() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ui_tx                                                   */
/* Description     : Network Stack <- Device                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys     ui_tx (usys type, usys buf_size, u8 *buf, usys *ret_size)
{
    usys len ;

#if 0 /* What to do with this ? */
       /* Check if the Start frame and End Frames are set */
       if ((dev->rx_desc[dev->rx_index].buf_stat & 0xc000) != 0xc000)
       {
	  printf ("Start and End Bits are not Set !!!\n") ;
	  return ; /* <-- DO */
       }
#endif

    /* Wait for a packet */
    if (sem_get (dev.rx_sem, WAIT_FOREVER) == GOOD)
    {
       len = dev.rx_desc [dev.rx_c_index].buf_stat & 0xfff ;
 
       /* Mark the device buffer as free by resetting bit zero. This */
       /* will also put it back to the correct buffer address for    */
       /* memcpy()                                                   */

       dev.rx_desc [dev.rx_c_index].buf_addr &= 0xfffffffe ;

       /* Copy the packet */
       memcpy (buf, (void *) dev.rx_desc [dev.rx_c_index].buf_addr, len) ;

       *ret_size = len ;

       dev.rx_c_index ++ ;
       if (dev.rx_c_index == NUM_BUFS)
	  dev.rx_c_index = 0 ;

       return GOOD ;
    } 

    return BAD ;
} /* End of function ui_tx() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : ui_rx                                                   */
/* Description     : Network Stack -> Device                                 */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static   usys      ui_rx (usys type, usys size, u8 *pkt)
{
    volatile u32 *reg, val ;
    void   *buf ;

    /* Enable the transmitter */
    reg = (volatile u32 *) EMAC_NCR ;
    val = *reg ;
    *reg = val | 0x8 ;

    /* Copy the pbuf into the device buffer */
    buf = (void *) dev.tx_desc [dev.tx_index].buf_addr ;
    memcpy (buf, pkt, size) ;

    /* Set the config word */
    dev.tx_desc [dev.tx_index].buf_stat = 0 ;

    /* Set the last buffer in the frame */
    dev.tx_desc [dev.tx_index].buf_stat |= 0x8000 ;

    /* Set the size */
    dev.tx_desc [dev.tx_index].buf_stat |= (size & 0xfff) ; 
       
    /* Increment the tx_index */
    dev.tx_index ++ ;
    if (dev.tx_index == NUM_BUFS)
       dev.tx_index = 0 ;

    /* Set the Transmit Start Bit */
    reg  = (volatile u32 *) EMAC_NCR ;
    val  = *reg ;
    *reg = val | 0x200 ;

    return GOOD ;

#if 0
    dev_t  *dev ;

    printf ("In send_data(): Struct = 0x%x\n", (usys) p) ;
    printf ("  Payload  = 0x%x\n", (usys) p->payload) ;
    printf ("  tot_len  = 0x%x\n", (usys) p->tot_len) ;
    printf ("  len      = 0x%x\n", (usys) p->len) ;

    dev = (dev_t *) netif->state ;
    if (p->len == p->tot_len)
    {

    } else {
       printf ("Shit Send !!!\n") ;
    }

    return ERR_OK ;
#endif

} /* End of function ui_tx() */



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : emac_init                                               */
/* Description     : Initialize the EMAC Device                              */
/* Notes           :                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
usys     emac_init (void)
{
    usys stat ; 

    /* Initialize the device structure */
    memset (&dev, 0, sizeof (dev_t)) ;

    /* Initialize the link structure */
    memset (&emac_link, 0, sizeof (link_t)) ;

    emac_link.ui_tx = ui_tx ;
    emac_link.ui_rx = ui_rx ;

    /* Create Rx Semaphore */
    if (sem_new (NUM_BUFS, 0, "EMAC Rx Sem", &dev.rx_sem) != GOOD)
       return BAD ;


    /* Initialize the device */
    dev.mac[0] = 0x90 ;
    dev.mac[1] = 0xe6 ;
    dev.mac[2] = 0xba ;
    dev.mac[3] = 0x46 ;
    dev.mac[4] = 0x67 ;
    dev.mac[5] = 0x89 ;

    if (dev_init (&dev) != GOOD)
       return BAD ;

    /* Create the EMAC Task */
    stat = task_new (100, 0, 0x400, 0, 0, emac_rx_task, (usys) &dev, 0,0, 
                     "emac_rx_task", &dev.rx_task) ;
    if (stat != GOOD)
       return BAD ;

    return GOOD ;
} /* End of function emac_init() */

























#if 0
void dump_emac(void)
{
    u32 *reg ;

    reg = (u32 *) EMAC_RBQP ;
    printf ("RBQP = 0x%x\n", *reg) ;
    reg = (u32 *) EMAC_TBQP ;
    printf ("TBQP = 0x%x\n", *reg) ;
    reg = (u32 *) EMAC_SA1B ;
    printf ("SA1B = 0x%x\n", *reg) ;
    reg = (u32 *) EMAC_SA1U ;
    printf ("SA1U = 0x%x\n", *reg) ;

    printf ("rx buf = 0x%x\n", (usys) dev->rx_buf) ;
    printf ("rx des = 0x%x\n", (usys) dev->rx_desc) ;
    printf ("tx buf = 0x%x\n", (usys) dev->tx_buf) ;
    printf ("tx des = 0x%x\n", (usys) dev->tx_desc) ;

} 

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Function Name   : dev_init_by_netif                                       */
/* Description     : Called by lwip to initialize the driver                 */
/* Notes           : The address of this function was passed when netif_add  */
/*                   was called.                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
err_t    dev_init_by_netif (struct netif *netif)
{
    usys task_id, stat, i ;

    if (netif == NULL)
    {
       printf ("ERR: dev_init_by_netif() has *netif NULL\n") ;
       return ERR_VAL ;
    }

    /* Start filling out the netif structure */
    
    netif->output     = etharp_output ;
    netif->linkoutput = tx_data ;
    netif->state      = (void *) &dev ;

    netif->mtu        = 1500 ;
    netif->hwaddr_len = ETHARP_HWADDR_LEN ;
    netif->hwaddr[0]  = 0x90 ;
    netif->hwaddr[1]  = 0xe6 ;
    netif->hwaddr[2]  = 0xba ;
    netif->hwaddr[3]  = 0x46 ;
    netif->hwaddr[4]  = 0x67 ;
    netif->hwaddr[5]  = 0x89 ;
    netif->flags      = NETIF_FLAG_BROADCAST | 
                        NETIF_FLAG_ETHARP    | 
                        NETIF_FLAG_LINK_UP ; /* Really ?? <- DO */

    netif->name[0]    = 'e' ;
    netif->name[1]    = 'n' ;

    /* Write the MAC address */
    for (i = 0; i < 6; i++)
       dev.mac [i] = netif->hwaddr[i] ;

    /* Initialize the device */
    if ((stat = dev_init (&dev)) != GOOD)
       return ERR_IF ;

    /* Spawn the Rx Task */
    stat = task_new (RX_TASK_PRIORITY, 0, RX_TASK_STACK_SIZE, 1, 16,
                     emac_rx_task, 
                     (usys) netif, 0, 0, 
                     "EMAC Rx Task", &task_id) ;
    if (stat == GOOD)
    {
       dev.rx_task = task_id ;
       return ERR_OK ;
    } else {
       return ERR_IF ;
    }

} /* End of function dev_init_by_netif() */ 



void     net_setup (void *arg)
{
    ip_addr_t ipaddr, netmask, gateway ;
    struct netif *netadd_ret ;

#if 0
    /* Initialize the LWIP network stack */
    net_init() ;
#endif

    /* Set the IP */
    IP4_ADDR(&ipaddr,  192,168,1,10) ;
    IP4_ADDR(&netmask, 255,255,0,0)  ;
    IP4_ADDR(&gateway, 192,168,1,10) ;

    /* Add the Network Device Interface */
    netadd_ret = netif_add (&netif_inst, 
                            &ipaddr, 
                            &netmask, 
                            &gateway,
                            NULL, 
                            dev_init_by_netif,
                            tcpip_input) ;
    if (netadd_ret == NULL)
    {
       printf ("ERR: net_init(). Failed on netif_add()\n") ;
       return ;
    }

    /* Set the interface as the default */
    netif_set_default (netadd_ret) ;

    /* Bring up the interface */
    netif_set_up (&netif_inst) ;

} /* End of function net_setup() */

void     a(void)
{
    tcpip_init (net_setup, NULL) ;
}
#endif

