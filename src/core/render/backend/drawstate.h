/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_DRAWSTATE_H
#define QT3D_DRAWSTATE_H

#include <QList>
#include <QSet>

namespace Qt3D {

class QGraphicsContext;

enum StateMask
{
    BlendStateMask          = 1 << 0,
    StencilWriteStateMask   = 1 << 1,
    StencilTestStateMask    = 1 << 2,
    ScissorStateMask        = 1 << 3,
    DepthTestStateMask      = 1 << 4,
    DepthWriteStateMask     = 1 << 5,
    CullFaceStateMask       = 1 << 6,
    AlphaTestMask           = 1 << 7,
    FrontFaceStateMask      = 1 << 8
};

typedef quint64 StateMaskSet;

// winuser.h has a #define called DrawState. Let's get rid of it
#if defined(DrawState)
#undef DrawState
#endif

class DrawState
{
public:
    virtual void apply(QGraphicsContext* gc) const = 0;

    virtual StateMaskSet mask() const = 0;

protected:
};

class DrawStateSet
{
public:
    DrawStateSet();

    void addState(DrawState* ds);

    /**
     * @brief changeCost - metric of cost to change to this state-set from
     * a candidate previous state-set. This is used to find an optimal
     * ordering of state-sets when sending draw commands.
     * @param previousState
     * @return
     */
    int changeCost(DrawStateSet* previousState);

    void apply(QGraphicsContext* gc);

    StateMaskSet stateMask() const;

    void resetMasked(StateMaskSet maskOfStatesToReset, QGraphicsContext* gc);
private:
    /**
     * @brief contains - check if this set contains a matching piece of state
     * @param ds
     * @return
     */
    bool contains(DrawState* ds) const;

    QSet<DrawState*> m_states;

    StateMaskSet m_stateMask;

    DrawStateSet* m_cachedPrevious;
    QList<DrawState*> m_cachedDeltaStates;
};

} // namespace Qt3D

#endif // QT3D_DRAWSTATE_H
