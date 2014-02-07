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

#include "gltfparser.h"

#include "texturedata.h"

#include <entity.h>
#include <mesh.h>
#include <material.h>
#include <technique.h>
#include <shaderprogram.h>
#include <effect.h>
#include <camera.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QColor>
#include <QVector2D>
#include <QVector3D>

// need to move these to somewhere common?
#include <drawstate.h>
#include <states/blendstate.h>

namespace Qt3D {

namespace
{

Parameter::StandardUniform parseSemanticName(QString s)
{
    if (s == "MODEL")           return Parameter::ModelMatrix;
    if (s == "VIEW")            return Parameter::ViewMatrix;
    if (s == "PROJECTION")      return Parameter::ProjectionMatrix;
    if (s == "MODELVIEW")       return Parameter::ModelView;
    if (s == "MODELVIEWPROJECTION")
        return Parameter::ModelViewProjection;

    if (s == "MODELINVERSE")    return Parameter::ModelInverse;
    if (s == "VIEWINVERSE")     return Parameter::ViewInverse;
    if (s == "PROJECTIONINVERSE")
        return Parameter::ProjectionInverse;
    if (s == "MODELVIEWINVERSE")
        return Parameter::ModelViewInverse;
    if (s == "MODELVIEWPROJECTIONINVERSE")
        return Parameter::ModelViewProjectionInverse;

    if (s == "MODELINVERSETRANSPOSE")
        return Parameter::ModelNormal;
    if (s == "MODELVIEWINVERSETRANSPOSE")
        return Parameter::ModelViewNormal;

    return Parameter::None;
}

} // of anonymous namespace

GLTFParser::GLTFParser() :
    m_parseDone(false)
{
}

bool GLTFParser::isGLTFPath(QString path)
{
    QFileInfo finfo(path);
    if (!finfo.exists())
        return false;

    // might need to detect other things in the future, but would
    // prefer to avoid doing a full parse.
    return (finfo.suffix().toLower() == "json");
}

void GLTFParser::setFilePath(QString path)
{
    QFileInfo finfo(path);
    if (!finfo.exists()) {
        qWarning() << "missing file:" << path;
        return;
    }

    QFile f(path);
    f.open(QIODevice::ReadOnly);

    if (!setJSON(QJsonDocument::fromJson(f.readAll()))) {
        qWarning() << "not a JSON document";
        return;
    }

    setBasePath(finfo.dir().absolutePath());
}

void GLTFParser::setBasePath(QString path)
{
    m_basePath = path;
}

bool GLTFParser::setJSON( QJsonDocument json )
{
    if ( !json.isObject() ) {
        return false;
    }

    m_json = json;
    m_parseDone = false;

    m_meshDict.clear();
    m_attributeDict.clear();
    m_defaultScene.clear();

    return true;
}

MeshDataPtr GLTFParser::mesh(QString id)
{
    parse();
    if (m_meshDict.contains(id))
        return m_meshDict.value(id);

    qWarning() << "Unknown mesh" << id << "in GLTF file" << m_basePath;
    return MeshDataPtr();
}

Entity* GLTFParser::defaultScene()
{
    parse();
    if (m_defaultScene.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "no default scene";
        return NULL;
    }

    return scene(m_defaultScene);
}

Entity* GLTFParser::scene(QString id)
{
    parse();

    QJsonObject scenes = m_json.object().value("scenes").toObject();
    if (!scenes.contains(id)) {
        qWarning() << "GLTF: no such scene" << id << "in file" << m_basePath;
        return NULL;
    }

    QJsonObject sceneObj = scenes.value(id).toObject();
    Entity* sceneEntity = new Entity;
    foreach (QJsonValue nnv, sceneObj.value("nodes").toArray()) {
        QString nodeName = nnv.toString();
        Entity* child = node(nodeName);
        if (!child)
            continue;

        sceneEntity->addChild(child);
    }

    return sceneEntity;
}

Entity* GLTFParser::node(QString id)
{
    QJsonObject nodes = m_json.object().value("nodes").toObject();
    if (!nodes.contains(id)) {
        qWarning() << "unknown node" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    QJsonObject jsonObj = nodes.value(id).toObject();
    Entity* result( new Entity );
    parse();

    if ( jsonObj.contains( "children" ) )
    {
        foreach (QJsonValue c, jsonObj.value( "children" ).toArray()) {
            Entity* child = node(c.toString());
            if (!child)
                continue;
            result->addChild(child);
        }
    }

    if ( jsonObj.contains( "meshes") )
    {
        typedef QList<Mesh*> MeshList;
        QMap<QString, MeshList> materialDict;

        foreach (QJsonValue m, jsonObj.value( "meshes" ).toArray())
        {
            if (!m_meshDict.contains(m.toString())) {
                qWarning() << "node" << id << "references unknown mesh" << m.toString();
                continue;
            }

            foreach (MeshDataPtr md, m_meshDict.values(m.toString())) {
                QString matId = m_meshMaterialDict[md.data()];
                Mesh* meshComp = new Mesh;
                meshComp->setData(md);
                materialDict[matId].append(meshComp);
            }
        }

        if (materialDict.size() == 1) {
            // common case
            result->addComponent(material(materialDict.firstKey()));
            foreach (Mesh* m, materialDict.first())
                result->addComponent(m);
        } else {
            // need to make a child entity per material
            foreach (QString matId, materialDict.keys()) {
                Entity* subEntity(new Entity);
                result->addChild(subEntity);

                subEntity->addComponent(material(matId));
                foreach (Mesh* m, materialDict.value(matId))
                    subEntity->addComponent(m);
            } // of distinct material iteration
        } // of multiple materials case
    }

    processName(jsonObj, result);

    if ( jsonObj.contains( "matrix") )
    {
        QMatrix4x4 m;
        QJsonArray matrixValues = jsonObj.value( "matrix" ).toArray();

        for (int i=0; i<16; ++i) {
            double v = matrixValues.at( i ).toDouble();
            m(i % 4, i >> 2) = v;
        }

        result->setMatrix( m );
    }

    if ( jsonObj.contains( "camera") )
    {
        Camera* cam = camera( jsonObj.value("camera").toString() );
        if (!cam) {
            qWarning() << "failed to build camera:" << jsonObj.value("camera")
                          << "on node" << id;
        } else {
            result->addComponent(cam);
        }
    } // of have camera attribute

    return result;
}

// Get rid of miniwindef.h clobbering legal variable names
#if defined(near)
#undef near
#endif

#if defined(far)
#undef far
#endif

Camera* GLTFParser::camera(QString id)
{
    parse();
    QJsonObject cams = m_json.object().value("cameras").toObject();
    if (!cams.contains(id)) {
        qWarning() << "unknown camera" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    QJsonObject jsonObj = cams.value(id).toObject();
    QString camTy = jsonObj.value("type").toString();

    if (camTy == "perspective") {
        if (!jsonObj.contains("perspective")) {
            qWarning() << "camera:" << id << "missing 'perspective' object";
            return NULL;
        }

        QJsonObject pObj = jsonObj.value("perspective").toObject();
        double yfov = pObj.value("yfov").toDouble();
        double near = pObj.value("znear").toDouble();
        double far = pObj.value("zfar").toDouble();

        Camera* result = new Camera;
        result->setPerspectiveProjection(yfov, 1.0, near, far);
        return result;
    } else if (camTy == "orthographic") {
        qWarning() << Q_FUNC_INFO << "implement me";

        return NULL;
    } else {
        qWarning() << "camera:" << id << "has unsupported type:" << camTy;
        return NULL;
    }
}

Material* GLTFParser::material(QString id)
{
    parse();

    if (m_materialCache.contains(id))
        return m_materialCache.value(id);

    QJsonObject mats = m_json.object().value("materials").toObject();
    if (!mats.contains(id)) {
        qWarning() << "unknown material" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    QJsonObject jsonObj = mats.value(id).toObject();

    QJsonObject tech = jsonObj.value("instanceTechnique").toObject();
    QString tname = tech.value("technique").toString();
    if (!m_techniques.contains(tname)) {
        qWarning() << "unknown technique" << tname <<
                      "for material" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    Technique *technique = m_techniques.value(tname);
    if (!m_effectProxies.contains(tname)) {
        Effect* eff = new Effect;
        eff->setObjectName(tname);
        eff->addTechnique(technique);
        m_effectProxies[tname] = eff;
    }

    Material* mat = new Material;
    mat->setEffect(m_effectProxies[tname]);

    processName(jsonObj, mat);

    QJsonObject values = tech.value("values").toObject();
    foreach (QString vName, values.keys()) {
        Parameter* param = technique->parameterByName(vName);
        if (!param) {
            qWarning() << "unknown parameter:" << vName << "in technique" << tname
                          << "processing material" << id;
            continue;
        }

        if (param->isTextureType()) {
            QString textureId = values.value(vName).toString();
            if (!m_textures.contains(textureId)) {
                qWarning() << "unknown texture" << textureId << "for parameter" << vName
                              << "of material" << id;
            } else {
                mat->setTextureParameter(vName, m_textures.value(textureId));
            }
        } else {
            QVariant var = parameterValueFromJSON(param, values.value(vName));
            mat->setParameter(vName, var);
        }
    } // of material technique-instance values iteration

    m_materialCache[id] = mat;
    return mat;
}

void GLTFParser::parse()
{
    if (m_parseDone)
        return;

    QJsonObject buffers = m_json.object().value("buffers").toObject();
    foreach (QString nm, buffers.keys()) {
        processJSONBuffer( nm, buffers.value(nm).toObject() );
    }

    QJsonObject views = m_json.object().value("bufferViews").toObject();
    foreach (QString nm, views.keys()) {
        processJSONBufferView( nm, views.value(nm).toObject() );
    }

    QJsonObject shaders = m_json.object().value("shaders").toObject();
    foreach (QString nm, shaders.keys()) {
        processJSONShader( nm, shaders.value(nm).toObject() );
    }

    QJsonObject programs = m_json.object().value("programs").toObject();
    foreach (QString nm, programs.keys()) {
        processJSONProgram( nm, programs.value(nm).toObject() );
    }

    QJsonObject techniques = m_json.object().value("techniques").toObject();
    foreach (QString nm, techniques.keys()) {
        processJSONTechnique( nm, techniques.value(nm).toObject() );
    }

    QJsonObject attrs = m_json.object().value("accessors").toObject();
    foreach (QString nm, attrs.keys()) {
        processJSONAccessor( nm, attrs.value(nm).toObject() );
    }

    QJsonObject meshes = m_json.object().value("meshes").toObject();
    foreach (QString nm, meshes.keys()) {
        processJSONMesh( nm, meshes.value(nm).toObject() );
    }

    QJsonObject images = m_json.object().value("images").toObject();
    foreach (QString nm, images.keys()) {
        processJSONImage( nm, images.value(nm).toObject() );
    }

    QJsonObject textures = m_json.object().value("textures").toObject();
    foreach (QString nm, textures.keys()) {
        processJSONTexture(nm, textures.value(nm).toObject() );
    }

    m_defaultScene = m_json.object().value("scene").toString();
    m_parseDone = true;
}

void GLTFParser::processJSONBuffer( QString id, const QJsonObject& json )
{
    // simply cache buffers for lookup by buffer-views
    m_bufferDatas[id] = BufferData(json);
}

void GLTFParser::processJSONBufferView( QString id, const QJsonObject& json )
{
    QString bufName = json.value("buffer").toString();
    if (!m_bufferDatas.contains(bufName)) {
        qWarning() << "unknown buffer:" << bufName << "processing view:" << id;
        return;
    }

    int target = json.value("target").toInt();
    QOpenGLBuffer::Type ty(QOpenGLBuffer::VertexBuffer);

    switch (target) {
    case GL_ARRAY_BUFFER:           ty = QOpenGLBuffer::VertexBuffer; break;
    case GL_ELEMENT_ARRAY_BUFFER:   ty = QOpenGLBuffer::IndexBuffer; break;
    default:
        qWarning() << Q_FUNC_INFO << "buffer" << id << "unsupported target:" << target;
        return;
    }

    quint64 offset = 0;
    if (json.contains("byteOffset")) {
        offset = json.value("byteOffset").toInt();
        qDebug() << "bv:" << id << "has offset:" << offset;
    }

    quint64 len = json.value("byteLength").toInt();
    QFile* f = resolveLocalData(m_bufferDatas[bufName].path);
    if (!f->seek(offset)) {
        qWarning() << "failed to seek to offset in file for bufferView:" << id << offset;
    }

    QByteArray bytes = f->read(len);
    if (bytes.count() != (int) len) {
        qWarning() << "failed to read sufficient bytes from:" << m_bufferDatas[bufName].path
                      << "for view" << id;
    }
    delete f;

    BufferPtr b(new Buffer(ty));
    b->setData(bytes);
    m_buffers[id] = b;
}

void GLTFParser::processJSONAccessor( QString id, const QJsonObject& json )
{
    QString bvName = json.value("bufferView").toString();
    if (!m_buffers.contains(bvName)) {
        qWarning() << "unknown buffer-view:" << bvName << "processing accessor:" << id;
        return;
    }

    BufferPtr buf = m_buffers.value(bvName);
    int offset = 0, stride = 0;
    int type = json.value("type").toInt();
    int count = json.value("count").toInt();

    if ( json.contains("byteOffset"))
        offset = json.value("byteOffset").toInt();
    if ( json.contains("byteStride"))
        stride = json.value("byteStride").toInt();

    AttributePtr attr( new Attribute( buf, type, count, offset, stride ) );
    m_attributeDict[id] = attr;
}

void GLTFParser::processJSONMesh( QString id, QJsonObject jsonObj )
{
    QJsonArray primsArray = jsonObj.value( "primitives").toArray();
    Q_FOREACH (QJsonValue primVal, primsArray) {
        QJsonObject primObj = primVal.toObject();
        int type = primObj.value( "primitive").toInt();
        QString material = primObj.value( "material").toString();

        if ( material.isEmpty()) {
            qWarning() << "malformed primitive on " << id << ", missing material value"
                          << material;
            continue;
        }

        MeshDataPtr md( new MeshData( type ) );
        m_meshMaterialDict[md.data()] = material;

        QJsonObject attrs = primObj.value("attributes").toObject();
        Q_FOREACH (QString attrName, attrs.keys()) {
            QString k = attrs.value(attrName).toString();
            if (!m_attributeDict.contains(k)) {
                qWarning() << "unknown attribute accessor:" << k << "on mesh" << id;
                continue;
            }

            md->addAttribute(attrName, m_attributeDict[k]);

          //  qDebug() << "DUMP of:" << attrName;
         //   m_attributeDict[k]->dump(20);
        }

        if ( primObj.contains( "indices" )) {
            QString k = primObj.value("indices").toString();
            if (!m_attributeDict.contains(k)) {
                qWarning() << "unknown index accessor:" << k << "on mesh" << id;
            } else {
                md->setIndexAttr(m_attributeDict[k]);
            //    m_attributeDict[k]->dump(100);
            }
        } // of has indices

        m_meshDict.insert( id, md );
    } // of primitives iteration
}

void GLTFParser::processName(const QJsonObject &json, QObject *ins)
{
    if ( json.contains( "name" ) )
    {
        ins->setObjectName( json.value("name").toString() );
    }
}

void GLTFParser::processJSONProgram( QString id, QJsonObject jsonObj)
{
    ShaderProgram* prog = new ShaderProgram;
    prog->setObjectName(id);

    QString fragName = jsonObj.value("fragmentShader").toString(),
            vertName = jsonObj.value("vertexShader").toString();
    if (!m_shaderPaths.contains(fragName) || !m_shaderPaths.contains(vertName)) {
        qWarning() << Q_FUNC_INFO << "program:" << id << "missing shader:" <<
                      fragName << vertName;
        return;
    }

    prog->setFragmentSourceFile(m_shaderPaths[fragName]);
    prog->setVertexSourceFile(m_shaderPaths[vertName]);
    m_programs[id] = prog;
}

void GLTFParser::processJSONShader( QString id, QJsonObject jsonObj)
{
    // shaders are trivial for the moment, defer the real work
    // to the program section
    QString path = jsonObj.value("path").toString();

    QFileInfo info(m_basePath, path);
    if (!info.exists()) {
        qWarning() << "can't find shader" << id << "from path" << path;
        return;
    }

    m_shaderPaths[id] = info.absoluteFilePath();
}

void GLTFParser::processJSONImage( QString id, QJsonObject jsonObj)
{
    QString path = jsonObj.value("path").toString();
    QFileInfo info(m_basePath, path);
    if (!info.exists()) {
        qWarning() << "can't find image" << id << "from path" << path;
        return;
    }

    QImage img(info.absoluteFilePath());
    if (img.isNull()) {
        qWarning() << "failed to load image:" << info.absoluteFilePath();
        return;
    }

    m_images[id] = img;
}

void GLTFParser::processJSONTexture( QString id, QJsonObject jsonObj)
{
    Texture* tex = new Texture();

    int target = jsonObj.value("target").toInt();
    int pixelFormat = jsonObj.value("format").toInt();
    int internalFormat = jsonObj.value("internalFormat").toInt();

    tex->setTarget(static_cast<QOpenGLTexture::Target>(target));
    tex->setInternalFormat(QOpenGLTexture::RGBA8_UNorm /* static_cast<QOpenGLTexture::TextureFormat>(internalFormat)*/);

    QString samplerId = jsonObj.value("sampler").toString();
    QString source = jsonObj.value("source").toString();
    if (!m_images.contains(source)) {
        qWarning() << "texture" << id << "references missing image" << source;
        return;
    }

    tex->setFromQImage(m_images[source]);

    QJsonObject samplersDict(m_json.object().value("samplers").toObject());
    if (!samplersDict.contains(samplerId)) {
        qWarning() << "texture" << id << "references unknown sampler" << samplerId;
        return;
    }

    QJsonObject sampler = samplersDict.value(samplerId).toObject();

    tex->setWrapMode(static_cast<QOpenGLTexture::WrapMode>(sampler.value("wrapS").toInt()));
    // tex->setWrapMode(sampler.value("wrapT").toInt());

    tex->setMinificationFilter(static_cast<QOpenGLTexture::Filter>(sampler.value("minFilter").toInt()));
    tex->setMagnificationFilter(static_cast<QOpenGLTexture::Filter>(sampler.value("magFilter").toInt()));

    m_textures[id] = tex;
}

void GLTFParser::processJSONTechnique( QString id, QJsonObject jsonObj )
{
    Technique *t = new Technique;
    t->setObjectName(id);

    QHash<QString, Parameter*> paramDict;
    QJsonObject params = jsonObj.value( "parameters").toObject();
    Q_FOREACH (QString pname, params.keys()) {
        QJsonObject po = params.value(pname).toObject();

        int dataType = po.value("type").toInt();
        QString semantic = po.value("semantic").toString();

        Parameter* p = new Parameter(t, pname, dataType);
        Parameter::StandardUniform su = parseSemanticName(semantic);
        if (su != Parameter::None) {
            p->setStandardUniform(su);
        } else {
            // should really verify it's an attribute parameter?
            // but what would be the way to do that?
            // check the accessor dict?
            p->setMeshAttributeName(semantic);
        }

        t->addParameter(p);

        paramDict[pname] = p;
    } // of parameters iteration

    QJsonObject passes = jsonObj.value("passes").toObject();
    Q_FOREACH (QString pname, passes.keys()) {
        QJsonObject po = passes.value(pname).toObject();
        QJsonObject ip = po.value("instanceProgram").toObject();

        QString programName = ip.value("program").toString();
        if (!m_programs.contains(programName)) {
            qWarning() << Q_FUNC_INFO << "technique" << id << "pass" << pname
                       << ": missing program" << programName;
            continue;
        }

        RenderPass* pass = new RenderPass;
        pass->setShaderProgram(m_programs[programName]);

        QJsonObject attrs = ip.value("attributes").toObject();
        Q_FOREACH ( QString attrName, attrs.keys() ) {
            QString pname = attrs.value(attrName).toString();
            pass->addAttributeBinding(paramDict[pname], attrName);
        } // of program-instance attributes


        QJsonObject uniforms = ip.value("uniforms").toObject();
        Q_FOREACH (QString uniformName, uniforms.keys()) {
            QString pname = uniforms.value(uniformName).toString();
            pass->addUniformBinding(paramDict[pname], uniformName);
        } // of program-instance attributes

        QJsonObject states = po.value("states").toObject();
        DrawStateSet* ss = new DrawStateSet;

        Q_FOREACH (QString stateName, states.keys()) {
            DrawState* s= buildState(stateName, states.value(stateName));
            if (!s)
                continue;

            ss->addState(s);
        } // of program-instance attributes

        pass->setStateSet(ss);
        t->addPass(pass);
    } // of passes iteration

    m_techniques[id] = t;
}

DrawState* GLTFParser::buildState(QString nm, QJsonValue v)
{
    if (nm == "blendEnable") {
        return NULL; // will see a blendEquation spec too
    }

    if (nm == "blendFunc") {
        QJsonObject obj =  v.toObject();
        GLenum srcF = static_cast<GLenum>(obj.value("sfactor").toInt());
        GLenum dstF = static_cast<GLenum>(obj.value("dfactor").toInt());
        return BlendState::getOrCreate(srcF, dstF);
    }

    if (nm == "blendEquation") {
        return BlendEquation::getOrCreate(static_cast<GLenum>(v.toInt()));
    }

    if (nm == "cullFaceEnable" && v.toInt()) {
        return CullFace::getOrCreate(GL_BACK);
    }

    if (nm == "depthTestEnable" && v.toInt()) {
        return DepthTest::getOrCreate(GL_LESS);
    }

    if (nm == "depthMask") {
        return DepthMask::getOrCreate(v.toInt() ? GL_TRUE : GL_FALSE);
    }

    qWarning() << Q_FUNC_INFO << "unsupported gltf state:" << nm;
    return NULL;
}

QFile *GLTFParser::resolveLocalData(QString path)
{
    QDir d(m_basePath);
    Q_ASSERT(d.exists());

    QString absPath = d.absoluteFilePath(path);
    QFile* f = new QFile(absPath);
    f->open(QIODevice::ReadOnly);
    return f;
}

GLTFParser::BufferData::BufferData() :
    length(0)
{
}

GLTFParser::BufferData::BufferData(QJsonObject json)
{
    path = json.value("path").toString();
    length = json.value("length").toInt();
    // url
}

QVariant GLTFParser::parameterValueFromJSON(Parameter* p, QJsonValue val)
{
    switch (p->datatype()) {
    case GL_BOOL:
        return val.toBool();

    case GL_FLOAT:
        return val.toDouble();

    case GL_FLOAT_VEC2: {
        QJsonArray a = val.toArray();
        return QVector2D(a[0].toDouble(), a[1].toDouble());
    }

    case GL_FLOAT_VEC3: {
        QJsonArray a = val.toArray();
        return QVector3D(a[0].toDouble(), a[1].toDouble(), a[3].toDouble());
    }

    case GL_FLOAT_VEC4: {
        QJsonArray a = val.toArray();
        return QVector4D(a[0].toDouble(),
                        a[1].toDouble(),
                        a[2].toDouble(),
                        a[3].toDouble());
    }

    case GL_FLOAT_MAT4: {
        QJsonArray a = val.toArray();

        QMatrix4x4 m;
        for (int i=0; i<16; ++i) {
            m(i % 4, i / 4) = a[i].toDouble();
        }
        return m;
    }

    default:
        qWarning() << Q_FUNC_INFO << "unhandled type:" << QString::number(p->datatype(), 16);
    }

    return QVariant();
}

} // of namespace Qt3D
