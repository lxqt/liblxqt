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

#include <QProcess>
#include "lxqtscreensaver.h"
#include "lxqttranslator.h"

#include <XdgIcon>
#include <QMessageBox>
#include <QAction>
#include <QPointer>
#include <QProcess>
#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS

namespace LXQt {

class ScreenSaverPrivate
{
    Q_DECLARE_TR_FUNCTIONS(LXQt::ScreenSaver);
    Q_DECLARE_PUBLIC(ScreenSaver)
    ScreenSaver* const q_ptr;

public:
    ScreenSaverPrivate(ScreenSaver *q) : q_ptr(q) {};

    void _l_xdgProcess_finished(int, QProcess::ExitStatus);
    QPointer<QProcess> m_xdgProcess;
};

void ScreenSaverPrivate::_l_xdgProcess_finished(int err, QProcess::ExitStatus status)
{
    // http://portland.freedesktop.org/xdg-utils-1.1.0-rc1/scripts/xdg-screensaver

    Q_Q(ScreenSaver);
    if (err == 0)
        emit q->activated();
    else
    {
        QMessageBox *box = new QMessageBox;
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->setIcon(QMessageBox::Warning);

        if (err == 1)
        {
            box->setWindowTitle(tr("Screen Saver Error"));
            box->setText(tr("An error occurred starting screensaver. "
                           "Syntax error in xdg-screensaver arguments."));
        }
        else if (err == 3)
        {
            box->setWindowTitle(tr("Screen Saver Activation Error"));
            box->setText(tr("An error occurred starting screensaver. "
                           "Ensure you have xscreensaver installed and running."));
        }
        else if (err == 4)
        {
            box->setWindowTitle(tr("Screen Saver Activation Error"));
            box->setText(tr("An error occurred starting screensaver. "
                           "Action 'activate' failed. "
                           "Ensure you have xscreensaver installed and running."));
        }
        else
        {
            box->setWindowTitle(tr("Screen Saver Activation Error"));
            box->setText(tr("An error occurred starting screensaver. "
                           "Unknown error - undocumented return value from xdg-screensaver: %1.")
                        .arg(err));
        }

        box->exec();
    }

    emit q->done();
}


ScreenSaver::ScreenSaver(QObject * parent)
    : QObject(parent),
      d_ptr(new ScreenSaverPrivate(this))
{
    Q_D(ScreenSaver);
    d->m_xdgProcess = new QProcess(this);
    connect(d->m_xdgProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(_l_xdgProcess_finished(int,QProcess::ExitStatus)));
}

ScreenSaver::~ScreenSaver()
{
    delete d_ptr;
}

QList<QAction*> ScreenSaver::availableActions()
{
    QList<QAction*> ret;
    QAction * act;

    act = new QAction(XdgIcon::fromTheme("system-lock-screen", "lock"), tr("Lock Screen"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(lockScreen()));
    ret.append(act);

    return ret;
}

void ScreenSaver::lockScreen()
{
    Q_D(ScreenSaver);
    d->m_xdgProcess->start("xdg-screensaver", QStringList() << "lock");
}

} // namespace LXQt

#include "moc_lxqtscreensaver.cpp"
