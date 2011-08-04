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

#include "qglcolladafxeffectloader.h"

#include <QList>
#include "qglcolladafxeffectfactory.h"
#include "qglcolladafxeffect.h"

#include <QStringList>

//Q_DECLARE_METATYPE(QGLColladaFxEffect)

/*!
    \class QGLColladaFxEffectLoader
    \brief The QGLColladaFxEffectLoader class is a convenient way to load
    effects from collada files for use with Qt3D.
    \since 4.8
    \ingroup qt3d

    QGLColladaFxEffectLoader provides a simple class to create and store
    QGLColladaEffect objects from Collada Fx files.  It currently supports
    a simple subset of the Collada 1.5 Fx specification.

    The QGLColladaEffect effects are destroyed automatically when the loader
    is destroyed. If finer control over effect lifetime is required, use
    QGLColladaFxEffectFactory::loadEffectsFromFile() directly.
*/

class QGLColladaFxEffectLoaderPrivate
{
public:
    QList<QGLColladaFxEffect*> effects;
    ~QGLColladaFxEffectLoaderPrivate()
    {
        deleteAndClearEffects();
    }

    void deleteAndClearEffects()
    {
        while (effects.count())
        {
            delete effects.back();
            effects.pop_back();
        }
    }

};


/*!
  Constructs an empty QGLColladaFxEffectLoader object.
*/
QGLColladaFxEffectLoader::QGLColladaFxEffectLoader() :
        d_ptr(new QGLColladaFxEffectLoaderPrivate())
{
}

/*!
  Destroys the QGLColladaFxEffectLoader and any generated QGLColladaFxEffect
  objects.
*/
QGLColladaFxEffectLoader::~QGLColladaFxEffectLoader()
{
}

/*!
  Reads the collada file indicated by \a filename and generates
  QGLColladaFxEffect objects from it.

  The QGLColladaFxEffectFactory owns all the effects it generates, and destroys
  them when it is destroyed, or when a new file is loaded.

  If effects are needed from multiple files, use one QGLColladaFxEffectLoader
  per file.

  Returns true if at least one effect was generated.

  \sa effectNames(), effect(), operator[](), QGLColladaFxEffectFactory::loadEffectsFromFile()
  */
bool QGLColladaFxEffectLoader::load(QString filename)
{
    Q_D(QGLColladaFxEffectLoader);
    d->deleteAndClearEffects();
    d->effects = QGLColladaFxEffectFactory::loadEffectsFromFile(filename);
    return d->effects.count() > 0;
}

/*!
  Returns a list of the sid attributes of effects that have been generated
  \sa effect()
  */
QStringList QGLColladaFxEffectLoader::effectNames()
{
    Q_D(QGLColladaFxEffectLoader);
    QStringList result;
    QGLColladaFxEffect *effect;
    foreach (effect, d->effects)
    {
        result.append(effect->sid());
    }
    return result;
}

/*!
  Returns a pointer to the effect with an sid matching \a effectName, or
  0 if no such effect exists.

  \sa load()
  */
QGLColladaFxEffect* QGLColladaFxEffectLoader::effect(QString effectName)
{
    Q_D(QGLColladaFxEffectLoader);
    QGLColladaFxEffect* effect;

    foreach (effect, d->effects)
    {
        if (effect && effect->sid() == effectName)
        {
            return effect;
        }
    }
    return 0;
}

/*!
  Returns the number of effects the loader has generated and stored.
  */
int QGLColladaFxEffectLoader::count()
{
    Q_D(QGLColladaFxEffectLoader);
    return d->effects.count();
}

/*!
  Returns a pointer to the effect in position \a i.  QGLColladaFxEffectLoader
  makes no guarantee about the ordering of effects relative to their position
  in a collada document, but does not reorder effects once they have been read.

  \sa load()
  */
QGLColladaFxEffect *QGLColladaFxEffectLoader::operator[](int i)
{
    Q_D(QGLColladaFxEffectLoader);
    Q_ASSERT(i < d->effects.count());

    return d->effects.at(i);
}
