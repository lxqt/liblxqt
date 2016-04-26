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


#include "lxqtpageselectwidget.h"
#include <QDebug>
#include <QStyledItemDelegate>
#include <QEvent>
#include <QScrollBar>
#include <QApplication>

using namespace LXQt;

class PageSelectWidgetItemDelegate: public QStyledItemDelegate
{
public:
    explicit PageSelectWidgetItemDelegate(PageSelectWidget *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    PageSelectWidget* mView;
};


/************************************************

 ************************************************/
PageSelectWidgetItemDelegate::PageSelectWidgetItemDelegate(PageSelectWidget *parent):
    QStyledItemDelegate(parent),
    mView(parent)
{
}


/************************************************

 ************************************************/
QSize PageSelectWidgetItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    //all items should have unified width
    QStyle * style = option.widget ? option.widget->style() : QApplication::style();
    //Note: this margin logic follows code in QCommonStylePrivate::viewItemLayout()
    const int margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, option.widget) + 1;
    //considering the icon size too
    size.setWidth(qMax(mView->maxTextWidth(), option.decorationSize.width()));
    size.rwidth() += 2 * margin;
    return size;
}



/************************************************

 ************************************************/
PageSelectWidget::PageSelectWidget(QWidget *parent) :
    QListWidget(parent)
    , mMaxTextWidth(0)
{
    setSelectionRectVisible(false);
    setViewMode(IconMode);
    setSpacing(2);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    setWordWrap(true);
    setDragEnabled(NoDragDrop);
    setEditTriggers(NoEditTriggers);
    setTextElideMode(Qt::ElideNone);
    setContentsMargins(0, 0, 0, 0);

    setItemDelegate(new PageSelectWidgetItemDelegate(this));
    connect(model(), &QAbstractItemModel::rowsInserted, this, &PageSelectWidget::updateMaxTextWidth);
    connect(model(), &QAbstractItemModel::rowsRemoved, this, &PageSelectWidget::updateMaxTextWidth);
    connect(model(), &QAbstractItemModel::dataChanged, this, &PageSelectWidget::updateMaxTextWidth);
}


/************************************************

 ************************************************/
PageSelectWidget::~PageSelectWidget()
{
}

/************************************************

 ************************************************/
int PageSelectWidget::maxTextWidth() const
{
    return mMaxTextWidth;
}

/************************************************

 ************************************************/
QSize PageSelectWidget::viewportSizeHint() const
{
    const int spacing2 = spacing() * 2;
    QSize size{sizeHintForColumn(0) + spacing2, (sizeHintForRow(0) + spacing2) * count()};
    if (verticalScrollBar()->isVisible())
        size.rwidth() += verticalScrollBar()->sizeHint().width();
    return size;
}

/************************************************

 ************************************************/
QSize PageSelectWidget::minimumSizeHint() const
{
    return QSize{0, 0};
}

/************************************************

 ************************************************/
void PageSelectWidget::updateMaxTextWidth()
{
    for(int i = count() - 1; 0 <= i; --i)
    {
        const QRect r = fontMetrics().boundingRect(QRect{},  Qt::AlignLeft | Qt::TextWordWrap, item(i)->text());
        mMaxTextWidth = qMax(mMaxTextWidth, r.width());
    }
}

/************************************************

 ************************************************/
bool PageSelectWidget::event(QEvent * event)
{
    if (QEvent::StyleChange == event->type())
        updateMaxTextWidth();

    return QListWidget::event(event);
}
