/****************************************************************************
**
** Copyright (C) 2015 Lorenz Esch (TU Ilmenau).
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

#ifndef QT3DEXTRAS_QPERVERTEXCOLORMATERIAL_P_H
#define QT3DEXTRAS_QPERVERTEXCOLORMATERIAL_P_H

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

#include <Qt3DRender/private/qmaterial_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKey;
class QEffect;
class QTechnique;
class QParameter;
class QShaderProgram;
class QRenderPass;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QPerVertexColorMaterial;

class QPerVertexColorMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QPerVertexColorMaterialPrivate();

    void init();

    Qt3DRender::QEffect *m_vertexEffect;
    Qt3DRender::QTechnique *m_vertexGL3Technique;
    Qt3DRender::QTechnique *m_vertexGL2Technique;
    Qt3DRender::QTechnique *m_vertexES2Technique;
    Qt3DRender::QTechnique *m_vertexRHITechnique;
    Qt3DRender::QRenderPass *m_vertexGL3RenderPass;
    Qt3DRender::QRenderPass *m_vertexGL2RenderPass;
    Qt3DRender::QRenderPass *m_vertexES2RenderPass;
    Qt3DRender::QRenderPass *m_vertexRHIRenderPass;
    Qt3DRender::QShaderProgram *m_vertexGL3Shader;
    Qt3DRender::QShaderProgram *m_vertexGL2ES2Shader;
    Qt3DRender::QShaderProgram *m_vertexRHIShader;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QPerVertexColorMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPERVERTEXCOLORMATERIAL_P_H

