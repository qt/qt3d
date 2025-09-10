// Copyright (C) 2019 Ford Motor Company
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
};

} //Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSUBTREEENABLER_H
