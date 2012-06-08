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

#include "qaianimation.h"
#include "qaianimation_p.h"


//#define Q_DEBUG_ANIMATIONS 1


QT_BEGIN_NAMESPACE

template <typename T>
T Lerp(const T& t1, const T& t2, qreal t)
{
    Q_ASSERT(t>=0.0 && t<=1.0);
    return t1*(1.0-t) + t2*t;
}

template <>
QQuaternion Lerp<QQuaternion>(const QQuaternion& t1, const QQuaternion& t2, qreal t)
{
    Q_ASSERT(t>=0.0 && t<=1.0);
    return QQuaternion::slerp(t1,t2,t).normalized();
}

//-----------------------------------------------------------------------------------------------------------------------

QAiAnimationPrivate::QAiAnimationPrivate(QObject* pParent) :
    QObject(pParent)
    ,m_bValid(false)
    ,m_bActive(false)
    ,m_bPaused(false)
    ,m_Duration(0)
    ,m_bLoop(true)
    ,m_Position(0)
    ,m_bPositionDirty(true)
    ,m_pPropertyAnim(0)
{}

QAiAnimationPrivate::~QAiAnimationPrivate()
{}

qreal QAiAnimationPrivate::position() const
{
    return m_Position;
}

void QAiAnimationPrivate::setPosition(qreal pos)
{
    m_Position = pos;
    m_bPositionDirty = true;
    emit positionChanged();
}

//-----------------------------------------------------------------------------------------------------------------------

QAiAnimation::QAiAnimation(QObject *parent) :
    QGLSceneAnimation(parent)
{
    d_ptr = new QAiAnimationPrivate(this);
    QObject::connect(d_ptr,SIGNAL(positionChanged()),this,SIGNAL(positionChanged()));
}

QAiAnimation::QAiAnimation(QObject *parent, const QString &name) :
    QGLSceneAnimation(parent,name)
{
    d_ptr = new QAiAnimationPrivate(this);
    QObject::connect(d_ptr,SIGNAL(positionChanged()),this,SIGNAL(positionChanged()));
}

QAiAnimation::~QAiAnimation()
{
}

qreal QAiAnimation::duration() const
{
    return d_ptr->m_Duration;
}

qreal QAiAnimation::position() const
{
    return d_ptr->m_Position;
}

void QAiAnimation::setPosition(qreal pos)
{
    d_ptr->m_Position = AdjustPosition(pos);
    d_ptr->m_bPositionDirty = true;
    if (d_ptr->m_bValid && d_ptr->m_bActive) {
        emit positionChanged();
    }
}

bool QAiAnimation::loop() const
{
    return d_ptr->m_bLoop;
}

void QAiAnimation::setLoop(bool l)
{
    d_ptr->m_bLoop = l;
    d_ptr->m_pPropertyAnim->setLoopCount( d_ptr->m_bLoop?-1:0 );
    emit loopChanged();
}

QList<QString> QAiAnimation::affectedNodes() const
{
    return d_ptr->m_AffectedNodes;
}

QList<QGLSceneAnimation::NodeTransform> QAiAnimation::transformations() const
{
    if (d_ptr->m_bValid && d_ptr->m_bActive) {
        if (d_ptr->m_bPositionDirty) {
            d_ptr->m_bPositionDirty = false;
            if (d_ptr->m_Position<0.0 || d_ptr->m_Position>d_ptr->m_Duration) {
                for (int i=0; i<d_ptr->m_TransformTracks.size(); ++i) {
                    Q_ASSERT(d_ptr->m_TransformTracks.at(i).m_BeforeBegin==TB_DEFAULT);
                    Q_ASSERT(d_ptr->m_TransformTracks.at(i).m_AfterEnd==TB_DEFAULT);
                    d_ptr->m_Transformations[i] = d_ptr->m_DefaultTransformations.at(i);
                }
            } else {
                for (int i=0; i<d_ptr->m_TransformTracks.size(); ++i) {
                    const TransformTrack& rTrack = d_ptr->m_TransformTracks.at(i);
                    QGLSceneAnimation::NodeTransform& rTransform = d_ptr->m_Transformations[i];
                    rTransform.m_Scale = rTrack.m_ScaleTrack.GetValue(d_ptr->m_Position);
                    rTransform.m_Rotate = rTrack.m_RotateTrack.GetValue(d_ptr->m_Position);
                    rTransform.m_Translate = rTrack.m_TranslateTrack.GetValue(d_ptr->m_Position);
                }
            }
        }
        return d_ptr->m_Transformations;
    } else {
        return QList<QGLSceneAnimation::NodeTransform>();
    }
}

template<typename T>
void GetMinMaxTime(int size, T* pKeys, double& rMin, double& rMax)
{
    Q_ASSERT(pKeys!=0);
    rMin = 1E9;
    rMax = -1E9;
    for (int i=0; i<size; ++i) {
        if (rMin > pKeys[i].mTime)
            rMin = pKeys[i].mTime;
        if (rMax < pKeys[i].mTime)
            rMax = pKeys[i].mTime;
    }
}

void DumpAnimation(aiAnimation* pAnim)
{
#ifdef Q_DEBUG_ANIMATIONS
    if (pAnim) {
        qDebug("Animation:");
        qDebug("  name          = '%s'",pAnim->mName.data);
        qDebug("  channels      = %d",pAnim->mNumChannels);
        for (unsigned int ch=0; ch<pAnim->mNumChannels; ++ch) {
            qDebug("  channel %d:",ch);
            aiNodeAnim* pNodeAnim = pAnim->mChannels[ch];
            Q_ASSERT(pNodeAnim);
            qDebug("    affected node      = '%s'",pNodeAnim->mNodeName.data);
            qDebug("    scale keys         = %d",pNodeAnim->mNumScalingKeys);
            double timeMin, timeMax;
            GetMinMaxTime(pNodeAnim->mNumScalingKeys, pNodeAnim->mScalingKeys, timeMin, timeMax);
            qDebug("    scale keys time    = { %f .. %f }",timeMin,timeMax);
            qDebug("    rotation keys      = %d",pNodeAnim->mNumRotationKeys);
            GetMinMaxTime(pNodeAnim->mNumRotationKeys, pNodeAnim->mRotationKeys, timeMin, timeMax);
            qDebug("    rotation keys time = { %f .. %f }",timeMin,timeMax);
            qDebug("    position keys      = %d",pNodeAnim->mNumPositionKeys);
            GetMinMaxTime(pNodeAnim->mNumPositionKeys, pNodeAnim->mPositionKeys, timeMin, timeMax);
            qDebug("    position keys time = { %f .. %f }",timeMin,timeMax);
            qDebug("    pre state          = %d",pNodeAnim->mPreState);
            qDebug("    post state         = %d",pNodeAnim->mPostState);
            Q_ASSERT(pNodeAnim->mPreState==aiAnimBehaviour_DEFAULT && pNodeAnim->mPostState==aiAnimBehaviour_DEFAULT);
        }
    }
#else
    Q_UNUSED(pAnim);
#endif
}

void QAiAnimation::loadData(aiAnimation* pSrcAnim, const QMap<QString,aiMatrix4x4>& rDefaultTransforms, QMap<QString,QGLSceneAnimation::NodeTransform>& rDefaultTransformsRaw)
{
    Q_ASSERT(pSrcAnim);
    DumpAnimation(pSrcAnim);
    double timeMin = 1E9;
    double timeMax = -1E9;
    for (unsigned int ch=0; ch<pSrcAnim->mNumChannels; ++ch) {
        aiNodeAnim* pNodeAnim = pSrcAnim->mChannels[ch];
        Q_ASSERT(pNodeAnim);

        QString NN(QLatin1String(pNodeAnim->mNodeName.data));
        d_ptr->m_AffectedNodes.append(NN);

        QGLSceneAnimation::NodeTransform transform;
        QMap<QString,aiMatrix4x4>::const_iterator It = rDefaultTransforms.find(NN);
        Q_ASSERT(It != rDefaultTransforms.end());
        aiQuaternion rotation;
        aiVector3D position;
        aiVector3D scale;
        It.value().Decompose(scale, rotation, position);
        transform.m_Scale = QVector3D(scale.x,scale.y, scale.z);
        transform.m_Translate = QVector3D(position.x, position.y, position.z);
        transform.m_Rotate = QQuaternion(rotation.w, rotation.x, rotation.y, rotation.z);
        d_ptr->m_DefaultTransformations.append(transform);
        d_ptr->m_Transformations.append(transform);
        rDefaultTransformsRaw.insert(NN,transform);

        d_ptr->m_TransformTracks.append(TransformTrack());
        TransformTrack& rTrack = d_ptr->m_TransformTracks.back();

        Q_ASSERT(pNodeAnim->mNumScalingKeys > 0);
        rTrack.m_ScaleTrack.m_Times.resize(pNodeAnim->mNumScalingKeys);
        rTrack.m_ScaleTrack.m_Values.resize(pNodeAnim->mNumScalingKeys);
        for (unsigned int i=0; i<pNodeAnim->mNumScalingKeys; ++i) {
            rTrack.m_ScaleTrack.m_Times[i] = pNodeAnim->mScalingKeys[i].mTime;
            rTrack.m_ScaleTrack.m_Values[i] = QVector3D(pNodeAnim->mScalingKeys[i].mValue.x,pNodeAnim->mScalingKeys[i].mValue.y,pNodeAnim->mScalingKeys[i].mValue.z);
            if (timeMin > pNodeAnim->mScalingKeys[i].mTime)
                timeMin = pNodeAnim->mScalingKeys[i].mTime;
            if (timeMax < pNodeAnim->mScalingKeys[i].mTime)
                timeMax = pNodeAnim->mScalingKeys[i].mTime;
        }

        Q_ASSERT(pNodeAnim->mNumRotationKeys > 0);
        rTrack.m_RotateTrack.m_Times.resize(pNodeAnim->mNumRotationKeys);
        rTrack.m_RotateTrack.m_Values.resize(pNodeAnim->mNumRotationKeys);
        for (unsigned int i=0; i<pNodeAnim->mNumRotationKeys; ++i) {
            rTrack.m_RotateTrack.m_Times[i] = pNodeAnim->mRotationKeys[i].mTime;
            QQuaternion qq(pNodeAnim->mRotationKeys[i].mValue.w,pNodeAnim->mRotationKeys[i].mValue.x,pNodeAnim->mRotationKeys[i].mValue.y,pNodeAnim->mRotationKeys[i].mValue.z);
            rTrack.m_RotateTrack.m_Values[i] = qq;
            if (timeMin > pNodeAnim->mRotationKeys[i].mTime)
                timeMin = pNodeAnim->mRotationKeys[i].mTime;
            if (timeMax < pNodeAnim->mRotationKeys[i].mTime)
                timeMax = pNodeAnim->mRotationKeys[i].mTime;
        }

        Q_ASSERT(pNodeAnim->mNumPositionKeys > 0);
        rTrack.m_TranslateTrack.m_Times.resize(pNodeAnim->mNumPositionKeys);
        rTrack.m_TranslateTrack.m_Values.resize(pNodeAnim->mNumPositionKeys);
        for (unsigned int i=0; i<pNodeAnim->mNumPositionKeys; ++i) {
            rTrack.m_TranslateTrack.m_Times[i] = pNodeAnim->mPositionKeys[i].mTime;
            rTrack.m_TranslateTrack.m_Values[i] = QVector3D(pNodeAnim->mPositionKeys[i].mValue.x,pNodeAnim->mPositionKeys[i].mValue.y,pNodeAnim->mPositionKeys[i].mValue.z);
            if (timeMin > pNodeAnim->mPositionKeys[i].mTime)
                timeMin = pNodeAnim->mPositionKeys[i].mTime;
            if (timeMax < pNodeAnim->mPositionKeys[i].mTime)
                timeMax = pNodeAnim->mPositionKeys[i].mTime;
        }

        Q_ASSERT(pNodeAnim->mPreState==aiAnimBehaviour_DEFAULT && pNodeAnim->mPostState==aiAnimBehaviour_DEFAULT);
        rTrack.m_BeforeBegin = TB_DEFAULT;
        rTrack.m_AfterEnd = TB_DEFAULT;
    }
    Q_ASSERT(d_ptr->m_AffectedNodes.size() == d_ptr->m_TransformTracks.size());
    Q_ASSERT(d_ptr->m_AffectedNodes.size() == d_ptr->m_DefaultTransformations.size());
    Q_ASSERT(d_ptr->m_AffectedNodes.size() == d_ptr->m_Transformations.size());
    qreal duration = timeMax-timeMin;
    d_ptr->m_bValid = (duration > 0.000001);
    if (d_ptr->m_bValid) {
        qreal timeShift = -timeMin;
        timeMax = -1E9;
        for (int tr=0; tr<d_ptr->m_TransformTracks.size(); ++tr) {
            TransformTrack& rTrack = d_ptr->m_TransformTracks[tr];
            // fix scale
            if (rTrack.m_ScaleTrack.m_Times.size() == 1) {
                rTrack.m_ScaleTrack.m_bConst = true;
            } else {
                rTrack.m_ScaleTrack.m_bConst = false;
                for (int t=0; t<rTrack.m_ScaleTrack.m_Times.size(); ++t) {
                    rTrack.m_ScaleTrack.m_Times[t] += timeShift;
                    Q_ASSERT(rTrack.m_ScaleTrack.m_Times[t] >= 0.0);
                    if (timeMax < rTrack.m_ScaleTrack.m_Times[t])
                        timeMax = rTrack.m_ScaleTrack.m_Times[t];
                }
                Q_ASSERT(rTrack.m_ScaleTrack.m_Times[0] >= 0.0);
                Q_ASSERT(rTrack.m_ScaleTrack.m_Times[0] < 0.00001);
                Q_ASSERT(qAbs((rTrack.m_ScaleTrack.m_Times.last()/duration)-1.0) < 0.00001);
            }
            // fix rotation
            if (rTrack.m_RotateTrack.m_Times.size() == 1) {
                rTrack.m_RotateTrack.m_bConst = true;
            } else {
                rTrack.m_RotateTrack.m_bConst = false;
                for (int t=0; t<rTrack.m_RotateTrack.m_Times.size(); ++t) {
                    rTrack.m_RotateTrack.m_Times[t] += timeShift;
                    Q_ASSERT(rTrack.m_RotateTrack.m_Times[t] >= 0.0);
                    if (timeMax < rTrack.m_RotateTrack.m_Times[t])
                        timeMax = rTrack.m_RotateTrack.m_Times[t];
                }
                Q_ASSERT(rTrack.m_RotateTrack.m_Times[0] >= 0.0);
                Q_ASSERT(rTrack.m_RotateTrack.m_Times[0] < 0.00001);
                Q_ASSERT(qAbs((rTrack.m_RotateTrack.m_Times.last()/duration)-1.0) < 0.00001);
            }
            // fix translation
            if (rTrack.m_TranslateTrack.m_Times.size() == 1) {
                rTrack.m_TranslateTrack.m_bConst = true;
            } else {
                rTrack.m_TranslateTrack.m_bConst = false;
                for (int t=0; t<rTrack.m_TranslateTrack.m_Times.size(); ++t) {
                    rTrack.m_TranslateTrack.m_Times[t] += timeShift;
                    Q_ASSERT(rTrack.m_TranslateTrack.m_Times[t] >= 0.0);
                    if (timeMax < rTrack.m_TranslateTrack.m_Times[t])
                        timeMax = rTrack.m_TranslateTrack.m_Times[t];
                }
                Q_ASSERT(rTrack.m_TranslateTrack.m_Times[0] >= 0.0);
                Q_ASSERT(rTrack.m_TranslateTrack.m_Times[0] < 0.00001);
                Q_ASSERT(qAbs((rTrack.m_TranslateTrack.m_Times.last()/duration)-1.0) < 0.00001);
            }
        }
        Q_ASSERT(qAbs((timeMax/duration)-1.0) < 0.00001);
    }
    d_ptr->m_Duration = duration;

    Q_ASSERT(d_ptr->m_pPropertyAnim==0);
    d_ptr->m_pPropertyAnim = new QPropertyAnimation(d_ptr,"position",d_ptr);
    d_ptr->m_pPropertyAnim->setStartValue(0.0f);
    d_ptr->m_pPropertyAnim->setEndValue(d_ptr->m_Duration);
    d_ptr->m_pPropertyAnim->setDuration(int(d_ptr->m_Duration*1000.0));
    d_ptr->m_pPropertyAnim->setLoopCount(-1);
}

void QAiAnimation::play()
{
    if (d_ptr->m_bValid) {
        bool bWasActive = d_ptr->m_bActive;
        bool bWasPaused = d_ptr->m_bPaused;
        d_ptr->m_bActive = true;
        d_ptr->m_bPaused = false;
        if (!bWasActive || bWasPaused) {
            d_ptr->m_bPositionDirty = true;
            emit positionChanged();
        }
        d_ptr->m_pPropertyAnim->start();
    }
}

void QAiAnimation::stop()
{
    if (d_ptr->m_bValid) {
        d_ptr->m_bActive = false;
        d_ptr->m_pPropertyAnim->stop();
        emit positionChanged();
    }
}

void QAiAnimation::pause(bool val)
{
    if (d_ptr->m_bValid) {
        bool bWasPaused = d_ptr->m_bPaused;
        d_ptr->m_bPaused = val;
        if (d_ptr->m_bPaused)
            d_ptr->m_pPropertyAnim->pause();
        else
            d_ptr->m_pPropertyAnim->resume();
        if (d_ptr->m_bActive && !d_ptr->m_bPaused && bWasPaused) {
            d_ptr->m_bPositionDirty = true;
            emit positionChanged();
        }
    }
}

qreal QAiAnimation::AdjustPosition(qreal newPos)
{
    if (d_ptr->m_bLoop) {
        if (newPos>d_ptr->m_Duration) {
            int n = int(newPos/d_ptr->m_Duration);
            newPos -= qreal(n);
            if (newPos<0.0)
                return 0.0;
            else if (newPos>d_ptr->m_Duration)
                return d_ptr->m_Duration;
            else
                return newPos;
        } else if (newPos<0.0) {
            int n = int(newPos/d_ptr->m_Duration);
            newPos -= qreal(n-1);
            if (newPos<0.0)
                return 0.0;
            else if (newPos>d_ptr->m_Duration)
                return d_ptr->m_Duration;
            else
                return newPos;
        } else {
            return newPos;
        }
    } else {
        return newPos;
    }
}

QT_END_NAMESPACE
