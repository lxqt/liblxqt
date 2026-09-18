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
 * This program or library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "driver/libbacklight_backend.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "driver/libbacklight_backend.c"
#ifdef __cplusplus
}
#endif

#include "linuxbackend.h"
#include <QProcess>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <QLatin1StringView>

using namespace Qt::Literals::StringLiterals;

namespace LXQt {

LinuxBackend::LinuxBackend(QObject *parent):VirtualBackEnd(parent)
{
    maxBacklight = lxqt_backlight_backend_get_max();
    backlightProcess = nullptr;
    if( isBacklightAvailable() ) {
        char *driver = lxqt_backlight_backend_get_driver();
        fileSystemWatcher = new QFileSystemWatcher(this);
        fileSystemWatcher->addPath("/sys/class/backlight/%1/actual_brightness"_L1.arg(QL1SV(driver)));
        fileSystemWatcher->addPath("/sys/class/backlight/%1/brightness"_L1.arg(QL1SV(driver)));
        fileSystemWatcher->addPath("/sys/class/backlight/%1/bl_power"_L1.arg(QL1SV(driver)));
        free(driver);
        actualBacklight = lxqt_backlight_backend_get();
        connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &LinuxBackend::fileSystemChanged);
    }
}

LinuxBackend::~LinuxBackend()
{
    closeBacklightStream();
    // Reap deterministically at teardown without ever blocking unboundedly on
    // the main thread (a bounded wait is acceptable at panel shutdown).
    if( backlightProcess != nullptr ) {
        backlightProcess->kill();
        backlightProcess->waitForFinished(1000);
    }
}

int LinuxBackend::getBacklight()
{
    actualBacklight = lxqt_backlight_backend_get();
    return actualBacklight;
}

int LinuxBackend::getMaxBacklight()
{
    return maxBacklight;
}

bool LinuxBackend::isBacklightAvailable()
{
    return maxBacklight > 0;
}

bool LinuxBackend::isBacklightOff()
{
    return lxqt_backlight_is_backlight_off() > 0;
}

void LinuxBackend::setBacklight(int value)
{
    if( ! isBacklightAvailable() )
        return;
    if( backlightProcess == nullptr || backlightProcess->state() != QProcess::ProcessState::Running ) {
        // (Re)start the privileged helper. It is started via pkexec so that
        // only the helper - not this process - runs as root, and is managed
        // with QProcess so that closing/reaping it never blocks the main
        // (GUI) thread in a synchronous waitpid(), which is what used to
        // freeze the panel completely.
        backlightProcess = new QProcess(this);
        backlightProcess->setProcessChannelMode(QProcess::ForwardedChannels);
        connect(backlightProcess, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError error) {
                if( error == QProcess::FailedToStart )
                    qWarning() << "Linux backlight: lxqt-backlight_backend failed to start";
            });
        connect(backlightProcess, &QProcess::finished, this,
            [this](int, QProcess::ExitStatus) {
                backlightProcess->deleteLater();
                backlightProcess = nullptr;
            });
        backlightProcess->start(QStringLiteral("pkexec"),
            QStringList() << QStringLiteral("lxqt-backlight_backend") << QStringLiteral("--stdin"));
        // Close stream after 60 seconds
        QTimer::singleShot(60000, this, SLOT(closeBacklightStream()));
    }
    if( backlightProcess != nullptr && backlightProcess->state() == QProcess::ProcessState::Running ) {
        // Normalise the value (to work around a QSlider issue)
        value = qBound(0, value, maxBacklight);
        QTextStream out(backlightProcess);
        out << value << '\n';
        out.flush();
    }
}

void LinuxBackend::closeBacklightStream()
{
    if( backlightProcess != nullptr ) {
        // Signal end of input to the helper and ask it to exit. Neither call
        // below waits on the main thread for the helper to actually finish:
        // QProcess reaps the child with the event loop, and the finished()
        // slot connected in setBacklight() releases the object.
        backlightProcess->closeWriteChannel();
        backlightProcess->terminate();
        // Safety net: force the helper to close if it is still alive a moment
        // later (non-blocking).
        QTimer::singleShot(2000, this, [this]() {
            if( backlightProcess != nullptr )
                backlightProcess->kill();
        });
    }
}

void LinuxBackend::fileSystemChanged(const QString & /*path*/)
{
    int value = actualBacklight;
    if( value != getBacklight() ) {
        Q_EMIT backlightChanged(actualBacklight);
    }
}

} // namespace LXQt
