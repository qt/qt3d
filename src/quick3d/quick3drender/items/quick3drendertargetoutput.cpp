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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QRenderTargetOutput;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *output) {
        Quick3DRenderTargetOutput *rT = qobject_cast<Quick3DRenderTargetOutput *>(list->object);
        if (rT)
            rT->parentRenderTarget()->addOutput(output);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DRenderTargetOutput *rT = qobject_cast<Quick3DRenderTargetOutput *>(list->object);
        if (rT)
            return rT->parentRenderTarget()->outputs().count();
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
