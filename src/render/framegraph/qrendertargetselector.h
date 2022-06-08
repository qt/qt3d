// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERTARGETSELECTOR_H
#define QT3DRENDER_QRENDERTARGETSELECTOR_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/qrendertarget.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderTargetSelectorPrivate;

class Q_3DRENDERSHARED_EXPORT QRenderTargetSelector : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QRenderTarget *target READ target WRITE setTarget NOTIFY targetChanged)
public:
    explicit QRenderTargetSelector(Qt3DCore::QNode *parent = nullptr);
    ~QRenderTargetSelector();

    QRenderTarget *target() const;

public Q_SLOTS:
    void setTarget(QRenderTarget *target);

Q_SIGNALS:
    void targetChanged(QRenderTarget *target);

protected:
    explicit QRenderTargetSelector(QRenderTargetSelectorPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderTargetSelector)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QList<Qt3DRender::QRenderTargetOutput::AttachmentPoint>) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QRENDERTARGETSELECTOR_H
