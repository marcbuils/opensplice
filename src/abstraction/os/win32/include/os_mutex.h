/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2011 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#ifndef OS_WIN32_MUTEX_H
#define OS_WIN32_MUTEX_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "os_defs.h"

typedef struct mutex {
    os_scopeAttr scope;
    long         id;
    long         lockCount;
} os_os_mutex;

#if defined (__cplusplus)
}
#endif

#endif /* OS_WIN32_MUTEX_H */
