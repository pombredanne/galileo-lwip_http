/* main.c - Start a web server using the onboard GMAC ethernet */

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

#define TIMER_INTERVAL 100 /* miliseconds, 100 or less */

#include <microkernel.h>
#include "zephyr.h"

#include "../httpserver_raw/httpd.h"
#include "gmac.h"
#include <lwip/timers.h>

extern struct netif gmac;

extern void setup_gmac_ethernet();

uint64_t deadline, now;

void main (void)
    {
    /*
     * Setup the GMAC ethernet device and assign IP address
     */

    setup_gmac_ethernet();

    /*
     * Initialize and start the HTTP server
     */

    httpd_init();

    /*
     * Get the current time so that we can schedule the next service call
     */
     
    now = task_tick_get();
    deadline = now + (sys_clock_ticks_per_sec * TIMER_INTERVAL/1000);

    /*
     * Start the ethernet servicing task
     */

#if GMAC_USE_IRQ
    do  {
        int r;
        r = task_event_recv_wait_timeout(LWIP, deadline - now);

        if (ERR_OK == r)
            {
            gmac_service(&gmac);
            }

        now = task_tick_get(); 

        if (now >= deadline)
            {
            sys_check_timeouts();
            now = task_tick_get();
            deadline = now + (sys_clock_ticks_per_sec * TIMER_INTERVAL/1000);
            }

        } while(1);
#else
    do  {
        /*
         * Use polling to service the ethernet driver
         */

        gmac_service(&gmac);
        
        now = task_tick_get();

        if (now >= deadline)
            {
            sys_check_timeouts();
            now = task_tick_get();
            deadline = now + (sys_clock_ticks_per_sec * TIMER_INTERVAL/1000);
            }

        task_sleep(1);

        } while(1);
#endif
    }

