/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

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

