#ifndef _APP_DEFAULT_CONFIG_H
#define _APP_DEFAULT_CONFIG_H

#include "app_config.h"

#ifndef CONFIG_ENABLE_MP3
	#define CONFIG_ENABLE_MP3 1
#endif

#ifndef CONFIG_ENABLE_GPS
	#define CONFIG_ENABLE_GPS 0
#endif

#ifndef CONFIG_GUI_ENABLE 
	#define CONFIG_GUI_ENABLE 1
#endif

#ifndef CONFIG_LOG_GUI_MEM
	#define CONFIG_LOG_GUI_MEM 0
#endif

#ifndef CONFIG_LOG_BAT_INFO
	#define CONFIG_LOG_BAT_INFO 0
#endif

#ifndef CONFIG_INIT_DS3231
	#define CONFIG_INIT_DS3231 0
#endif

#endif


