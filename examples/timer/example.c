
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Mar 17 14:15:19 BRT 2017
 * Project: libcollections
 *
 * Copyright (C) 2017 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "collections.h"

void timer_1(ctimer_arg_t arg)
{
    ctimer_set_state(arg, CL_TIMER_ST_RUNNING);
    printf("%s\n", __FUNCTION__);
    ctimer_set_state(arg, CL_TIMER_ST_WAITING);
}

void timer_2(ctimer_arg_t arg)
{
    ctimer_set_state(arg, CL_TIMER_ST_RUNNING);
    printf("%s\n", __FUNCTION__);
    ctimer_set_state(arg, CL_TIMER_ST_WAITING);
}

int main(int argc, char **argv)
{
    const char *opt = "hf:\0";
    int option;
    ctimer_t *timers = NULL;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                return 1;

            case '?':
                return -1;
        }
    } while (option != -1);

    collections_init(NULL);

    if (ctimer_register(&timers, 1, CL_TIMER_IMODE_DISCOUNT_RUNTIME, 100,
                        "timer_1", timer_1, NULL, NULL, NULL) < 0)
    {
        printf("Error 1: %s\n", cstrerror(cget_last_error()));
        return -1;
    }

    if (ctimer_register(&timers, 2, CL_TIMER_IMODE_DISCOUNT_RUNTIME, 100,
                        "timer_2", timer_2, NULL, NULL, NULL) < 0)
    {
        printf("Error 2: %s\n", cstrerror(cget_last_error()));
        return -1;
    }

    ctimer_install(timers);
    sleep(10);
    ctimer_uninstall(timers);

    collections_uninit();

    return 0;
}

