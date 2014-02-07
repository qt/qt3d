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

#ifndef QGLMATERIALCOLLECTION_H
#define QGLMATERIALCOLLECTION_H

#include <Qt3D/qt3dglobal.h>
#include <Qt3D/qglmaterial.h>
#include <Qt3D/qgltexture2d.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QGLMaterialCollectionPrivate;

class Q_QT3D_EXPORT QGLMaterialCollection : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLMaterialCollection)
    Q_DISABLE_COPY(QGLMaterialCollection)
public:
    QGLMaterialCollection(QObject *parent = 0);
    virtual ~QGLMaterialCollection();

    QGLMaterial *material(int index) const;
    QGLMaterial *material(const QString &name) const;

    bool contains(QGLMaterial *material) const;
    bool contains(const QString &name) const;

    int indexOf(QGLMaterial *material) const;
    int indexOf(const QString &name) const;

    QString materialName(int index) const;

    bool isMaterialUsed(int index) const;
    void markMaterialAsUsed(int index);
    void removeUnusedMaterials();

    int addMaterial(QGLMaterial *material);
    void removeMaterial(QGLMaterial *material);
    QGLMaterial *removeMaterial(int index);

    bool isEmpty() const;
    int size() const;

private Q_SLOTS:
    void materialDeleted();

private:
    QScopedPointer<QGLMaterialCollectionPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QGLMATERIALCOLLECTION_H
