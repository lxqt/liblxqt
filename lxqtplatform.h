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

#ifndef LXQTPLATFORM_H
#define LXQTPLATFORM_H

#include <QGuiApplication> 
#include "lxqtglobals.h"

namespace LXQt
{
/*!
 * The Platform class provides a way to check the actual environment, X11, Wayland,...
 */
class LXQT_API Platform
{
public:
    enum PLATFORM {
        WAYLAND,    //!< Wayland platform
        X11,        //!< X11 platform
        WINDOWS,    //!< Windows platform
        QNX,
        OPENWFD,
        OFFSCREEN,
        MINIMALEGL,
        MINIMAL,
        LINUXFB,
        KMS,
        IOS,
        EGLFS,
        DIRECTFB,
        COCOA,
        ANDROID,
        OTHER       //!< Unknown platform
    };

    /**
      Returns the actual platform.
      \return Actual platform   .
     **/
    static PLATFORM getPlatform();

};

} // namespace LXQt
#endif // LXQTPLATFORM_H
