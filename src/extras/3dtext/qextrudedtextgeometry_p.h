// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef QT3DEXTRAS_QEXTRUDEDTEXTGEOMETRY_P_H
#define QT3DEXTRAS_QEXTRUDEDTEXTGEOMETRY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DExtras/qextrudedtextgeometry.h>
#include <QFont>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;
class QBuffer;

} // namespace Qt3DCore

namespace Qt3DExtras {

class QExtrudedTextGeometry;

class QExtrudedTextGeometryPrivate : public Qt3DCore::QGeometryPrivate
{
public:
    QExtrudedTextGeometryPrivate();
    void init();
    void update() override;

    QString m_text;
    QFont m_font;
    float m_depth;
    float m_edgeSplitAngle;

    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QAttribute *m_normalAttribute;
    Qt3DCore::QAttribute *m_indexAttribute;
    Qt3DCore::QBuffer *m_vertexBuffer;
    Qt3DCore::QBuffer *m_indexBuffer;

    Q_DECLARE_PUBLIC(QExtrudedTextGeometry)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QEXTRUDEDTEXTGEOMETRY_P_H
