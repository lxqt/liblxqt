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

#ifndef LXQTAPPLICATION_H
#define LXQTAPPLICATION_H

#include <QtGui/QApplication>
#include <QtGui/QProxyStyle>

namespace LxQt
{

/*! \brief LXDE-Qt wrapper around QApplication.
 * It loads various LXDE-Qt related stuff by default (window icon, icon theme...)
 *
 * \note This wrapper is intended to be used only inside LXDE-Qt project. Using it
 *       in external application will automatically require linking to various
 *       LXDE-Qt libraries.
 *
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    /*! Construct a LXDE-Qt application object.
     * \param argc standard argc as in QApplication
     * \param argv standard argv as in QApplication
     */
    Application(int &argc, char **argv);
    virtual ~Application() {}

private slots:
    void updateTheme();

signals:
    void themeChanged();
};

#if defined(lxqtApp)
#undef lxqtApp
#endif
#define lxqtApp (static_cast<LxQt::Application *>(qApp))

} // namespace LxQt
#endif // LXQTAPPLICATION_H
