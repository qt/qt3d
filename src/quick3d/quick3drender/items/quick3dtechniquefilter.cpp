// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/quick3dtechniquefilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DTechniqueFilter::Quick3DTechniqueFilter(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QFilterKey> Quick3DTechniqueFilter::matchList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QFilterKey;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *criterion) {
        Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        if (filter) {
            criterion->setParent(filter->parentTechniqueFilter());
            filter->parentTechniqueFilter()->addMatch(criterion);
        }
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        if (filter)
            return filter->parentTechniqueFilter()->matchAll().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        if (filter)
            return filter->parentTechniqueFilter()->matchAll().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        if (filter) {
            const auto criteria = filter->parentTechniqueFilter()->matchAll();
            for (QFilterKey *criterion : criteria)
                filter->parentTechniqueFilter()->removeMatch(criterion);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QParameter> Quick3DTechniqueFilter::parameterList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        techniqueFilter->parentTechniqueFilter()->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        return techniqueFilter->parentTechniqueFilter()->parameters().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        return techniqueFilter->parentTechniqueFilter()->parameters().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        const auto parameters = techniqueFilter->parentTechniqueFilter()->parameters();
        for (QParameter *p : parameters)
            techniqueFilter->parentTechniqueFilter()->removeParameter(p);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dtechniquefilter_p.cpp"
