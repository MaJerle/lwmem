/* Config header in lwmem_config.h file */
#define LWMEM_CFG_OS_MUTEX_HANDLE      osMutexId

/* System file */
uint8_t
lwmem_sys_mutex_create(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    osMutexDef(mut);
    *m = osMutexCreate(osMutex(mut));
    return 1;
}

uint8_t
lwmem_sys_mutex_isvalid(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    return *m != NULL;
}

uint8_t
lwmem_sys_mutex_wait(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    if (osMutexWait(*m, osWaitForever) != osOK) {
        return 0;
    }
    return 1;
}

uint8_t
lwmem_sys_mutex_release(LWMEM_CFG_OS_MUTEX_HANDLE* m) {
    if (osMutexRelease(*m) != osOK) {
        return 0;
    }
    return 1;
}