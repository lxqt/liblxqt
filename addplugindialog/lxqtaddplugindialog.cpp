/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
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
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>
#include <QTextBrowser>
#include <QAbstractTextDocumentLayout>
#include <QLineEdit>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <qt5xdg/XdgIcon>
#include <QtWidgets>
#else
#include <qtxdg/XdgIcon>
#include <QtGui/QListWidgetItem>
#include <QtGui/QItemDelegate>
#include <QtGui/QTextDocument>
#include <QtGui/QIcon>
#endif

#include "lxqttranslator.h"

using namespace LxQt;

#define SEARCH_ROLE  Qt::UserRole
#define INDEX_ROLE   SEARCH_ROLE+1

class HtmlDelegate : public QStyledItemDelegate
{
public:
    HtmlDelegate(const QSize iconSize, QObject* parent = 0):
        QStyledItemDelegate(parent),
        mIconSize(iconSize)
    {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
    QSize mIconSize;
};


/************************************************

 ************************************************/
void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);
    QIcon icon = options.icon;

    options.text = "";
    options.icon = QIcon();
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // Draw icon ................................
    QSize iconSize = icon.actualSize(mIconSize);
    painter->translate(options.rect.left(), options.rect.top());
    QRect iconRect = QRect(4, 4, iconSize.width(), iconSize.height());

    icon.paint(painter, iconRect);

    doc.setTextWidth(options.rect.width() - iconRect.right() - 10);

    // shift text right to make icon visible
    painter->translate(iconRect.right() + 8, 0);
    QRect clip(0, 0, options.rect.width()-iconRect.right()- 10, options.rect.height());


    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;
    // set text color to red for selected item
    if (option.state & QStyle::State_Selected)
    {
        ctx.palette.setColor(QPalette::Text, option.palette.color(QPalette::HighlightedText));
    }
    ctx.clip = clip;
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}


/************************************************

 ************************************************/
QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QSize iconSize = QIcon().actualSize(mIconSize);
    QRect iconRect = QRect(4, 4, iconSize.width(), iconSize.height());

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width()-iconRect.right()- 10);
    return QSize(doc.idealWidth(), doc.size().height() + 8);
}


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

void AddPluginDialog::setPluginsInUse(const PluginInfoList pluginsInUse)
{
    mPluginsInUse = pluginsInUse;
    init();
}

void AddPluginDialog::init()
{
    QListWidget* pluginList = ui->pluginList;
    
    pluginList->clear();

    QIcon fallIco = XdgIcon::fromTheme("preferences-plugin");

    for (int i=0; i< mPlugins.length(); ++i)
    {
        bool count = mPluginsInUse.count(mPlugins.at(i));
        QString countStr;
        if (count)
            countStr = tr("(%1 active)").arg(count);

        const PluginInfo &plugin = mPlugins.at(i);
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
