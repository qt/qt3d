/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglobjscenehandler.h"
#include "qglobjscene.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qglbuilder.h"

#include <QtCore/qiodevice.h>
#include <QtCore/qfile.h>
#include <QtGui/qimage.h>

QT_BEGIN_NAMESPACE

QGLObjSceneHandler::QGLObjSceneHandler()
    : QGLSceneFormatHandler()
    , palette(0)
    , smoothing(QGL::Faceted)
    , smoothingForced(false)
{
}

// Documentation for OBJ and MTL files from:
// http://www.fileformat.info/format/wavefrontobj/egff.htm
// http://www.fileformat.info/format/material/

static int objSkipWS(const QByteArray& line, int posn)
{
    while (posn < line.size() && (line[posn] == ' ' || line[posn] == '\t'))
        ++posn;
    return posn;
}

static int objSkipNonWS(const QByteArray& line, int posn, int stopch)
{
    while (posn < line.size() &&
            line[posn] != ' ' && line[posn] != '\t' && line[posn] != stopch)
        ++posn;
    return posn;
}

static qreal objReadFloat(const QByteArray& line, int *posn)
{
    *posn = objSkipWS(line, *posn);
    int end = objSkipNonWS(line, *posn, 0);
    qreal value;
    if (end > *posn)
        value = qreal(line.mid(*posn, end - *posn).toDouble());
    else
        value = 0.0f;
    *posn = end;
    return value;
}

static int objReadInteger(const QByteArray& line, int *posn)
{
    *posn = objSkipWS(line, *posn);
    int end = objSkipNonWS(line, *posn, '/');
    int value;
    if (end > *posn)
        value = line.mid(*posn, end - *posn).toInt();
    else
        value = 0;
    //*posn = objSkipNonWS(line, end, 0);
    *posn = end;
    return value;
}

static int objReadSlashInteger(const QByteArray& line, int *posn)
{
    if (*posn >= line.size() || line[*posn] != '/')
        return 0;
    ++(*posn);
    int end = objSkipNonWS(line, *posn, '/');
    int value;
    if (end > *posn)
        value = line.mid(*posn, end - *posn).toInt();
    else
        value = 0;
    //*posn = objSkipNonWS(line, end, 0);
    *posn = end;
    return value;
}

static QColor objReadColor(const QByteArray& line, int posn)
{
    qreal red = objReadFloat(line, &posn);
    qreal green = objReadFloat(line, &posn);
    qreal blue = objReadFloat(line, &posn);
    qreal alpha = 1.0f;
    posn = objSkipWS(line, posn);
    if (posn < line.size())
        alpha = objReadFloat(line, &posn);
    return QColor::fromRgbF(red, green, blue, alpha);
}

void QGLObjSceneHandler::decodeOptions(const QString &options)
{
    if (options.contains(QLatin1String("ForceSmooth")))
    {
        smoothingForced = true;
        smoothing = QGL::Smooth;
    }
    else
    {
        smoothingForced = true;
        smoothing = QGL::Faceted;
    }
}

QGLAbstractScene *QGLObjSceneHandler::read()
{
    QByteArray line;
    QByteArray keyword;
    int posn, index, count;
    int tindex, nindex;
    QVector3DArray positions;
    QVector2DArray texCoords;
    QVector3DArray normals;
    qreal x, y, z;
    quint32 fields = 0;
    QGLMaterial *material = 0;
    QGLSceneNode *defaultNode;

    // Create the geometry builder and start an initial Faceted section.
    QGLBuilder builder;
    builder.newSection(smoothing);
    QGLSceneNode *root = builder.sceneNode();
    palette = root->palette();
    defaultNode = root;
    defaultNode->setObjectName(QLatin1String("__main"));
    builder.pushNode();

    QGeometryData op;
    while (!device()->atEnd()) {
        // Read the next line, including any backslash continuations.
        line = device()->readLine().trimmed();
        while (line.endsWith('\\')) {
            line.truncate(line.size() - 1);
            if (device()->atEnd())
                break;
            line += device()->readLine().trimmed();
        }
        if (line.startsWith('#') || line.isEmpty())
            continue;   // Skip comments and blank lines.

        // Extract the keyword at the start of the line.
        posn = 0;
        while (posn < line.size() &&
               line[posn] != ' ' && line[posn] != '\t')
            ++posn;
        keyword = line.left(posn);

        // Determine how to process this line from the keyword.
        if (keyword == "v") {
            x = objReadFloat(line, &posn);
            y = objReadFloat(line, &posn);
            z = objReadFloat(line, &posn);
            positions.append(x, y, z);
        } else if (keyword == "vt") {
            x = objReadFloat(line, &posn);
            y = objReadFloat(line, &posn);
            texCoords.append(x, y);
        } else if (keyword == "vn") {
            x = objReadFloat(line, &posn);
            y = objReadFloat(line, &posn);
            z = objReadFloat(line, &posn);
            normals.append(x, y, z);
        } else if (keyword == "f") {
            posn = objSkipWS(line, posn);
            count = 0;
            //QGeometryData op; //(dlist, QGL::TRIANGLE_FAN);
            op = QGeometryData();  // clear leaves field definitions
            while (posn < line.size()) {
                // Note: we currently only read the initial vertex
                // index and also use it for texture co-ordinates
                // and normals.  e.g. "2/2", "3/3", etc.  This will
                // need to be fixed to handle "2/1", "3/7", etc.
                index = objReadInteger(line, &posn);
                tindex = objReadSlashInteger(line, &posn);
                nindex = objReadSlashInteger(line, &posn);
                if (index < 0)
                    index = positions.count() + index;
                else if (index > 0)
                    --index;        // Indices in obj are 1-based.
                if (index >= 0 && index < positions.count())
                    op.appendVertex(positions[index]);
                if (tindex < 0)
                    tindex = texCoords.count() + tindex;
                else if (tindex > 0)
                    --tindex;        // Indices in obj are 1-based.
                else
                    tindex = -1;
                if (tindex >= 0 && tindex < texCoords.count())
                    op.appendTexCoord(texCoords[tindex]);
                if (nindex < 0)
                    nindex = normals.count() + nindex;
                else if (nindex > 0)
                    --nindex;        // Indices in obj are 1-based.
                else
                    nindex = -1;
                if (nindex >= 0 && nindex < normals.count())
                    op.appendNormal(normals[nindex]);
                ++count;
                posn = objSkipNonWS(line, posn, 0);
                posn = objSkipWS(line, posn);
            }
            // if geometry has already been added with a different combination
            // of fields start a new section
            // the primitive doesn't get posted to the section until op.end()
            if (op.fields() != fields)
            {
                if (fields && builder.currentNode()->count() > 0)
                    builder.newSection(smoothing);
                fields = op.fields();
            }
            builder.addTriangleFan(op);
        } else if (keyword == "usemtl") {
            // Specify a material for the faces that follow.
            posn = objSkipWS(line, posn);
            QByteArray rest = line.mid(posn);
            QString materialName = QString::fromLocal8Bit(rest.constData(), rest.size());
            if (!materialName.isEmpty() &&
                materialName != QLatin1String("(null)")) {
                index = palette->indexOf(materialName);
                if (index != -1) {
                    QGLSceneNode *node = builder.newNode();
                    node->setMaterialIndex(index);
                    QGLMaterial *material = palette->material(index);
                    if (material->texture())
                        node->setEffect(QGL::LitDecalTexture2D);
                    else
                        node->setEffect(QGL::LitMaterial);
                } else {
                    qWarning() << "obj material" << materialName << "not found";
                    material = 0;
                }
            }
        } else if (keyword == "mtllib") {
            // Load a material library.
            posn = objSkipWS(line, posn);
            QByteArray filename = line.mid(posn);
            loadMaterialLibrary(QString::fromLocal8Bit(filename.constData(), filename.size()));
        } else if (keyword == "s") {
            if (!smoothingForced)
            {
                // Set smoothing on or off.
                posn = objSkipWS(line, posn);
                index = objSkipNonWS(line, posn, 0);
                QByteArray arg = line.mid(posn, index - posn);
                QGL::Smoothing smooth;
                if (arg == "on" || arg == "1")
                    smooth = QGL::Smooth;
                else
                    smooth = QGL::Faceted;
                if (smoothing != smooth) {
                    smoothing = smooth;
                    builder.newSection(smooth);
                }
            }
        } else if (keyword == "g" || keyword == "o") {
            // Label the faces that follow as part of a named group or object.
            posn = objSkipWS(line, posn);
            QByteArray rest = line.mid(posn);
            QString objectName = QString::fromLocal8Bit(rest.constData(), rest.size());
            QGLSceneNode *node = builder.currentNode();
            // if content has already been added to a current group, then
            // create a new node in the scene graph for the group, otherwise
            // just label the existing group with this name
            QGLSceneNode *p = qobject_cast<QGLSceneNode*>(node->parent());
            if (node->count() > 0 && p && p->objectName().isEmpty())
            {
                node = p;
            }
            else
            {
                builder.popNode();
                node = builder.currentNode();
                builder.pushNode();
            }
            node->setObjectName(objectName);
        } else {
            qWarning() << "unsupported obj command: " << keyword.constData();
        }
    }

    // Create a scene from the geometry
    return new QGLObjScene(builder.finalizedSceneNode());
}

void QGLObjSceneHandler::loadMaterialLibrary(const QString& name)
{
    QUrl materialUrl = url().resolved(name);
    if (materialUrl.scheme() == QLatin1String("file")) {
        QFile file(materialUrl.toLocalFile());
        if (!file.open(QIODevice::ReadOnly))
            qWarning() << "QGLObjSceneHandler::loadMaterialLibrary: could not open:" << materialUrl.toLocalFile();
        else
            loadMaterials(&file);
    } else {
        // TODO
        qWarning("QGLObjSceneHandler::loadMaterialLibrary: non-file urls not supported");
    }
}

void QGLObjSceneHandler::loadMaterials(QIODevice *device)
{
    QByteArray line;
    QByteArray keyword;
    int posn, index;
    QGLMaterial *material = 0;
    QString materialName;
    QString textureName;

    while (!device->atEnd()) {
        // Read the next line, including any backslash continuations.
        line = device->readLine().trimmed();
        while (line.endsWith('\\')) {
            line.truncate(line.size() - 1);
            if (device->atEnd())
                break;
            line += device->readLine().trimmed();
        }
        if (line.startsWith('#') || line.isEmpty())
            continue;   // Skip comments and blank lines.

        // Extract the keyword at the start of the line.
        posn = 0;
        while (posn < line.size() &&
               line[posn] != ' ' && line[posn] != '\t')
            ++posn;
        keyword = line.left(posn);

        // Determine how to process this line from the keyword.
        if (keyword == "newmtl") {
            // Start a new material definition.
            posn = objSkipWS(line, posn);
            QByteArray rest = line.mid(posn);
            materialName = QString::fromLocal8Bit(rest.constData(), rest.size());
            index = palette->indexOf(materialName);
            if (index != -1) {
                qWarning() << "redefining obj material:" << materialName;
                material = palette->material(index);
            } else {
                material = new QGLMaterial();
                material->setObjectName(materialName);
                palette->addMaterial(material);
            }
        } else if (keyword == "Ka") {
            // Ambient color of the material.
            if (material)
                material->setAmbientColor(objReadColor(line, posn));
        } else if (keyword == "Kd") {
            // Diffuse color of the material.
            if (material)
                material->setDiffuseColor(objReadColor(line, posn));
        } else if (keyword == "Ks") {
            // Specular color of the material.
            if (material)
                material->setSpecularColor(objReadColor(line, posn));
        } else if (keyword == "map_Kd") {
            // Texture associated with the material.
            posn = objSkipWS(line, posn);
            QByteArray rest = line.mid(posn);
            textureName = QString::fromLocal8Bit(rest.constData(), rest.size());
            QGLTexture2D *texture = loadTexture(textureName);
            if (texture) {
                index = palette->indexOf(materialName);
                if (index >= 0) {
                    QGLMaterial *material = palette->material(index);
                    texture->setParent(material);
                    material->setTexture(texture);
                } else {
                    delete texture;
                }
            }
        } else if (keyword == "d") {
            // "Dissolve factor" of the material, which is its opacity.
            if (material) {
                qreal alpha = objReadFloat(line, &posn);
                QColor ambient = material->ambientColor();
                QColor diffuse = material->diffuseColor();
                ambient.setAlphaF(alpha);
                diffuse.setAlphaF(alpha);
                material->setAmbientColor(ambient);
                material->setDiffuseColor(diffuse);
            }
        } else if (keyword == "Ns") {
            // Specular exponent of the material.
            if (material)
                material->setShininess(qRound(objReadFloat(line, &posn)));
        } else if (keyword == "illum") {
            // Illumination model - ignored at present.
        } else if (keyword == "Ni") {
            // Optical density - ignored at present.
        } else {
            qWarning() << "unsupported obj material command: " << keyword.constData();
        }
    }
}

QGLTexture2D *QGLObjSceneHandler::loadTexture(const QString& name)
{
    QUrl textureUrl = url().resolved(name);
    if (textureUrl.scheme() == QLatin1String("file")) {
        QFile file(textureUrl.toLocalFile());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "QGLObjSceneHandler::loadTexture: could not open:" << textureUrl.toLocalFile();
            return 0;
        } else {
            file.close();
            QImage image(textureUrl.toLocalFile());
            QGLTexture2D *tex = new QGLTexture2D();
            tex->setImage(image);
            return tex;
        }
    } else {
        // TODO
        qWarning("QGLObjSceneHandler::loadTexture: non-file urls not supported");
        return 0;
    }
}

QT_END_NAMESPACE
