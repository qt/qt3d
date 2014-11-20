/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_RENDER_RENDERSHADERDATA_P_H
#define QT3D_RENDER_RENDERSHADERDATA_P_H

#include <Qt3DCore/qbackendnode.h>
#include <private/shadervariables_p.h>
#include <private/uniformbuffer_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

class QGraphicsContext;

class RenderShaderData : public QBackendNode
{
public:
    RenderShaderData();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    inline QHash<const char *, QVariant> properties() const { return m_properties; }
    inline QHash<const char *, ShaderUniform> activeProperties() const { return m_activeProperties; }
    inline bool initialized() const { return m_initialized; }

    void initialize(const ShaderUniformBlock &block);
    void appendActiveProperty(const char *propertyName, const ShaderUniform &description);
    void apply(QGraphicsContext *ctx, int bindingPoint);

protected:
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    void updateUniformBuffer(QGraphicsContext *ctx);

private:
    QHash<const char *, QVariant> m_properties;
    QHash<const char *, ShaderUniform> m_activeProperties;
    ShaderUniformBlock m_block;
    QByteArray m_data;
    UniformBuffer m_ubo;
    bool m_initialized;
    bool m_needsBufferUpdate;
};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSHADERDATA_P_H
