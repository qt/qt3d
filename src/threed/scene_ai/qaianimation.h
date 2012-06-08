/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QAIANIMATION_H
#define QAIANIMATION_H

#include "qglsceneanimation.h"
#include "aiScene.h"

#include <QtCore/qmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QAiAnimationPrivate;

class QAiAnimation : public QGLSceneAnimation
{
public:
    explicit QAiAnimation(QObject *parent = 0);
    explicit QAiAnimation(QObject *parent, const QString &name);
    ~QAiAnimation();

    qreal duration() const;

    qreal position() const;
    void setPosition(qreal pos);

    bool loop() const;
    void setLoop(bool l);

    QList<QString>          affectedNodes() const;
    QList<NodeTransform>    transformations() const;

    void loadData(aiAnimation* pSrcAnim, const QMap<QString,aiMatrix4x4>& rDefaultTransforms, QMap<QString,QGLSceneAnimation::NodeTransform>& rDefaultTransformsRaw);

public Q_SLOTS:
    void play();
    void stop();
    void pause(bool);

private:
    Q_DISABLE_COPY(QAiAnimation)
    QAiAnimationPrivate* d_ptr;

    qreal AdjustPosition(qreal newPos);
};

QT_END_NAMESPACE

QT_END_HEADER


#endif // QAIANIMATION_H
