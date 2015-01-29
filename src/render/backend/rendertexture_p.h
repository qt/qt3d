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

#ifndef QT3D_RENDER_RENDERTEXTURE_H
#define QT3D_RENDER_RENDERTEXTURE_H

#include <QOpenGLContext>
#include <QMutex>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/texturedata.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DRenderer/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLTexture;

namespace Qt3D {

class QAbstractTextureProvider;

namespace Render {

class TextureManager;

class RenderTexture : public QBackendNode
{
public:
    RenderTexture();
    ~RenderTexture();
    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;

    QOpenGLTexture* getOrCreateGLTexture() ;

    GLint textureId();

    bool isTextureReset() const;

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

private:
    QOpenGLTexture *m_gl;

    QOpenGLTexture *buildGLTexture();
    void setToGLTexture(TexImageDataPtr imgData);
    void updateWrapAndFilters();

    int m_width;
    int m_height;
    int m_depth;
    bool m_generateMipMaps;
    QAbstractTextureProvider::Target m_target;
    QAbstractTextureProvider::TextureFormat m_format;
    QAbstractTextureProvider::Filter m_magnificationFilter;
    QAbstractTextureProvider::Filter m_minificationFilter;
    QTextureWrapMode::WrapMode m_wrapModeX;
    QTextureWrapMode::WrapMode m_wrapModeY;
    QTextureWrapMode::WrapMode m_wrapModeZ;
    float m_maximumAnisotropy;
    QAbstractTextureProvider::ComparisonFunction m_comparisonFunction;
    QAbstractTextureProvider::ComparisonMode m_comparisonMode;

    QList<TexImageDataPtr> m_imageData;
    QList<QNodeId> m_textureImages;
    bool m_isDirty;
    bool m_filtersAndWrapUpdated;
    QMutex *m_lock;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::Render::RenderTexture*)

#endif // QT3D_RENDER_RENDERTEXTURE_H
