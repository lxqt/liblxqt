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

#include "virtual_backend.h"

namespace LXQt {

VirtualBackEnd::VirtualBackEnd(QObject *parent):QObject(parent)
{
}

bool VirtualBackEnd::isBacklightAvailable()
{
    return false;
}

bool VirtualBackEnd::isBacklightOff()
{
    return false;
}

void VirtualBackEnd::setBacklight(int /*value*/)
{
}

int VirtualBackEnd::getBacklight()
{
    return -1;
}

int VirtualBackEnd::getMaxBacklight()
{
    return -1;
}

} // namespace LXQt
