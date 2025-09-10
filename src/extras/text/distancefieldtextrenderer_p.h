// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_H
#define QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_H

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

#include <QtCore/QRectF>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DExtras/qt3dextras_global.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
}

namespace Qt3DExtras {

class DistanceFieldTextRendererPrivate;

class DistanceFieldTextRenderer : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    DistanceFieldTextRenderer(Qt3DCore::QNode *parent = nullptr);
    ~DistanceFieldTextRenderer();

    void setGlyphData(Qt3DRender::QAbstractTexture *glyphTexture,
                      const std::vector<float> &vertexData,
                      const std::vector<quint16> &indexData);

    void setColor(const QColor &color);

    Q_DECLARE_PRIVATE(DistanceFieldTextRenderer)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_H
