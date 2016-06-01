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
#include <QList>
#include <QVector3D>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderState;

namespace Render {

class GraphicsContext;

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
    FrontFaceStateMask      = 1 << 8,
    DitheringStateMask      = 1 << 9,
    AlphaCoverageStateMask  = 1 << 10,
    PolygonOffsetStateMask  = 1 << 11,
    ColorStateMask          = 1 << 12,
    ClipPlaneMask           = 1 << 13,
    StencilOpMask           = 1 << 14,
    PointSizeMask           = 1 << 15,
    SeamlessCubemapMask     = 1 << 16,
    MSAAEnabledStateMask    = 1 << 17
};

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

    static RenderStateImpl *getOrCreateState(QRenderState *renderState);
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


template <class StateSetImpl, StateMaskSet mask, typename T>
class GenericState1 : public MaskedRenderState<StateSetImpl, mask>
{
public:
    GenericState1 *set(const T& v1)
    {
        m_1 = v1;
        return this;
    }
    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        const GenericState1 *other = static_cast<const GenericState1*>(&renderState);
        return (other != NULL
                && other->m_1 == m_1);
    }

protected:
    T m_1;
};

template <class StateSetImpl, StateMaskSet mask, typename T, typename S>
class GenericState2 : public MaskedRenderState<StateSetImpl, mask>
{
public:
    GenericState2 *set(const T& v1, const S& v2)
    {
        m_1 = v1;
        m_2 = v2;
        return this;
    }
    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        const GenericState2 *other = static_cast<const GenericState2*>(&renderState);
        return (other != NULL
                && other->m_1 == m_1
                && other->m_2 == m_2);
    }

protected:
    T m_1;
    S m_2;
};

template <class StateSetImpl, StateMaskSet mask, typename T, typename S, typename U>
class GenericState3 : public MaskedRenderState<StateSetImpl, mask>
{
public:
    GenericState3 *set(const T& v1, const S& v2, const U& v3)
    {
        m_1 = v1;
        m_2 = v2;
        m_3 = v3;
        return this;
    }
    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        const GenericState3 *other = static_cast<const GenericState3*>(&renderState);
        return (other != NULL
                && other->m_1 == m_1
                && other->m_2 == m_2
                && other->m_3 == m_3);
    }

protected:
    T m_1;
    S m_2;
    U m_3;
};

template <class StateSetImpl, StateMaskSet mask, typename T, typename S, typename U, typename Z>
class GenericState4 : public MaskedRenderState<StateSetImpl, mask>
{
public:
    GenericState4 *set(const T& v1, const S& v2, const U& v3, const Z& v4)
    {
        m_1 = v1;
        m_2 = v2;
        m_3 = v3;
        m_4 = v4;
        return this;
    }
    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        const GenericState4 *other = static_cast<const GenericState4*>(&renderState);
        return (other != NULL
                && other->m_1 == m_1
                && other->m_2 == m_2
                && other->m_3 == m_3
                && other->m_4 == m_4);
    }

protected:
    T m_1;
    S m_2;
    U m_3;
    Z m_4;
};

template <class StateSetImpl, StateMaskSet mask, typename T, typename S, typename U, typename V, typename W, typename Z>
class GenericState6 : public MaskedRenderState<StateSetImpl, mask>
{
public:
    GenericState6 *set(const T& v1, const S& v2, const U& v3, const V& v4, const W& v5, const Z& v6)
    {
        m_1 = v1;
        m_2 = v2;
        m_3 = v3;
        m_4 = v4;
        m_5 = v5;
        m_6 = v6;
        return this;
    }
    bool equalTo(const RenderStateImpl &renderState) const Q_DECL_OVERRIDE
    {
        const GenericState6 *other = static_cast<const GenericState6*>(&renderState);
        return (other != NULL
                && other->m_1 == m_1
                && other->m_2 == m_2
                && other->m_3 == m_3
                && other->m_4 == m_4
                && other->m_5 == m_5
                && other->m_6 == m_6);
    }

protected:
    T m_1;
    S m_2;
    U m_3;
    V m_4;
    W m_5;
    Z m_6;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GENERICSTATE_H
