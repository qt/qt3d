/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
