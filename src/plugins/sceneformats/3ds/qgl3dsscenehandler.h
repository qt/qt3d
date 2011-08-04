/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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

#ifndef QGL3DSSCENEHANDLER_H
#define QGL3DSSCENEHANDLER_H

#include "qglsceneformatplugin.h"
#include <QtCore/qmap.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace QGL {
    enum ModelOption
    {
        NativeIndices     = 0x01,
        CorrectNormals    = 0x02,
        CorrectAcute      = 0x04,
        ForceSmooth       = 0x08,
        ForceFaceted      = 0x10,
        ShowWarnings      = 0x20
    };
    Q_DECLARE_FLAGS(ModelOptions, ModelOption);

    typedef QMap<QString, QGL::ModelOptions> MeshOptionMap;
};

class QGL3dsSceneHandler : public QGLSceneFormatHandler
{
public:
    QGL3dsSceneHandler();
    ~QGL3dsSceneHandler();

    QGLAbstractScene *read();

    void setOptions(QGL::ModelOptions options) { m_options |= options; }
    QGL::ModelOptions options() const { return m_options; }

    void setMeshOptions(QGL::ModelOptions options, const QString &meshName)
    {
        m_meshOptions[meshName] |= options;
    }
    QGL::ModelOptions meshOptions(const QString &meshName) const
    {
        if (m_meshOptions.contains(meshName))
            return m_meshOptions.value(meshName);
        return 0;
    }
    QGL::MeshOptionMap meshOptions() const { return m_meshOptions; }
    void decodeOptions(const QString &options);

private:
    QGL::ModelOptions m_options;
    QGL::MeshOptionMap m_meshOptions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGL::ModelOptions);

QT_END_NAMESPACE

#endif
