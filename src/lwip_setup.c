/* setup.c - Setup routines for GMAC and Web Server */

/*
 * Copyright (c) 2015, Wind River Systems, Inc. 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 */

#include <autoconf.h>
 
#define USE_DHCP    TRUE  
#define USE_LCD     TRUE

#include <string.h>
#include <microkernel.h>
#include "zephyr.h"

#include "gmac.h"
#include <drivers/pci/pci.h>
#include <drivers/ioapic.h>
#include <lwip/init.h>
#include <lwip/timers.h>
#include <lwip/raw.h>
#include <lwip/inet.h>
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <netif/etharp.h>

#include "hd44780Lcd.h"
#include "groveLCDUtils.h"

struct device *i2c = NULL;

struct gmac_config gmac_dev = { .pci.bus = 0, .pci.dev = 20, .pci.function = 6 , .event = LWIP };
ip4_addr_t         ipaddr, netmask;
struct netif     * eth = NULL;
struct netif       gmac;

/*
 * LWIP support routine required to be implemented by the OS
 */

u32_t sys_now()
    {
    return (task_tick_get_32()*1000)/sys_clock_ticks_per_sec;
    }

/*
 * LWIP supports a callback routine that will be called each time
 * the link status changes.
 * We use it to display the current IP address to make it easier
 * to retrieve the IP adsress assigned via DHCP.
 */

static void netif_combined_callback(struct netif *netif)
    {
    char string[20];

#ifdef USE_LCD
    if (i2c == NULL)
        {
        i2c = device_get_binding("I2C0");
        groveLcdInit(i2c);
        groveLcdCommand(i2c, LCD_CLEAR);
        groveLcdColorSet(i2c, 0, 200, 200);
        }
#endif

    if (netif_is_up(netif) && netif_is_link_up(netif))
        {
        /*
         * Start your thing here.
         * Might not have an IP address if DHCP is used
         */

        if (strcmp("0.0.0.0", ipaddr_ntoa(&netif->ip_addr)) != 0)
            {
            printk("Link is up\n");

#ifdef USE_DHCP
            printk("IP address: %s - dhcp \n",
                ipaddr_ntoa(&netif->ip_addr));
#else
            printk("IP address: %s - static \n",
                ipaddr_ntoa(&netif->ip_addr));
#endif /* USE_DHCP */

            /*
             * Show the web server URL on the console of
             * the HAC to make it easier to access. The user can just
             * do a control click with left mouse button to open in
             * their browser.
             */

            printk("\nWeb Server URL (mouse ctrl-click): http://%s\n\n",
                ipaddr_ntoa(&netif->ip_addr));

#ifdef USE_LCD

#ifdef USE_DHCP
            snprintf(string, 20, "IP ADDR: dhcp       ");
#else
            snprintf(string, 20, "IP ADDR: static     ");
#endif /* USE_DHCP */

            groveLcdPrint(i2c, 0,0, string, 20);
            snprintf(string, 20, "%s        ",
                ipaddr_ntoa(&netif->ip_addr));
            groveLcdPrint(i2c, 1,0, string, 20);
#endif /* USE_LCD */

            } /* End of strncmp */
        }
    else
        {
        /*
         * Stop your thing here
         */

        printk("Link is down\n");
#ifdef USE_LCD
        snprintf(string, 20, "Link is down        ");
        groveLcdPrint(i2c, 0,0, string, 20);
        snprintf(string, 20, "                    ");
        groveLcdPrint(i2c, 1,0, string, 20);
#endif
        }
    }

/*
 * This routine does the core setup of the GMAC ethernet driver (gmac.c)
 * and assigns an IP address to be used.
 */

void setup_gmac_ethernet(void)
    {
    /*
     * This should be in arch/x86/platforms/galileo/galileo.c
     * Until it is not present in the base, add it manually here.
     */

#define INT_VEC_IRQ0 0x20
    _ioapic_irq_set(18, 18 + INT_VEC_IRQ0, (IOAPIC_LEVEL | IOAPIC_LOW));

    lwip_init();

    printk("Adding gmac\n");
  
    eth = netif_add(&gmac, NULL, NULL, NULL, &gmac_dev, gmac_init,
                    ethernet_input);

    if (!eth)
        {
        printk("No ethernet interface\n");
        while(1);
        }

#if LWIP_IPV6
    netif_create_ip6_linklocal_address(eth, 1);
#if LWIP_IPV6_AUTOCONFIG
    eth->ip6_autoconfig_enabled = 1;
#endif
#endif

    netif_set_link_callback(eth, netif_combined_callback);
    netif_set_status_callback(eth, netif_combined_callback);

#ifdef USE_DHCP
    /*
     * interface must be "admin up" when DHCP is started.  set to default
     * interface to allow ip routing.
     */

    netif_set_up(eth);  
    netif_set_default(eth);
    dhcp_start(eth);
#else
    /*
     * When using a static IP address and netmask
     */

    IP4_ADDR(&ipaddr, 90,0,0,2);
    IP4_ADDR(&netmask, 255,255,255,0);
    netif_set_addr(eth, &ipaddr, &netmask, NULL);
    netif_set_up(eth);

    /*
     * Unless you set a gateway, use the default
     */

    netif_set_default(eth);
#endif

    }

