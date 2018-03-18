/*
 * Copyright (C) 2016  P.L. Lucas
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/********************************************************************************
 *    This library uses Linux /sys/class/backlight files to read and change the 
 *    backlight level.
 *    
 *    If screen backlight can be controlled, the Linux kernel will show inside 
 *    /sys/class/backlight directory one or more directories. Each directory has
 *    got the following files:
 *        /sys/class/backlight/driver/max_brightness
 *        /sys/class/backlight/driver/actual_brightness
 *        /sys/class/backlight/driver/brightness
 *        /sys/class/backlight/driver/type
 *        /sys/class/backlight/driver/bl_power
 *    
 *    The "max_brightness" file contains the maximum value that can be set to the
 *    backlight level.
 *    
 *    In "brightness" file you can write the value of backlight and the Linux 
 *    kernel will set that value.
 *    
 *    The "bl_power" controls if backlight is turn on (0) or turn off (>0).
 *    
 *    You must read actual backlight level from "actual_brightness" file. Never
 *    read the backlight level from "brightness" file.
 *    
 *    The "type" file is the type of control and it can be:
 *        firmware
 *        platform 
 *        raw
 *    The firmware control should be preferred to platform control. The platform
 *    control should be preferred to raw control.        
 *    If there are several directories in /sys/class/backlight/, you should use
 *    the directory which its "type" file has got the "firmware" value.
 *    
 *    In order to write in /sys/class/backlight/driver/brightness file root
 *    permissions are needed. This library calls to a command line tool called 
 *    "lxqtbacklight_backend". "lxqtbacklight_backend" has a policy in Polkit 
 *    in order to write in /sys/class/backlight/driver/brightness file.
 *******************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libbacklight_backend.h"

#define True 1
#define False 0

static int read_backlight(char *driver);
static int read_max_backlight(char *driver);
static int read_bl_power(char *driver);

int lxqt_backlight_backend_get()
{
    char *driver = lxqt_backlight_backend_get_driver();
    if( driver == NULL ) {
        fprintf(stderr, "Error: /sys/class/backlight is empty.");
        return -1;
    }
    int value = read_backlight(driver);
    free(driver);
    return value;
}

int lxqt_backlight_backend_get_max()
{
    char *driver = lxqt_backlight_backend_get_driver();
    if( driver == NULL ) {
        fprintf(stderr, "Error: /sys/class/backlight is empty.");
        return -1;
    }
    int value = read_max_backlight(driver);
    int bl_power = read_bl_power(driver);
    free(driver);
    return bl_power==0 ? value : -1;
}

FILE *lxqt_backlight_backend_get_write_stream()
{
    FILE *stream = popen("pkexec lxqt-backlight_backend --stdin", "w");
    return stream;
}

static int read_int(char *path)
{
    FILE *in = fopen(path, "r");
    if( in == NULL ) {
        char buffer[1024];
        sprintf(buffer, "Couldn't open %s", path);
        perror(buffer);
        return -1;
    }
    int value;
    int ok = fscanf(in, "%d", &value);
    fclose(in);
    if( ok == EOF ) {
        value = 0;
    }
    return value;
}

static int read_backlight(char *driver)
{
    char path[1024];
    sprintf(path, "/sys/class/backlight/%s/actual_brightness", driver);
    return read_int(path);
}

static int read_max_backlight(char *driver)
{
    char path[1024];
    sprintf(path, "/sys/class/backlight/%s/max_brightness", driver);
    return read_int(path);
}

static int read_bl_power(char *driver)
{
    char path[1024];
    sprintf(path, "/sys/class/backlight/%s/bl_power", driver);
    return read_int(path);
}

typedef enum {FIRMWARE, PLATFORM, RAW, OTHER, N_BACKLIGHT} BackligthTypes;

char *lxqt_backlight_backend_get_driver()
{
    DIR *dirp;
    struct dirent *dp;
    
    char *drivers[N_BACKLIGHT];
    char *driver;
    int n;
    char path[1024], type[1024];
    
    for(n=0;n<N_BACKLIGHT;n++)
        drivers[n] = NULL;

    if ((dirp = opendir("/sys/class/backlight")) == NULL) {
        perror("Couldn't open /sys/class/backlight");
        return NULL;
    }

    do {
        errno = 0;
        if ((dp = readdir(dirp)) != NULL) {
            if( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
                continue;
            driver = dp->d_name;
            sprintf(path, "/sys/class/backlight/%s/type", driver);
            FILE *in = fopen(path, "r");
            if( in == NULL )
                continue;
            int ok = fscanf(in, "%s", type);
            fclose(in);
            if( ok != EOF ) {
                // firmware control should be preferred to platform control should be preferred to raw control.
                if( ! strcmp("firmware", type) ) {
                    drivers[FIRMWARE] = strdup(driver);
                    break;
                } else if( ! strcmp("platform", type) )
                    drivers[PLATFORM] = strdup(driver);
                else if( ! strcmp("raw", type) )
                    drivers[RAW] = strdup(driver);
                else // Only, firmware, platform and raw are defined, but...
                    drivers[OTHER] = strdup(driver);
            }
        }
    } while (dp != NULL);

    closedir(dirp);

    if (errno != 0)
        perror("Error reading directory");    
    
    driver = NULL;
    for(n=0;n<N_BACKLIGHT;n++) {
        if( drivers[n] != NULL && driver == NULL )
            driver = drivers[n];
        else
            free(drivers[n]);
    }

    return driver;
}


