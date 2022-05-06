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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QTechnique;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        Quick3DEffect *eff = qobject_cast<Quick3DEffect*>(list->object);
        if (eff)
            eff->parentEffect()->addTechnique(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DEffect *eff = qobject_cast<Quick3DEffect*>(list->object);
        if (eff)
            return eff->parentEffect()->techniques().count();
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DEffect *effect = qobject_cast<Quick3DEffect *>(list->object);
        qobject_cast<QEffect *>(effect->parentEffect())->addParameter(param);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DEffect *effect = qobject_cast<Quick3DEffect *>(list->object);
        return qobject_cast<QEffect *>(effect->parentEffect())->parameters().count();
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
