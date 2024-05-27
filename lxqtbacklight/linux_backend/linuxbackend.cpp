/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2016 - LXQt team
 * Authors:
 *   P.L. Lucas <selairi@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "driver/libbacklight_backend.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "driver/libbacklight_backend.c"
#ifdef __cplusplus
}
#endif

#include "linuxbackend.h"
#include <QTimer>
#include <QDebug>

namespace LXQt {

LinuxBackend::LinuxBackend(QObject *parent):VirtualBackEnd(parent)
{
    maxBacklight = lxqt_backlight_backend_get_max();
    backlightStream = NULL;
    if( isBacklightAvailable() ) {
        char *driver = lxqt_backlight_backend_get_driver();
        fileSystemWatcher = new QFileSystemWatcher(this);
        fileSystemWatcher->addPath(QString::fromLatin1("/sys/class/backlight/%1/actual_brightness").arg(QL1SV(driver)));
        fileSystemWatcher->addPath(QString::fromLatin1("/sys/class/backlight/%1/brightness").arg(QL1SV(driver)));
        fileSystemWatcher->addPath(QString::fromLatin1("/sys/class/backlight/%1/bl_power").arg(QL1SV(driver)));
        free(driver);
        actualBacklight = lxqt_backlight_backend_get();
        connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &LinuxBackend::fileSystemChanged);
    }
}

LinuxBackend::~LinuxBackend()
{
    closeBacklightStream();
}

int LinuxBackend::getBacklight()
{
    actualBacklight = lxqt_backlight_backend_get();
    return actualBacklight;
}

int LinuxBackend::getMaxBacklight()
{
    return maxBacklight;
}

bool LinuxBackend::isBacklightAvailable()
{
    return maxBacklight > 0;
}

bool LinuxBackend::isBacklightOff()
{
    return lxqt_backlight_is_backlight_off() > 0;
}

void LinuxBackend::setBacklight(int value)
{
    if( ! isBacklightAvailable() )
        return;
    if( backlightStream == NULL ) {
        backlightStream = lxqt_backlight_backend_get_write_stream();
        if( backlightStream != NULL ) {
            // Close stream after 60 seconds
            QTimer::singleShot(60000, this, SLOT(closeBacklightStream()));
        }
    }
    if( backlightStream != NULL ) {
        // normalize the value (to work around an issue in QSlider)
        value = qBound(0, value, maxBacklight);
        fprintf(backlightStream, "%d\n", value);
        fflush(backlightStream);
    }
}

void LinuxBackend::closeBacklightStream()
{
    if( backlightStream != NULL ) {
        fclose(backlightStream);
        backlightStream = NULL;
    }
}

void LinuxBackend::fileSystemChanged(const QString & /*path*/)
{
    int value = actualBacklight;
    if( value != getBacklight() ) {
        Q_EMIT backlightChanged(actualBacklight);
    }
}

} // namespace LXQt
