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

#ifndef QAIANIMATION_P_H
#define QAIANIMATION_P_H

#include "qt3dglobal.h"
#include "qglsceneanimation.h"
#include <QtCore/qobject.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qquaternion.h>
#include <qarray.h>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE

enum TerminalBehaviour {
    TB_DEFAULT,
    TB_CONSTANT,
    TB_LINEAR,
    TB_REPEAT
};

template <typename T>
struct ValueTrack
{
    ValueTrack() : m_bConst(false) {}
    QArray<qreal>   m_Times;
    QArray<T>       m_Values;
    bool            m_bConst;
    T GetValue(qreal t) const
    {
        if (m_bConst) {
            Q_ASSERT(m_Times.size()==1 && m_Values.size()==1);
            return m_Values[0];
        } else {
            Q_ASSERT(m_Times.size()>=2 && m_Times.size()==m_Values.size());
            Q_ASSERT(m_Times.first()>=0.0 && m_Times.first()<0.00001);
            Q_ASSERT(t<=m_Times.last());
            unsigned int iFirst = 0;
            unsigned int iLast = m_Times.size()-1;
            for (;iLast-iFirst>1;) {
                unsigned int iProbe = (iLast+iFirst)/2;
                Q_ASSERT(iProbe!=iFirst && iProbe!=iLast);
                if (t<=m_Times[iProbe])
                    iLast = iProbe;
                else if (t>=m_Times[iProbe])
                    iFirst = iProbe;
            }
//            if (iLast-iFirst!=1) {
//                qDebug("n times  = %d",m_Times.size());
//                qDebug("n values = %d",m_Values.size());
//                qDebug("[%f .. %f .. %f]",m_Times.first(),t,m_Times.last());
//                qDebug("iFirst = %d",iFirst);
//                qDebug("times[%d] = %f",iFirst,m_Times[iFirst]);
//                qDebug("iLast = %d",iLast);
//                qDebug("times[%d] = %f",iLast,m_Times[iLast]);
//            }
            Q_ASSERT(iLast-iFirst==1);
            Q_ASSERT(m_Times[iFirst]<=t && t<=m_Times[iLast]);
            return Lerp(    m_Values[iFirst],
                            m_Values[iLast],
                            (t-m_Times[iFirst])/(m_Times[iLast]-m_Times[iFirst])    );
        }
    }
};

struct TransformTrack
{
    TerminalBehaviour       m_BeforeBegin;
    TerminalBehaviour       m_AfterEnd;
    ValueTrack<QVector3D>   m_ScaleTrack;
    ValueTrack<QQuaternion> m_RotateTrack;
    ValueTrack<QVector3D>   m_TranslateTrack;
};

class QAiAnimationPrivate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal position READ position WRITE setPosition)
public:
    QAiAnimationPrivate(QObject* pParent);
    ~QAiAnimationPrivate();

    qreal position() const;
    void setPosition(qreal pos);

    bool    m_bValid;

    bool    m_bActive;
    bool    m_bPaused;

    qreal   m_Duration;
    bool    m_bLoop;
    qreal   m_Position;
    bool    m_bPositionDirty;

    QList<QString>                          m_AffectedNodes;
    QList<QGLSceneAnimation::NodeTransform> m_DefaultTransformations;
    QList<TransformTrack>                   m_TransformTracks;
    QList<QGLSceneAnimation::NodeTransform> m_Transformations;

    QPropertyAnimation*     m_pPropertyAnim;

Q_SIGNALS:
    void positionChanged();
};

QT_END_NAMESPACE

#endif // QAIANIMATION_P_H
