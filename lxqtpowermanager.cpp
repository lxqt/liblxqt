/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
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

#include "lxqtpowermanager.h"
#include "lxqtpower/lxqtpower.h"
#include <QDBusInterface>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>
#include "lxqttranslator.h"
#include "lxqtglobals.h"
#include "lxqtsettings.h"
#include <XdgIcon>

namespace LxQt {

class LXQT_API MessageBox: public QMessageBox
{
public:
    explicit MessageBox(QWidget *parent = 0): QMessageBox(parent) {}

    static QWidget *parentWidget()
    {
        QWidgetList widgets = QApplication::topLevelWidgets();

        if (widgets.count())
            return widgets.at(0);
        else
            return 0;
    }

    static bool question(const QString& title, const QString& text)
    {
        MessageBox msgBox(parentWidget());
        msgBox.setWindowTitle(title);
        msgBox.setText(text);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        return (msgBox.exec() == QMessageBox::Yes);
    }


    static void warning(const QString& title, const QString& text)
    {
        QMessageBox::warning(parentWidget(), tr("LxQt Power Manager Error"), tr("Hibernate failed."));
    }


protected:
    virtual void resizeEvent(QResizeEvent* event)
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        move((screen.width()  - this->width()) / 2,
             (screen.height() - this->height()) / 2);

    }
};

PowerManager::PowerManager(QObject * parent, bool skipWarning)
    : QObject(parent),
        m_skipWarning(skipWarning)
{
    m_power = new Power(this);
//    connect(m_power, SIGNAL(suspendFail()), this, SLOT(suspendFailed()));
//    connect(m_power, SIGNAL(hibernateFail()), this, SLOT(hibernateFailed()));
//    connect(m_power, SIGNAL(monitoring(const QString &)),
//            this, SLOT(monitoring(const QString&)));

    QString sessionConfig(getenv("LXQT_SESSION_CONFIG"));
    Settings settings(sessionConfig.isEmpty() ? "session" : sessionConfig);
    m_skipWarning = settings.value("leave_confirmation").toBool() ? false : true;
}

PowerManager::~PowerManager()
{
//    delete m_power;
}

QList<QAction*> PowerManager::availableActions()
{
    QList<QAction*> ret;
    QAction * act;

    // TODO/FIXME: icons
    if (m_power->canHibernate())
    {
        act = new QAction(XdgIcon::fromTheme("system-suspend-hibernate"), tr("Hibernate"), this);
        act->setData(QVariant("LxQt_PowerManager_Hibernate"));
        connect(act, SIGNAL(triggered()), this, SLOT(hibernate()));
        ret.append(act);
    }

    if (m_power->canSuspend())
    {
        act = new QAction(XdgIcon::fromTheme("system-suspend"), tr("Suspend"), this);
        act->setData(QVariant("LxQt_PowerManager_Suspend"));
        connect(act, SIGNAL(triggered()), this, SLOT(suspend()));
        ret.append(act);
    }

    if (m_power->canReboot())
    {
        act = new QAction(XdgIcon::fromTheme("system-reboot"), tr("Reboot"), this);
        act->setData(QVariant("LxQt_PowerManager_Reboot"));
        connect(act, SIGNAL(triggered()), this, SLOT(reboot()));
        ret.append(act);
    }

    if (m_power->canShutdown())
    {
        act = new QAction(XdgIcon::fromTheme("system-shutdown"), tr("Shutdown"), this);
        act->setData(QVariant("LxQt_PowerManager_Shutdown"));
        connect(act, SIGNAL(triggered()), this, SLOT(shutdown()));
        ret.append(act);
    }

    if (m_power->canLogout())
    {
        act = new QAction(XdgIcon::fromTheme("system-log-out"), tr("Logout"), this);
        act->setData(QVariant("LxQt_PowerManager_Logout"));
        connect(act, SIGNAL(triggered()), this, SLOT(logout()));
        ret.append(act);
    }

    return ret;
}


void PowerManager::suspend()
{
     if (m_skipWarning ||
         MessageBox::question(tr("LxQt Session Suspend"),
                              tr("Do you want to really suspend your computer?<p>Suspends the computer into a low power state. System state is not preserved if the power is lost.")))
    {
        m_power->suspend();
    }
}

void PowerManager::hibernate()
{
    if (m_skipWarning ||
        MessageBox::question(tr("LxQt Session Hibernate"),
                             tr("Do you want to really hibernate your computer?<p>Hibernates the computer into a low power state. System state is preserved if the power is lost.")))
    {
        m_power->hibernate();
    }
}

void PowerManager::reboot()
{
    if (m_skipWarning ||
        MessageBox::question(tr("LxQt Session Reboot"),
                             tr("Do you want to really restart your computer? All unsaved work will be lost...")))
    {
        m_power->reboot();
    }
}

void PowerManager::shutdown()
{
    if (m_skipWarning ||
        MessageBox::question(tr("LxQt Session Shutdown"),
                             tr("Do you want to really switch off your computer? All unsaved work will be lost...")))
    {
        m_power->shutdown();
    }
}

void PowerManager::logout()
{
    if (m_skipWarning ||
        MessageBox::question(tr("LxQt Session Logout"),
                             tr("Do you want to really logout? All unsaved work will be lost...")))
    {
        m_power->logout();
    }
}

void PowerManager::hibernateFailed()
{
    MessageBox::warning(tr("LxQt Power Manager Error"), tr("Hibernate failed."));
}

void PowerManager::suspendFailed()
{
    MessageBox::warning(tr("LxQt Power Manager Error"), tr("Suspend failed."));
}

} // namespace LxQt
