/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012-2013 Razor team
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

#include <QtCore/QDir>

#include "lxqtapplication.h"
#include "lxqtsettings.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <qt5xdg/XdgIcon>
#include <qt5xdg/XdgDirs>
#else
#include <qtxdg/XdgIcon>
#include <qtxdg/XdgDirs>
#endif

using namespace LxQt;

#define COLOR_DEBUG "\033[32;2m"
#define COLOR_WARN "\033[33;2m"
#define COLOR_CRITICAL "\033[31;1m"
#define COLOR_FATAL "\033[33;1m"
#define COLOR_RESET "\033[0m"

#define QAPP_NAME qApp ? qApp->objectName().toUtf8().constData() : ""

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <QDateTime>
/*! \brief Log qDebug input to file
Used only in pure Debug builds or when is the system environment
variable LXQT_DEBUG set
*/
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void dbgMessageOutput(QtMsgType type, const QMessageLogContext &ctx, const QString & msgStr)
{
    QByteArray msgBuf = msgStr.toUtf8();
    const char* msg = msgBuf.constData();
#else
void dbgMessageOutput(QtMsgType type, const char *msg)
{
#endif
    QDir dir(XdgDirs::configHome().toUtf8() + "/lxqt");
    dir.mkpath(".");

    const char* typestr;
    const char* color;
    switch (type) {
    case QtDebugMsg:
        typestr = "Debug";
        color = COLOR_DEBUG;
        break;
    case QtWarningMsg:
        typestr = "Warning";
        color = COLOR_WARN;
        break;
    case QtFatalMsg:
        typestr = "Fatal";
        color = COLOR_FATAL;
        break;
    default: // QtCriticalMsg
        typestr = "Critical";
        color = COLOR_CRITICAL;
    }

    QByteArray dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    if (isatty(STDERR_FILENO))
        fprintf(stderr, "%s %s(%p) %s: %s%s\n", color, QAPP_NAME, qApp, typestr, msg, COLOR_RESET);
    else
        fprintf(stderr, "%s(%p) %s: %s\n", QAPP_NAME, qApp, typestr, msg);

    FILE *f = fopen(dir.absoluteFilePath("debug.log").toUtf8().constData(), "a+");
    fprintf(f, "%s %s(%p) %s: %s\n", dt.constData(), QAPP_NAME, qApp, typestr, msg);
    fclose(f);

    if (type == QtFatalMsg)
        abort();
}

Application::Application(int &argc, char** argv)
    : QApplication(argc, argv)
{
#ifdef DEBUG

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qInstallMessageHandler(dbgMessageOutput);
#else
    qInstallMsgHandler(dbgMessageOutput);
#endif

#else //  DEBUG
    if (!qgetenv("LXQT_DEBUG").isNull())
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qInstallMessageHandler(dbgMessageOutput);
#else
        qInstallMsgHandler(dbgMessageOutput);
#endif

#endif // DEBUG

    XdgIcon::setThemeName(Settings::globalSettings()->value("icon_theme").toString());
    setWindowIcon(QIcon(QString(LXQT_SHARE_DIR) + "/graphics/lxqt_logo.png"));
    connect(Settings::globalSettings(), SIGNAL(lxqtThemeChanged()), this, SLOT(updateTheme()));
    updateTheme();
}


void Application::updateTheme()
{
    QString styleSheetKey = QFileInfo(applicationFilePath()).fileName();
    setStyleSheet(lxqtTheme.qss(styleSheetKey));
    emit themeChanged();
}
