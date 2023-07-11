#ifndef _TIME_SYNC_H_
#define _TIME_SYNC_H_

#include "esp_sntp.h"
#include "esp_log.h"

extern void obtain_time(const char *timezone);

#endif