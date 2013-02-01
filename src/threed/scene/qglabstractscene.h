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

#ifndef QGLABSTRACTSCENE_H
#define QGLABSTRACTSCENE_H

#include <Qt3D/qt3dglobal.h>
#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglsceneanimation.h>

#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_NAMESPACE

class QGLAbstractScenePrivate;
class QIODevice;
class QGLPickNode;
class QGLSceneAnimation;

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

    virtual QList<QGLSceneAnimation *> animations() const;

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

Q_SIGNALS:
    void sceneUpdated();

protected:
    void childEvent(QChildEvent * event);

private:
    QScopedPointer<QGLAbstractScenePrivate> d_ptr;

    static void checkSupportedFormats();
    static bool m_bFormatListReady;
    static QStringList m_Formats;
    static QStringList m_FormatsFilter;

    Q_DISABLE_COPY(QGLAbstractScene)
    Q_DECLARE_PRIVATE(QGLAbstractScene)
};

QT_END_NAMESPACE

#endif
