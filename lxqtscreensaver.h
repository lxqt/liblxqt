/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
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

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include "lxqtglobals.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui/QAction>
#endif

class QProcess;

namespace LxQt
{

class LXQT_API ScreenSaver : public QObject
{
    Q_OBJECT

public:
    ScreenSaver(QObject * parent=0);

    QList<QAction*> availableActions();

signals:
    void activated();

public slots:
    void lockScreen();

private:
    QProcess * m_xdgProcess;

private slots:
    void xdgProcess_finished(int err, QProcess::ExitStatus status);

};

} // namespace LxQt
#endif

