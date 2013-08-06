/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLATTRIBUTESET_H
#define QGLATTRIBUTESET_H

#include <Qt3D/qt3dglobal.h>
#include <Qt3D/qglnamespace.h>

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

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
    return isValidAttr(attr) ? ((m_attrs & (1U << flag)) != 0) : false;
}

inline void QGLAttributeSet::insert(QGL::VertexAttribute attr)
{
    quint32 flag = quint32(attr);
    if (isValidAttr(attr))
        m_attrs |= (1U << flag);
}

inline void QGLAttributeSet::remove(QGL::VertexAttribute attr)
{
    quint32 flag = quint32(attr);
    if (isValidAttr(attr))
        m_attrs &= ~(1U << flag);
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

#endif
