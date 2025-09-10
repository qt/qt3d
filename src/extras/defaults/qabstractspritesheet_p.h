// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QABSTRACTSPRITESHEET_P_H
#define QT3DEXTRAS_QABSTRACTSPRITESHEET_P_H

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
#include <QMatrix3x3>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKey;
class QEffect;
class QAbstractTexture;
class QTechnique;
class QParameter;
class QShaderProgram;
class QRenderPass;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QAbstractSpriteSheet;

class QAbstractSpriteSheetPrivate : public Qt3DCore::QNodePrivate
{
public:
    QAbstractSpriteSheetPrivate();

    virtual void init();
    void updateTexture(Qt3DRender::QAbstractTexture *texture);
    void updateIndex(int newIndex);
    virtual int maxIndex() const = 0;
    virtual void updateSizes() = 0;
    virtual void updateTransform() = 0;

    Qt3DRender::QAbstractTexture *m_texture;
    QMatrix3x3 m_textureTransform;
    QSize m_textureSize;
    int m_currentIndex;

    Q_DECLARE_PUBLIC(QAbstractSpriteSheet)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QABSTRACTSPRITESHEET_P_H

