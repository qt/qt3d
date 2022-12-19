// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;

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
            return self->parentRenderPassFilter()->matchAny().size();
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
    using qt_size_type = qsizetype;

    auto appendFunction = [](QQmlListProperty<QParameter> *list, QParameter *v) {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self) {
            self->parentRenderPassFilter()->addParameter(v);
        }
    };
    auto countFunction = [](QQmlListProperty<QParameter> *list) -> qt_size_type {
        auto self = qobject_cast<Quick3DRenderPassFilter *>(list->object);
        if (self)
            return self->parentRenderPassFilter()->parameters().size();
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
