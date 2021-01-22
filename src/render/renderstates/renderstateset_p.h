/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_RENDERSTATE_H
#define QT3DRENDER_RENDER_RENDERSTATE_H

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

#include <Qt3DRender/private/genericstate_p.h>
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/statevariant_p.h>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderState;

namespace Render {

class RenderState;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderStateSet
{
public:
    RenderStateSet();
    ~RenderStateSet();

    template<typename GenericState>
    void addState(const GenericState &state)
    {
        addState(StateVariant::fromValue(state));
    }

    /**
     * @brief changeCost - metric of cost to change to this state-set from
     * a candidate previous state-set. This is used to find an optimal
     * ordering of state-sets when sending draw commands.
     * @param previousState
     * @return
     */
    int changeCost(RenderStateSet* previousState);

    StateMaskSet stateMask() const;
    void merge(const RenderStateSet *other);

    const QVector<StateVariant>& states() const noexcept { return m_states; }
    QVector<StateVariant>& states() noexcept { return m_states; }

    bool canAddStateOfType(StateMask type) const;

    /**
     * @brief contains - check if this set contains a matching piece of state
     * @param ds
     * @return
     */
    bool contains(const StateVariant &ds) const;

private:
    bool hasStateOfType(StateMask type) const;
    bool allowMultipleStatesOfType(StateMask type) const;

private:
    StateMaskSet m_stateMask;
    QVector<StateVariant> m_states;
};

template<>
Q_3DRENDERSHARED_PRIVATE_EXPORT void RenderStateSet::addState<StateVariant>(const StateVariant &state);

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERSTATE_H
