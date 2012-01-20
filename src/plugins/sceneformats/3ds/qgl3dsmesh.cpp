/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgl3dsmesh.h"
#include "qglmaterialcollection.h"
#include "qglsection_p.h"
#include "qarray.h"
#include "qvector_utils_p.h"

#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qmath.h>

// Faceted meshes look terrible when they have more than a small number
// of faces.  Usually if large meshes are faceted its some kind of error
// in the model, or in importing of the model by lib3ds.  Force on
// smoothing when greater than this many faces are detected.
#define FACETED_THRESHOLD 1000

// Two faces that have an angle between their plane vectors with a cosine
// less than this are judged to form a sharp (acute) angle.
// -ve cosine (less than 0.0f) means 90 degrees or sharper like the sides
// of a rectangular prism so this is a good value.
#define ACUTE -0.0001f

// Two vectors that have an angle between them with a cosine less than this
// value are judged to be approximately the inverse of each other, for the
// purposes of determining whether a normal has been inverted or not
#define INVERSE -0.20f

// Only try to correct normals if this many neighbour faces contribute to
// the assessment.  2 or 3 are good values.  4 or greater is the same as
// defining DO_NORMAL_CORRECT false
#define AVG_CNT 2

// make a QVector3D from a Lib3dsVector - by a hard cast.  Its required
// that QVector3D behave like a POD type - float[3] - for all of the
// OpenGL stuff
static inline QVector3D &l2v(Lib3dsVector &vec)
{
    return reinterpret_cast<QVector3D&>(vec);
}

// is this a null 3ds vector - same as QVector3D::isNull()
static inline bool qIsNull(const Lib3dsVector &vec)
{
    return (qIsNull(vec[0]) && qIsNull(vec[1]) && qIsNull(vec[2]));
}

static inline bool qFskCompare(const Lib3dsVector &a, const Lib3dsVector &b)
{
    return (qFskCompare(a[0], b[0]) && qFskCompare(a[1], b[1]) && qFskCompare(a[2], b[2]));
}

// find the normalized plane vector, that is a unit vector perpendicular
// to the plane of the face.  this is the same thing as the default normal
static inline void planeVec(Lib3dsFace *face, Lib3dsPoint *pointList, Lib3dsVector result)
{
    Lib3dsVector &l3a = pointList[face->points[0]].pos;
    Lib3dsVector &l3b = pointList[face->points[1]].pos;
    Lib3dsVector &l3c = pointList[face->points[2]].pos;
    lib3ds_vector_normal(result, l3a, l3b, l3c);
}

struct ModulateRecord
{
    bool disabled;
    bool keyFresh;
    int facesProcessed;
    int numModulated;
    Lib3dsDword altKey;
    Lib3dsDword key;
};

QGL3dsMesh::QGL3dsMesh(Lib3dsMesh *mesh, QGLMaterialCollection *materials)
    : QGLBuilder(materials)
    , m_mesh(mesh)
    , m_texFlip(false)
    , m_hasZeroSmoothing(false)
    , m_faceMap(0)
{
}

QGL3dsMesh::~QGL3dsMesh()
{
    delete[] m_faceMap;
}

void QGL3dsMesh::processNodeForMaterial(int matIx, QGLSceneNode *node)
{
    QGLSceneNode *s = sceneNode();
    QString baseName = s->objectName();
    node->setMaterialIndex(matIx);
    node->setObjectName(baseName + QLatin1String("::") +
                        ((matIx == -1)
                         ? QLatin1String("No_Material")
                             : s->palette()->materialName(matIx)));
    checkTextures(matIx);
    generateVertices();
    s->palette()->markMaterialAsUsed(matIx);
}

void QGL3dsMesh::initAdjacencyMap()
{
    Lib3dsFace *face;
    m_faceMap = new FacePtr[m_mesh->faces * 3];
    qMemSet(m_faceMap, 0, sizeof(m_faceMap));
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        planeVec(face, m_mesh->pointL, face->normal);
        m_mesh->faceL[f].user.p = &m_faceMap[f*3];
    }
}

void QGL3dsMesh::initialize()
{
    QGLSceneNode *s = sceneNode();
    s->setObjectName(QString::fromLatin1(m_mesh->name));
    if ((m_options & QGL::ForceSmooth) && (m_options & QGL::ForceFaceted))
    {
        if (m_options & QGL::ShowWarnings)
            qWarning("Both smooth and faceted forced on for %s: forcing smooth\n",
                     m_mesh->name);
        m_options &= ~QGL::ForceFaceted;
    }
    initAdjacencyMap();
    if (m_options & (QGL::CorrectNormals | QGL::CorrectAcute))
        modulateMesh();
    analyzeMesh();

    if (m_smoothingGroups == 0)
    {
        bool forceSmooth = (m_options & QGL::ForceSmooth);
        if (!forceSmooth && m_mesh->faces > FACETED_THRESHOLD)
        {
            if (m_options & QGL::ShowWarnings)
                qWarning("Mesh %s has %d faces (threshold is %d):"
                        "forcing smooth render", m_mesh->name, m_mesh->faces,
                        FACETED_THRESHOLD);
            forceSmooth = true;
        }
        if (forceSmooth)
        {
            for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
                m_mesh->faceL[f].smoothing = 1;
            analyzeMesh();
        }
    }

    bool mixedTexturedAndPlain = m_plainMaterials.count() > 0 &&
                                 m_textureMaterials.count() > 0;

    s->setLocalTransform(meshMatrix());

    // start a new section and node
    newSection(m_smoothingGroups ? QGL::Smooth : QGL::Faceted);
    QGLSceneNode *node = currentNode();

    // process all the plain materials first, then textured to avoid effect swapping
    QList<int> matList = m_plainMaterials.toList();
    if (mixedTexturedAndPlain)
    {
        node->setEffect(QGL::LitMaterial);
        node->setObjectName(s->objectName() + QLatin1String("::Materials"));
        //qDebug() << ">>> mixed:" << node;
        pushNode();
        //qDebug() << "   pushed - current now:" << currentNode();
    }
    else
    {
        s->setEffect(m_textureMaterials.count() > 0 ? QGL::LitModulateTexture2D : QGL::LitMaterial);
    }
    //qDebug() << "processing:" << matList.count() << "materials";
    while (matList.count() > 0)
    {
        int matIx = matList.takeFirst();
        processNodeForMaterial(matIx, node);
        if (matList.count() > 0)
            node = newNode();
    }
    matList = m_textureMaterials.toList();
    if (mixedTexturedAndPlain)
    {
        //qDebug() << "<<< mixed:" << currentNode();
        popNode();
        //qDebug() << "   popped - current now:" << currentNode();
        node = currentNode();
        node->setEffect(QGL::LitModulateTexture2D);
        node->setObjectName(s->objectName() + QLatin1String("::Textures"));
    }
    while (matList.count() > 0)
    {
        int matIx = matList.takeFirst();
        processNodeForMaterial(matIx, node);
        if (matList.count() > 0)
            node = newNode();
    }
}

// Build a linked list, in a QArray: the first N*2 entries correspond
// to the N vertices: for each n'th vertex, n*2 is the face number,
// n*2+1is the index of the next entry for that vertex, or -1 if there
// is no next entry.
//
// While on the job find the smoothing keys and store in allKeys.
//
// And zero out the user data pointer for each face, and calculate the
// smoothing group count.
QArray<int> QGL3dsMesh::mapFacesToVerts(Lib3dsDword *allKeys)
{
    Lib3dsFace *face;
    QArray<int> vlist(m_mesh->points * 2, -1);
    int nx = 2 * m_mesh->points;
    m_smoothingGroupCount = 0;
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        if (face->smoothing)
        {
            if (!(face->smoothing & *allKeys))
            {
                *allKeys |= face->smoothing;
                m_smoothingGroupCount += 1;
            }
        }
        for (int i = 0; i < 3; ++i)
        {
            int v = face->points[i];
            int ptr = v*2;
            int prv = -1;
            if (vlist[ptr] != -1)
            {
                prv = ptr;
                while (vlist[ptr+1] != -1)
                {
                    ptr = vlist[ptr+1];
                    prv = ptr;
                }
                ptr = nx;
                vlist.extend(2);
                nx += 2;
            }
            vlist[ptr] = f;
            vlist[ptr+1] = -1;
            if (prv != -1)
                vlist[prv+1] = ptr;
        }
    }
    return vlist;
}

void QGL3dsMesh::addToAdjacencyMap(Lib3dsFace *face, Lib3dsFace *neighbour)
{
    Q_ASSERT(face);
    Q_ASSERT(face->user.p);
    Q_ASSERT(neighbour);
    FacePtr *h = static_cast<FacePtr*>(face->user.p);
    while (*h != 0)
        ++h;
    *h = neighbour;
}

// add to each face a linked list of its <= 3 neighbouring faces
// and also ensure each face knows is own normal.  Rather than doing
// lots of small allocs, just assume every face has 3 neighbours
// and allocate the list nodes in one big lump - the m_faceMap.
// use the user data pointer in the lib3ds face struct to point
// to the first node in the list.
void QGL3dsMesh::buildAdjacencyMap(const QArray<int> &vlist)
{
    Lib3dsFace *face;
    Lib3dsFace *nbr;
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        // for each edge (where an edge is a pair of verts) find ones
        // adjacent, ie both verts in edge have same face (which is
        // not this face).
        face = &m_mesh->faceL[f];
        for (int i = 0; i < 3; ++i)
        {
            Q_ASSERT(face->points[i] < m_mesh->points);
            int vi = 2 * face->points[i];
            int neighbouri = -1;
            while (true)
            {
                neighbouri = vlist[vi];
                if (neighbouri != (int)f)
                {
                    Q_ASSERT(face->points[(i + 1) % 3] < m_mesh->points);
                    int vj = 2 * face->points[(i + 1) % 3];
                    int neighbourj = -1;
                    while (true)
                    {
                        neighbourj = vlist[vj];
                        if (neighbourj == neighbouri)
                        {
                            nbr = &m_mesh->faceL[neighbouri];
                            // only add as neighbour if at least one smoothing group
                            // is shared - still have to test again below for each group
                            if (nbr->smoothing & face->smoothing)
                                addToAdjacencyMap(face, nbr);
                            break;
                        }
                        if (vlist[vj+1] == -1)
                            break;
                        vj = vlist[vj+1];
                    }
                }
                if (vlist[vi+1] == -1)
                    break;
                else
                    vi = vlist[vi+1];
            }
        }
    }
}

static inline void incOrWarn(ModulateRecord *mod)
{
    if (mod->altKey != 0x80000000)
    {
        mod->altKey <<= 1;
    }
    else
    {
        qWarning("Overflowed smoothing keys - modulation disabled.");
        mod->disabled = true;
    }
}

static inline void nextUnusedKey(ModulateRecord *mod, const Lib3dsDword &allKeys)
{
    if (!mod->disabled)
    {
        if (mod->altKey != 0)
            incOrWarn(mod);
        else
        {
            mod->altKey = 1;  // initialize
        }
        while ((mod->altKey & allKeys) && !mod->disabled)
        {
            incOrWarn(mod);
        }
        mod->keyFresh = true;
    }
}

static int numCorrected = 0;
static int numWindingCorrected = 0;

static inline void doNormalCorrect(Lib3dsFace *face)
{
    QGL3dsMesh::FacePtr *n = reinterpret_cast<QGL3dsMesh::FacePtr*>(face->user.p);
    int acnt = 0;
    Lib3dsVector avgn = { 0 };
    for (; *n; ++n)
    {
        if ((*n)->smoothing & face->smoothing)
        {
            lib3ds_vector_add(avgn, avgn, (*n)->normal);
            ++acnt;
        }
    }
    if (acnt >= AVG_CNT)
    {
        lib3ds_vector_normalize(avgn);
        float na = lib3ds_vector_dot(face->normal, avgn);
        if (na < INVERSE)
        {
            //qWarning("corrected: %p - %d, %d, %d", face,
            //        face->points[0], face->points[1], face->points[2]);
            //lib3ds_vector_dump(face->normal);
            lib3ds_vector_neg(face->normal);
            numWindingCorrected++;
            qSwap(face->points[1], face->points[2]);
        }
    }
}

static inline void modFace(Lib3dsFace *face, ModulateRecord *mod)
{
    ++mod->numModulated;
    face->smoothing &= ~mod->key;    // remove old key
    face->smoothing |= mod->altKey;  // add in new key
    ::strncpy(face->material, "bright-red", 60);  //debug
}

static inline void doModulate(Lib3dsFace *face, ModulateRecord *mod)
{
    QGL3dsMesh::FacePtr *n = reinterpret_cast<QGL3dsMesh::FacePtr*>(face->user.p);
    ++mod->facesProcessed;
    for ( ; *n; ++n)
    {
        QGL3dsMesh::FacePtr neighbour = *n;
        if (neighbour->smoothing & face->smoothing)
        {
            if (lib3ds_vector_dot(face->normal, neighbour->normal) < ACUTE)
            {
                qWarning("Modulated due to ACUTE\n");
                modFace(neighbour, mod);
                mod->keyFresh = false;
            }
        }
    }
}

bool operator<(const QVector3D &a, const QVector3D &b)
{
    if (qFskCompare(a.x(), b.x()))
    {
        if (qFskCompare(a.y(), b.y()))
        {
            if (qFskCompare(a.z(), b.z()))
            {
                return false; // they're equal a is not less than b
            }
            else
            {
                return a.z() < b.z();
            }
        }
        else
        {
            return a.y() < b.y();
        }
    }
    else
    {
        return a.x() < b.x();
    }
}

// Go thru each smoothing group - we don't care about the zero group since
// they're already faceted.  In each group process all connected faces
// starting the queue off with a seed face (the first face in the group).
// Set the alternate group to be the first unused smoothing group key.
//
// To process a face: add the face to the processed set; for each adjacent
// face if its in this same smoothing group, add it to the queue.
//
// Modulate smoothing mode: if an adjacent face is at an acute angle to this
// face, move it to the alternate group.  Note that the alternate groups are
// not further iterated since they are not in "each smoothing group"
// (the allKeys variable).
//
// When the queue is empty, go to the next smoothing group in the mesh.

// Normal repair mode:  if a face normal is flipped in error, ie a mistake
// in model building (typically points specified in the wrong order)
// this can be detected and repaired.  The normal is flipped if the inverse
// of the normal is at an acute angle to the average of neighbour normals.
// If detected the normals is corrected and the winding is also checked to
// see if it needs to be corrected also.
void QGL3dsMesh::modulateMesh()
{
    if (!(m_options & (QGL::CorrectNormals | QGL::CorrectAcute)))
        return;
    FacePtr face;
    Lib3dsDword allKeys = 0;
    QArray<int> vlist = mapFacesToVerts(&allKeys);
    buildAdjacencyMap(vlist);
    QSet<FacePtr> processed;
    ModulateRecord mod;
    qMemSet(&mod, 0, sizeof(struct ModulateRecord));
    mod.key = 1;
    while ((mod.key < allKeys) && !mod.disabled && mod.key)
    {
        if (!(allKeys & mod.key))
        {
            ++mod.key;
            continue;
        }
        QArray<FacePtr> queue;
        Lib3dsDword fptr = 0;
        int head = 0;
        while (true)
        {
            if (head >= queue.size())   // seed another island of faces
            {
                if (!mod.keyFresh && head > 0)  // if not first time thru, did we use last key?
                    allKeys = allKeys | mod.altKey;
                FacePtr qf = 0;
                for (  ; fptr < m_mesh->faces; ++fptr)
                {
                    qf = &m_mesh->faceL[fptr];
                    if ((mod.key & qf->smoothing) && !processed.contains(qf))
                        break;
                }
                if (fptr == m_mesh->faces)
                    break;
                queue.append(qf);
                if (!mod.keyFresh)
                {
                    nextUnusedKey(&mod, allKeys);
                    if (mod.disabled)
                        break;
                }
            }
            face = queue.at(head++);
            processed.insert(face);
            doNormalCorrect(face);
            doModulate(face, &mod);
            FacePtr *n = reinterpret_cast<FacePtr*>(face->user.p);
            for ( ; *n; ++n)
                if ((mod.key & (*n)->smoothing) && !processed.contains(*n))
                    queue.append(*n);
        }
    }
    if ((m_options & QGL::CorrectNormals) && (m_options & QGL::ShowWarnings))
        qDebug() << "CorrectNormals mode:" << numCorrected << "normals corrected.";
    if ((m_options & QGL::CorrectAcute) && (m_options & QGL::ShowWarnings))
        qDebug() << "CorrectAcute mode:" << mod.numModulated << "normals corrected";
}

int QGL3dsMesh::cachedMaterialLookup(const char *material)
{
    static bool initialized = false;
    static int lastLookup = -1;
    static char lastName[512];
    if (!initialized)
    {
        ::memset(lastName, 0, 512);
        initialized = true;
    }
    if (qstrncmp(lastName, material, 510) != 0)
    {
        lastLookup = sceneNode()->palette()->indexOf(QString::fromLatin1(material));
        qstrncpy(lastName, material, 510);
    }
    return lastLookup;
}

/*!
    \internal
    Find material indexes and smoothing groups used in this mesh.  If any
    face has no material assigned then a -1 index will be listed.
    Also figures out how complex the mesh is, by finding the count of
    smoothing groups for the material with the greatest number of groups.
    If the mesh is faceted (no smoothing) but has greater than FACETED_THRESHOLD
    faces then smoothing is forced on and the mesh rescanned.

*/
void QGL3dsMesh::analyzeMesh()
{
    QGLMaterialCollection *pal = sceneNode()->palette();
    Lib3dsFace *face;
    Lib3dsDword allKeys = 0;
    m_smoothingGroupCount = 0;
    m_hasZeroSmoothing = false;
    m_plainMaterials.clear();
    m_textureMaterials.clear();
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        int matIx = cachedMaterialLookup(face->material);
#ifndef QT_NO_DEBUG_STREAM
        if (matIx == -1 && strlen(face->material) != 0 && (m_options & QGL::ShowWarnings))
            qDebug("Bad .3ds file: no material %s! (Referenced in mesh %s)\n",
                     face->material, m_mesh->name);
#endif
        if (face->smoothing)
        {
            if ((face->smoothing & allKeys) != face->smoothing)
            {
                Lib3dsDword key = 1;
                while (key)
                {
                    if ((key & face->smoothing) && !(allKeys & key))
                    {
                        allKeys = allKeys | key;
                        m_smoothingGroupCount += 1;
                    }
                    key <<= 1;
                }
            }
        }
        else
        {
            m_hasZeroSmoothing = true;
        }
        if (!m_plainMaterials.contains(matIx) && !m_textureMaterials.contains(matIx))
        {
            QGLMaterial *mat = pal->material(matIx);
            if (mat && mat->texture())
                m_textureMaterials.insert(matIx);
            else
                m_plainMaterials.insert(matIx);
        }
    }
    m_smoothingGroups = allKeys;
}

/*!
    \internal
    Check the meshes textures and update the record of whether this mesh
    has textures or not.  In debug mode issue a warning if the textures are
    corrupt (number of texels and vertices not equal).
*/
void QGL3dsMesh::checkTextures(int material)
{
    QGLMaterial *mat = sceneNode()->palette()->material(material);
    QGLTexture2D *tex = (mat ? mat->texture() : 0);
    m_hasTextures = false;
    if (tex)
    {
        m_hasTextures = true;
#ifndef QT_NO_DEBUG_STREAM
        if (m_mesh->points != m_mesh->texels)
            qWarning("Mesh %s has unequal number of texels (%d) and vertices (%d)",
                     m_mesh->name, m_mesh->texels, m_mesh->points);
#endif
        // all texture coordinates from 3ds have to be flipped because
        // 3ds uses the correct coordinate system, whilst qt uses
        // upside-down coordinates
        m_texFlip = (tex->bindOptions() & QGLTexture2D::InvertedYBindOption) == 0;
    }
}

/*!
    \internal
    Returns any local transformation matrix for the mesh.
*/
QMatrix4x4 QGL3dsMesh::meshMatrix() const
{
    Lib3dsMatrix &m = m_mesh->matrix;  // typedef for float[4][4]
    QMatrix4x4 mat;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row) {
            float e = m[col][row];
            if (qFuzzyIsNull(e))
                mat(row, col) = 0.0f;
            else
                mat(row, col) = e;
        }
    mat.optimize();  // setup to use optimizations
    if (mat.isIdentity())
        return mat;
    // The reverse transform is what we apply to model-view in order
    // to draw the underlying geometry
    bool invertible = true;
    mat = mat.inverted(&invertible);
    if (invertible)
         return mat;
    if (m_options & QGL::ShowWarnings)
        qWarning("Could not invert matrix for mesh %s", m_mesh->name);
    return QMatrix4x4();
}

/*!
    \internal
    Generate the vertices for the faces based on their smoothing keys and
    the current nodes material.
*/
void QGL3dsMesh::generateVertices()
{
    int matIx = currentNode()->materialIndex();
    int keyCount = m_smoothingGroupCount;
    if (m_hasZeroSmoothing)
        ++keyCount;
    QString baseName = currentNode()->objectName();
    Lib3dsDword key = 0;
    while (key <= m_smoothingGroups)
    {
        if ((key & m_smoothingGroups) || ((key == 0) && m_hasZeroSmoothing))
        {
            if (key == 0)
                currentSection()->setSmoothing(QGL::Faceted);
            keyCount -= 1;
            currentNode()->setMaterialIndex(matIx);
            currentNode()->setObjectName(baseName + QLatin1String("::") + QString::number(key));
            QGeometryData tri;
            int cur = 0;
            for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
            {
                Lib3dsFace *face = &m_mesh->faceL[f];
                int faceMat = cachedMaterialLookup(face->material);
                if (faceMat == matIx &&
                    ((key & face->smoothing) || (key == 0 && face->smoothing == 0)))
                {
                    QVector3D norm = l2v(face->normal);
                    for (int i = 0; i < 3; ++i)
                    {
                        int a = face->points[i];
                        Lib3dsVector &l3a = m_mesh->pointL[a].pos;
                        tri.appendVertex(l2v(l3a));
                        tri.appendNormal(norm);
                        if (m_hasTextures)
                        {
                            Lib3dsTexel &t0 = m_mesh->texelL[a];
                            tri.appendTexCoord(QVector2D(t0[0], m_texFlip ? 1.0f - t0[1] : t0[1]));
                        }
                        if (m_options & QGL::NativeIndices)
                            currentSection()->appendSmooth(tri.logicalVertexAt(cur++), a);
                    }
                }
            }
            if (m_options & QGL::NativeIndices)
                currentNode()->setCount(cur);
            else
                addTriangles(tri);
            if (keyCount > 0)
                newSection(QGL::Smooth);
            else
                break;
        }
        if (key == 0)
            key = 1;
        else
            key <<= 1;
    }
}
