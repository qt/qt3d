/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_RENDERTECHNIQUE_H
#define QT3D_RENDER_RENDERTECHNIQUE_H

#include <QVector>
#include <QStringList>
#include <Qt3DCore/private/qobserverinterface_p.h>
#include <Qt3DRenderer/private/parameterpack_p.h>
#include <Qt3DRenderer/qannotation.h>
#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTechnique;
class QParameter;
class QOpenGLFilter;
class QAnnotation;
class QRenderPass;

namespace Render {

class TechniqueManager;

class RenderTechnique : public QBackendNode
{
public:
    RenderTechnique();
    ~RenderTechnique();
    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;

    void sceneChangeEvent(const QSceneChangePtr &e);
    const QHash<QString, QVariant> parameters() const;

    void appendRenderPass(QRenderPass *rPass);
    void removeRenderPass(const QUuid &renderPassId);

    void appendAnnotation(QAnnotation *criterion);
    void removeAnnotation(const QUuid &criterionId);

    QList<QUuid> annotations() const;
    QList<QUuid> renderPasses() const;
    QOpenGLFilter *openGLFilter() const;

private:
    QOpenGLFilter *m_openglFilter;

    ParameterPack m_parameterPack;
    QList<QUuid> m_annotationList;
    QList<QUuid> m_renderPasses;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERTECHNIQUE_H
