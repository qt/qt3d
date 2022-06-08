// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QWAITFENCE_H
#define QT3DRENDER_QWAITFENCE_H

#include <Qt3DRender/QFrameGraphNode>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QWaitFencePrivate;

class Q_3DRENDERSHARED_EXPORT QWaitFence : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(HandleType handleType READ handleType WRITE setHandleType NOTIFY handleTypeChanged)
    Q_PROPERTY(QVariant handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(bool waitOnCPU READ waitOnCPU WRITE setWaitOnCPU NOTIFY waitOnCPUChanged)
    Q_PROPERTY(quint64 timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    enum HandleType {
        NoHandle,
        OpenGLFenceId
    };
    Q_ENUM(HandleType) // LCOV_EXCL_LINE
    explicit QWaitFence(Qt3DCore::QNode *parent = nullptr);
    ~QWaitFence();

    void setHandleType(HandleType type);
    void setHandle(QVariant handle);

    HandleType handleType() const;
    QVariant handle() const;

    bool waitOnCPU() const;
    void setWaitOnCPU(bool waitOnCPU);

    quint64 timeout() const;
    void setTimeout(quint64 timeout);

Q_SIGNALS:
    void waitOnCPUChanged(bool waitOnCPU);
    void timeoutChanged(quint64 timeoutChanged);
    void handleTypeChanged(HandleType handleType);
    void handleChanged(QVariant handle);

protected:
   explicit QWaitFence(QWaitFencePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QWaitFence)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QWAITFENCE_H
