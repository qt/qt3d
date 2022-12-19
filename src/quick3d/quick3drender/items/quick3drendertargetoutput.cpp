// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/quick3drendertargetoutput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DRenderTargetOutput::Quick3DRenderTargetOutput(QObject * parent)
    : QObject(parent)
{
}

QQmlListProperty<QRenderTargetOutput> Quick3DRenderTargetOutput::qmlAttachments()
{
    using qt_size_type = qsizetype;
    using ListContentType = QRenderTargetOutput;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *output) {
        Quick3DRenderTargetOutput *rT = qobject_cast<Quick3DRenderTargetOutput *>(list->object);
        if (rT)
            rT->parentRenderTarget()->addOutput(output);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderTargetOutput *rT = qobject_cast<Quick3DRenderTargetOutput *>(list->object);
        if (rT)
            return rT->parentRenderTarget()->outputs().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DRenderTargetOutput *rT = qobject_cast<Quick3DRenderTargetOutput *>(list->object);
        if (rT)
            return rT->parentRenderTarget()->outputs().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DRenderTargetOutput *rT = qobject_cast<Quick3DRenderTargetOutput *>(list->object);
        if (rT) {
            const auto outputs = rT->parentRenderTarget()->outputs();
            for (QRenderTargetOutput *output : outputs)
                rT->parentRenderTarget()->removeOutput(output);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3drendertargetoutput_p.cpp"
