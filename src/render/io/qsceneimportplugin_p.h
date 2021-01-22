/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QSCENEIMPORTPLUGIN_P_H
#define QSCENEIMPORTPLUGIN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QFactoryInterface>

#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

#define QSceneImportFactoryInterface_iid "org.qt-project.Qt3DRender.QSceneImportFactoryInterface 5.7"

class QSceneImporter;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QSceneImportPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QSceneImportPlugin(QObject *parent = nullptr);
    ~QSceneImportPlugin();

    virtual QSceneImporter *create(const QString &key, const QStringList &paramList);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QSCENEIMPORTPLUGIN_P_H
