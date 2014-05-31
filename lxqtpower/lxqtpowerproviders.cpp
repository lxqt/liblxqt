/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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
#include <QDebug>
#include "lxqtnotification.h"
#include <signal.h> // for kill()

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
#define LXQT_PATH         "/LxQtSession"
#define LXQT_INTERFACE    "org.lxqt.session"

#define LXSESSION_SERVICE      "org.lxde.SessionManager"
#define LXSESSION_PATH         "/org/lxde/SessionManager"
#define LXSESSION_INTERFACE    "org.lxde.SessionManager"

#define PROPERTIES_INTERFACE    "org.freedesktop.DBus.Properties"

using namespace LxQt;

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
                                    QObject::tr("QDBusInterface is invalid")+ "\n\n" + service + " " + path + " " + interface + " " + method,
                                    "lxqt-logo.png");
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
                                    msg.errorName() + "\n\n" + msg.errorMessage(),
                                    "lxqt-logo.png");
        }
    }

    // If the method no returns value, we believe that it was successful.
    return msg.arguments().isEmpty() ||
           msg.arguments().first().isNull() ||
           msg.arguments().first().toBool();
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
                                    QObject::tr("QDBusInterface is invalid")+ "\n\n" + service + " " + path + " " + interface + " " + method,
                                    "lxqt-logo.png");
        }
        return false;
    }

    QDBusMessage msg = dbus.call(method, needBoolArg ? QVariant(true) : QVariant());

    if (!msg.errorName().isEmpty())
    {
        printDBusMsg(msg);
        if (errorCheck == PowerProvider::CheckDBUS)
        {
            Notification::notify(
                                    QObject::tr("Power Manager Error (D-BUS call)"),
                                    msg.errorName() + "\n\n" + msg.errorMessage(),
                                    "lxqt-logo.png");
        }
    }

    // If the method no returns value, we believe that it was successful.
    if (msg.arguments().isEmpty() || msg.arguments().first().isNull())
        return true;

    QString response = msg.arguments().first().toString();
    qDebug() << "systemd:" << method << "=" << response;
    return response == "yes" || response == "challenge";
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
//        Notification::notify(QObject::tr("LxQt Power Manager"),
//                                  "lxqt-logo.png",
//                                  QObject::tr("Power Manager Error"),
//                                  QObject::tr("QDBusInterface is invalid")+ "\n\n" + service +" " + path +" " + interface +" " + property);

        return false;
    }

    QDBusMessage msg = dbus.call("Get", dbus.interface(), property);

    if (!msg.errorName().isEmpty())
    {
        printDBusMsg(msg);
//        Notification::notify(QObject::tr("LxQt Power Manager"),
//                                  "lxqt-logo.png",
//                                  QObject::tr("Power Manager Error (Get Property)"),
//                                  msg.errorName() + "\n\n" + msg.errorMessage());
    }

    return !msg.arguments().isEmpty() &&
            msg.arguments().first().value<QDBusVariant>().variant().toBool();
}




/************************************************
 PowerProvider
 ************************************************/
PowerProvider::PowerProvider(QObject *parent):
    QObject(parent)
{
}


PowerProvider::~PowerProvider()
{
}



/************************************************
 UPowerProvider
 ************************************************/
UPowerProvider::UPowerProvider(QObject *parent):
    PowerProvider(parent)
{
}


UPowerProvider::~UPowerProvider()
{
}


bool UPowerProvider::canAction(Power::Action action) const
{
    QString command;
    QString property;
    switch (action)
    {
    case Power::PowerHibernate:
        property = "CanHibernate";
        command  = "HibernateAllowed";
        break;

    case Power::PowerSuspend:
        property = "CanSuspend";
        command  = "SuspendAllowed";
        break;

    default:
        return false;
    }

    return  dbusGetProperty(  // Whether the system is able to hibernate.
                UPOWER_SERVICE,
                UPOWER_PATH,
                PROPERTIES_INTERFACE,
                QDBusConnection::systemBus(),
                property
            )
            &&
            dbusCall( // Check if the caller has (or can get) the PolicyKit privilege to call command.
                UPOWER_SERVICE,
                UPOWER_PATH,
                UPOWER_INTERFACE,
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
        command = "Hibernate";
        break;

    case Power::PowerSuspend:
        command = "Suspend";
        break;

    default:
        return false;
    }


    return dbusCall(UPOWER_SERVICE,
             UPOWER_PATH,
             UPOWER_INTERFACE,
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


ConsoleKitProvider::~ConsoleKitProvider()
{
}


bool ConsoleKitProvider::canAction(Power::Action action) const
{
    QString command;

    switch (action)
    {
    case Power::PowerReboot:
        command = "CanRestart";
        break;

    case Power::PowerShutdown:
        command = "CanStop";
        break;

    default:
        return false;
    }

    return dbusCall(CONSOLEKIT_SERVICE,
                    CONSOLEKIT_PATH,
                    CONSOLEKIT_INTERFACE,
                    QDBusConnection::systemBus(),
                    command,
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
        command = "Restart";
        break;

    case Power::PowerShutdown:
        command = "Stop";
        break;

    default:
        return false;
    }

    return dbusCall(CONSOLEKIT_SERVICE,
             CONSOLEKIT_PATH,
             CONSOLEKIT_INTERFACE,
             QDBusConnection::systemBus(),
             command
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


SystemdProvider::~SystemdProvider()
{
}


bool SystemdProvider::canAction(Power::Action action) const
{
    QString command;

    switch (action)
    {
    case Power::PowerReboot:
        command = "CanReboot";
        break;

    case Power::PowerShutdown:
        command = "CanPowerOff";
        break;

    case Power::PowerSuspend:
        command = "CanSuspend";
        break;

    case Power::PowerHibernate:
        command = "CanHibernate";
        break;

    default:
        return false;
    }

    return dbusCallSystemd(SYSTEMD_SERVICE,
                    SYSTEMD_PATH,
                    SYSTEMD_INTERFACE,
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
        command = "Reboot";
        break;

    case Power::PowerShutdown:
        command = "PowerOff";
        break;

    case Power::PowerSuspend:
        command = "Suspend";
        break;

    case Power::PowerHibernate:
        command = "Hibernate";
        break;

    default:
        return false;
    }

    return dbusCallSystemd(SYSTEMD_SERVICE,
             SYSTEMD_PATH,
             SYSTEMD_INTERFACE,
             QDBusConnection::systemBus(),
             command,
	     true
            );
}


/************************************************
  LxQtProvider
 ************************************************/
LxQtProvider::LxQtProvider(QObject *parent):
    PowerProvider(parent)
{
}


LxQtProvider::~LxQtProvider()
{
}


bool LxQtProvider::canAction(Power::Action action) const
{
    switch (action)
    {
        case Power::PowerLogout:
            // there can be case when razo-session does not run
            return dbusCall(LXQT_SERVICE, LXQT_PATH, LXQT_SERVICE,
                            QDBusConnection::sessionBus(), "canLogout",
                            PowerProvider::DontCheckDBUS);
        default:
            return false;
    }
}


bool LxQtProvider::doAction(Power::Action action)
{
    QString command;

    switch (action)
    {
    case Power::PowerLogout:
        command = "logout";
        break;

    default:
        return false;
    }

    return dbusCall(LXQT_SERVICE,
             LXQT_PATH,
             LXQT_INTERFACE,
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
    pid = (Q_PID)qgetenv("_LXSESSION_PID").toLong();
}


LxSessionProvider::~LxSessionProvider()
{
}


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


HalProvider::~HalProvider()
{
}


bool HalProvider::canAction(Power::Action action) const
{
    return false;
}


bool HalProvider::doAction(Power::Action action)
{
    return false;
}


/************************************************
  CustomProvider
 ************************************************/
CustomProvider::CustomProvider(QObject *parent):
    PowerProvider(parent),
    mSettings("power")
{
}

CustomProvider::~CustomProvider()
{
}

bool CustomProvider::canAction(Power::Action action) const
{
    switch (action)
    {
    case Power::PowerShutdown:
        return mSettings.contains("shutdownCommand");

    case Power::PowerReboot:
        return mSettings.contains("rebootCommand");

    case Power::PowerHibernate:
        return mSettings.contains("hibernateCommand");

    case Power::PowerSuspend:
        return mSettings.contains("suspendCommand");

    case Power::PowerLogout:
        return mSettings.contains("logoutCommand");

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
        command = mSettings.value("shutdownCommand").toString();
        break;

    case Power::PowerReboot:
        command = mSettings.value("rebootCommand").toString();
        break;

    case Power::PowerHibernate:
        command = mSettings.value("hibernateCommand").toString();
        break;

    case Power::PowerSuspend:
        command = mSettings.value("suspendCommand").toString();
        break;

    case Power::PowerLogout:
        command = mSettings.value("logoutCommand").toString();
        break;

    default:
        return false;
    }

    return QProcess::startDetached(command);
}
