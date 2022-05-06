/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2022 The Qt Company Ltd and/or its subsidiary(-ies).
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
******************************************************************************/

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QFilterKey;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *filterKey) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        rPass->parentRenderPass()->addFilterKey(filterKey);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->filterKeys().count();
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QRenderState;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *state) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        rPass->parentRenderPass()->addRenderState(state);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->renderStates().count();
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        rPass->parentRenderPass()->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
        return rPass->parentRenderPass()->parameters().count();
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
