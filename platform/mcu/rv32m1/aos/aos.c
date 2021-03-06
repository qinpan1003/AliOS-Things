/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <aos/aos.h>
#include <hal/hal.h>
#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"

#define AOS_START_STACK 256

extern int application_start(int argc, char **argv);
extern int vfs_init(void);
extern int vfs_device_init(void);
extern uart_dev_t uart_0;

ktask_t *g_aos_app;
static void sys_init(void)
{
#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif

#ifdef CONFIG_AOS_CLI
    aos_cli_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    aos_loop_init();
#endif

    application_start(0, NULL);
}

static void platform_init(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
}

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

void hal_reboot(void)
{
    EVENT_SystemReset();
}

int main(void)
{
    platform_init();

    aos_init();
    hal_uart_init(&uart_0);
    krhino_task_dyn_create(&g_aos_app, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);

    SystemSetupSystick(RHINO_CONFIG_TICKS_PER_SECOND, 7);

    aos_start();
    return 0;
}
