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


#ifndef QGLRENDERORDER_H
#define QGLRENDERORDER_H

#include "qglscenenode.h"
#include "qglrenderstate.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class Q_QT3D_EXPORT QGLRenderOrder
{
public:
    explicit QGLRenderOrder(const QGLSceneNode *node = 0, const QGLRenderState &state = QGLRenderState())
        : m_node(node)
        , m_state(state)
    {
    }
    ~QGLRenderOrder() {}

    uint effectHash() const;
    bool isEqual(const QGLRenderOrder &rhs) const;
    bool isLessThan(const QGLRenderOrder &rhs) const;
    inline bool isValid() const;

    inline bool operator!=(const QGLRenderOrder &rhs) const;
    inline bool operator==(const QGLRenderOrder &rhs) const;
    inline bool operator<(const QGLRenderOrder &rhs) const;

    inline const QGLSceneNode *node() const;
    inline void setState(const QGLRenderState &state);
    inline QGLRenderState state() const;
    inline QGLAbstractEffect *effectiveUserEffect() const;
    inline QGL::StandardEffect effectiveStandardEffect() const;
    inline QGLMaterial *effectiveMaterial() const;
    inline QGLMaterial *effectiveBackMaterial() const;
    inline bool effectiveHasEffect() const;
private:
    const QGLSceneNode *m_node;
    QGLRenderState m_state;
};


inline bool QGLRenderOrder::isValid() const
{
    return m_node;
}

inline bool QGLRenderOrder::operator!=(const QGLRenderOrder &rhs) const
{
    return !isEqual(rhs);
}

inline bool QGLRenderOrder::operator==(const QGLRenderOrder &rhs) const
{
    return isEqual(rhs);
}

inline bool QGLRenderOrder::operator<(const QGLRenderOrder &rhs) const
{
    return isLessThan(rhs);
}

inline const QGLSceneNode *QGLRenderOrder::node() const
{
    return m_node;
}

inline QGLRenderState QGLRenderOrder::state() const
{
    return m_state;
}

inline void QGLRenderOrder::setState(const QGLRenderState &state)
{
    m_state = state;
}

inline QGLAbstractEffect *QGLRenderOrder::effectiveUserEffect() const
{
    QGLAbstractEffect *result = 0;
    if (m_node)
    {
        if (m_node->userEffect())
            result = m_node->userEffect();
        else if (m_state.userEffect())
            result = m_state.userEffect();
    }
    return result;
}

inline QGL::StandardEffect QGLRenderOrder::effectiveStandardEffect() const
{
    QGL::StandardEffect result = QGL::FlatColor;
    if (m_node)
    {
        if (m_node->hasEffect())
            result = m_node->effect();
        else if (m_state.hasEffect())
            result = m_state.standardEffect();
    }
    return result;
}

inline QGLMaterial *QGLRenderOrder::effectiveMaterial() const
{
    QGLMaterial *result = 0;
    if (m_node)
    {
        if (m_node->material())
            result = m_node->material();
        else if (m_state.material())
            result = m_state.material();
    }
    return result;
}

inline QGLMaterial *QGLRenderOrder::effectiveBackMaterial() const
{
    QGLMaterial *result = 0;
    if (m_node)
    {
        if (m_node->backMaterial())
            result = m_node->backMaterial();
        else if (m_state.backMaterial())
            result = m_state.backMaterial();
    }
    return result;
}

inline bool QGLRenderOrder::effectiveHasEffect() const
{
    bool result = false;
    if (m_node)
    {
        if (m_node->hasEffect())
            result = true;
        else
            result = m_state.hasEffect();
    }
    return result;
}

inline uint qHash(const QGLRenderOrder &order)
{
    quint64 result = order.effectHash();
    return result ^ reinterpret_cast<quint64>(order.effectiveMaterial());
}

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLRenderOrder &order);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLRENDERORDER_H
