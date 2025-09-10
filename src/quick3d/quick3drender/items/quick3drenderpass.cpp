// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = QFilterKey;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *filterKey) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        rPass->parentRenderPass()->addFilterKey(filterKey);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->filterKeys().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->filterKeys().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        const auto keys = rPass->parentRenderPass()->filterKeys();
        for (QFilterKey *c : keys)
            rPass->parentRenderPass()->removeFilterKey(c);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QRenderState> Quick3DRenderPass::renderStateList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QRenderState;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *state) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        rPass->parentRenderPass()->addRenderState(state);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->renderStates().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->renderStates().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        const auto states = rPass->parentRenderPass()->renderStates();
        for (QRenderState *s : states)
            rPass->parentRenderPass()->removeRenderState(s);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QParameter> Quick3DRenderPass::parameterList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        rPass->parentRenderPass()->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->parameters().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->parameters().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        const auto parameters = rPass->parentRenderPass()->parameters();
        for (QParameter *p : parameters)
            rPass->parentRenderPass()->removeParameter(p);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3drenderpass_p.cpp"
