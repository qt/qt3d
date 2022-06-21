// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qscissortest.h"
#include "qscissortest_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QScissorTest
    \brief The QScissorTest class discards fragments that fall outside of a
    certain rectangular portion of the screen.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    A QScissorTest class enables scissor test, which discards fragments outside
    the rectangular area of the screen specified by the left, bottom, width and
    height properties.
 */

/*!
    \qmltype ScissorTest
    \brief The ScissorTest type discards fragments that fall outside of a
    certain rectangular portion of the screen.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QScissorTest
    \inherits RenderState

    A ScissorTest type enables scissor test, which discards fragments outside
    the rectangular area of the screen specified by the left, bottom, width and
    height properties.
 */

/*!
    \qmlproperty int ScissorTest::left
    Holds the left coordinate of the scissor box.
*/

/*!
    \qmlproperty int ScissorTest::bottom
    Holds the bottom coordinate of the scissor box.
*/

/*!
    \qmlproperty int ScissorTest::width
    Holds the width of the scissor box.
*/

/*!
    \qmlproperty int ScissorTest::height
    Holds the height of the scissor box.
*/

/*!
    \property QScissorTest::left
    Holds the left coordinate of the scissor box.
*/

/*!
    \property QScissorTest::bottom
    Holds the bottom coordinate of the scissor box.
*/

/*!
    \property QScissorTest::width
    Holds the width of the scissor box.
*/

/*!
    \property QScissorTest::height
    Holds the height of the scissor box.
*/

/*!
    The constructor creates a new QScissorTest::QScissorTest instance with the
    specified \a parent
 */
QScissorTest::QScissorTest(QNode *parent)
    : QRenderState(*new QScissorTestPrivate, parent)
{
}

/*! \internal */
QScissorTest::~QScissorTest()
{
}

int QScissorTest::left() const
{
    Q_D(const QScissorTest);
    return d->m_left;
}

void QScissorTest::setLeft(int left)
{
    Q_D(QScissorTest);
    if (d->m_left != left) {
        d->m_left = left;
        emit leftChanged(left);
    }
}

int QScissorTest::bottom() const
{
    Q_D(const QScissorTest);
    return d->m_bottom;
}

void QScissorTest::setBottom(int bottom)
{
    Q_D(QScissorTest);
    if (d->m_bottom != bottom) {
        d->m_bottom = bottom;
        emit bottomChanged(bottom);
    }
}

int QScissorTest::width() const
{
    Q_D(const QScissorTest);
    return d->m_width;
}

void QScissorTest::setWidth(int width)
{
    Q_D(QScissorTest);
    if (d->m_width != width) {
        d->m_width = width;
        emit widthChanged(width);
    }
}

int QScissorTest::height() const
{
    Q_D(const QScissorTest);
    return d->m_height;
}

void QScissorTest::setHeight(int height)
{
    Q_D(QScissorTest);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged(height);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qscissortest.cpp"
