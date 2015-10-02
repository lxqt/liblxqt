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

#include "lxqtscreensaver.h"
#include "lxqttranslator.h"

#include <QDebug>
#include <XdgIcon>

using namespace LXQt;

ScreenSaver::ScreenSaver(QObject *parent)
    : QObject(parent),
    messageBox(QMessageBox::Warning, tr("Screen Saver Error"), QString())
{
    mProcess.setProgram("xscreensaver-command");
    mProcess.setArguments(QStringList() << "-lock");

    connect(&mProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onError(QProcess::ProcessError)));
    connect(&mProcess, &QProcess::readyReadStandardOutput, this, &ScreenSaver::onReadyRead);
    connect(&mProcess, &QProcess::readyReadStandardError, this, &ScreenSaver::onReadyRead);
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
    if (mProcess.state() == QProcess::NotRunning)
        mProcess.start();
}

void ScreenSaver::onError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart)
    {
        messageBox.setText(tr("An error occurred starting screensaver. "
                              "Ensure you have xscreensaver installed and running."));
        if (messageBox.isHidden())
            messageBox.show();

        // only emit on FailedToStart because it will
        // be emitted by onReadyRead otherwise
        emit done();
    }
}

void ScreenSaver::onReadyRead()
{
    QString output;
    output = mProcess.readAllStandardOutput();
    if (output.isEmpty())
        output = mProcess.readAllStandardError();

    if (output.contains("locking"))
        emit activated();

    else if (output.contains("already locked"))
        qDebug() << "LXQt Screen Saver: already locked";

    else if (output.contains("no screensaver is running"))
    {
        messageBox.setText(tr("An error occurred starting screensaver. "
                              "Ensure you have xscreensaver running."));
        if (messageBox.isHidden())
            messageBox.show();
    }

    emit done();
}
