// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QBACKENDNODE_H
#define QT3DCORE_QBACKENDNODE_H

#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;
class QBackendNodePrivate;
class QBackendNode;
class QAspectEngine;

#if defined(QT_BUILD_INTERNAL)
class QBackendNodeTester;
#endif

class Q_3DCORESHARED_EXPORT QBackendNodeMapper
{
public:
    virtual ~QBackendNodeMapper();
    virtual QBackendNode *create(QNodeId id) const = 0;
    virtual QBackendNode *get(QNodeId id) const = 0;
    virtual void destroy(QNodeId id) const = 0;
};

typedef QSharedPointer<QBackendNodeMapper> QBackendNodeMapperPtr;

class Q_3DCORESHARED_EXPORT QBackendNode
{
public:
    enum Mode {
        ReadOnly = 0,
        ReadWrite
    };

    explicit QBackendNode(Mode mode = ReadOnly);
    virtual ~QBackendNode();

    QNodeId peerId() const noexcept;

    void setEnabled(bool enabled) noexcept;
    bool isEnabled() const noexcept;

    Mode mode() const noexcept;

    virtual void syncFromFrontEnd(const QNode *frontEnd, bool firstTime);

protected:
    Q_DECLARE_PRIVATE(QBackendNode)
    explicit QBackendNode(QBackendNodePrivate &dd);

    QBackendNodePrivate *d_ptr;

private:
    Q_DISABLE_COPY(QBackendNode)
    void setPeerId(QNodeId id) noexcept;

    friend class QAbstractAspectPrivate;
#if defined(QT_BUILD_INTERNAL)
    friend class QBackendNodeTester;
#endif
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBACKENDNODE_H
