// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qspritegrid.h"
#include "qspritegrid_p.h"

#include <Qt3DRender/qabstracttexture.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DExtras {

QSpriteGridPrivate::QSpriteGridPrivate()
    : QAbstractSpriteSheetPrivate()
    , m_numColumns(1)
    , m_numRows(1)
{
}

int QSpriteGridPrivate::maxIndex() const
{
    return m_numColumns * m_numRows - 1;
}

void QSpriteGridPrivate::updateSizes()
{
    Q_Q(QSpriteGrid);
    if (m_texture && m_numColumns && m_numRows) {
        m_textureSize = QSize(m_texture->width(), m_texture->height());
        m_cellSize = QSizeF((float) m_texture->width() / m_numColumns, (float) m_texture->height() / m_numRows);
    } else {
        m_textureSize = QSize();
        m_cellSize = QSizeF();
    }

    if (m_cellSize.isEmpty() || m_numColumns == 0 || m_numRows == 0) {
        if (m_currentIndex != -1) {
            m_currentIndex = -1;
            emit q->currentIndexChanged(m_currentIndex);
        }
        m_textureTransform.setToIdentity();
        emit q->textureTransformChanged(m_textureTransform);
        return;
    }

    if (m_currentIndex == -1) {
        m_currentIndex = 0;
        emit q->currentIndexChanged(m_currentIndex);
    }
    updateTransform();
}

void QSpriteGridPrivate::updateTransform()
{
    Q_Q(QSpriteGrid);
    const float xScale = (float) m_cellSize.width() / (float) m_textureSize.width();
    const float yScale = (float) m_cellSize.height() / (float) m_textureSize.height();

    const int currentRow = m_currentIndex / m_numColumns;
    const int currentColumn = m_currentIndex % m_numColumns;
    const float xTranslate = currentColumn * xScale;
    const float yTranslate = currentRow * yScale;

    m_textureTransform.setToIdentity();
    m_textureTransform(0, 0) = xScale;
    m_textureTransform(1, 1) = yScale;
    m_textureTransform(0, 2) = xTranslate;
    m_textureTransform(1, 2) = yTranslate;
    emit q->textureTransformChanged(m_textureTransform);
}

/*!
    Constructs a new QSpriteGrid instance with parent object \a parent.
 */
QSpriteGrid::QSpriteGrid(QNode *parent)
    : QAbstractSpriteSheet(*new QSpriteGridPrivate, parent)
{
}

QSpriteGrid::~QSpriteGrid()
{
}

int QSpriteGrid::rows() const
{
    Q_D(const QSpriteGrid);
    return d->m_numRows;
}

void QSpriteGrid::setRows(int rows)
{
    Q_D(QSpriteGrid);
    if (d->m_numRows != rows) {
        d->m_numRows = rows;
        d->updateSizes();
        emit rowsChanged(rows);
    }
}

int QSpriteGrid::columns() const
{
    Q_D(const QSpriteGrid);
    return d->m_numColumns;
}

void QSpriteGrid::setColumns(int columns)
{
    Q_D(QSpriteGrid);
    if (d->m_numColumns != columns) {
        d->m_numColumns = columns;
        d->updateSizes();
        emit columnsChanged(columns);
    }
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qspritegrid.cpp"
