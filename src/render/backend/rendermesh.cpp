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

#include "rendermesh.h"
#include <Qt3DCore/qaspectmanager.h>
#include "rendertechnique.h"
#include "rendermaterial.h"
#include "rendereraspect.h"
#include "qmesh.h"
#include "qgraphicscontext.h"
#include <meshdata.h>
#include <technique.h>

#include <Qt3DCore/qscenepropertychange.h>

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

/*!
 * \class RenderMesh
 *
 * Monitor a frontend Mesh for source changes. If the source is changed,
 * the MeshData returned is either a valid MeshData corresponding to the source
 * or null if the new MeshData hasn't been loaded. If this is the case the meshDirty
 * flag is set to true.
 *
 * \sa MeshData
 */

RenderMesh::RenderMesh() :
    m_rendererAspect(0),
    m_peer(0),
    m_meshDirty(true),
    m_lock(new QReadWriteLock())
{
}

void RenderMesh::setPeer(QMesh *peer)
{
    if (m_peer != peer) {
        QChangeArbiter *arbiter = m_rendererAspect->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_peer = peer;
        m_meshDirty = true;
        if (m_peer)
            arbiter->registerObserver(this, m_peer, ComponentUpdated);
    }
}

void RenderMesh::setRendererAspect(RendererAspect *rendererAspect)
{
    m_rendererAspect = rendererAspect;
}

void RenderMesh::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->m_type) {
    case ComponentUpdated: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        //        QString propertyName = QString::fromLatin1(propertyChange->m_propertyName);
        QVariant propertyValue = propertyChange->m_value;
        m_source = propertyValue.toString();
        m_meshDirty = true;
        break;
    }

    default:
        break;
    }
}

bool RenderMesh::meshDirty() const
{
    QReadLocker lock(m_lock);
    return m_meshDirty;
}

HMeshData RenderMesh::meshData() const
{
    QReadLocker lock(m_lock);
    return m_meshDataHandle;
}

void RenderMesh::setMeshData(HMeshData handle)
{
    QWriteLocker lock(m_lock);
    m_meshDataHandle = handle;
    m_meshDirty = false;
}

//DrawStateSet *RenderMesh::stateSet()
//{
//    return m_technique->stateSetForPass(m_pass);
//}

//void RenderMesh::initializeGL(QGraphicsContext* gc)
//{
//    if (m_meshData.isNull())
//        return ;
//    m_vao.create();
//    m_vao.bind();

//    gc->activateShader(m_technique->shaderForPass(m_pass));

//    foreach (QString nm, m_meshData->attributeNames()) {
//        AttributePtr attr(m_meshData->attributeByName(nm));
//        QString glsl = m_technique->glslNameForMeshAttribute(m_pass, nm);
//        if (glsl.isEmpty())
//            continue; // not used in this pass
//        gc->specifyAttribute(glsl, attr);
//    }

//    m_drawIndexed = (m_meshData->indexAttr() != NULL);
//    if (m_drawIndexed)
//        gc->specifyIndices(m_meshData->indexAttr());
//    m_vao.release();
//}

//void RenderMesh::releaseGL()
//{
//    m_vao.destroy();
//}

//void RenderMesh::sendDrawingCommands(QGraphicsContext* gc)
//{
//    if (m_meshData.isNull())
//        return ;
//    if (gc->activeMaterial() != m_material) {
//        gc->setActiveMaterial(m_material);
//        m_material->setUniformsForPass(m_pass, gc);
//    }

//    gc->setModelMatrix(m_modelMatrix);

//    m_vao.bind();
//    GLint primType = m_meshData->primitiveType();
//    GLint primCount = m_meshData->primitiveCount();
//    GLint indexType = m_drawIndexed ? m_meshData->indexAttr()->type() : 0;

//    if (m_instanceCount > 0) {
//        if (m_drawIndexed)
//            gc->drawElementsInstanced(primType,
//                                      primType,
//                                      indexType,
//                                      0,
//                                      m_instanceCount);
//        else
//            gc->drawArraysInstanced(primType,
//                                    0,
//                                    primCount,
//                                    m_instanceCount);
//    }
//    else {
//        if (m_drawIndexed)
//            gc->drawElements(primType,
//                             primCount,
//                             indexType,
//                             reinterpret_cast<void*>(m_meshData->indexAttr()->byteOffset()));
//        else
//            gc->drawArrays(primType, 0, primCount);
//    }  // non-instanced drawing

//    int err = glGetError();
//    if (err)
//        qWarning() << "GL error after drawing mesh:" << QString::number(err, 16);


//    m_vao.release();
//}

//RenderShader *RenderMesh::shader()
//{
//    return m_technique->shaderForPass(m_pass);
//}

//AxisAlignedBoundingBox RenderMesh::boundingBox() const
//{
//    if (!m_meshData)
//        return AxisAlignedBoundingBox();

//    return m_meshData->boundingBox();
//}

#if 0
static bool areIndicesShort( const QVector<unsigned int>& indices )
{
    // optimization; check the last index first, since it is often
    // (but not always) going to be the larger index values
    if (indices.back() > 0xffff) {
        return false;
    }

    for (int i=0; i<indices.size(); ++i) {
        if (indices.at(i) > 0xffff) {
            return false;
        }
    } // of indices iteration

    return true;
}

void RenderMesh::setMeshData( DrawContext& dc )
{
    // Construct buffers for the vertices, normals, texCoord, tangents and indices
    QVector<QVector3D> points = m_loader->vertices();
    QVector<QVector3D> normals = m_loader->normals();
    QVector<QVector2D> texCoords = m_loader->textureCoordinates();
    QVector<QVector4D> tangents = m_loader->tangents();

    int vertexCount  = points.size();
    QVector<unsigned int> elements = m_loader->indices();
    m_drawCount = elements.size();
    int faces = elements.size() / 3;

    bool shortIndices = areIndicesShort(elements);

    /** \todo Use QScopedPointers or try reinterpret_cast on the
      * QVector<> objects.
      */
    float* v = new float[3 * vertexCount];
    float* n = new float[3 * vertexCount];
    float* tc = 0;
    float* tang = 0;

    if ( !texCoords.isEmpty() ) {
        tc = new float[ 2 * vertexCount ];
        if ( !tangents.isEmpty() )
            tang = new float[ 4 * vertexCount ];
    }

    int idx = 0, tcIdx = 0, tangIdx = 0;
    for ( int i = 0; i < vertexCount; ++i ) {
        v[idx]   = points[i].x();
        v[idx+1] = points[i].y();
        v[idx+2] = points[i].z();
        n[idx]   = normals[i].x();
        n[idx+1] = normals[i].y();
        n[idx+2] = normals[i].z();
        idx += 3;
        if ( tc != 0 ) {
            tc[tcIdx]   = texCoords[i].x();
            tc[tcIdx+1] = texCoords[i].y();
            tcIdx += 2;
        }
        if ( tang != 0 ) {
            tang[tangIdx]   = tangents[i].x();
            tang[tangIdx+1] = tangents[i].y();
            tang[tangIdx+2] = tangents[i].z();
            tang[tangIdx+3] = tangents[i].w();
            tangIdx += 4;
        }
    }

    m_mode = GL_TRIANGLES;

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v, 3 * vertexCount * sizeof( float ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( n, 3 * vertexCount * sizeof( float ) );

    if ( tc ) {
        m_textureCoordBuffer.create();
        m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_textureCoordBuffer.bind();
        m_textureCoordBuffer.allocate( tc, 2 * vertexCount * sizeof( float ) );
    }

    if ( tang ) {
        m_tangentBuffer.create();
        m_tangentBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_tangentBuffer.bind();
        m_tangentBuffer.allocate( tang, 4 * vertexCount * sizeof( float ) );
    }

    if ( shortIndices ) {
        unsigned short* el = new unsigned short[elements.size()];
        for ( int i = 0; i < elements.size(); ++i )
            el[i] = static_cast<unsigned short>(elements[i]);

        m_indexBuffer.create();
        m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_indexBuffer.bind();
        m_indexBuffer.allocate( el, 3 * faces * sizeof( unsigned short ) );
        m_indexType = GL_UNSIGNED_SHORT;
        delete[] el;
    } else {
        unsigned int* el = new unsigned int[elements.size()];
        for ( int i = 0; i < elements.size(); ++i )
            el[i] = elements[i];

        m_indexBuffer.create();
        m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_indexBuffer.bind();
        m_indexBuffer.allocate( el, 3 * faces * sizeof( unsigned int ) );
        m_indexType = GL_UNSIGNED_INT;

        delete[] el;
    }

    // Calculate the bounding volume
    // m_boundingVolume->update( points );

    // Clean up
    delete[] v;
    delete[] n;
    if ( tc != NULL )
        delete[] tc;
    if ( tang != NULL )
        delete[] tang;

    m_loader.reset();

    setupVertexArrayObject( dc );
}
#endif

} // Render
} // Qt3D

QT_END_NAMESPACE
