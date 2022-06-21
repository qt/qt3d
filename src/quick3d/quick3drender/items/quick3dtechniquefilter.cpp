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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        techniqueFilter->parentTechniqueFilter()->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
        return techniqueFilter->parentTechniqueFilter()->parameters().count();
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
