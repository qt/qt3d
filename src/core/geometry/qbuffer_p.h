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

#ifndef QT3DCORE_QBUFFER_P_H
#define QT3DCORE_QBUFFER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <private/qnode_p.h>
#include <QByteArray>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORESHARED_EXPORT QBufferPrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QBuffer)

    QBufferPrivate();

    static QBufferPrivate *get(QBuffer *q);

    QByteArray m_data;
    QBuffer::UsageType m_usage;
    QBuffer::AccessType m_access;
    bool m_dirty;

    void update() override;
    void setData(const QByteArray &data);

    static const char *UpdateDataPropertyName;
};

struct QBufferUpdate
{
    int offset;
    QByteArray data;
};

} // namespace Qt3DCore

QT_END_NAMESPACE
Q_DECLARE_METATYPE(Qt3DCore::QBufferUpdate) // LCOV_EXCL_LINE

#endif // QT3DCORE_QBUFFER_P_H
