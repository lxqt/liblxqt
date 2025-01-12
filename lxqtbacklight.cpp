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

#include "lxqtbacklight.h"
#include "lxqtbacklight/virtual_backend.h"
#ifdef USE_BACKLIGHT_LINUX_BACKEND
    #include "lxqtbacklight/linux_backend/linuxbackend.h"
#endif

namespace LXQt {

Backlight::Backlight(QObject *parent):QObject(parent)
{
#ifdef USE_BACKLIGHT_LINUX_BACKEND
    m_backend = (VirtualBackEnd *) new LinuxBackend(this);
#else
    m_backend = new VirtualBackEnd(this);
#endif
    connect(m_backend, &VirtualBackEnd::backlightChanged, this, &Backlight::backlightChangedSlot);
}

Backlight::~Backlight()
{
    delete m_backend;
}

int Backlight::getBacklight()
{
    return m_backend->getBacklight();
}

int Backlight::getMaxBacklight()
{
    return m_backend->getMaxBacklight();
}

bool Backlight::isBacklightAvailable()
{
    return m_backend->isBacklightAvailable();
}

bool Backlight::isBacklightOff()
{
    return m_backend->isBacklightOff();
}

void Backlight::setBacklight(int value)
{
    m_backend->setBacklight(value);
}

void Backlight::backlightChangedSlot(int value)
{
    Q_EMIT backlightChanged(value);
}

} // namespace LXQt
