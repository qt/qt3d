/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include <Qt3DQuickRender/private/quick3drenderpassfilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DRenderPassFilter::Quick3DRenderPassFilter(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QFilterKey> Quick3DRenderPassFilter::includeList()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    auto appendFunction = [](QQmlListProperty<QFilterKey> *list, QFilterKey *v) {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self) {
            v->setParent(self->parentRenderPassFilter());
            self->parentRenderPassFilter()->addMatch(v);
        }
    };
    auto countFunction = [](QQmlListProperty<QFilterKey> *list) -> qt_size_type {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self)
            return self->parentRenderPassFilter()->matchAny().count();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<QFilterKey> *list, qt_size_type index) -> QFilterKey * {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self)
            return self->parentRenderPassFilter()->matchAny().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<QFilterKey> *list) {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self) {
            const auto l = self->parentRenderPassFilter()->matchAny();
            for (auto *v : l)
                self->parentRenderPassFilter()->removeMatch(v);
        }
    };

    return QQmlListProperty<QFilterKey>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QParameter> Quick3DRenderPassFilter::parameterList()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    auto appendFunction = [](QQmlListProperty<QParameter> *list, QParameter *v) {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self) {
            self->parentRenderPassFilter()->addParameter(v);
        }
    };
    auto countFunction = [](QQmlListProperty<QParameter> *list) -> qt_size_type {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self)
            return self->parentRenderPassFilter()->parameters().count();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<QParameter> *list, qt_size_type index) -> QParameter * {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self)
            return self->parentRenderPassFilter()->parameters().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<QParameter> *list) {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self) {
            const auto l = self->parentRenderPassFilter()->parameters();
            for (auto *v : l)
                self->parentRenderPassFilter()->removeParameter(v);
        }
    };

    return QQmlListProperty<QParameter>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3drenderpassfilter_p.cpp"
