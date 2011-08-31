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

#ifndef QGLABSTRACTSCENE_H
#define QGLABSTRACTSCENE_H

#include "qt3dglobal.h"
#include "qglscenenode.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLAbstractScenePrivate;
class QIODevice;
class QGLPickNode;

class Q_QT3D_EXPORT QGLAbstractScene : public QObject
{
    Q_OBJECT
public:
    explicit QGLAbstractScene(QObject *parent = 0);
    virtual ~QGLAbstractScene();

    virtual void setPickable(bool enable);
    virtual bool pickable() const;
    virtual void generatePickNodes();
    QList<QGLPickNode *> pickNodes() const;
    int nextPickId();

    virtual QList<QObject *> objects() const = 0;
    virtual QStringList objectNames() const;
    virtual QObject *object(const QString& name) const;
    virtual QGLSceneNode *mainNode() const = 0;

    static QGLAbstractScene *loadScene
        (const QUrl& url, const QString& format = QString(),
        const QString& options = QString());
    static QGLAbstractScene *loadScene
        (QIODevice *device, const QUrl& url, const QString& format = QString(),
     const QString& options = QString());
    static QGLAbstractScene *loadScene
        (const QString& fileName, const QString& format = QString(),
     const QString& options = QString());

    enum FormatListType {
        AsFilter, AsSuffix
    };

    static QStringList supportedFormats(FormatListType t = AsFilter);

signals:
    void sceneUpdated();

protected:
    void childEvent(QChildEvent * event);

private:
    QScopedPointer<QGLAbstractScenePrivate> d_ptr;

    Q_DISABLE_COPY(QGLAbstractScene)
    Q_DECLARE_PRIVATE(QGLAbstractScene)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
