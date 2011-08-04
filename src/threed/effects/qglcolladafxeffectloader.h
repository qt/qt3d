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

#ifndef QGLCOLLADAFXEFFECTLOADER_H
#define QGLCOLLADAFXEFFECTLOADER_H

#include <qglobal.h>
#include <QString>
#include <QStringList>
#include <QtCore/qscopedpointer.h>
#include "qt3dglobal.h"

class QGLColladaFxEffect;
class QGLColladaFxEffectLoaderPrivate;

class Q_QT3D_EXPORT QGLColladaFxEffectLoader
{
public:
    QGLColladaFxEffectLoader();
    ~QGLColladaFxEffectLoader();
    bool load(QString filename);
    QStringList effectNames();
    QGLColladaFxEffect *effect(QString effectName);
    int count();
    QGLColladaFxEffect* operator[](int);
private:
    Q_DECLARE_PRIVATE(QGLColladaFxEffectLoader);
    QScopedPointer<QGLColladaFxEffectLoaderPrivate> d_ptr;
};

#endif // QGLCOLLADAFXEFFECTLOADER_H
