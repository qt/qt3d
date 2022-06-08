// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QATTRIBUTE_P_H
#define QT3DCORE_QATTRIBUTE_P_H

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

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <private/qnode_p.h>
#include <private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

class Q_3DCORE_PRIVATE_EXPORT QAttributePrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QAttribute)

    QAttributePrivate();
    void update() override;

    static QAttributePrivate *get(QAttribute *q);

    QBuffer *m_buffer;
    QString m_name;
    QAttribute::VertexBaseType m_vertexBaseType;
    uint m_vertexSize;
    uint m_count;
    uint m_byteStride;
    uint m_byteOffset;
    uint m_divisor;
    QAttribute::AttributeType m_attributeType;
    bool m_dirty;
};

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QATTRIBUTE_P_H
