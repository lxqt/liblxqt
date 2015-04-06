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

#include "lxqtaddplugindialog.h"
#include "ui_lxqtaddplugindialog.h"
#include "lxqthtmldelegate.h"
#include <QDebug>
#include <QLineEdit>
#include <XdgIcon>
#include <QListWidgetItem>
#include <QIcon>
#include <QMenu>

#include "lxqttranslator.h"

using namespace LxQt;

#define SEARCH_ROLE  Qt::UserRole
#define INDEX_ROLE   SEARCH_ROLE+1

/************************************************

 ************************************************/
bool pluginDescriptionLessThan(const PluginInfo &p1, const PluginInfo &p2)
{
    int cmp = QString::compare(p1.name(), p2.name());
    if (cmp != 0)
        return cmp < 0;

    return p1.comment() < p2.comment();
}


/************************************************

 ************************************************/
AddPluginDialog::AddPluginDialog(const QStringList& desktopFilesDirs,
                                 const QString &serviceType,
                                 const QString &nameFilter,
                                 QWidget *parent):
    QDialog(parent),
    ui(new Ui::AddPluginDialog),
    mTimerId(0)
{
    Translator::translateLibrary("liblxqt");
    ui->setupUi(this);

    mPlugins = PluginInfo::search(desktopFilesDirs, serviceType, nameFilter);
    qSort(mPlugins.begin(), mPlugins.end(), pluginDescriptionLessThan);

    ui->pluginList->setItemDelegate(new HtmlDelegate(QSize(32, 32), ui->pluginList));
    ui->pluginList->setContextMenuPolicy(Qt::CustomContextMenu);

    init();

    connect(ui->pluginList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(emitPluginSelected()));
    connect(ui->pluginList, SIGNAL(itemSelectionChanged()), this, SLOT(toggleButtonsState()));
    connect(ui->pluginList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(ui->searchEdit, SIGNAL(textEdited(QString)), this, SLOT(searchEditTexChanged(QString)));
    connect(ui->addButton, SIGNAL(clicked(bool)), this, SLOT(emitPluginSelected()));
    connect(ui->manageButton, &QPushButton::clicked, [this] (bool) { showContextMenu(ui->pluginList->visualRect(ui->pluginList->currentIndex()).center()); });
}

void AddPluginDialog::setPluginsInUse(QList<PluginData> const & pluginsInUse)
{
    mPluginsInfo = pluginsInUse;
    Q_FOREACH (PluginData const & data, mPluginsInfo)
    {
        if (!mPluginsInUseAmount.contains(data.typeId))
            mPluginsInUseAmount[data.typeId] = 0;
        mPluginsInUseAmount[data.typeId]++;
    }

    init();
}

void AddPluginDialog::setPluginsInUse(const QStringList pluginsInUseIDs)
{
    QList<PluginData> plugins;
    Q_FOREACH (QString const & id, pluginsInUseIDs)
    {
        plugins << PluginData(id, 0, 0);
    }
    setPluginsInUse(plugins);
}

void AddPluginDialog::init()
{
    QListWidget* pluginList = ui->pluginList;

    const int curr_item = 0 < pluginList->count() ? pluginList->currentRow() : 0;
    pluginList->clear();

    QIcon fallIco = XdgIcon::fromTheme("preferences-plugin");

    int pluginCount = mPlugins.length();
    for (int i = 0; i < pluginCount; ++i)
    {
        const PluginInfo &plugin = mPlugins.at(i);

        QString countStr;
        int amount = mPluginsInUseAmount[plugin.id()];
            if (amount)
                countStr = tr("(%1 active)").arg(amount);

        QListWidgetItem* item = new QListWidgetItem(ui->pluginList);
        item->setText(QString("<b>%1 %2</b><br>\n%3\n").arg(plugin.name(), countStr, plugin.comment()));
        item->setIcon(plugin.icon(fallIco));
        item->setData(INDEX_ROLE, i);
        item->setData(SEARCH_ROLE, QString("%1 %2 %3 %4").arg(
                        plugin.name(),
                        plugin.comment(),
                        plugin.value("Name").toString(),
                        plugin.value("Comment").toString()
                       )
                     );
    }

    if (pluginCount > 0)
        ui->pluginList->setCurrentRow(curr_item < pluginCount ? curr_item : pluginCount - 1);
}

/************************************************

 ************************************************/
void AddPluginDialog::pluginAdded(PluginData const & plugin)
{
    mPluginsInfo << plugin;
    if (!mPluginsInUseAmount.contains(plugin.typeId))
        mPluginsInUseAmount[plugin.typeId] = 0;
    mPluginsInUseAmount[plugin.typeId]++;
    init();
}

void AddPluginDialog::pluginAdded(const QString &id)
{
    pluginAdded(PluginData(id, 0, 0));
}

/************************************************

 ************************************************/
void AddPluginDialog::pluginRemoved(PluginData const & plugin)
{
    for (QList<PluginData>::iterator i = mPluginsInfo.begin(), i_e = mPluginsInfo.end(); i_e != i; ++i)
    {
        if (plugin.plugin == i->plugin)
        {
            mPluginsInfo.erase(i);
            break;
        }
    }
    mPluginsInUseAmount[plugin.typeId]--;
    init();
}

void AddPluginDialog::pluginRemoved(const QString &id)
{
    for (QList<PluginData>::iterator i = mPluginsInfo.begin(), i_e = mPluginsInfo.end(); i_e != i; ++i)
    {
        if (id == i->typeId)
        {
            mPluginsInfo.erase(i);
            break;
        }
    }
    mPluginsInUseAmount[id]--;
    init();
}

/************************************************

 ************************************************/
AddPluginDialog::~AddPluginDialog()
{
    delete ui;
}


/************************************************

 ************************************************/
void AddPluginDialog::searchEditTexChanged(const QString& text)
{
    if (mTimerId)
        killTimer(mTimerId);

    mTimerId = startTimer(SEARCH_DELAY);
}


/************************************************

 ************************************************/
void AddPluginDialog::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == mTimerId)
    {
        killTimer(mTimerId);
        QListWidget* pluginList = ui->pluginList;
        QString s = ui->searchEdit->text();

        for (int i=0; i < pluginList->count(); ++i)
        {
            QListWidgetItem* item = pluginList->item(i);
            item->setHidden(! item->data(SEARCH_ROLE).toString().contains(s, Qt::CaseInsensitive));
            if (item->isSelected() && item->isHidden())
                pluginList->setCurrentRow(-1);
        }
    }
}



/************************************************

 ************************************************/
void AddPluginDialog::emitPluginSelected()
{
    QListWidget* pluginList = ui->pluginList;
    if (pluginList->currentItem() && pluginList->currentItem()->isSelected())
    {
        PluginInfo plugin = mPlugins.at(pluginList->currentItem()->data(INDEX_ROLE).toInt());
        emit pluginSelected(plugin);
    }
}

/************************************************

 ************************************************/
void AddPluginDialog::toggleButtonsState()
{
    QListWidgetItem * item = ui->pluginList->currentItem();
    ui->addButton->setEnabled(item && item->isSelected());

    ui->manageButton->setEnabled(nullptr != item && mPluginsInfo.end()
            != std::find_if(mPluginsInfo.begin(), mPluginsInfo.end(), [this, item] (PluginData const & d) -> bool
                {
                    return mPlugins.at(item->data(INDEX_ROLE).toInt()).id() == d.typeId;
                })
            );
}

/************************************************

 ************************************************/
void AddPluginDialog::showContextMenu(const QPoint& pos)
{
    QListWidgetItem * item = ui->pluginList->currentItem();
    QMenu menu;
    menu.addAction(XdgIcon::fromTheme(QLatin1String("list-add")), tr("Add widget")
            , this, SLOT(emitPluginSelected()));
    int count = 0;
    if (item)
    {
        PluginInfo plugin = mPlugins.at(item->data(INDEX_ROLE).toInt());
        for (QList<PluginData>::const_iterator i = mPluginsInfo.begin(), i_e = mPluginsInfo.end(); i_e != i; ++i)
        {
            if (plugin.id() == i->typeId && !i->pluginMenu.isNull())
            {
                ++count;
                menu.addSeparator();
                QAction* heading = new QAction(tr("Plugin #%1").arg(count), &menu);
                menu.addSeparator();
                heading->setEnabled(false);
                menu.addAction(heading);
                Q_FOREACH(QAction * a, i->pluginMenu->actions())
                {
                    if (!a->isSeparator())
                        menu.addAction(a);
                }
            }
        }

    }
    menu.exec(ui->pluginList->mapToGlobal(pos));
}
