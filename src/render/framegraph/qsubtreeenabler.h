/****************************************************************************
**
** Copyright (C) 2019 Ford Motor Company
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

#ifndef QT3DRENDER_QSUBTREEENABLER_H
#define QT3DRENDER_QSUBTREEENABLER_H

#include <Qt3DRender/QFrameGraphNode>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

class QSubtreeEnablerPrivate;

class Q_3DRENDERSHARED_EXPORT QSubtreeEnabler : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(Enablement enablement READ enablement WRITE setEnablement NOTIFY enablementChanged)
public:
    explicit QSubtreeEnabler(Qt3DCore::QNode *parent = nullptr);
    ~QSubtreeEnabler();

    enum Enablement {
        Persistent,
        SingleShot
    };
    Q_ENUM(Enablement)

    Enablement enablement() const;
    void setEnablement(Enablement enablement);

    Q_INVOKABLE void requestUpdate();

Q_SIGNALS:
    void enablementChanged(Qt3DRender::QSubtreeEnabler::Enablement enablement);

private:
    Q_DECLARE_PRIVATE(QSubtreeEnabler)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} //Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSUBTREEENABLER_H
