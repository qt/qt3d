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
****************************************************************************/

#ifndef QT3DRENDER_QRENDERTARGETSELECTOR_H
#define QT3DRENDER_QRENDERTARGETSELECTOR_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/qrendertargetoutput.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderTargetSelectorPrivate;
class QRenderTarget;

class Q_3DRENDERSHARED_EXPORT QRenderTargetSelector : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QRenderTarget *target READ target WRITE setTarget NOTIFY targetChanged)
public:
    explicit QRenderTargetSelector(Qt3DCore::QNode *parent = nullptr);
    ~QRenderTargetSelector();

    QRenderTarget *target() const;

    void setOutputs(const QVector<QRenderTargetOutput::AttachmentPoint> &buffers);
    QVector<QRenderTargetOutput::AttachmentPoint> outputs() const;

public Q_SLOTS:
    void setTarget(QRenderTarget *target);

Q_SIGNALS:
    void targetChanged(QRenderTarget *target);

protected:
    explicit QRenderTargetSelector(QRenderTargetSelectorPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderTargetSelector)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QVector<Qt3DRender::QRenderTargetOutput::AttachmentPoint>) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QRENDERTARGETSELECTOR_H
