// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/quick3daxis_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DAxis::Quick3DAxis(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractAxisInput> Quick3DAxis::qmlAxisInputs()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAbstractAxisInput;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *input) {
        Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
        axis->parentAxis()->addInput(input);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
        return axis->parentAxis()->inputs().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
        return axis->parentAxis()->inputs().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
        const auto inputs = axis->parentAxis()->inputs();
        for (QAbstractAxisInput *input : inputs)
            axis->parentAxis()->removeInput(input);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3daxis_p.cpp"
