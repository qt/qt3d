// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/qeffect.h>

#include <Qt3DQuickRender/private/quick3deffect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DEffect::Quick3DEffect(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QTechnique> Quick3DEffect::techniqueList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QTechnique;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        Quick3DEffect *eff = qobject_cast<Quick3DEffect*>(list->object);
        if (eff)
            eff->parentEffect()->addTechnique(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DEffect *eff = qobject_cast<Quick3DEffect*>(list->object);
        if (eff)
            return eff->parentEffect()->techniques().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        // TO DO : Return a QAbstractTechnique once properly defined
        Quick3DEffect *eff = qobject_cast<Quick3DEffect*>(list->object);
        if (eff)
            return qobject_cast<QTechnique*>(eff->parentEffect()->techniques().at(index));
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DEffect *eff = qobject_cast<Quick3DEffect*>(list->object);
        if (eff) {
            // Ownership of techniques is handled by the QmlEngine so we shouldn't class clearTechniques
            // which deletes techniques
            const auto techniques = eff->parentEffect()->techniques();
            for (QTechnique *tech : techniques)
                eff->parentEffect()->removeTechnique(tech);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QParameter> Quick3DEffect::parameterList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DEffect *effect = qobject_cast<Quick3DEffect *>(list->object);
        qobject_cast<QEffect *>(effect->parentEffect())->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DEffect *effect = qobject_cast<Quick3DEffect *>(list->object);
        return qobject_cast<QEffect *>(effect->parentEffect())->parameters().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DEffect *effect = qobject_cast<Quick3DEffect *>(list->object);
        return qobject_cast<QEffect *>(effect->parentEffect())->parameters().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DEffect *effect = qobject_cast<Quick3DEffect *>(list->object);
        const auto parameters = qobject_cast<QEffect *>(effect->parentEffect())->parameters();
        for (QParameter *p : parameters)
            qobject_cast<QEffect *>(effect->parentEffect())->removeParameter(p);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3deffect_p.cpp"
