/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Petr Vanek <petr@scribus.info>
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


#include "lxqtpowerproviders.h"
#include <QDBusInterface>
#include <QProcess>
#include <QGuiApplication>
#include <QDebug>
#include "lxqtnotification.h"
#include <csignal> // for kill()

#define UPOWER_SERVICE          "org.freedesktop.UPower"
#define UPOWER_PATH             "/org/freedesktop/UPower"
#define UPOWER_INTERFACE        UPOWER_SERVICE

#define CONSOLEKIT_SERVICE      "org.freedesktop.ConsoleKit"
#define CONSOLEKIT_PATH         "/org/freedesktop/ConsoleKit/Manager"
#define CONSOLEKIT_INTERFACE    "org.freedesktop.ConsoleKit.Manager"

#define SYSTEMD_SERVICE         "org.freedesktop.login1"
#define SYSTEMD_PATH            "/org/freedesktop/login1"
#define SYSTEMD_INTERFACE       "org.freedesktop.login1.Manager"

#define LXQT_SERVICE      "org.lxqt.session"
#define LXQT_PATH         "/LXQtSession"
#define LXQT_INTERFACE    "org.lxqt.session"

#define PROPERTIES_INTERFACE    "org.freedesktop.DBus.Properties"

using namespace LXQt;

/************************************************
 Helper func
 ************************************************/
void printDBusMsg(const QDBusMessage &msg)
{
    qWarning() << "** Dbus error **************************";
    qWarning() << "Error name " << msg.errorName();
    qWarning() << "Error msg  " << msg.errorMessage();
    qWarning() << "****************************************";
}


/************************************************
 Helper func
 ************************************************/
static bool dbusCall(const QString &service,
              const QString &path,
              const QString &interface,
              const QDBusConnection &connection,
              const QString & method,
              PowerProvider::DbusErrorCheck errorCheck = PowerProvider::CheckDBUS
              )
{
    QDBusInterface dbus(service, path, interface, connection);
    if (!dbus.isValid())
    {
        qWarning() << "dbusCall: QDBusInterface is invalid" << service << path << interface << method;
        if (errorCheck == PowerProvider::CheckDBUS)
        {
            Notification::notify(
                                    QObject::tr("Power Manager Error"),
                                    QObject::tr("QDBusInterface is invalid") + QL1SV("\n\n") + service + QL1C(' ') + path + QL1C(' ') + interface + QL1C(' ') + method,
                                    QL1SV("lxqt-logo.png"));
        }
        return false;
    }

    QDBusMessage msg = dbus.call(method);

    if (!msg.errorName().isEmpty())
    {
        printDBusMsg(msg);
        if (errorCheck == PowerProvider::CheckDBUS)
        {
            Notification::notify(
                                    QObject::tr("Power Manager Error (D-BUS call)"),
                                    msg.errorName() + QL1SV("\n\n") + msg.errorMessage(),
                                    QL1SV("lxqt-logo.png"));
        }
    }

    // If the method no returns value, we believe that it was successful.
    return msg.arguments().isEmpty() ||
           msg.arguments().constFirst().isNull() ||
           msg.arguments().constFirst().toBool();
}

/************************************************
 Helper func

 Just like dbusCall(), except that systemd
 returns a string instead of a bool, and it takes
 an "interactivity boolean" as an argument.
 ************************************************/
static bool dbusCallSystemd(const QString &service,
                     const QString &path,
                     const QString &interface,
                     const QDBusConnection &connection,
                     const QString &method,
                     bool needBoolArg,
                     PowerProvider::DbusErrorCheck errorCheck = PowerProvider::CheckDBUS
                     )
{
    QDBusInterface dbus(service, path, interface, connection);
    if (!dbus.isValid())
    {
        qWarning() << "dbusCall: QDBusInterface is invalid" << service << path << interface << method;
        if (errorCheck == PowerProvider::CheckDBUS)
        {
            Notification::notify(
                                    QObject::tr("Power Manager Error"),
                                    QObject::tr("QDBusInterface is invalid") + QL1SV("\n\n") + service + QL1C(' ') + path + QL1C(' ')+ interface + QL1C(' ') + method,
                                    QL1SV("lxqt-logo.png"));
        }
        return false;
    }

    QDBusMessage msg = needBoolArg ? dbus.call(method, QVariant(true)) : dbus.call(method);

    if (!msg.errorName().isEmpty())
    {
        printDBusMsg(msg);
        if (errorCheck == PowerProvider::CheckDBUS)
        {
            Notification::notify(
                                    QObject::tr("Power Manager Error (D-BUS call)"),
                                    msg.errorName() + QL1SV("\n\n") + msg.errorMessage(),
                                    QL1SV("lxqt-logo.png"));
        }
    }

    // If the method no returns value, we believe that it was successful.
    if (msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
        return true;

    QString response = msg.arguments().constFirst().toString();
    qDebug() << "systemd:" << method << "=" << response;
    return response == QL1SV("yes") || response == QL1SV("challenge");
}


/************************************************
 Helper func
 ************************************************/
bool dbusGetProperty(const QString &service,
                     const QString &path,
                     const QString &interface,
                     const QDBusConnection &connection,
                     const QString & property
                    )
{
    QDBusInterface dbus(service, path, interface, connection);
    if (!dbus.isValid())
    {
        qWarning() << "dbusGetProperty: QDBusInterface is invalid" << service << path << interface << property;
//        Notification::notify(QObject::tr("LXQt Power Manager"),
//                                  "lxqt-logo.png",
//                                  QObject::tr("Power Manager Error"),
//                                  QObject::tr("QDBusInterface is invalid")+ "\n\n" + service +" " + path +" " + interface +" " + property);

        return false;
    }

    QDBusMessage msg = dbus.call(QL1SV("Get"), dbus.interface(), property);

    if (!msg.errorName().isEmpty())
    {
        printDBusMsg(msg);
//        Notification::notify(QObject::tr("LXQt Power Manager"),
//                                  "lxqt-logo.png",
//                                  QObject::tr("Power Manager Error (Get Property)"),
//                                  msg.errorName() + "\n\n" + msg.errorMessage());
    }

    return !msg.arguments().isEmpty() &&
            msg.arguments().constFirst().value<QDBusVariant>().variant().toBool();
}




/************************************************
 PowerProvider
 ************************************************/
PowerProvider::PowerProvider(QObject *parent):
    QObject(parent)
{
}


PowerProvider::~PowerProvider() = default;



/************************************************
 UPowerProvider
 ************************************************/
UPowerProvider::UPowerProvider(QObject *parent):
    PowerProvider(parent)
{
}


UPowerProvider::~UPowerProvider() = default;


bool UPowerProvider::canAction(Power::Action action) const
{
    QString command;
    QString property;
    switch (action)
    {
    case Power::PowerHibernate:
        property = QL1SV("CanHibernate");
        command  = QL1SV("HibernateAllowed");
        break;

    case Power::PowerSuspend:
        property = QL1SV("CanSuspend");
        command  = QL1SV("SuspendAllowed");
        break;

    default:
        return false;
    }

    return  dbusGetProperty(  // Whether the system is able to hibernate.
                QL1SV(UPOWER_SERVICE),
                QL1SV(UPOWER_PATH),
                QL1SV(PROPERTIES_INTERFACE),
                QDBusConnection::systemBus(),
                property
            )
            &&
            dbusCall( // Check if the caller has (or can get) the PolicyKit privilege to call command.
                QL1SV(UPOWER_SERVICE),
                QL1SV(UPOWER_PATH),
                QL1SV(UPOWER_INTERFACE),
                QDBusConnection::systemBus(),
                command,
                // canAction should be always silent because it can freeze
                // g_main_context_iteration Qt event loop in QMessageBox
                // on panel startup if there is no DBUS running.
                PowerProvider::DontCheckDBUS
            );
}


bool UPowerProvider::doAction(Power::Action action)
{
    QString command;

    switch (action)
    {
    case Power::PowerHibernate:
        command = QL1SV("Hibernate");
        break;

    case Power::PowerSuspend:
        command = QL1SV("Suspend");
        break;

    default:
        return false;
    }


    return dbusCall(QL1SV(UPOWER_SERVICE),
             QL1SV(UPOWER_PATH),
             QL1SV(UPOWER_INTERFACE),
             QDBusConnection::systemBus(),
             command );
}



/************************************************
 ConsoleKitProvider
 ************************************************/
ConsoleKitProvider::ConsoleKitProvider(QObject *parent):
    PowerProvider(parent)
{
}


ConsoleKitProvider::~ConsoleKitProvider() = default;


bool ConsoleKitProvider::canAction(Power::Action action) const
{
    QString command;
    switch (action)
    {
    case Power::PowerReboot:
        command = QL1SV("CanReboot");
        break;

    case Power::PowerShutdown:
        command = QL1SV("CanPowerOff");
        break;

    case Power::PowerHibernate:
        command  = QL1SV("CanHibernate");
        break;

    case Power::PowerSuspend:
        command  = QL1SV("CanSuspend");
        break;

    default:
        return false;
    }

    return dbusCallSystemd(QL1SV(CONSOLEKIT_SERVICE),
                    QL1SV(CONSOLEKIT_PATH),
                    QL1SV(CONSOLEKIT_INTERFACE),
                    QDBusConnection::systemBus(),
                    command,
                    false,
                    // canAction should be always silent because it can freeze
                    // g_main_context_iteration Qt event loop in QMessageBox
                    // on panel startup if there is no DBUS running.
                    PowerProvider::DontCheckDBUS
                   );
}


bool ConsoleKitProvider::doAction(Power::Action action)
{
    QString command;
    switch (action)
    {
    case Power::PowerReboot:
        command = QL1SV("Reboot");
        break;

    case Power::PowerShutdown:
        command = QL1SV("PowerOff");
        break;

    case Power::PowerHibernate:
        command = QL1SV("Hibernate");
        break;

    case Power::PowerSuspend:
        command = QL1SV("Suspend");
        break;

    default:
        return false;
    }

    return dbusCallSystemd(QL1SV(CONSOLEKIT_SERVICE),
                QL1SV(CONSOLEKIT_PATH),
                QL1SV(CONSOLEKIT_INTERFACE),
                QDBusConnection::systemBus(),
                command,
                true
               );
}

/************************************************
  SystemdProvider

  http://www.freedesktop.org/wiki/Software/systemd/logind
 ************************************************/

SystemdProvider::SystemdProvider(QObject *parent):
    PowerProvider(parent)
{
}


SystemdProvider::~SystemdProvider() = default;


bool SystemdProvider::canAction(Power::Action action) const
{
    QString command;

    switch (action)
    {
    case Power::PowerReboot:
        command = QL1SV("CanReboot");
        break;

    case Power::PowerShutdown:
        command = QL1SV("CanPowerOff");
        break;

    case Power::PowerSuspend:
        command = QL1SV("CanSuspend");
        break;

    case Power::PowerHibernate:
        command = QL1SV("CanHibernate");
        break;

    default:
        return false;
    }

    return dbusCallSystemd(QL1SV(SYSTEMD_SERVICE),
                    QL1SV(SYSTEMD_PATH),
                    QL1SV(SYSTEMD_INTERFACE),
                    QDBusConnection::systemBus(),
                    command,
                    false,
                    // canAction should be always silent because it can freeze
                    // g_main_context_iteration Qt event loop in QMessageBox
                    // on panel startup if there is no DBUS running.
                    PowerProvider::DontCheckDBUS
                   );
}


bool SystemdProvider::doAction(Power::Action action)
{
    QString command;

    switch (action)
    {
    case Power::PowerReboot:
        command = QL1SV("Reboot");
        break;

    case Power::PowerShutdown:
        command = QL1SV("PowerOff");
        break;

    case Power::PowerSuspend:
        command = QL1SV("Suspend");
        break;

    case Power::PowerHibernate:
        command = QL1SV("Hibernate");
        break;

    default:
        return false;
    }

    return dbusCallSystemd(QL1SV(SYSTEMD_SERVICE),
             QL1SV(SYSTEMD_PATH),
             QL1SV(SYSTEMD_INTERFACE),
             QDBusConnection::systemBus(),
             command,
             true
            );
}


/************************************************
  LXQtProvider
 ************************************************/
LXQtProvider::LXQtProvider(QObject *parent):
    PowerProvider(parent)
{
}


LXQtProvider::~LXQtProvider() = default;


bool LXQtProvider::canAction(Power::Action action) const
{
    QString command;
    switch (action)
    {
        case Power::PowerLogout:
            command = QL1SV("canLogout");
            break;
        case Power::PowerReboot:
            command = QL1SV("canReboot");
            break;
        case Power::PowerShutdown:
            command = QL1SV("canPowerOff");
            break;
        default:
            return false;
    }

    // there can be case when lxqtsession-session does not run
    return dbusCall(QL1SV(LXQT_SERVICE), QL1SV(LXQT_PATH), QL1SV(LXQT_INTERFACE),
            QDBusConnection::sessionBus(), command,
            PowerProvider::DontCheckDBUS);
}


bool LXQtProvider::doAction(Power::Action action)
{
    QString command;
    switch (action)
    {
        case Power::PowerLogout:
            command = QL1SV("logout");
            break;
        case Power::PowerReboot:
            command = QL1SV("reboot");
            break;
        case Power::PowerShutdown:
            command = QL1SV("powerOff");
            break;
        default:
            return false;
    }

    return dbusCall(QL1SV(LXQT_SERVICE),
             QL1SV(LXQT_PATH),
             QL1SV(LXQT_INTERFACE),
             QDBusConnection::sessionBus(),
             command
            );
}

/************************************************
  LxSessionProvider
 ************************************************/
LxSessionProvider::LxSessionProvider(QObject *parent):
    PowerProvider(parent)
{
    pid = (qint64)qgetenv("_LXSESSION_PID").toLong();
}


LxSessionProvider::~LxSessionProvider() = default;


bool LxSessionProvider::canAction(Power::Action action) const
{
    switch (action)
    {
        case Power::PowerLogout:
            return pid != 0;
        default:
            return false;
    }
}


bool LxSessionProvider::doAction(Power::Action action)
{
    switch (action)
    {
    case Power::PowerLogout:
        if(pid)
            ::kill(pid, SIGTERM);
        break;
    default:
        return false;
    }

    return true;
}


/************************************************
  HalProvider
 ************************************************/
HalProvider::HalProvider(QObject *parent):
    PowerProvider(parent)
{
}


HalProvider::~HalProvider() = default;


bool HalProvider::canAction(Power::Action action) const
{
    Q_UNUSED(action)
    return false;
}


bool HalProvider::doAction(Power::Action action)
{
    Q_UNUSED(action)
    return false;
}


/************************************************
  CustomProvider
 ************************************************/
CustomProvider::CustomProvider(QObject *parent):
    PowerProvider(parent),
    mSettings(QL1SV("power"))
{
}

CustomProvider::~CustomProvider() = default;

bool CustomProvider::canAction(Power::Action action) const
{
    switch (action)
    {
    case Power::PowerShutdown:
        return mSettings.contains(QL1SV("shutdownCommand"));

    case Power::PowerReboot:
        return mSettings.contains(QL1SV("rebootCommand"));

    case Power::PowerHibernate:
        return mSettings.contains(QL1SV("hibernateCommand"));

    case Power::PowerSuspend:
        return mSettings.contains(QL1SV("suspendCommand"));

    case Power::PowerLogout:
        return mSettings.contains(QL1SV("logoutCommand"));

    case Power::PowerMonitorOff:
        if (QGuiApplication::platformName() == QStringLiteral("xcb"))
            return mSettings.contains(QL1SV("monitorOffCommand"));
        else if (QGuiApplication::platformName() == QStringLiteral("wayland"))
            return mSettings.contains(QL1SV("monitorOffCommand_wayland"));
        else
            return false;

    case Power::PowerShowLeaveDialog:
        return mSettings.contains(QL1SV("showLeaveDialogCommand"));

    default:
        return false;
    }
}

bool CustomProvider::doAction(Power::Action action)
{
    QString command;

    switch(action)
    {
    case Power::PowerShutdown:
        command = mSettings.value(QL1SV("shutdownCommand")).toString();
        break;

    case Power::PowerReboot:
        command = mSettings.value(QL1SV("rebootCommand")).toString();
        break;

    case Power::PowerHibernate:
        command = mSettings.value(QL1SV("hibernateCommand")).toString();
        break;

    case Power::PowerSuspend:
        command = mSettings.value(QL1SV("suspendCommand")).toString();
        break;

    case Power::PowerLogout:
        command = mSettings.value(QL1SV("logoutCommand")).toString();
        break;

    case Power::PowerMonitorOff:
        if (QGuiApplication::platformName() == QStringLiteral("xcb"))
            command = mSettings.value(QL1SV("monitorOffCommand")).toString();
        else if (QGuiApplication::platformName() == QStringLiteral("wayland"))
            command = mSettings.value(QL1SV("monitorOffCommand_wayland")).toString();
        break;

    case Power::PowerShowLeaveDialog:
        command = mSettings.value(QL1SV("showLeaveDialogCommand")).toString();
        break;

    default:
        return false;
    }

    QStringList args = QProcess::splitCommand(command);
    if (args.isEmpty())
        return false;

    QProcess process;
    process.setProgram(args.takeFirst());
    process.setArguments(args);
    return process.startDetached();
}
