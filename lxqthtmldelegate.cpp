/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Paulo Lieuthier <paulolieuthier@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "lxqthtmldelegate.h"
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>

using namespace LxQt;


HtmlDelegate::HtmlDelegate(const QSize iconSize, QObject* parent) :
    QStyledItemDelegate(parent),
    mIconSize(iconSize)
{
}

HtmlDelegate::~HtmlDelegate()
{
}

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

    // icon size
    QSize iconSize = icon.actualSize(mIconSize);
    QRect iconRect = QRect(8, 8, iconSize.width(), iconSize.height());

    // set doc size
    doc.setTextWidth(options.rect.width() - iconRect.right() - 8);

    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // paint icon
    painter->translate(options.rect.left(), options.rect.top());
    icon.paint(painter, iconRect);

    // shift text right to make icon visible
    painter->translate(iconRect.right() + 8, 0);
    QRect clip(0, 0, options.rect.width() - iconRect.right() - 8, options.rect.height());
    painter->setClipRect(clip);

    // set text color to red for selected item
    QAbstractTextDocumentLayout::PaintContext ctx;
    if (option.state & QStyle::State_Selected)
    {
        QPalette::ColorGroup colorGroup = (option.state & QStyle::State_Active) ? QPalette::Active : QPalette::Inactive;
        ctx.palette.setColor(QPalette::Text, option.palette.color(colorGroup, QPalette::HighlightedText));
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

    QSize iconSize = options.icon.actualSize(mIconSize);
    QRect iconRect = QRect(8, 8, iconSize.width(), iconSize.height());

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width() - iconRect.right() - 8);
    return QSize(options.rect.width(), doc.size().height() + 8);
}
