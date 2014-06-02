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

#ifndef QT3D_QABSTRACTSCENE_H
#define QT3D_QABSTRACTSCENE_H

#include <Qt3DCore/entity.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class AbstractSceneParser;

namespace Render {

class QAbstractScene;

class QAbstractScenePrivate
{
public:

    QAbstractScenePrivate(QAbstractScene *qq)
        : q_ptr(qq)
        , m_sceneChild(Q_NULLPTR)
        , m_currentParser(Q_NULLPTR)
    {}

    Q_DECLARE_PUBLIC(QAbstractScene)
    QAbstractScene *q_ptr;

    QString m_source;
    QString m_sceneId;
    Node *m_sceneNode;
    Node *m_sceneChild;
    AbstractSceneParser *m_currentParser;
};

class QT3DRENDERERSHARED_EXPORT QAbstractScene
{
public:
    explicit QAbstractScene(Node *sceneNode);

    QString source() const;
    void setSource(QString arg);

    QString sceneId() const;
    void setSceneId(QString arg);

    virtual Node *node(QString id);
    virtual Node *scene(QString id);

    void clear();

    /**
     * @brief findInTree - given a Node* object rooting a tree, find
     * the top-most Scene entity within.
     * @param root - the found Scene or NULL if no Scene was found
     * @return
     */
    static QAbstractScene* findInTree(Node* root);
Q_SIGNALS:

    virtual void sourceChanged(QString arg) = 0;
    virtual void sceneIdChanged(QString arg) = 0;

private:
    void rebuild();
    Q_DECLARE_PRIVATE(QAbstractScene)
    QAbstractScenePrivate *d_ptr;

};

} // Render

} // namespace Qt3D

Q_DECLARE_INTERFACE(Qt3D::Render::QAbstractScene, "org.qt-project.org.Qt3D.QAbstractScene/2.0")

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTSCENE_H
