/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QVector>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QDir>
#include "qgltexture2d.h"
#include "qgl.h"

#include "qglcolladafxeffect.h"
#include "qglcolladafxeffectfactory.h"
//#include "qglcolladafxeffectfactory_p.h"
#include "qglcolladafxeffect_p.h"

QT_BEGIN_NAMESPACE

// The QGLColladaFxFactory class creates a list of QGLColladaFx objects from
// COLLADA FX information according to the 1.4.1 March 2008 Release from
// http://www.khronos.org/collada/

//#define DEBUG_QGL_COLLADA_PARSING
//#define DEBUG_MATERIALS

#define INDENT_SIZE 4

static int QGLColladaIndentLevel = 0;

// This is a simple RAII helper class to conveniently indent across
// various functions when building up the collada elements.
// Instantiate an indent to increment the indent level with that scope.
// add an indent object to the beginning of a string to indent it
// the correct amount.
class Indent
{
public:
    static int currentIndent() { return QGLColladaIndentLevel; };
    Indent()
    {
        ++QGLColladaIndentLevel;
    };
    ~Indent()
    {
        --QGLColladaIndentLevel;
    }
};

QString operator +(Indent&, QString string)
{
    return QString(QGLColladaIndentLevel * INDENT_SIZE, QLatin1Char(' ')) + string;
}

QString operator +(char c, Indent&)
{
    return QLatin1Char(c) + QString(QGLColladaIndentLevel * INDENT_SIZE, QLatin1Char(' '));
}

// xml convenience function - find the first end tag with the given tagname
// Note that this is not as smart as xml.skipCurrentElement(), and does
// not handle the case where (grand)children node share tagName
// With this caveat, can be called from within any (grand)child node.
static inline void findEndTag( QXmlStreamReader& xml, QString tagName )
{
    while ( !xml.atEnd() && !( xml.tokenType() == xml.EndElement && xml.name() == tagName))
    {
        xml.readNext();
    }
}



/*!
    \internal
    Convenience function to find parameters in \a resultState that have an id,
    sid, or name that is equal to \a stringToMatch
    Note that this using this is not in line with the Collada specification,
    and that non-unique sids will always return the last parameter parsed.
  */
static inline QVariant findParameterVariant(ResultState* resultState, QString stringToMatch)
{
    QVariant result = resultState->paramSids.value(stringToMatch);

    if ( result.isNull() )
        result = resultState->paramIds.value(stringToMatch);

    if ( result.isNull() )
        result = resultState->paramNames.value(stringToMatch);

    return result;
}

/*!
    Parse a collada 1.4 or 1.5 .dae file \a fileName, find the effects in the
    library_effects element, and convert these into a list of QGLColladaFxEffect objects
    suitable for use with Qt3D. Specific effects can be identified out of
    this list using their sid.

    This is the only function intended to be called from outside this class, e.g.:

    QList<QGLColladaFxEffect*> colladaEffects = QGLColladaFxEffectFactory::loadEffectsFromFile("myColladaFile.dae");

    \sa QGLColladaFxEffect
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::loadEffectsFromFile( const QString& fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ))
    {
        qWarning() << "Warning: QGLColladaFxEffect failed to open file" << fileName;
        return QList<QGLColladaFxEffect*>();
    }


    QXmlStreamReader xml;
    xml.setDevice( &file );
    if (xml.tokenType() == QXmlStreamReader::Invalid)
        file.readLine();
    xml.setDevice( &file );

    QFileInfo fileInfo(file);

    return loadEffectsFromXml( xml, fileInfo.dir() );
}



QString QGLColladaFxEffectFactory::exportEffect(QGLColladaFxEffect *effect, QString effectId, QString techniqueSid)
{
    QStringList result;

    result += QLatin1String("<?xml version=\"1.0\"?>");

    result += QLatin1String("<COLLADA "\
              "xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.5.0\" >");

    {
        Indent indent;
        result += indent + QLatin1String("<asset>");
        {
            Indent indent;
            QDateTime time = QDateTime::currentDateTime();
            result += indent + QLatin1String("<created>") + time.toString(Qt::ISODate) + QLatin1String("</created>");
            result += indent + QLatin1String("<modified>") + time.toString(Qt::ISODate) + QLatin1String("</modified>");
        }
        result += indent + QLatin1String("</asset>");

        result += indent + QLatin1String("<library_effects>");
        {
            Indent indent;
            result += indent + QLatin1String("<effect id=\"") + effectId + QLatin1String("\">");
            result += glslProfileFromEffect(effect, techniqueSid);
            result += indent + QLatin1String("</effect>");
        }
        result += indent + QLatin1String("</library_effects>");
        result += QLatin1String("</COLLADA>");
        return result.join(QLatin1String("\n"));
    }
}


/*!
  \internal
  parse the top level \a xml from a .dae file and process the library_effects elements therein.
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::loadEffectsFromXml( QXmlStreamReader& xml, QDir homeDirectory )
{
    ResultState resultState;
    resultState.sourceDir = homeDirectory;
    QList<QGLColladaFxEffect*> result;

    while ( !xml.atEnd() ) {
        xml.readNextStartElement();
        if ( xml.name() == "library_effects" ) {
            result += processLibraryEffectsElement( xml , &resultState );
        } else if (xml.name() == "library_images")
        {
            processLibraryImagesElement( xml, &resultState );
        }
    }

    // Try and resolve outstanding textures
    QList<QGLTexture2D*> unresolvedTexturePointers = resultState.unresolvedTexture2Ds.keys();
    for (int i = 0; i < unresolvedTexturePointers.count(); i++)
    {
        QGLTexture2D* texture = unresolvedTexturePointers[i];
        QString parameterName = resultState.unresolvedTexture2Ds.value(texture);
        resolveTexture2DImage(texture, &resultState, parameterName);
    }
    return result;
}



QGLColladaParam::~QGLColladaParam()
{

}



/*!
  \internal
*/
int QGLColladaParam::type()
{
    return mType;
}



/*!
  \internal
*/
QString QGLColladaParam::sid()
{
    return mSid;
}



/*!
  \internal
*/
QVector<float> QGLColladaParam::value() {
    return mValue;
}



/*!
  \internal
*/
QGLColladaParam::QGLColladaParam(QString sid, int type) : mSid(sid)
        , mType(type)
{
}



/*!
  \internal
*/
QString QGLColladaParam::id()
{
    return mId;
}



/*!
  \internal
*/
QString QGLColladaParam::typeString(int type)
{
    const char* typeStringArray[] = {
        "UnkownType",
        "Sampler2DType",
        "Texture2DType",
        "SurfaceType",
        "ImageType"};

    if (type >= UserDefinedType)
        return QLatin1String("UserDefinedType");
    else if ( type < 0 || type > ImageType)
        return QLatin1String("Unrecognized Type");
    else
        return QLatin1String(typeStringArray[type]);
}



/*!
  \internal
*/
QGLColladaTextureParam::QGLColladaTextureParam(QString sid, QGLTexture2D* texture) : QGLColladaParam(sid, Texture2DType)
{
    mTexture = texture;
}



/*!
  \internal
*/
QGLTexture2D* QGLColladaTextureParam::texture()
{
    Q_ASSERT( mType == Sampler2DType || mType == Texture2DType);
    return mTexture;
}



/*!
  \internal
*/
QString QGLColladaTextureParam::samplerSid()
{
    return sampler2DSid;
}



/*!
  \internal
*/
QGLColladaFxEffectFactory::QGLColladaFxEffectFactory()
{
}



/*!
  \internal
*/
QGLColladaSampler2DParam::QGLColladaSampler2DParam(QString sid, QGLTexture2D* texture)
    : QGLColladaParam(sid, QGLColladaParam::Sampler2DType)
    , mTexture(texture)
{
}



/*!
  \internal
*/
QGLColladaSampler2DParam::QGLColladaSampler2DParam(QString sid, QString sourceSid)
    : QGLColladaParam(sid, QGLColladaParam::Sampler2DType)
    , mTexture(0)
    , mSourceSid(sourceSid)
{
}



/*!
  \internal
*/
QString QGLColladaSampler2DParam::sourceSid()
{
    return mSourceSid;
}



/*!
  \internal
*/
QGLColladaSurfaceParam::QGLColladaSurfaceParam(QString sid) : QGLColladaParam(sid, QGLColladaParam::SurfaceType)
        , mInitFrom()
        , mFormat()
        , mFormatHint()
        , mSize()
        , mSizeVector( 3, 0 )
        , mViewportRatio( 1.0, 1.0 )
        , mMipLevels( 0 )
        , mMipMapGenerate( false )
        , mExtra()
        , mGenerator()
{
}



/*!
  \internal
*/
QGLColladaImageParam::QGLColladaImageParam(QString sid, QImage image)
    : QGLColladaParam(sid, QGLColladaParam::ImageType)
    , mImage(image)
{
}



/*!
  \internal
*/
QImage QGLColladaImageParam::image()
{
    return mImage;
}



/*!
  \internal
*/
QString QGLColladaImageParam::name()
{
    return mName;
}



/*!
  \internal
*/
QGLColladaSurfaceParam* QGLColladaFxEffectFactory::processSurfaceElement( QXmlStreamReader& xml , ResultState* resultState, QString passedInSid)
{
    Q_UNUSED(resultState);
    QXmlStreamAttributes attributes = xml.attributes();
    QString surfaceSid = attributes.value(QLatin1String("sid")).toString();
    // Surfaces are the only children of a newparam, but don't have their own
    // sids. For simplicity, use the parent's sid.

    if (surfaceSid.isEmpty() && !passedInSid.isEmpty())
        surfaceSid = passedInSid;
    QGLColladaSurfaceParam* result = new QGLColladaSurfaceParam( surfaceSid );
    xml.readNextStartElement();
    if ( xml.name().toString().left(4) == QLatin1String("init") )
    {
        if (xml.name().toString() != QLatin1String("init_from"))
            qWarning() << "Warning: only ""init_from"" supported in surface element ( line:" << xml.lineNumber() << ")";
        QString init_from = xml.readElementText();
        result->mInitFrom =  init_from;
        resultState->paramSids[surfaceSid] = init_from;
    }
    return result;
}



/*!
  \internal
    Processes a list of floating point numbers.  If the list contains only 1
    element, a QVariant<float> is returned.  If the list containst 2, 3 or 4
    elements, they are converted into a QVariant containing a QVector2D,
    QVector3D, or QVector4D respectively.
    If the list containst more elements than that, they are returned as a
    QArray<float>.

*/
QVariant QGLColladaFxEffectFactory::processFloatList( QXmlStreamReader& xml )
{
    QArray<float> floats;
    QString elementString = xml.readElementText();
    QStringList list = elementString.split( QRegExp( QLatin1String("\\s+") ), QString::SkipEmptyParts );
    bool ok;
    float f;
    foreach ( QString string, list )
    {
        f = string.toFloat( &ok );
        if ( ok )
            floats.append(string.toFloat());
        else
        {
            qWarning() << "Warning: malformed float ( line" << xml.lineNumber() << ")";
        }
    }

    switch(floats.count())
    {
    case 0:
        return QVariant();
        // no break necessary
    case 1:
        return QVariant(floats[0]);
        // no break necessary
    case 2:
        return QVariant(QVector2D(floats[0], floats[1]));
        // no break necessary
    case 3:
        return QVariant(QVector3D(floats[0], floats[1], floats[2]));
        // no break necessary
    case 4:
        return QVariant(QVector4D(floats[0], floats[1], floats[2], floats[3]));
        // no break necessary
    default:
        {
            QVariant result;
            result.setValue(floats);
            return result;
        }
    }
    // Function should always return out of switch statement
}



/*!
  \internal
*/
void QGLColladaFxEffectFactory::processSampler2DElement( QXmlStreamReader& xml, ResultState* resultState, QString passedInSid )
{
    Q_UNUSED(resultState);
    QXmlStreamAttributes attributes = xml.attributes();
    QString sid = attributes.value(QLatin1String("sid")).toString();
    if (sid.isEmpty() && !passedInSid.isEmpty())
    {
        sid = passedInSid;
    }

    xml.readNextStartElement();
    if ( xml.name() == QLatin1String("source"))
    {
        // Collada 1.4 Spec
        QString sourceSurfaceSid = xml.readElementText().trimmed();
        resultState->paramSids[sid] = sourceSurfaceSid;
    }

    if ( xml.name() == QLatin1String("instance_image") )
    {
        // Collada 1.5 Spec
        qWarning() << "collada 1.5 sampler elements not supported ( line:" << xml.lineNumber() << ")";
    }
    // exit cleanly, just in case.
    findEndTag( xml, QLatin1String("sampler2D"));
    return;
}



/*!
    \internal
    Parses and consumes a color collada element from \a xml.
*/
QColor QGLColladaFxEffectFactory::processColorElement( QXmlStreamReader& xml )
{
    QVariant floatList = processFloatList( xml );

    QColor result( 0, 0, 0, 255 );
    if (floatList.type() == QVariant::Vector3D)
    {
        QVector3D vector3D = floatList.value<QVector3D>();
        if ( !vector3D.isNull())
        {
            result.setRgbF( vector3D.x()
                            , vector3D.y()
                            , vector3D.z()
                            , 1.0 );
            return result;
        }
    }
    else if (floatList.type() == QVariant::Vector4D)
    {
        QVector4D vector4D = floatList.value<QVector4D>();
        if (!vector4D.isNull())
        {
            result.setRgbF( vector4D.x()
                            , vector4D.y()
                            , vector4D.z()
                            , vector4D.w() );
            return result;
        }
    }
    qWarning() << "Warning: Malformed color element ( line" << xml.lineNumber() << ")";
    return result;
}



/*!
    \internal
    Parses and consumes an fx_common_color_or_texture_type collada element from \a xml.
*/
QVariant QGLColladaFxEffectFactory::processColorOrTextureElement( QXmlStreamReader& xml )
{
    if ( xml.name() == QLatin1String("color"))
    {
        return processColorElement( xml );
    } else if ( xml.name() == QLatin1String("texture") )
    {
        qWarning() << "Warning: texture element not supported ( line" << xml.lineNumber()<<")";
    } else
    {
        qWarning() << "Color or Texture expected ( line" << xml.lineNumber() << ")";
    }
    xml.skipCurrentElement();
    return( QColor( 0, 0, 0,255 ));
}



/*!
    \internal
    Parses an fx_common_float_or_param_type collada element from \a xml.
    Always consumes the element.
*/
float QGLColladaFxEffectFactory::processParamOrFloatElement( QXmlStreamReader& xml )
{
    if ( xml.name() == QLatin1String("param") )
    {
        qWarning() << "Warning: params not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        return 0.0;
    } else
    {
        return xml.readElementText().toFloat();
    }
}



/*!
    Parses and consumes a library_images collada element pointed to by \a xml,
    and pushes any images found onto the \a resultState for use in resolving
    elements later.
*/
void QGLColladaFxEffectFactory::processLibraryImagesElement( QXmlStreamReader& xml, ResultState* resultState )
{
    xml.readNextStartElement();

    if ( xml.name() == QLatin1String("asset") )
    {
        qWarning() << "Warning: effect asset handling not supported in library_images element ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("image") && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        processImageElement( xml , resultState );
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }
}


/*!
  \internal
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::processLibraryEffectsElement( QXmlStreamReader& xml, ResultState* resultState )
{
    QList<QGLColladaFxEffect*> result;
    // A collada library_effects is
    //    0 or 1 <asset>
    //    1 or more <effect>,
    //    0 or more <extra>;
    xml.readNextStartElement();

    if ( xml.name() == QLatin1String("asset") )
    {
        qWarning() << "Warning: effect asset handling not supported in effects library ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("effect") && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        result += processEffectElement( xml , resultState );
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("extra") )
    {
        qWarning() << "Warning: extra element not handled in effects library ( line" << xml.lineNumber() << ")";
        xml.readNextStartElement();
    }

    // be sure to exit cleanly
    findEndTag(xml, QLatin1String("library_effects"));
    return result;
}



/*!
  \internal
  Parses and consumes an effect element from \a xml.
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::processEffectElement( QXmlStreamReader& xml, ResultState* resultState )
{
    //    An effect element is:
    //    0 or 1 <annotate>
    //    0 or more newparam
    //    1 or more profile ( either <profile_BRIDGE>, <profile_CG>, <profile_GLES>, <profile_GLES2>, <profile_GLSL>, or <profile_COMMON>
    //    0 or more <extra>

    QList<QGLColladaFxEffect*> result;
    xml.readNextStartElement();

    if ( xml.name() == QLatin1String("annotate") )
    {
        qWarning() << "effect annotation not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("newparam") && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        processNewparamElement( xml , resultState );
        xml.readNextStartElement();
    }

    // find any of the profile_* elements defined in the spec
    QRegExp profileRegExp( QLatin1String("profile_(BRIDGE|CG|GLES2?|GLSL|COMMON)") );
    while ( profileRegExp.indexIn( xml.name().toString() ) == 0 && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        result += processProfileElement( xml, resultState );
        xml.readNextStartElement();
    }

    findEndTag(xml, QLatin1String("effect"));
    return result;
}



/*!
  \internal
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::processProfileElement( QXmlStreamReader& xml, ResultState* resultState )
{
    // A profile_GLES2 element is:
    // 0 or 1 <asset>
    // 0 or more <code> or <include>
    // 0 or more <newparam>
    // 1 or more <technique>

    // A profile_GLSL element is:
    // 0 or 1 <asset>
    // 0 or more <code>
    // 0 or more <include>
    // 0 or more <newparam>
    // 1 or more <technique>
    // 0 or more <extra>

    // A profile_COMMON element is
    // an optional id element
    // 0 or 1 <asset>
    // 0 or more <newparam>
    // 1 <technique>
    // 0 or more <extra>

    // Note: techniques need to be handled differently for different profiles

    QString rootNodeString = xml.name().toString();
    QList<QGLColladaFxEffect*> result;

    xml.readNextStartElement();
    if ( xml.name() == QLatin1String("asset") )
    {
        qWarning() << "Warning: asset element not supported in " << rootNodeString << "elements ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    if (rootNodeString == QLatin1String("profile_GLSL"))
    {
        while ( xml.name() == QLatin1String("code") )
        {
            QString codeSid = xml.attributes().value(QLatin1String("sid")).toString();
            QString codeText = xml.readElementText();
            resultState->paramSids[codeSid] = codeText;

            findEndTag(xml, QLatin1String("code"));
            xml.readNextStartElement();
        }

        while ( xml.name() == QLatin1String("include") )
        {
            QString includeSid = xml.attributes().value(QLatin1String("sid")).toString();
            QString includeUrl = xml.attributes().value(QLatin1String("url")).toString();

            // create an include param?
            qWarning() << "Warning: include element not supported in " << rootNodeString << "elements ( line" << xml.lineNumber() << ")";

            findEndTag(xml, QLatin1String("include"));
            xml.readNextStartElement();
        }

    }

    while ( xml.tokenType() == QXmlStreamReader::StartElement &&
           ( xml.name() == QLatin1String("newparam") || xml.name() == QLatin1String("image") ))
    {
        if ( xml.name() == QLatin1String("newparam") )
            processNewparamElement( xml , resultState );
        else if ( xml.name() == QLatin1String("image") )
            processImageElement( xml , resultState );

        xml.readNextStartElement();
    }


    while ( xml.name() == QLatin1String("technique") )
    {
        result.append(
                processTechniqueElement( xml, resultState, rootNodeString ));
        xml.readNextStartElement();
        // only 1 technique in profile_COMMON
        if ( rootNodeString == QLatin1String("profile_COMMON"))
        {
            break;
        }
    };

    while ( xml.name() == QLatin1String("extra") )
    {
        qWarning() << "extra elements currently not supported in " << rootNodeString << "elements ( line" << xml.lineNumber() << ")";
        findEndTag( xml, QLatin1String("extra") );
        xml.readNextStartElement();
    };

    findEndTag( xml, rootNodeString );
    return result;
}


QGLColladaParam* QGLColladaFxEffectFactory::processPassElement( QXmlStreamReader& xml, ResultState* resultState, QGLColladaFxEffect* effect )
{
    QGLColladaParam* result = 0;
    // a profile_GLSL pass is:
    // 0 or 1 <annotate>
    // 0 or 1 <states>
    // 0 or 1 <program> (CG, GLES2 or GLSL only)
    // 0 or 1 <evaluate>
    // 0 or 1 <extra>

    xml.readNextStartElement();

    if ( xml.name() == QLatin1String("annotate") )
    {
        qWarning() << "Warning: annotate element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, QLatin1String("annotate") );
        xml.readNextStartElement();
    }

    if ( xml.name() == QLatin1String("states") )
    {
        qWarning() << "Warning: states element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, QLatin1String("states") );
        xml.readNextStartElement();
    }

    // 0 or 1 <program> (CG, GLES2 or GLSL only)
    if ( xml.name() == QLatin1String("program") )
    {
        processProgramElement( xml, resultState, effect );
        findEndTag( xml, QLatin1String("program") );
        xml.readNextStartElement();
    }

    // 0 or 1 <evaluate>
    if ( xml.name() == QLatin1String("evaluate") )
    {
        qWarning() << "Warning: evaluate element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, QLatin1String("evaluate") );
        xml.readNextStartElement();
    }

    // 0 or more <extra>
    while ( xml.name() == QLatin1String("extra") )
    {
        qWarning() << "Warning: extra element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, QLatin1String("extra") );
        xml.readNextStartElement();
    }

    findEndTag( xml, QLatin1String("pass"));
    return result;
}

QGLColladaFxEffect* QGLColladaFxEffectFactory::processTechniqueElement( QXmlStreamReader& xml, ResultState* resultState, QString &profileName )
{
    // A 1.4 technique is:
    // 0 or 1 <asset>
    // 0 or more <newparam> or <image> (in any order)
    // 0 or more of <constant>, <lambert>, <phong>, <blinn>
    // 0 or more <extra>

    // A 1.5 profile_COMMON technique is:
    // 0 or 1 <asset>
    // exactly 1 of <blinn>, <constant>, <lambert>, or <phong>
    // 0 or more <extra>

    // a profile_GLSL technique is:
    // 0 or 1 <asset>
    // 0 or more <annotate>
    // 1 or more <pass>
    // 0 or more <extra>

    QGLColladaFxEffect* effect = new QGLColladaFxEffect();

    QXmlStreamAttributes attributes = xml.attributes();
    effect->setSid( attributes.value( QLatin1String("sid") ).toString() );
    QStringRef id = attributes.value( QLatin1String("id") );
    Q_UNUSED( id );

    xml.readNextStartElement();

    if ( xml.name() == QLatin1String("asset") )
    {
        qWarning() << "Warning: asset element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("annotate") && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        qWarning() << "Warning: annotate element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    // Collada specifies exactly one of blinn, lambert, constant or phong
    // If the effect is malformed, default QGLMaterial will be used.
    QGLMaterial* material = new QGLMaterial;

    if ( profileName == QLatin1String("profile_COMMON") &&
        (xml.name() == QLatin1String("blinn") || xml.name() == QLatin1String("phong") ||
         xml.name() == QLatin1String("constant") || xml.name() == QLatin1String("lambert")) )
    {
        if ( xml.name() == QLatin1String("blinn") )
        {
            effect->setLighting( QGLColladaFxEffect::BlinnLighting );
        } else if ( xml.name() == QLatin1String("phong") ) {
            effect->setLighting( QGLColladaFxEffect::PhongLighting );
        } else if ( xml.name() == QLatin1String("constant") ) {
            effect->setLighting( QGLColladaFxEffect::ConstantLighting );
        } else if ( xml.name() == QLatin1String("lambert") ) {
            effect->setLighting( QGLColladaFxEffect::LambertLighting );
        }

        // TODO: get appropriate shader fragments for the specified lighting models
        if (  xml.readNextStartElement() )
        {
            // a blinn element is 0 or 1 of each of:
            //        <emission>
            //        <diffuse>
            //        <specular>
            //        <shininess>
            //        <reflectivity>
            //        <transparent>
            //        <transparency>
            //        <index_of_refraction>

            if ( xml.name() == QLatin1String("emission") )
            {
                if ( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if (xml.name() == QLatin1String("color"))
                    {
                        material->setEmittedLight( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set emitted light to " << material->emittedLight();
#endif
                    }
                    else if ( xml.name() == QLatin1String("texture"))
                    {
                        effect->d->emissiveTexture = processTextureElement( xml, resultState );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set emissive texture to " << effect->d->emissiveTexture;
#endif
                    } else if ( xml.name() == QLatin1String("param"))
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("ambient") )
            {
                if ( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if (xml.name() == QLatin1String("color"))
                    {
                        material->setAmbientColor( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set ambient color to " << material->ambientColor();
#endif
                    }
                    else if ( xml.name() == QLatin1String("texture"))
                    {
                        effect->d->ambientTexture = processTextureElement( xml, resultState );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set ambient texture to " << effect->d->ambientTexture;
#endif
                    } else if ( xml.name() == QLatin1String("param"))
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("diffuse") )
            {
                if ( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if (xml.name() == QLatin1String("color"))
                    {
                        material->setDiffuseColor( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set diffuse color to " << material->diffuseColor();
#endif
                    }
                    else if ( xml.name() == QLatin1String("texture"))
                    {
                        effect->d->diffuseTexture = processTextureElement( xml, resultState );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set diffuse texture to " << effect->d->diffuseTexture;
#endif
                    } else if ( xml.name() == QLatin1String("param"))
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("specular") )
            {
                if ( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if (xml.name() == QLatin1String("color"))
                    {
                        material->setSpecularColor( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set specular color to " << material->specularColor();
#endif
                    }
                    else if ( xml.name() == QLatin1String("texture"))
                    {
                        effect->d->specularTexture = processTextureElement( xml, resultState );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set specular texture to " << effect->d->specularTexture;
#endif
                    } else if ( xml.name() == QLatin1String("param"))
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("shininess") )
            {
                if ( xml.readNextStartElement() )
                {
                    float shininess = processParamOrFloatElement( xml );
                    if ( 0.0 < shininess && shininess < 1.0 )
                    {
                        qWarning() << "Warning: Blinn-Torrance-Sparrow specular lighting not suported ( line" << xml.lineNumber()
                                << "), converting to Blinn-Phong specular model";
                        material->setShininess( int( shininess * 128.0 ));
                    }
                    else
                        material->setShininess( int( shininess ) );
#ifdef DEBUG_MATERIALS
                    qDebug() << "set shininess to " << material->shininess();
#endif
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("reflective") )
            {
                qWarning() << "Warning reflective not supported ( line" << xml.lineNumber() << ")";
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("reflectivity") )
            {
                qWarning() << "Warning: reflectivity not supported ( line" << xml.lineNumber() << ")";
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("transparent") )
            {
                if ( xml.readNextStartElement() )
                {
                    if (xml.name() == QLatin1String("texture"))
                    {
                        QGLTexture2D* transparentTexture = processTextureElement( xml , resultState );
                        Q_UNUSED(transparentTexture);
                        qWarning() << "Warning: transparent not supported ( line" << xml.lineNumber() << ")";
#ifdef DEBUG_MATERIALS
                        qDebug() << "unused transparent texture" << transparentTexture;
#endif
                    }
                    else if (xml.name() == QLatin1String("color"))
                    {
                        QColor transparent = processColorElement( xml );
                        Q_UNUSED( transparent );
                        qWarning() << "Warning: transparent not supported ( line" << xml.lineNumber() << ")";
#ifdef DEBUG_MATERIALS
                        qDebug() << "unused transparent color of " << transparent;
#endif
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if ( xml.name() == QLatin1String("transparency") )
            {
                if ( xml.readNextStartElement() )
                {
                    float transparency = processParamOrFloatElement( xml );
                    if ( transparency < 1.0 )
                    {
                        qWarning() << "Warning: transparency not supported";
                    }
                    xml.skipCurrentElement();
                    xml.readNextStartElement();
                }
            }

            if ( xml.name() == QLatin1String("index_of_refraction") )
            {
                if ( xml.readNextStartElement() )
                {
                    float indexOfRefraction = processParamOrFloatElement( xml );
                    Q_UNUSED( indexOfRefraction );
                    qWarning() << "Warning: index_of_refraction not supported ( line" << xml.lineNumber() << ")";
                    xml.skipCurrentElement();
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }
        }
        // end of lighting scope

        effect->setMaterial( material );

        switch(effect->lighting())
        {
        case QGLColladaFxEffect::PhongLighting:
        case QGLColladaFxEffect::LambertLighting:
            qWarning() << "Warning: requested lighting not supported, using Blinn-Phong instead";
        case QGLColladaFxEffect::BlinnLighting:
            effect->addBlinnPhongLighting();
        case QGLColladaFxEffect::ConstantLighting:
        case QGLColladaFxEffect::NoLighting:
        default:
            break;
        }
    };

    while ( xml.name() == QLatin1String("pass") && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        processPassElement( xml, resultState, effect);
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    // Make sure to exit cleanly
    findEndTag( xml, QLatin1String("technique") );

    return effect;
}



QGLColladaParam* QGLColladaFxEffectFactory::processNewparamElement( QXmlStreamReader& xml, ResultState* resultState)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString sidString = attributes.value(QLatin1String("sid")).toString();
    QGLColladaParam* result = 0;
    if (xml.readNextStartElement())
    {
        if (xml.name().toString().left(5) == QLatin1String("float"))
        {

            QVariant floatValues = processFloatList( xml );
            resultState->paramSids[sidString] = floatValues;

            if ( xml.name() == QLatin1String("float")
                && ( floatValues.type() !=
                     static_cast<QVariant::Type>(QMetaType::Float )))
            {
                qWarning() << "Warning: parsed type incorrectly, expected float ( line" << xml.lineNumber() << ")";
                resultState->paramSids[sidString] = floatValues.value<float>();
            }
            else if ( xml.name() == QLatin1String("float2")
                     && floatValues.type() != QVariant::Vector2D )
            {
                qWarning() << "Warning: parsed type incorrectly, expected float2 ( line" << xml.lineNumber() << ")";
            }
            else if ( xml.name() == QLatin1String("float3")
                     && floatValues.type() != QVariant::Vector3D )
            {
                qWarning() << "Warning: parsed type incorrectly, expected float3 ( line" << xml.lineNumber() << ")";
            }
            else if ( xml.name() == QLatin1String("float4")
                     && floatValues.type() != QVariant::Vector4D)
            {
                qWarning() << "Warning: parsed type incorrectly, expected float4 ( line" << xml.lineNumber() << ")";
            }
        } else if ( xml.name() == QLatin1String("sampler2D") )
        {
            processSampler2DElement( xml , resultState, sidString );
        } else if ( xml.name() == QLatin1String("surface") )
        {
            result = processSurfaceElement( xml, resultState, sidString);
        } else {
            qWarning() << "unrecognized parameter type ( line:" << xml.lineNumber() << ")";
            findEndTag( xml, QLatin1String("newparam") );
            return 0;
        }
    }
    findEndTag(xml, QLatin1String("newparam"));
    return result;
}


/*!
  \internal
  the library_images can come after the library_effects, so textures referenced
  in effects might not have been defined when the effect was created.  Try and
  resolve those images now.  (Other properties - wrap, mipmap and filters etc
  should have been resolved when the texture was originally parsed).
  */
bool QGLColladaFxEffectFactory::resolveTexture2DImage(QGLTexture2D *texture, ResultState *resultState, QString paramName)
{
    if (texture == 0)
    {
        qWarning() << "Warning: Cannot resolve images for null QGLTexture2D";
        return false;
    }
    QVariant samplerParam = findParameterVariant(resultState, paramName);
    QString surfaceName = samplerParam.value<QString>();
    QImage image;

    if (!surfaceName.isEmpty())
    {
        QVariant surfaceParam =  findParameterVariant(resultState, surfaceName);
        QString initFrom = surfaceParam.value<QString>();
        if (!initFrom.isEmpty())
        {
            image = resolveImageURI(resultState, initFrom);
        }
    }

    // If that's failed, try again with the passed in paramName
    if (image.isNull())
    {
        image = resolveImageURI(resultState, paramName);
    }

    texture->setImage(image);
    return !image.isNull();
}


QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGLTexture2D*);

QT_BEGIN_NAMESPACE
/*!
    \internal
    Parses and consumes a texture collada element from \a xml.
*/
QGLTexture2D* QGLColladaFxEffectFactory::processTextureElement( QXmlStreamReader& xml , ResultState* resultState )
{
    QGLTexture2D* result = new QGLTexture2D();
    QXmlStreamAttributes attributes = xml.attributes();

    foreach (QXmlStreamAttribute attribute, attributes)
    {
        if ( attribute.name() == QLatin1String("texture") )
        {
            QString paramName = attribute.value().toString();

            // in Collada Fx Textures must reference a previously defined
            // sampler2D param.
            // However, this sampler may refer to images in the library_images,
            // which is parsed after the library_effects, so try and resolve
            // now, but save failures to try again later.

            if (!resolveTexture2DImage(result, resultState, paramName))
            {
                resultState->unresolvedTexture2Ds[result] = paramName;
            }
        } else if ( attribute.name() == QLatin1String("texcoord") )
        {
            // TODO: Create a repository for this sort of data, to be used in creating the shader progams later
            // I'm pretty sure the effect is going to need to be passed in.
            qWarning() << "texcoord not supported yet: " << attribute.name() << attribute.value() << " ( line" << xml.lineNumber() << ")";
        } else if (attribute.name() == QLatin1String("extra"))
        {
            qWarning() << "extra elements in texture elements not supported ( line" << xml.lineNumber() << ")";
        }
    }
    xml.skipCurrentElement();

    return result;
}

/*!
  \internal
  Try and get an image to attach to a texture.  The URI could be a reference to
  a param in the collada document (which may in turn be a reference), or
  file referenced either absolutely or relative to the original dae file.
  (It could concievably be a web URL, but that is not handled here.)
  */
QImage QGLColladaFxEffectFactory::resolveImageURI(ResultState *resultState, QString URI)
{
    QImage result;
    QString imageFileName;
    QString workingURI = URI;
    if (workingURI.length() > 0 && workingURI.at(0) == QLatin1Char('#'))
    {
        workingURI = workingURI.right(workingURI.length() - 1);
    }

    QVariant potentialParameter = findParameterVariant(resultState, workingURI);
    // Might be parameter itself:
    if ( !potentialParameter.value<QImage>().isNull() )
        return potentialParameter.value<QImage>();
    // or might be another URI
    if (!potentialParameter.value<QString>().isNull())
    {
        imageFileName = potentialParameter.value<QString>();
    } else {
        imageFileName = workingURI;
    }

    // First try as a relative path.
    QString filePath = resultState->sourceDir.path() + QLatin1Char('/') + imageFileName;
    result.load(filePath);
    if (result.isNull())
    {
        // No relative file found, so try as an absolute path
        result.load(imageFileName);
    }
    return result;
}

/*!
  \internal
  Parses and consumes an image element from \a xml.
*/
void QGLColladaFxEffectFactory::processImageElement( QXmlStreamReader& xml, ResultState* resultState )
{
    // 1.4 has a bunch of optional values in the attributes:
    QString sid = xml.attributes().value(QLatin1String("sid")).toString();
    QString id = xml.attributes().value(QLatin1String("id")).toString();
    QString name = xml.attributes().value(QLatin1String("name")).toString();

    QString height = xml.attributes().value(QLatin1String("height")).toString();
    QString width = xml.attributes().value(QLatin1String("width")).toString();
    QString depth = xml.attributes().value(QLatin1String("depth")).toString();

    Q_UNUSED(height);
    Q_UNUSED(width);
    Q_UNUSED(depth);

    QImage result;

    xml.readNextStartElement();
    if (xml.name() == QLatin1String("asset"))
    {
        qWarning() << "asset element not supported in image elements ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    if (xml.name() == QLatin1String("init_from"))
    {
        QString imageFileName = xml.readElementText().trimmed();
        QDir sourceDir = resultState->sourceDir;
        // ignore path information for resources
        QString filePath = sourceDir.path() + QLatin1Char('/') + imageFileName;
        result.load(filePath);
        if (result.isNull())
        {
            // Catch resources or files with absolute paths
            result.load(imageFileName);
        }
        if (!sid.isEmpty())
            resultState->paramSids[sid] = result;
        if (!id.isEmpty())
            resultState->paramIds[id] = result;
        if (!name.isEmpty())
            resultState->paramNames[name] = result;
    }

    // exit cleanly
    findEndTag( xml, QLatin1String("image"));
}

QStringList QGLColladaFxEffectFactory::glslProfileFromEffect(QGLColladaFxEffect* effect, QString sid)
{
    Q_UNUSED(effect)
    Indent indent;
    QStringList result;
    result += indent + QLatin1String("<profile_GLSL>");
    {
        result += generateCodeElements(effect, sid);
        result += indent + QLatin1String("<technique sid=\"") + sid + QLatin1String("\">");
        {
            Indent indent;
            result += indent + QLatin1String("<pass>");
            result += generateProgramElement(effect, sid);
            result += indent + QLatin1String("</pass>");
        }
        result += indent + QLatin1String("</technique>");
    }
    result += indent + QLatin1String("</profile_GLSL>");

    return result;
}

QStringList QGLColladaFxEffectFactory::generateProgramElement(QGLColladaFxEffect* effect, QString techniqueSid)
{
    QStringList result;
    QString vertexShaderRefSid = QLatin1String("VertexShaderRefSidRefsCodeOrIncludeAtProfileOrEffectLevel");
    QString fragmentShaderRefSid = QLatin1String("FragmentShaderRefSidRefsCodeOrIncludeAtProfileOrEffectLevel");
    Indent indent;
    result += indent + QLatin1String("<program>");
    result += generateShaderElement(effect, techniqueSid + QLatin1String("VertexShader"), techniqueSid + QLatin1String("FragmentShader"));
    // 0 or more
    result += generateBindAttributeElement( effect );
    // 0 or more
    result += generateBindUniformElements( effect );
    result += indent + QLatin1String("</program>");
    return result;
}

QStringList QGLColladaFxEffectFactory::generateShaderElement( QGLColladaFxEffect* effect, QString vertexShaderRefSid, QString fragmentShaderRefSid )
{
    Q_UNUSED(effect);
    QStringList result;
    Indent indent;
    result += indent + QLatin1String("<shader stage=\"VERTEX\">");
    {
        Indent indent;
        result += indent + QLatin1String("<sources>");
        {
            // 0 or more <import> elements
            Indent indent;
            result += indent + QLatin1String("<import ref=\"") + vertexShaderRefSid + QLatin1String("\"/>");
        }
        result += indent + QLatin1String("</sources>");
        // 0 or <extra> elements;
    }
    result += indent + QLatin1String("</shader>");

    result += indent + QLatin1String("<shader stage=\"FRAGMENT\">");
    {
        Indent indent;
        result += indent + QLatin1String("<sources>");
        {
            Indent indent;
            result += indent + QLatin1String("<import ref=\"") + fragmentShaderRefSid + QLatin1String("\"/>");
        }
        result += indent + QLatin1String("</sources>");
        // <extra> element(s) here if necessary;
    }
    result += indent + QLatin1String("</shader>");
    return result;
}

QStringList QGLColladaFxEffectFactory::generateBindAttributeElement( QGLColladaFxEffect* effect )
{
    // Currently no need for bind_attribute elements.
    Q_UNUSED(effect);
    QStringList result;
    //    Indent indent;
    //    result += indent + "<bind_attribute>";
    //    result += indent + "</bind_attribute>";
    return result;
}

QStringList generateBindUniformParamElement( QString symbol, QString ref)
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;
    result += indent + QLatin1String("<bind_uniform symbol=\"") + symbol + QLatin1String("\">");
    {
        Indent indent;
        result += indent + QLatin1String("<param ref=\"") + ref + QLatin1String("\">");
    }
    result += indent + QLatin1String("</bind_uniform>");
    return result;
}

QStringList generateBindUniformParamElement( QString symbol, const QVector3D& value)
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;
    result += indent + QLatin1String("<bind_uniform symbol=\"") + symbol + QLatin1String("\">");
    {
        Indent indent;
        result += indent + QString(QLatin1String("<float3> %1 %2 %3 </float3>")).arg(value.x()).arg(value.y()).arg(value.z());
    }
    result += indent + QLatin1String("</bind_uniform>");
    return result;
}

QStringList generateBindUniformParamElement( QString symbol, const QColor& value)
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;
    result += indent + QLatin1String("<bind_uniform symbol=\"") + symbol + QLatin1String("\">");
    {
        Indent indent;
        result += indent + QString(QLatin1String("<float3> %1 %2 %3 </float3>")).arg(value.redF()).arg(value.greenF()).arg(value.blueF());
    }
    result += indent + QLatin1String("</bind_uniform>");
    return result;
}


QStringList QGLColladaFxEffectFactory::generateBindUniformElements( QGLColladaFxEffect* effect )
{
    QStringList result;
    if (effect == 0)
        return result;
    //    // 0 or more <bind_uniform> elements
    // Example uniforms
    //        result += generateBindUniformParamElement( "exampleRefSymbol", QString("exampleRef"));
    //        result += generateBindUniformParamElement( "exampleFloat3Symbol", QVector3D(0.1, 0.2, 0.3) );

    if (effect->material() != 0)
    {
        QGLMaterial* material = effect->material();

        // Actual uniforms:
        result += generateBindUniformParamElement( QLatin1String("ambientColor"), material->ambientColor());
        result += generateBindUniformParamElement( QLatin1String("diffuseColor"), material->diffuseColor());
        result += generateBindUniformParamElement( QLatin1String("emittedLight"), material->emittedLight());
        result += generateBindUniformParamElement( QLatin1String("objectName"), material->objectName());
        result += generateBindUniformParamElement( QLatin1String("shininess"), material->shininess());
        result += generateBindUniformParamElement( QLatin1String("specularColor"), material->specularColor());

        effect->supportsPicking();

        // TODO: Find and store effect uniforms
        //    effect->bindProgramUniforms();
    }
    return result;
}

QStringList QGLColladaFxEffectFactory::generateCodeElements( QGLColladaFxEffect* effect, QString baseSid )
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;

    // put all this on one line to avoid adding carriage returns to the
    // shader programs
    result += indent + QLatin1String("<code sid=\"") + baseSid + QLatin1String("VertexShader\">")
              + effect->vertexShader() + QLatin1String("</code>");

    result += indent + QLatin1String("<code sid=\"") + baseSid + QLatin1String("FragmentShader\">")
              + effect->fragmentShader() + QLatin1String("</code>");

    return result;
}

void QGLColladaFxEffectFactory::processProgramElement( QXmlStreamReader& xml, ResultState* resultState, QGLColladaFxEffect* effect )
{
    // A profile_GLSL shader element is
    // 0 or more <shader>
    // 0 or more <bind_attribute>
    // 0 or more <bind_uniform>

    xml.readNextStartElement();

    while ( xml.name() == QLatin1String("shader") )
    {
        // in profile_GLSL a shader is
        // exactly 1  <source>
        // 0 or more <extra>

        QString stage = xml.attributes().value(QLatin1String("stage")).toString();
        xml.readNextStartElement();
        if ( xml.name() == QLatin1String("sources") )
        {
            // a <sources> element is
            // 1 or more <inline> elements
            // 0 or more <import> elements
            // Note: child elements can appear in any order

            xml.readNextStartElement();
            while ( (xml.name() == QLatin1String("inline") || xml.name() == QLatin1String("import")) && xml.tokenType() == QXmlStreamReader::StartElement)
            {
                if ( xml.name() == QLatin1String("import"))
                {
                    QString ref = xml.attributes().value(QLatin1String("ref")).toString();

                    QXmlStreamAttribute attr;
                    if (xml.attributes().count())
                    {
                        attr = xml.attributes().first();
                    }

                    QVariant param = findParameterVariant(resultState, ref);
                    if (param.isNull() || param.type() != QVariant::String)
                    {
                        qWarning() << "null or unexpected parameter found in import element ( line"
                                <<  xml.lineNumber()<<")";
                    }
                    else
                    {
                        if (stage == QLatin1String("VERTEX"))
                        {
                            effect->setVertexShader( param.value<QString>().toLatin1() );
                        }
                        else if (stage == QLatin1String("FRAGMENT"))
                        {
                            effect->setFragmentShader( param.value<QString>().toLatin1() );
                        } else
                        {
                            qWarning() << "unrecognized shader stage: "
                                    << stage << " ( line" <<  xml.lineNumber()<<")";
                        }
                    }

                } else if ( xml.name() == QLatin1String("inline"))
                {

                }
                xml.readNextStartElement();
            }
        } else {
            qWarning() << "collada parsing error. expected <sources> element ( line"
                    <<  xml.lineNumber()<<")";
        }

        if (xml.name() == QLatin1String("extra"))
            qWarning() << "Warning: extra element not supported in profile_GLSL <shader> element ( line" << xml.lineNumber()<<")";

        findEndTag ( xml, QLatin1String("shader"));
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("bind_attribute") )
    {
        qWarning() << "Warning: bind_attribute element not supported ( line" << xml.lineNumber()<<")";
        findEndTag ( xml, QLatin1String("bind_attribute"));
        xml.readNextStartElement();
    }

    while ( xml.name() == QLatin1String("bind_uniform") )
    {
        qWarning() << "Warning: bind_uniform element not supported ( line" << xml.lineNumber()<<")";
        findEndTag ( xml, QLatin1String("bind_uniform"));
        xml.readNextStartElement();
    }

    findEndTag(xml, QLatin1String("program"));
    return;
}

QT_END_NAMESPACE
