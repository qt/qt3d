/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QAISCENEHANDLER_H
#define QAISCENEHANDLER_H

#include <Qt3D/qglsceneformatplugin.h>

#include "aiPostProcess.h"
#include "assimp.hpp"

#include <QtCore/qurl.h>
#include <QObject>

Q_DECLARE_FLAGS(aiPostProcessFlags, aiPostProcessSteps)
Q_DECLARE_OPERATORS_FOR_FLAGS(aiPostProcessFlags)


QT_BEGIN_NAMESPACE

class QIODevice;

class QAiSceneHandler : public QGLSceneFormatHandler
{
    Q_OBJECT
public:
    enum Options {
        NoOptions,
        ShowWarnings,        // show any warnings while loading the file
        CalculateNormals,    // replace normals from the file with smooth generated ones
        ForceFaceted,        // generate non-smooth normals (implies CalculateNormals)
        ForceSmooth,         // deprecated - retained only for backward compatibility
        IncludeAllMaterials, // include even redundant (unused) materials
        IncludeLinesPoints,  // include even collapsed triangles (lines or points)
        FixNormals,          // try to fix incorrect (in facing) normals
        DeDupMeshes,         // replace copied meshes with refs to a single instance
        Optimize,            // collapse meshes, nodes & scene heierarchies
        FlipUVs,             // flips UV's on the y-axis (for upside-down textures)
        FlipWinding,         // makes faces CW instead of CCW
        UseVertexColors,     // use vertex colors that are in a model
        VertexSplitLimitx2,  // double the vertex count which will split a large mesh
        TriangleSplitLimitx2 // double the triangle count which will split a large mesh
    };

    QAiSceneHandler();
    ~QAiSceneHandler();

    static QStringList supportedFormats();

    QGLAbstractScene *read();
    QGLAbstractScene *download();

    void decodeOptions(const QString &options);

    bool showWarnings() const { return m_showWarnings; }
    bool mayHaveLinesPoints() const { return m_mayHaveLinesPoints; }

    aiPostProcessFlags options() const { return m_options; }
    quint32 removeComponentFlags() const { return m_removeComponentFlags; }
    quint32 removeSortFlags() const { return m_removeSortFlags; }

public Q_SLOTS:
    void downloadComplete(QByteArray *sceneData);

private:
    aiPostProcessFlags m_options;
    bool m_showWarnings;
    bool m_mayHaveLinesPoints;
    int m_meshSplitVertexLimit;
    int m_meshSplitTriangleLimit;
    Assimp::Importer m_importer;
    quint32 m_removeComponentFlags;
    quint32 m_removeSortFlags;
};

QT_END_NAMESPACE

#endif
