// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/quick3dtechnique_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DTechnique::Quick3DTechnique(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QRenderPass> Quick3DTechnique::renderPassList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QRenderPass;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *renderPass) {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique)
            technique->parentTechnique()->addRenderPass(renderPass);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique)
            return technique->parentTechnique()->renderPasses().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique)
            return qobject_cast<QRenderPass *>(technique->parentTechnique()->renderPasses().at(index));
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique) {
            const auto passes = technique->parentTechnique()->renderPasses();
            for (QRenderPass *pass : passes)
                technique->parentTechnique()->removeRenderPass(pass);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QParameter> Quick3DTechnique::parameterList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        technique->parentTechnique()->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        return technique->parentTechnique()->parameters().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        return technique->parentTechnique()->parameters().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        const auto parameters = technique->parentTechnique()->parameters();
        for (QParameter *p : parameters)
            technique->parentTechnique()->removeParameter(p);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QFilterKey> Quick3DTechnique::filterKeyList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QFilterKey;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *filterKey) {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique) {
            if (!filterKey->parent())
                filterKey->setParent(technique->parentTechnique());
            technique->parentTechnique()->addFilterKey(filterKey);
        }
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique)
            return technique->parentTechnique()->filterKeys().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique)
            return technique->parentTechnique()->filterKeys().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
        if (technique) {
            const auto keys = technique->parentTechnique()->filterKeys();
            for (QFilterKey *a : keys)
                technique->parentTechnique()->removeFilterKey(a);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dtechnique_p.cpp"
