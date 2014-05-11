/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#include "lxqtgridlayout.h"
#include <QtCore/QDebug>
#include <math.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QWidget>
#else
#include <QtGui/QWidget>
#endif

using namespace LxQt;

class LxQt::GridLayoutPrivate
{
public:
    GridLayoutPrivate();

    QList<QLayoutItem*> mItems;
    int mRowCount;
    int mColumnCount;
    GridLayout::Direction mDirection;

    bool mIsValid;
    QSize mCellSizeHint;
    QSize mCellMaxSize;
    int mVisibleCount;
    GridLayout::Stretch mStretch;


    void updateCache();
    int rows() const;
    int cols() const;
    QSize mPrefCellMinSize;
    QSize mPrefCellMaxSize;
};


/************************************************

 ************************************************/
GridLayoutPrivate::GridLayoutPrivate()
{
    mColumnCount = 0;
    mRowCount = 0;
    mDirection = GridLayout::LeftToRight;
    mIsValid = false;
    mVisibleCount = 0;
    mStretch = GridLayout::StretchHorizontal | GridLayout::StretchVertical;
    mPrefCellMinSize = QSize(0,0);
    mPrefCellMaxSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}


/************************************************

 ************************************************/
void GridLayoutPrivate::updateCache()
{
    mCellSizeHint = QSize(0, 0);
    mCellMaxSize = QSize(0, 0);
    mVisibleCount = 0;

    for (int i=0; i<mItems.count(); ++i)
    {
        QLayoutItem *item = mItems.at(i);
        if (!item->widget() || item->widget()->isHidden())
            continue;

        int h = qBound(item->minimumSize().height(),
                       item->sizeHint().height(),
                       item->maximumSize().height());

        int w = qBound(item->minimumSize().width(),
                       item->sizeHint().width(),
                       item->maximumSize().width());

        mCellSizeHint.rheight() = qMax(mCellSizeHint.height(), h);
        mCellSizeHint.rwidth()  = qMax(mCellSizeHint.width(), w);

        mCellMaxSize.rheight() = qMax(mCellMaxSize.height(), item->maximumSize().height());
        mCellMaxSize.rwidth()  = qMax(mCellMaxSize.width(), item->maximumSize().width());
        mVisibleCount++;

#if 0
        qDebug() << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
        qDebug() << "item.min" << item->minimumSize().width();
        qDebug() << "item.sz " << item->sizeHint().width();
        qDebug() << "item.max" << item->maximumSize().width();
        qDebug() << "w h" << w << h;
        qDebug() << "wid.sizeHint" << item->widget()->sizeHint();
        qDebug() << "mCellSizeHint:" << mCellSizeHint;
        qDebug() << "mCellMaxSize: " << mCellMaxSize;
        qDebug() << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
#endif

    }
    mCellSizeHint.rwidth() = qBound(mPrefCellMinSize.width(),  mCellSizeHint.width(),  mPrefCellMaxSize.width());
    mCellSizeHint.rheight()= qBound(mPrefCellMinSize.height(), mCellSizeHint.height(), mPrefCellMaxSize.height());
    mIsValid = !mCellSizeHint.isEmpty();
}


/************************************************

 ************************************************/
int GridLayoutPrivate::rows() const
{
    if (mRowCount)
        return mRowCount;

    if (!mColumnCount)
        return 1;

    return ceil(mVisibleCount * 1.0 / mColumnCount);
}


/************************************************

 ************************************************/
int GridLayoutPrivate::cols() const
{
    if (mColumnCount)
        return mColumnCount;

    int rows = mRowCount;
    if (!rows)
        rows = 1;

    return ceil(mVisibleCount * 1.0 / rows);
}



/************************************************

 ************************************************/
GridLayout::GridLayout(QWidget *parent):
    QLayout(parent),
    d_ptr(new GridLayoutPrivate())
{
}


/************************************************

 ************************************************/
GridLayout::~GridLayout()
{
    delete d_ptr;
}


/************************************************

 ************************************************/
void GridLayout::addItem(QLayoutItem *item)
{
    d_ptr->mItems.append(item);
}


/************************************************

 ************************************************/
QLayoutItem *GridLayout::itemAt(int index) const
{
    Q_D(const GridLayout);
    if (index < 0 || index >= d->mItems.count())
        return 0;

    return d->mItems.at(index);
}


/************************************************

 ************************************************/
QLayoutItem *GridLayout::takeAt(int index)
{
    Q_D(GridLayout);
    if (index < 0 || index >= d->mItems.count())
        return 0;

    QLayoutItem *item = d->mItems.takeAt(index);
    return item;
}


/************************************************

 ************************************************/
int GridLayout::count() const
{
    Q_D(const GridLayout);
    return d->mItems.count();
}


/************************************************

 ************************************************/
void GridLayout::invalidate()
{
    Q_D(GridLayout);
    d->mIsValid = false;
    QLayout::invalidate();
}


/************************************************

 ************************************************/
int GridLayout::rowCount() const
{
    Q_D(const GridLayout);
    return d->mRowCount;
}


/************************************************

 ************************************************/
void GridLayout::setRowCount(int value)
{
    Q_D(GridLayout);
    if (d->mRowCount != value)
    {
        d->mRowCount = value;
        invalidate();
    }
}


/************************************************

 ************************************************/
int GridLayout::columnCount() const
{
    Q_D(const GridLayout);
    return d->mColumnCount;
}


/************************************************

 ************************************************/
void GridLayout::setColumnCount(int value)
{
    Q_D(GridLayout);
    if (d->mColumnCount != value)
    {
        d->mColumnCount = value;
        invalidate();
    }
}


/************************************************

 ************************************************/
GridLayout::Direction GridLayout::direction() const
{
    Q_D(const GridLayout);
    return d->mDirection;
}


/************************************************

 ************************************************/
void GridLayout::setDirection(GridLayout::Direction value)
{
    Q_D(GridLayout);
    if (d->mDirection != value)
    {
        d->mDirection = value;
        invalidate();
    }
}

/************************************************

 ************************************************/
GridLayout::Stretch GridLayout::stretch() const
{
    Q_D(const GridLayout);
    return d->mStretch;
}

/************************************************

 ************************************************/
void GridLayout::setStretch(Stretch value)
{
    Q_D(GridLayout);
    if (d->mStretch != value)
    {
        d->mStretch = value;
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::moveItem(int from, int to)
{
    Q_D(GridLayout);
    d->mItems.move(from, to);
    invalidate();
}


/************************************************

 ************************************************/
QSize GridLayout::cellMinimumSize() const
{
    Q_D(const GridLayout);
    return d->mPrefCellMinSize;
}


/************************************************

 ************************************************/
void GridLayout::setCellMinimumSize(QSize minSize)
{
    Q_D(GridLayout);
    if (d->mPrefCellMinSize != minSize)
    {
        d->mPrefCellMinSize = minSize;
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellMinimumHeight(int value)
{
    Q_D(GridLayout);
    if (d->mPrefCellMinSize.height() != value)
    {
        d->mPrefCellMinSize.setHeight(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellMinimumWidth(int value)
{
    Q_D(GridLayout);
    if (d->mPrefCellMinSize.width() != value)
    {
        d->mPrefCellMinSize.setWidth(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
QSize GridLayout::cellMaximumSize() const
{
    Q_D(const GridLayout);
    return d->mPrefCellMaxSize;
}


/************************************************

 ************************************************/
void GridLayout::setCellMaximumSize(QSize maxSize)
{
    Q_D(GridLayout);
    if (d->mPrefCellMaxSize != maxSize)
    {
        d->mPrefCellMaxSize = maxSize;
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellMaximumHeight(int value)
{
    Q_D(GridLayout);
    if (d->mPrefCellMaxSize.height() != value)
    {
        d->mPrefCellMaxSize.setHeight(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellMaximumWidth(int value)
{
    Q_D(GridLayout);
    if (d->mPrefCellMaxSize.width() != value)
    {
        d->mPrefCellMaxSize.setWidth(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellFixedSize(QSize size)
{
    Q_D(GridLayout);
    if (d->mPrefCellMinSize != size ||
        d->mPrefCellMaxSize != size)
    {
        d->mPrefCellMinSize = size;
        d->mPrefCellMaxSize = size;
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellFixedHeight(int value)
{
    Q_D(GridLayout);
    if (d->mPrefCellMinSize.height() != value ||
        d->mPrefCellMaxSize.height() != value)
    {
        d->mPrefCellMinSize.setHeight(value);
        d->mPrefCellMaxSize.setHeight(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
void GridLayout::setCellFixedWidth(int value)
{
    Q_D(GridLayout);
    if (d->mPrefCellMinSize.width() != value ||
        d->mPrefCellMaxSize.width() != value)
    {
        d->mPrefCellMinSize.setWidth(value);
        d->mPrefCellMaxSize.setWidth(value);
        invalidate();
    }
}


/************************************************

 ************************************************/
QSize GridLayout::sizeHint() const
{
    Q_D(const GridLayout);

    if (!d->mIsValid)
        const_cast<GridLayoutPrivate*>(d)->updateCache();

    return QSize(d->cols() * d->mCellSizeHint.width(),
                 d->rows() * d->mCellSizeHint.height());
}


/************************************************

 ************************************************/
void GridLayout::setGeometry(const QRect &geometry)
{
    Q_D(GridLayout);

    if (!d->mIsValid)
        d->updateCache();

    int y = geometry.top();
    int x = geometry.left();

    // For historical reasons QRect::right returns left() + width() - 1
    // and QRect::bottom() returns top() + height() - 1;
    // So we use left() + height() and top() + height()
    //
    // http://qt-project.org/doc/qt-4.8/qrect.html

    int maxX = geometry.left() + geometry.width();
    int maxY = geometry.top() + geometry.height();

    int itemWidth;
    if (d->mStretch.testFlag(StretchHorizontal))
    {
        itemWidth = geometry.width() * 1.0 / d->cols();
        itemWidth = qMin(itemWidth, d->mCellMaxSize.width());
    }
    else
    {
        itemWidth = d->mCellSizeHint.width();
    }

    itemWidth = qBound(d->mPrefCellMinSize.width(), itemWidth, d->mPrefCellMaxSize.width());

    int itemHeight;
    if (d->mStretch.testFlag(StretchVertical))
    {
        itemHeight = geometry.height() * 1.0 / d->rows();
        itemHeight = qMin(itemHeight, d->mCellMaxSize.height());
    }
    else
    {
        itemHeight = d->mCellSizeHint.height();
    }

    itemHeight = qBound(d->mPrefCellMinSize.height(), itemHeight, d->mPrefCellMaxSize.height());


#if 0
    qDebug() << "** GridLayout::setGeometry *******************************";
    qDebug() << "Geometry:" << geometry;
    qDebug() << "CellSize:" << d->mCellSizeHint;
    qDebug() << "Constraints:" << "min" << d->mPrefCellMinSize << "max" << d->mPrefCellMaxSize;
    qDebug() << "Count" << count();
    qDebug() << "Cols:" << d->cols() << "(" << d->mColumnCount << ")";
    qDebug() << "Rows:" << d->rows() << "(" << d->mRowCount << ")";
    qDebug() << "Stretch:" << "h:" << (d->mStretch.testFlag(StretchHorizontal)) << " v:" << (d->mStretch.testFlag(StretchVertical));
    qDebug() << "Item:" << "h:" << itemHeight << " w:" << itemWidth;
#endif

    if (d->mDirection == LeftToRight)
    {       
        foreach(QLayoutItem *item, d->mItems)
        {
            if (!item->widget() || item->widget()->isHidden())
                continue;

            if (x + itemWidth > maxX)
            {
                x = geometry.left();
                if (d->mStretch.testFlag(StretchVertical))
                    y += geometry.height() / d->rows();
                else
                    y += itemHeight;

            }

            item->setGeometry(QRect(x, y, itemWidth, itemHeight));
            x += itemWidth;
        }
    }
    else
    {
        foreach(QLayoutItem *item, d->mItems)
        {
            if (!item->widget() || item->widget()->isHidden())
                continue;

            if (y + itemHeight > maxY)
            {
                y = geometry.top();
                if (d->mStretch.testFlag(StretchHorizontal))
                    x += geometry.width() / d->rows();
                else
                    x += itemWidth;

            }
            item->setGeometry(QRect(x, y, itemWidth, itemHeight));
            y += itemHeight;
        }
    }
}

