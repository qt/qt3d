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

#include <Qt3DQuickRender/private/quick3drenderpass_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DRenderPass::Quick3DRenderPass(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QFilterKey> Quick3DRenderPass::filterKeyList()
{
    return QQmlListProperty<QFilterKey>(this, 0,
                                         &Quick3DRenderPass::appendFilterKey,
                                         &Quick3DRenderPass::filterKeysCount,
                                         &Quick3DRenderPass::filterKeyAt,
                                         &Quick3DRenderPass::clearFilterKey);
}

QQmlListProperty<QRenderState> Quick3DRenderPass::renderStateList()
{
    return QQmlListProperty<QRenderState>(this, 0,
                                          &Quick3DRenderPass::appendRenderState,
                                          &Quick3DRenderPass::renderStateCount,
                                          &Quick3DRenderPass::renderStateAt,
                                          &Quick3DRenderPass::clearRenderStates);
}

QQmlListProperty<QParameter> Quick3DRenderPass::parameterList()
{
    return QQmlListProperty<QParameter>(this, 0,
                                        &Quick3DRenderPass::appendParameter,
                                        &Quick3DRenderPass::parametersCount,
                                        &Quick3DRenderPass::parameterAt,
                                        &Quick3DRenderPass::clearParameterList);
}

void Quick3DRenderPass::appendFilterKey(QQmlListProperty<QFilterKey> *list, QFilterKey *filterKey)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addFilterKey(filterKey);
}

QFilterKey *Quick3DRenderPass::filterKeyAt(QQmlListProperty<QFilterKey> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->filterKeys().at(index);
}

int Quick3DRenderPass::filterKeysCount(QQmlListProperty<QFilterKey> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->filterKeys().count();
}

void Quick3DRenderPass::clearFilterKey(QQmlListProperty<QFilterKey> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    const auto keys = rPass->parentRenderPass()->filterKeys();
    for (QFilterKey *c : keys)
        rPass->parentRenderPass()->removeFilterKey(c);
}

void Quick3DRenderPass::appendRenderState(QQmlListProperty<QRenderState> *list, QRenderState *state)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addRenderState(state);
}

QRenderState *Quick3DRenderPass::renderStateAt(QQmlListProperty<QRenderState> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->renderStates().at(index);
}

int Quick3DRenderPass::renderStateCount(QQmlListProperty<QRenderState> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->renderStates().count();
}

void Quick3DRenderPass::clearRenderStates(QQmlListProperty<QRenderState> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    const auto states = rPass->parentRenderPass()->renderStates();
    for (QRenderState *s : states)
        rPass->parentRenderPass()->removeRenderState(s);
}

void Quick3DRenderPass::appendParameter(QQmlListProperty<QParameter> *list, QParameter *param)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addParameter(param);
}

QParameter *Quick3DRenderPass::parameterAt(QQmlListProperty<QParameter> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->parameters().at(index);
}

int Quick3DRenderPass::parametersCount(QQmlListProperty<QParameter> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->parameters().count();
}

void Quick3DRenderPass::clearParameterList(QQmlListProperty<QParameter> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    const auto parameters = rPass->parentRenderPass()->parameters();
    for (QParameter *p : parameters)
        rPass->parentRenderPass()->removeParameter(p);
}


} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3drenderpass_p.cpp"
