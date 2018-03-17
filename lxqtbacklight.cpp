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

#include "lxqtbacklight.h"
#include "lxqtbacklight/virtual_backend.h"
#include "lxqtbacklight/linux_backend/linuxbackend.h"

namespace LXQt {

Backlight::Backlight(QObject *parent):QObject(parent)
{
    _backend = (VirtualBackEnd *) new LinuxBackend(this);
    connect(_backend, SIGNAL(backlightChanged(int)), this, SLOT(backlightChangedSlot(int)));
}

Backlight::~Backlight()
{
    delete _backend;
}

int Backlight::getBacklight()
{
    return _backend->getBacklight();
}

int Backlight::getMaxBacklight()
{
    return _backend->getMaxBacklight();
}

bool Backlight::isBacklightAvailable()
{
    return _backend->isBacklightAvailable();
}

void Backlight::setBacklight(int value)
{
    _backend->setBacklight(value);
}

void Backlight::backlightChangedSlot(int value)
{
    emit backlightChanged(value);
}

} // namespace LXQt
