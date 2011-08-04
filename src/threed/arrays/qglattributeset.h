/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLATTRIBUTESET_H
#define QGLATTRIBUTESET_H

#include "qt3dglobal.h"
#include "qglnamespace.h"
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class Q_QT3D_EXPORT QGLAttributeSet
{
public:
    QGLAttributeSet() : m_attrs(0) {}

    bool isEmpty() const { return !m_attrs; }
    void clear() { m_attrs = 0; }

    bool contains(QGL::VertexAttribute attr) const;
    void insert(QGL::VertexAttribute attr);
    void remove(QGL::VertexAttribute attr);

    QList<QGL::VertexAttribute> toList() const;
    static QGLAttributeSet fromList(const QList<QGL::VertexAttribute> &list);

    void unite(const QGLAttributeSet &other);
    void intersect(const QGLAttributeSet &other);
    void subtract(const QGLAttributeSet &other);

    bool operator==(const QGLAttributeSet &other) const;
    bool operator!=(const QGLAttributeSet &other) const;

private:
    bool isValidAttr(QGL::VertexAttribute attr) const;

    quint32 m_attrs;
};

inline bool QGLAttributeSet::isValidAttr(QGL::VertexAttribute attr) const
{
    int a = int(attr);
    return (a > -1 && a < 32);
}

inline bool QGLAttributeSet::contains(QGL::VertexAttribute attr) const
{
    quint32 flag = quint32(attr);
    return isValidAttr(attr) ? ((m_attrs & (((quint32)1) << flag)) != 0) : false;
}

inline void QGLAttributeSet::insert(QGL::VertexAttribute attr)
{
    quint32 flag = quint32(attr);
    if (isValidAttr(attr))
        m_attrs |= (((quint32)1) << flag);
}

inline void QGLAttributeSet::remove(QGL::VertexAttribute attr)
{
    quint32 flag = quint32(attr);
    if (isValidAttr(attr))
        m_attrs &= ~(((quint32)1) << flag);
}

inline void QGLAttributeSet::unite(const QGLAttributeSet &other)
{
    m_attrs |= other.m_attrs;
}

inline void QGLAttributeSet::intersect(const QGLAttributeSet &other)
{
    m_attrs &= other.m_attrs;
}

inline void QGLAttributeSet::subtract(const QGLAttributeSet &other)
{
    m_attrs &= ~(other.m_attrs);
}

inline bool QGLAttributeSet::operator==(const QGLAttributeSet &other) const
{
    return m_attrs == other.m_attrs;
}

inline bool QGLAttributeSet::operator!=(const QGLAttributeSet &other) const
{
    return m_attrs != other.m_attrs;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
