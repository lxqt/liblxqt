/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#ifndef LXQTPOWER_PROVIDERS_H
#define LXQTPOWER_PROVIDERS_H

#include <QtCore/QObject>
#include <razorsettings.h>
#include "lxqtpower.h"

namespace LxQt
{


class PowerProvider: public QObject
{
    Q_OBJECT
public:
    
    enum DbusErrorCheck {
        CheckDBUS,
        DontCheckDBUS
    };
    
    explicit PowerProvider(QObject *parent = 0);
    virtual ~PowerProvider();

    /*! Returns true if the Power can perform action.
        This is a pure virtual function, and must be reimplemented in subclasses. */
    virtual bool canAction(Power::Action action) const = 0 ;

public slots:
    /*! Performs the requested action.
        This is a pure virtual function, and must be reimplemented in subclasses. */
    virtual bool doAction(Power::Action action) = 0;
};


class UPowerProvider: public PowerProvider
{
    Q_OBJECT
public:
    UPowerProvider(QObject *parent = 0);
    ~UPowerProvider();
    bool canAction(Power::Action action) const;

public slots:
    bool doAction(Power::Action action);
};


class ConsoleKitProvider: public PowerProvider
{
    Q_OBJECT
public:
    ConsoleKitProvider(QObject *parent = 0);
    ~ConsoleKitProvider();
    bool canAction(Power::Action action) const;

public slots:
    bool doAction(Power::Action action);
};


class SystemdProvider: public PowerProvider
{
    Q_OBJECT
public:
    SystemdProvider(QObject *parent = 0);
    ~SystemdProvider();
    bool canAction(Power::Action action) const;

public slots:
    bool doAction(Power::Action action);
};


class RazorProvider: public PowerProvider
{
    Q_OBJECT
public:
    RazorProvider(QObject *parent = 0);
    ~RazorProvider();
    bool canAction(Power::Action action) const;

public slots:
    bool doAction(Power::Action action);
};


class HalProvider: public PowerProvider
{
    Q_OBJECT
public:
    HalProvider(QObject *parent = 0);
    ~HalProvider();
    bool canAction(Power::Action action) const;

public slots:
    bool doAction(Power::Action action);
};


class CustomProvider: public PowerProvider
{
    Q_OBJECT
public:
    CustomProvider(QObject *parent = 0);
    ~CustomProvider();
    bool canAction(Power::Action action) const;

public slots:
    bool doAction(Power::Action action);

private:
    RazorSettings mSettings;
};

} // namespace LxQt
#endif // LXQTPOWER_PROVIDERS_H
