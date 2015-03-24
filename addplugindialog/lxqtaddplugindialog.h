/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
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


#ifndef LXQTADDPLUGINDIALOG_H
#define LXQTADDPLUGINDIALOG_H

#include <QDialog>
#include "lxqtplugininfo.h"
#include "lxqtglobals.h"

#define SEARCH_DELAY 125

namespace Ui {
    class AddPluginDialog;
}

class QMenu;


namespace LxQt
{

/*! Information about existing/running plugin
 */
struct PluginData
{
    PluginData(QString const & _typeId, QObject * const _plugin, QMenu * const _pluginMenu)
        : typeId(_typeId), plugin(_plugin), pluginMenu(_pluginMenu) {}

    QString typeId; //!< plugin Id - name of desktop file (w/o .desktop)
    QObject* plugin; //!< plugin instance (used just for instance identification)
    QSharedPointer<QMenu> pluginMenu; //!< menu(actions) to show for this instance of plugin
};

/*! The AddPluginDialog class provides a dialog that allow users to add plugins.
 */
class LXQT_API AddPluginDialog : public QDialog
{
    Q_OBJECT

public:
    /*! Constructs a dialog with the given parent that initially displays
       PluginInfo objects for the matched files in the directories
      @param desktopFilesDirs - list of the scanned directories names.
      @param serviceType - type of the plugin, for example "LxQtPanel/Plugin".
      @param nameFilter  - wildcard filter that understands * and ? wildcards. */
    AddPluginDialog(const QStringList& desktopFilesDirs,
                    const QString& serviceType,
                    const QString& nameFilter="*",
                    QWidget *parent = 0);

    ~AddPluginDialog();

    /*! Set information of existing/running plugin instances
     */
    void setPluginsInUse(QList<PluginData> const & pluginsInUse);
    /*! Obsolete, use setPluginsInUse(QList<PluginData> const & pluginsInUse) instead
     */
    void setPluginsInUse(const QStringList pluginsInUseIDs);

signals:
    void pluginSelected(const LxQt::PluginInfo &plugin);

protected:
    void timerEvent(QTimerEvent* event);

private:
    void init();
    Ui::AddPluginDialog *ui;
    PluginInfoList mPlugins;
    int mTimerId;

    // store the amount of instances of the plugins using their ids
    QHash<QString, int> mPluginsInUseAmount;
    QList<PluginData> mPluginsInfo;

public slots:
    void pluginAdded(PluginData const & plugin);
    void pluginRemoved(PluginData const & plugin);
    /*! Obsolete, use pluginAdded(PluginData const & plugin) instead
     */
    void pluginAdded(const QString &id);
    /*! Obsolete, use pluginRemoved(PluginData const & plugin) instead
     */
    void pluginRemoved(const QString &id);

private slots:
    void emitPluginSelected();
    void searchEditTexChanged(const QString& text);
    void toggleAddButtonState();
    void showContextMenu(const QPoint& pos);
};

} // namecpase LxQt

#endif // LXQTADDPLUGINDIALOG_H
