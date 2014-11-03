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

    init();

    connect(ui->pluginList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(emitPluginSelected()));
    connect(ui->pluginList, SIGNAL(itemSelectionChanged()), this, SLOT(toggleAddButtonState()));
    connect(ui->searchEdit, SIGNAL(textEdited(QString)), this, SLOT(searchEditTexChanged(QString)));
    connect(ui->addButton, SIGNAL(clicked(bool)), this, SLOT(emitPluginSelected()));
}

void AddPluginDialog::setPluginsInUse(const QStringList pluginsInUseIDs)
{
    Q_FOREACH (QString id, pluginsInUseIDs)
    {
        if (!mPluginsInUseAmount.contains(id))
            mPluginsInUseAmount[id] = 0;
        mPluginsInUseAmount[id]++;
    }

    init();
}

void AddPluginDialog::init()
{
    QListWidget* pluginList = ui->pluginList;

    pluginList->clear();

    QIcon fallIco = XdgIcon::fromTheme("preferences-plugin");

    for (int i=0; i< mPlugins.length(); ++i)
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

    ui->addButton->setEnabled(false);
}

/************************************************

 ************************************************/
void AddPluginDialog::pluginAdded(const QString &id)
{
    if (!mPluginsInUseAmount.contains(id))
        mPluginsInUseAmount[id] = 0;
    mPluginsInUseAmount[id]++;
    init();
}

/************************************************

 ************************************************/
void AddPluginDialog::pluginRemoved(const QString &id)
{
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
void AddPluginDialog::toggleAddButtonState()
{
    ui->addButton->setEnabled(ui->pluginList->currentItem() && ui->pluginList->currentItem()->isSelected());
}
