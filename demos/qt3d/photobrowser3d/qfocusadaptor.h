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

#ifndef QFOCUSADAPTOR_H
#define QFOCUSADAPTOR_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QGLView;
class QGLSceneNode;
QT_END_NAMESPACE

class QFocusAdaptorPrivate;

class QFocusAdaptor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
public:
    explicit QFocusAdaptor(QObject *parent = 0);
    ~QFocusAdaptor();

    qreal progress() const;
    void setProgress(qreal progress);

    QGLView *view() const;
    void setView(QGLView *view);

    QGLSceneNode *target() const;
    void setTarget(QGLSceneNode *target);

signals:
    void progressChanged();

public slots:

private:
    void calculateValues();

    QFocusAdaptorPrivate *d;
};

#endif // QFOCUSADAPTOR_H
