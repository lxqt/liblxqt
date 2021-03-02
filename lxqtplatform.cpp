/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
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


#include "lxqtplatform.h"
using namespace LXQt;

Platform::PLATFORM Platform::getPlatform()
{
    QString platform = QGuiApplication::platformName();

    if(platform == QSL("wayland"))
        return PLATFORM::WAYLAND;
    else if(platform == QSL("xcb"))
        return PLATFORM::X11;
    else if(platform == QSL("windows"))
        return PLATFORM::WINDOWS;
    else if(platform == QSL("qnx"))
        return PLATFORM::QNX;
     else if(platform == QSL("openwfd"))
        return PLATFORM::OPENWFD;
    else if(platform == QSL("offscreen"))
        return PLATFORM::OFFSCREEN;
    else if(platform == QSL("minimalegl"))
        return PLATFORM::MINIMALEGL;
    else if(platform == QSL("minimal"))
        return PLATFORM::MINIMAL;
    else if(platform == QSL("linuxfb"))
        return PLATFORM::LINUXFB;
    else if(platform == QSL("kms"))
        return PLATFORM::KMS;
    else if(platform == QSL("ios"))
        return PLATFORM::IOS;
    else if(platform == QSL("eglfs"))
        return PLATFORM::EGLFS;
    else if(platform == QSL("directfb"))
        return PLATFORM::DIRECTFB;
    else if(platform == QSL("cocoa"))
        return PLATFORM::COCOA;
    else if(platform == QSL("android"))
        return PLATFORM::ANDROID;
     else
        return PLATFORM::OTHER;
}
