/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_GENERICSTATE_H
#define QT3DRENDER_RENDER_GENERICSTATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRender/private/statemask_p.h>
#include <QList>
#include <QVector3D>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderState;

namespace Render {

class GraphicsContext;

typedef quint64 StateMaskSet;

class RenderStateImpl
{
public:
    virtual ~RenderStateImpl() {}

    virtual void apply(GraphicsContext* gc) const = 0;
    virtual StateMaskSet mask() const = 0;
    virtual bool equalTo(const RenderStateImpl &renderState) const = 0;

    /*!
     * \brief Stored statically?
     *
     * Should each unique RenderStateImpl be stored statically and thus shared
     * between RenderStateNodes?
     * This should be disabled for RenderStateImpls that may have a large
     * number of values, e.g. when they are using arbitrary floats
     * Default: true
     */
    virtual bool isPooledImpl() const Q_DECL_NOTHROW;

    /*!
     * \brief Return pointer to pooled object with given property change
     *
     * For pooled impls: apply property change, get pooled object reflecting
     * new state
     */
    virtual RenderStateImpl *getOrCreateWithPropertyChange(const char *name, const QVariant &value) const = 0;

    static RenderStateImpl *getOrCreateState(const Qt3DRender::QRenderStateCreatedChangeBasePtr change);
    virtual void updateProperty(const char *name, const QVariant &value);
};


template <class State>
State *getOrCreateRenderStateEqualTo(const State &prototype)
{
    static Qt3DCore::QResourceManager<State, int, 16,
            Qt3DCore::ArrayAllocatingPolicy,
            Qt3DCore::ObjectLevelLockingPolicy> manager;
    static int currIndex = 0;

    if (!prototype.isPooledImpl()) {
        State *state = new State();
        *state = prototype;
        return state;
    }

    // find existing state?
    for (int idx = 0; idx < currIndex; ++idx) {
        State *state = manager.lookupResource(idx);
        if (state->equalTo(prototype))
            return state;
    }

    // create new state
    int thisIndex = currIndex++;
    State *state = manager.getOrCreateResource(thisIndex);
    *state = prototype;

    return state;
}

/**
 * CRTP base class, in order to simplify object template construction
 */
template <class StateSetImpl, StateMaskSet Mask>
class MaskedRenderState : public RenderStateImpl
{
public:
    StateMaskSet mask() const Q_DECL_OVERRIDE
    {
        return Mask;
    }
    // apply delta on temporary template, return instance of actual pooled RenderStateImpl
    RenderStateImpl *getOrCreateWithPropertyChange(const char *name, const QVariant &value) const Q_DECL_OVERRIDE
    {
        Q_ASSERT(isPooledImpl());
        StateSetImpl newState = *(static_cast<const StateSetImpl*>(this));
        newState.updateProperty(name, value);
        return getOrCreateRenderStateEqualTo(newState);
    }
    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        return mask() == renderState.mask();
    }
};

template <class StateSetImpl, StateMask mask, typename ... T>
class GenericState : public MaskedRenderState<StateSetImpl, mask>
{
public:

    GenericState *set(T... values)
    {
        m_values = std::tuple<T ...>(values...);
        return this;
    }

    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        const GenericState *other = static_cast<const GenericState*>(&renderState);
        return (other != NULL && other->m_values == m_values);
    }

    static StateMask type()
    {
        return mask;
    }

protected:
    std::tuple<T ...> m_values;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GENERICSTATE_H
