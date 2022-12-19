// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/qtexture.h>

#include <Qt3DQuickRender/private/quick3dmaterial_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Quick {

Quick3DMaterial::Quick3DMaterial(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QParameter> Quick3DMaterial::qmlParameters()
{
    using qt_size_type = qsizetype;
    using ListContentType = QParameter;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *param) {
        Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
        if (mat) {
            param->setParent(mat->parentMaterial());
            mat->parentMaterial()->addParameter(param);
        }
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
        if (mat)
            return mat->parentMaterial()->parameters().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
        if (mat)
            return mat->parentMaterial()->parameters().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DMaterial *mat = qobject_cast<Quick3DMaterial *>(list->object);
        if (mat) {
            const auto parameters = mat->parentMaterial()->parameters();
            for (QParameter *p : parameters)
                mat->parentMaterial()->removeParameter(p);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // Quick

} // namespace Render

} // Qt3D

QT_END_NAMESPACE

#include "moc_quick3dmaterial_p.cpp"
