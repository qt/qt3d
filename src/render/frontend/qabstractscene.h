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
class QAbstractScenePrivate;

class QT3DRENDERERSHARED_EXPORT QAbstractScene : public Entity
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString sceneId READ sceneId WRITE setSceneId NOTIFY sceneIdChanged)
public:
    explicit QAbstractScene(QNode *parent = 0);

    QString source() const;
    void setSource(QString arg);

    QString sceneId() const;
    void setSceneId(QString arg);

    virtual QNode *node(QString id);
    virtual QNode *scene(QString id);

    void clear();

    /**
     * @brief findInTree - given a Node* object rooting a tree, find
     * the top-most Scene entity within.
     * @param root - the found Scene or NULL if no Scene was found
     * @return
     */
    static QAbstractScene* findInTree(QNode* root);
Q_SIGNALS:

    void sourceChanged(QString arg);
    void sceneIdChanged(QString arg);

protected:
    void rebuild();
    Q_DECLARE_PRIVATE(QAbstractScene)
    QAbstractScene(QAbstractScenePrivate &dd, QNode *parent = 0);
};

} // Render

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTSCENE_H
