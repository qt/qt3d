// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef TESSELLATEDQUAD_H
#define TESSELLATEDQUAD_H

#include <Qt3DRender/qgeometryrenderer.h>

class TessellatedQuadMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
public:
    explicit TessellatedQuadMesh(Qt3DCore::QNode *parent = 0);
};

#endif // TESSELLATEDQUAD_H
