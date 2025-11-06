/**
 * \file            lwmem_sys_cmsis_os.c
 * \brief           System functions for CMSIS-OS based operating system
 */

/*
 * Copyright (c) 2024 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwMEM - Lightweight dynamic memory manager library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v2.2.4
 */
#include "system/lwmem_sys.h"

#if LWMEM_CFG_OS && !__DOXYGEN__

/*
 * To use this module, options must be defined as
 *
 * #define LWMEM_CFG_OS_MUTEX_HANDLE        TX_MUTEX
 */

/* Include ThreadX API module */
#include "tx_api.h"
#include "tx_mutex.h"

uint8_t
lwmem_sys_mutex_create(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    static char name[] = "lwmem_mutex";
    return tx_mutex_create(m, name, TX_INHERIT) == TX_SUCCESS;
}

uint8_t
lwmem_sys_mutex_isvalid(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    return m->tx_mutex_id == TX_MUTEX_ID;
}

uint8_t
lwmem_sys_mutex_wait(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    return tx_mutex_get(m, TX_WAIT_FOREVER) == TX_SUCCESS;
}

uint8_t
lwmem_sys_mutex_release(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    return tx_mutex_put(m) == TX_SUCCESS;
}

#endif /* LWMEM_CFG_OS && !__DOXYGEN__ */
