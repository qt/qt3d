// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/QFrameGraphNode>
#include <QtCore/QVariant>

#ifndef QT3DRENDER_QSETFENCE_H
#define QT3DRENDER_QSETFENCE_H

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSetFencePrivate;

class Q_3DRENDERSHARED_EXPORT QSetFence : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(HandleType handleType READ handleType NOTIFY handleTypeChanged)
    Q_PROPERTY(QVariant handle READ handle NOTIFY handleChanged)
public:
    enum HandleType {
        NoHandle,
        OpenGLFenceId
    };
    Q_ENUM(HandleType) // LCOV_EXCL_LINE

    explicit QSetFence(Qt3DCore::QNode *parent = nullptr);
    ~QSetFence();

    HandleType handleType() const;
    QVariant handle() const;

Q_SIGNALS:
    void handleTypeChanged(HandleType handleType);
    void handleChanged(QVariant handle);

protected:
   explicit QSetFence(QSetFencePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSetFence)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSETFENCE_H
