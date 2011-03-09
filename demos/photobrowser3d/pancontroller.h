/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PANCONTROLLER_H
#define PANCONTROLLER_H

#include <QObject>
#include <Qt>

class PanControllerPrivate;
class QGLView;

class PanController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)
public:
    explicit PanController(QObject *parent = 0);
    ~PanController();

    qreal defaultDistance() const;
    void setDefaultDistance(qreal d);

    qreal panDistance() const;
    void setPanDistance(qreal d);

    qreal panViewAngle() const;
    void setPanViewAngle(qreal angle);

    qreal maxSpeed() const;
    void setMaxSpeed(qreal maxSpeed);

    qreal speed() const;
    void setSpeed(qreal speed);

    Qt::ArrowType direction() const;
    void setDirection(Qt::ArrowType arrow);

    QGLView *view() const;
    void setView(QGLView *view);

    void pan();

signals:
    void speedChanged();

private:
    PanControllerPrivate *d;
};

#endif // PANCONTROLLER_H
