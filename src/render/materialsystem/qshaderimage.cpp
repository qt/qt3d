/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qshaderimage.h"
#include "qshaderimage_p.h"
#include <Qt3DRender/qabstracttexture.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QShaderImagePrivate::QShaderImagePrivate()
    : Qt3DCore::QNodePrivate()
    , m_texture(nullptr)
    , m_mipLevel(0)
    , m_layer(0)
    , m_access(QShaderImage::ReadWrite)
    , m_format(QShaderImage::Automatic)
    , m_layered(false)
{
}

QShaderImagePrivate::~QShaderImagePrivate()
{
}

/*!
    \qmltype ShaderImage
    \instantiates Qt3DRender::QShaderImage
    \inqmlmodule Qt3D.Render
    \since 5.14
    \brief Provides Image access to shader programs.

    To make the content of textures available for read and write operations in
    a shader, they need to exposed as ShaderImage. Textures can be composed of
    several mip levels, layers and faces. Additionally declaring a ShaderImage
    allows specifying which level, layer or face of the texture content we want
    to access.

    ShaderImage has to be assigned as a Parameter's value and reference a valid
    Qt3D.Render.AbstractTexture to work properly.

    If the referenced texture is a one-dimensional array, two-dimensional array,
    three-dimensional, cube map, cube map array, or two-dimensional multisample
    array texture, it is possible to bind either the entire texture level or a
    single layer or face of the texture level. This can be controlled with the
    \l layered property.

    Support for ShaderImage is only supported with OpenGL 4 and partially with
    OpenGL ES 3.1 and 3.2.

    OpenGL 4 supports the following image types:
    \table
    \header
       \li GLSL Type
       \li OpenGL Type Enum
       \li Texture Type
    \row
       \li image1D
       \li GL_IMAGE_1D
       \li Texture1D
    \row
       \li image2D
       \li GL_IMAGE_2D
       \li Texture2D
    \row
       \li image3D
       \li GL_IMAGE_3D
       \li Texture3D
    \row
       \li image2DRect
       \li GL_IMAGE_2D_RECT
       \li TextureRectangle
    \row
       \li imageCube
       \li GL_IMAGE_CUBE
       \li TextureCubeMap
    \row
       \li imageBuffer
       \li GL_IMAGE_BUFFER
       \li TextureBuffer
    \row
       \li image1DArray
       \li GL_IMAGE_1D_ARRAY
       \li Texture1DArray
    \row
       \li image2DArray
       \li GL_IMAGE_2D_ARRAY
       \li Texture2DArray
    \row
       \li imageCubeArray
       \li GL_IMAGE_CUBE_MAP_ARRAY
       \li TextureCubeMapArray
    \row
       \li image2DMS
       \li GL_IMAGE_2D_MULTISAMPLE
       \li Texture2DMultisample
    \row
       \li image2DMSArray
       \li GL_IMAGE_2D_MULTISAMPLE_ARRAY
       \li Texture2DMultisampleArray
    \row
       \li iimage1D
       \li GL_INT_IMAGE_1D
       \li Texture1D
    \row
       \li iimage2D
       \li GL_INT_IMAGE_2D
       \li Texture2D
    \row
       \li iimage3D
       \li GL_INT_IMAGE_3D
       \li Texture3D
    \row
       \li iimage2DRect
       \li GL_INT_IMAGE_2D_RECT
       \li TextureRectangle
    \row
       \li iimageCube
       \li GL_INT_IMAGE_CUBE
       \li TextureCubeMap
    \row
       \li iimageBuffer
       \li GL_INT_IMAGE_BUFFER
       \li TextureBuffer
    \row
       \li iimage1DArray
       \li GL_INT_IMAGE_1D_ARRAY
       \li Texture1DArray
    \row
       \li iimage2DArray
       \li GL_INT_IMAGE_2D_ARRAY
       \li Texture2DArray
    \row
       \li iimageCubeArray
       \li GL_INT_IMAGE_CUBE_MAP_ARRAY
       \li TextureCubeMapArray
    \row
       \li iimage2DMS
       \li GL_INT_IMAGE_2D_MULTISAMPLE
       \li Texture2DMultisample
    \row
       \li iimage2DMSArray
       \li GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
       \li Texture2DMultisampleArray
    \row
       \li uimage1D
       \li GL_UNSIGNED_INT_IMAGE_1D
       \li Texture1D
    \row
       \li uimage2D
       \li GL_UNSIGNED_INT_IMAGE_2D
       \li Texture2D
    \row
       \li uimage3D
       \li GL_UNSIGNED_INT_IMAGE_3D
       \li Texture3D
    \row
       \li uimage2DRect
       \li GL_UNSIGNED_INT_IMAGE_2D_RECT
       \li TextureRectangle
    \row
       \li uimageCube
       \li GL_UNSIGNED_INT_IMAGE_CUBE
       \li TextureCubeMap
    \row
       \li uimageBuffer
       \li GL_UNSIGNED_INT_IMAGE_BUFFER
       \li TextureBuffer
    \row
       \li uimage1DArray
       \li GL_UNSIGNED_INT_IMAGE_1D_ARRAY
       \li Texture1DArray
    \row
       \li uimage2DArray
       \li GL_UNSIGNED_INT_IMAGE_2D_ARRAY
       \li Texture2DArray
    \row
       \li uimageCubeArray
       \li GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
       \li TextureCubeMapArray
    \row
       \li uimage2DMS
       \li GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
       \li Texture2DMultisample
    \row
       \li uimage2DMSArray
       \li GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
       \li Texture2DMultisampleArray
    \endtable

    OpenGL ES 3.1 supports the following image types:
    \table
    \header
       \li GLSL Type
       \li OpenGL Type Enum
       \li Texture Type
    \row
       \li image2D
       \li GL_IMAGE_2D
       \li Texture2D
    \row
       \li image3D
       \li GL_IMAGE_3D
       \li Texture3D
    \row
       \li imageCube
       \li GL_IMAGE_CUBE
       \li TextureCubeMap
    \row
       \li image2DArray
       \li GL_IMAGE_2D_ARRAY
       \li Texture2DArray
    \row
       \li iimage2D
       \li GL_INT_IMAGE_2D
       \li Texture2D
    \row
       \li iimage3D
       \li GL_INT_IMAGE_3D
       \li Texture3D
    \row
       \li iimageCube
       \li GL_INT_IMAGE_CUBE
       \li TextureCubeMap
    \row
       \li iimage2DArray
       \li GL_INT_IMAGE_2D_ARRAY
       \li Texture2DArray
    \row
       \li uimage2D
       \li GL_UNSIGNED_INT_IMAGE_2D
       \li Texture2D
    \row
       \li uimage3D
       \li GL_UNSIGNED_INT_IMAGE_3D
       \li Texture3D
    \row
       \li uimageCube
       \li GL_UNSIGNED_INT_IMAGE_CUBE
       \li TextureCubeMap
    \row
       \li uimage2DArray
       \li GL_UNSIGNED_INT_IMAGE_2D_ARRAY
       \li Texture2DArray
    \endtable

    OpenGL ES 3.2 supports all of the OpenGL ES 3.1 image types as well as the
    following:
    \table
    \header
       \li GLSL Type
       \li OpenGL Type Enum
       \li Texture Type
    \row
       \li imageBuffer
       \li GL_IMAGE_BUFFER
       \li TextureBuffer
    \row
       \li imageCubeArray
       \li GL_IMAGE_CUBE_MAP_ARRAY
       \li TextureCubeMapArray
    \row
       \li iimageBuffer
       \li GL_IMAGE_BUFFER
       \li TextureBuffer
    \row
       \li iimageCubeArray
       \li GL_INT_IMAGE_CUBE_MAP_ARRAY
       \li TextureCubeMapArray
    \row
       \li uimageBuffer
       \li GL_UNSIGNED_INT_IMAGE_BUFFER
       \li TextureBuffer
    \row
       \li uimageCubeArray
       \li GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
       \li TextureCubeMapArray
    \endtable

    Expected use would look like:

    \badcode

    import Qt3D.Render 2.14

    Entity {
    ...
        Texture2D {
            id: tex2D
            ...
        }

        Material {
            parameters: Parameter {
                name: "imageUniformName"
                value: ShaderImage {
                    texture: tex2D
                }
            }
            ...
        }
     ...
    }
    \endcode
 */

/*!
    \qmlproperty int Qt3D.Render::ShaderImage::mipLevel

    Holds which mipLevel out of the referenced texture should be used for the
    ShaderImage.

    The default value is 0.
 */

/*!
    \qmlproperty int Qt3D.Render::ShaderImage::layer

    Holds which layer out of the referenced texture should be used for the
    ShaderImage. This property does nothing if \a layered is set to true or if
    the reference texture's type isn't compatible with layers.

    \note When the referenced texture is of type cube map or cube map array and
    \a ĺayered is set to false, the face and layer are retrieved in the
    following manner:
    \badcode
    cubeMapLayer = layer / 6
    cubeMapFace = layer - (cubeMapLayer * 6)
    \endcode

    The default value is 0.
 */

/*!
 * \qmlproperty bool Qt3D.Render::ShaderImage::layered

    If set to true, if the referenced texture is a one-dimensional array,
    two-dimensional array, three-dimensional, cube map, cube map array, or
    two-dimensional multisample array texture, the entire level will be bound
    for all layers. If set to false, only the single layer specified by the \a
    layer property will be bound.

    The default value is \c false.
 */

/*!
    \qmlproperty enumeration Qt3D.Render::ShaderImage::access

    Specifies the type of access we want to allow from our shader instances to
    the image. If a shader tries to write or read from an image that has
    incompatible access, the behavior is undefined.

    \value ShaderImage.ReadOnly
           Read-only access.
    \value ShaderImage.WriteOnly
           Write-only access.
    \value ShaderImage.ReadWrite
           Read-write access.

    The default value is ShaderImage.ReadWrite.
 */

/*!
    \qmlproperty enumeration Qt3D.Render::ShaderImage::format

    Specifies the image format, which is essentially important when storing values
    in the ShaderImage from a shader.

    The format doesn't have to be the same as the referenced texture's format.
    It however has to be compatible (matching in size but not necessarily by
    class type). For instance a texture of format R32F (size 32bits, class
    1x32) could be used with an image of format RGBA8I (size 32bits, class
    4x8). Table 8.27 of the
    \l{https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf}{OpenGL specifications}
    shows the size and class for all supported image formats.

    By default Qt3D will try to set the image format to match that of the
    referenced texture.

    The default value is ShaderImage.Automatic.
 */

/*!
    \class Qt3DRender::QShaderImage
    \inmodule Qt3DRender
    \since 5.14
    \brief Provides Image access to shader programs.

    To make the content of textures available for read and write operations in
    a shader, they need to exposed as QShaderImage. Textures can be composed of
    several mip levels, layers and faces. Additionally declaring a QShaderImage
    allows specifying which level, layer or face of the texture content we want
    to access.

    QShaderImage has to be assigned as a QParameter's value and reference a valid
    Qt3DRender::QAbstractTexture to work properly.

    If the referenced texture is a one-dimensional array, two-dimensional array,
    three-dimensional, cube map, cube map array, or two-dimensional multisample
    array texture, it is possible to bind either the entire texture level or a
    single layer or face of the texture level. This can be controlled with the
    \l layered property.

    Support for QShaderImage is only supported with OpenGL 4 and partially with
    OpenGL ES 3.1 and 3.2.

    OpenGL 4 supports the following image types:
    \table
    \header
       \li GLSL Type
       \li OpenGL Type Enum
       \li Texture Type
    \row
       \li image1D
       \li GL_IMAGE_1D
       \li QTexture1D
    \row
       \li image2D
       \li GL_IMAGE_2D
       \li QTexture2D
    \row
       \li image3D
       \li GL_IMAGE_3D
       \li QTexture3D
    \row
       \li image2DRect
       \li GL_IMAGE_2D_RECT
       \li QTextureRectangle
    \row
       \li imageCube
       \li GL_IMAGE_CUBE
       \li QTextureCubeMap
    \row
       \li imageBuffer
       \li GL_IMAGE_BUFFER
       \li QTextureBuffer
    \row
       \li image1DArray
       \li GL_IMAGE_1D_ARRAY
       \li QTexture1DArray
    \row
       \li image2DArray
       \li GL_IMAGE_2D_ARRAY
       \li QTexture2DArray
    \row
       \li imageCubeArray
       \li GL_IMAGE_CUBE_MAP_ARRAY
       \li QTextureCubeMapArray
    \row
       \li image2DMS
       \li GL_IMAGE_2D_MULTISAMPLE
       \li QTexture2DMultisample
    \row
       \li image2DMSArray
       \li GL_IMAGE_2D_MULTISAMPLE_ARRAY
       \li QTexture2DMultisampleArray
    \row
       \li iimage1D
       \li GL_INT_IMAGE_1D
       \li QTexture1D
    \row
       \li iimage2D
       \li GL_INT_IMAGE_2D
       \li QTexture2D
    \row
       \li iimage3D
       \li GL_INT_IMAGE_3D
       \li QTexture3D
    \row
       \li iimage2DRect
       \li GL_INT_IMAGE_2D_RECT
       \li QTextureRectangle
    \row
       \li iimageCube
       \li GL_INT_IMAGE_CUBE
       \li QTextureCubeMap
    \row
       \li iimageBuffer
       \li GL_INT_IMAGE_BUFFER
       \li QTextureBuffer
    \row
       \li iimage1DArray
       \li GL_INT_IMAGE_1D_ARRAY
       \li QTexture1DArray
    \row
       \li iimage2DArray
       \li GL_INT_IMAGE_2D_ARRAY
       \li QTexture2DArray
    \row
       \li iimageCubeArray
       \li GL_INT_IMAGE_CUBE_MAP_ARRAY
       \li QTextureCubeMapArray
    \row
       \li iimage2DMS
       \li GL_INT_IMAGE_2D_MULTISAMPLE
       \li QTexture2DMultisample
    \row
       \li iimage2DMSArray
       \li GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
       \li QTexture2DMultisampleArray
    \row
       \li uimage1D
       \li GL_UNSIGNED_INT_IMAGE_1D
       \li QTexture1D
    \row
       \li uimage2D
       \li GL_UNSIGNED_INT_IMAGE_2D
       \li QTexture2D
    \row
       \li uimage3D
       \li GL_UNSIGNED_INT_IMAGE_3D
       \li QTexture3D
    \row
       \li uimage2DRect
       \li GL_UNSIGNED_INT_IMAGE_2D_RECT
       \li QTextureRectangle
    \row
       \li uimageCube
       \li GL_UNSIGNED_INT_IMAGE_CUBE
       \li QTextureCubeMap
    \row
       \li uimageBuffer
       \li GL_UNSIGNED_INT_IMAGE_BUFFER
       \li QTextureBuffer
    \row
       \li uimage1DArray
       \li GL_UNSIGNED_INT_IMAGE_1D_ARRAY
       \li QTexture1DArray
    \row
       \li uimage2DArray
       \li GL_UNSIGNED_INT_IMAGE_2D_ARRAY
       \li QTexture2DArray
    \row
       \li uimageCubeArray
       \li GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
       \li QTextureCubeMapArray
    \row
       \li uimage2DMS
       \li GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
       \li QTexture2DMultisample
    \row
       \li uimage2DMSArray
       \li GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
       \li QTexture2DMultisampleArray
    \endtable

    OpenGL ES 3.1 supports the following image types:
    \table
    \header
       \li GLSL Type
       \li OpenGL Type Enum
       \li Texture Type
    \row
       \li image2D
       \li GL_IMAGE_2D
       \li QTexture2D
    \row
       \li image3D
       \li GL_IMAGE_3D
       \li QTexture3D
    \row
       \li imageCube
       \li GL_IMAGE_CUBE
       \li QTextureCubeMap
    \row
       \li image2DArray
       \li GL_IMAGE_2D_ARRAY
       \li QTexture2DArray
    \row
       \li iimage2D
       \li GL_INT_IMAGE_2D
       \li QTexture2D
    \row
       \li iimage3D
       \li GL_INT_IMAGE_3D
       \li QTexture3D
    \row
       \li iimageCube
       \li GL_INT_IMAGE_CUBE
       \li QTextureCubeMap
    \row
       \li iimage2DArray
       \li GL_INT_IMAGE_2D_ARRAY
       \li QTexture2DArray
    \row
       \li uimage2D
       \li GL_UNSIGNED_INT_IMAGE_2D
       \li QTexture2D
    \row
       \li uimage3D
       \li GL_UNSIGNED_INT_IMAGE_3D
       \li QTexture3D
    \row
       \li uimageCube
       \li GL_UNSIGNED_INT_IMAGE_CUBE
       \li QTextureCubeMap
    \row
       \li uimage2DArray
       \li GL_UNSIGNED_INT_IMAGE_2D_ARRAY
       \li QTexture2DArray
    \endtable

    OpenGL ES 3.2 supports all of the OpenGL ES 3.1 image types as well as the
    following:
    \table
    \header
       \li GLSL Type
       \li OpenGL Type Enum
       \li Texture Type
    \row
       \li imageBuffer
       \li GL_IMAGE_BUFFER
       \li QTextureBuffer
    \row
       \li imageCubeArray
       \li GL_IMAGE_CUBE_MAP_ARRAY
       \li QTextureCubeMapArray
    \row
       \li iimageBuffer
       \li GL_IMAGE_BUFFER
       \li QTextureBuffer
    \row
       \li iimageCubeArray
       \li GL_INT_IMAGE_CUBE_MAP_ARRAY
       \li QTextureCubeMapArray
    \row
       \li uimageBuffer
       \li GL_UNSIGNED_INT_IMAGE_BUFFER
       \li QTextureBuffer
    \row
       \li uimageCubeArray
       \li GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
       \li QTextureCubeMapArray
    \endtable

    Expected use would look like:

    \badcode
    Qt3DRender::QTexture2D *tex2D = new Qt3DRender::QTexture2D();
    ...
    Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
    ...
    Qt3DRender::QParameter *imageParameter = new Qt3DRender::QParameter();
    Qt3DRender::QShaderImage *shaderImage = new Qt3DRender::QShaderImage();

    shaderImage->setTexture(tex2D);

    imageParameter->setName("imageUniformName");
    imageParameter->setValue(QVariant::fromValue(shaderImage));

    material->addParameter(imageParamenter);
    \endcode
 */

/*!
    \property Qt3DRender::QShaderImage::mipLevel

    Holds which mipLevel out of the referenced texture should be used for the
    QShaderImage.

    The default value is 0.
*/

/*!
    \property Qt3DRender::QShaderImage::layer

    Holds which layer out of the referenced texture should be used for the
    QShaderImage. This property does nothing if \a layered is set to true or if the
    reference texture's type isn't compatible with layers.

    \note When the referenced texture is of type cube map or cube map array and
    \a ĺayered is set to false, the face and layer are retrieved in the
    following manner:
    \badcode
    cubeMapLayer = layer / 6
    cubeMapFace = layer - (cubeMapLayer * 6)
    \endcode

    The default value is 0.
 */

/*!
 * \property Qt3DRender::QShaderImage::layered

    If set to true, if the referenced texture is a one-dimensional array,
    two-dimensional array, three-dimensional, cube map, cube map array, or
    two-dimensional multisample array texture, the entire level will be bound
    for all layers. If set to false, only the single layer specified by the \a
    layer property will be bound.

    The default value is \c false.
 */

/*!
    \property Qt3DRender::QShaderImage::access

    Specifies the type of access we want to allow from our shader instances to
    the image. If a shader tries to write or read from an image that has
    incompatible access, the behavior is undefined.

    The default value is QShaderImage::ReadWrite.
 */

/*!
    \property Qt3DRender::QShaderImage::format

    Specifies the image format, which is essentially important when storing values
    in the Image from a shader.

    The format doesn't have to be the same as the referenced texture's format.
    It however has to be compatible (matching in size but not necessarily by
    class type). For instance a texture of format R32F (size 32bits, class
    1x32) could be used with an image of format RGBA8I (size 32bits, class
    4x8). Table 8.27 of the
    \l{https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf}{OpenGL specifications}
    shows the size and class for all supported Image formats.

    By default Qt3D will try to set the image format to match that of the
    referenced texture.

    The default value is QShaderImage::Automatic.
 */

/*!
    \enum Qt3DRender::QShaderImage::Access

    \value ReadOnly
           Image will only be read from in shaders
    \value WriteOnly
           Image will only be written into from shaders
    \value ReadWrite
           Image will only be read and written into from shaders
*/

/*!
    \enum Qt3DRender::QShaderImage::ImageFormat

    This list describes all possible image formats

    \value NoFormat
          GL_NONE
    \value Automatic
          Qt 3D will try to determine the format automatically based on
          the referenced texture.
    \value R8_UNorm
          GL_R8 (GLSL type r8, supported by OpenGL 4.2+)
    \value RG8_UNorm
          GL_RG8 (GLSL type rg8, supported by OpenGL 4.2+)
    \value RGBA8_UNorm
          GL_RGBA8 (GLSL type rgba8, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R16_UNorm
          GL_R16 (GLSL type r16, supported by OpenGL 4.2+)
    \value RG16_UNorm
          GL_RG16 (GLSL type rg16, supported by OpenGL 4.2+)
    \value RGBA16_UNorm
          GL_RGBA16 (GLSL type rgba16, supported by OpenGL 4.2+)
    \value R8_SNorm
          GL_R8_SNORM (GLSL type r8_snorm, supported by OpenGL 4.2+)
    \value RG8_SNorm
          GL_RG8_SNORM (GLSL type rg8_snorm, supported by OpenGL 4.2+)
    \value RGBA8_SNorm
          GL_RGBA8_SNORM (GLSL type rgba8_snorm, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R16_SNorm
          GL_R16_SNORM (GLSL type r16_snorm, supported by OpenGL 4.2+)
    \value RG16_SNorm
          GL_RG16_SNORM (GLSL type rg16_snorm, supported by OpenGL 4.2+)
    \value RGBA16_SNorm
          GL_RGBA16_SNORM (GLSL type rgba16_snorm, supported by OpenGL 4.2+)
    \value R8U
          GL_R8UI (GLSL type r8ui, supported by OpenGL 4.2+)
    \value RG8U
          GL_RG8UI (GLSL type rg8ui, supported by OpenGL 4.2+)
    \value RGBA8U
          GL_RGBA8UI (GLSL type rgba8ui, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R16U
          GL_R16UI (GLSL type r16ui, supported by OpenGL 4.2+)
    \value RG16U
          GL_RG16UI (GLSL type rg16ui, supported by OpenGL 4.2+)
    \value RGBA16U
          GL_RGBA16UI (GLSL type rgba16ui, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R32U
          GL_R32UI (GLSL type r32ui, supported by OpenGL 4.2+, OpenGL ES 3.1)
    \value RG32U
          GL_RG32UI (GLSL type rg32ui, supported by OpenGL 4.2+)
    \value RGBA32U
          GL_RGBA32UI (GLSL type rgba32ui, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R8I
          GL_R8I (GLSL type r8i, supported by OpenGL 4.2+)
    \value RG8I
          GL_RG8I (GLSL type rg8i, supported by OpenGL 4.2+)
    \value RGBA8I
          GL_RGBA8I (GLSL type rgba8i, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R16I
          GL_R16I (GLSL type r16i, supported by OpenGL 4.2+)
    \value RG16I
          GL_RG16I (GLSL type rg16i, supported by OpenGL 4.2+)
    \value RGBA16I
          GL_RGBA16I (GLSL type rgba16i, supported by OpenGL 4.2+, OpenGL ES 3.1)
    \value R32I
          GL_R32I (GLSL type r32i, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value RG32I
          GL_RG32I (GLSL type rg32i, supported by OpenGL 4.2+)
    \value RGBA32I
          GL_RGBA32I (GLSL type rgba32i, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R16F
          GL_R16F (GLSL type r16f, supported by OpenGL 4.2+)
    \value RG16F
          GL_RG16F (GLSL type rg16f, supported by OpenGL 4.2+)
    \value RGBA16F
          GL_RGBA16F (GLSL type rgba16f, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value R32F
          GL_R32F (GLSL type r32f, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value RG32F
          GL_RG32F (GLSL type rg32f, supported by OpenGL 4.2+)
    \value RGBA32F
          GL_RGBA32F (GLSL type rgba32f, supported by OpenGL 4.2+, OpenGL ES 3.1+)
    \value RG11B10F
          GL_R11F_G11F_B10F (GLSL type r11f_g11f_b10f, supported by OpenGL 4.2+)
    \value RGB10A2
          GL_RGB10_A2 (GLSL type rgb10_a2, supported by OpenGL 4.2+)
    \value RGB10A2U
          GL_RGB10_A2UI (GLSL type rgb10_a2ui, supported by OpenGL 4.2+)
*/


QShaderImage::QShaderImage(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QShaderImagePrivate, parent)
{
}

QShaderImage::~QShaderImage()
{

}

QAbstractTexture *QShaderImage::texture() const
{
    Q_D(const QShaderImage);
    return d->m_texture;
}

bool QShaderImage::layered() const
{
    Q_D(const QShaderImage);
    return d->m_layered;
}

int QShaderImage::mipLevel() const
{
    Q_D(const QShaderImage);
    return d->m_mipLevel;
}

int QShaderImage::layer() const
{
    Q_D(const QShaderImage);
    return d->m_layer;
}

QShaderImage::Access QShaderImage::access() const
{
    Q_D(const QShaderImage);
    return d->m_access;
}

QShaderImage::ImageFormat QShaderImage::format() const
{
    Q_D(const QShaderImage);
    return d->m_format;
}

void QShaderImage::setTexture(QAbstractTexture *texture)
{
    Q_D(QShaderImage);
    if (texture == d->m_texture)
        return;

    if (d->m_texture)
        d->unregisterDestructionHelper(d->m_texture);

    if (texture && !texture->parent())
        texture->setParent(this);

    d->m_texture = texture;

    if (d->m_texture)
        d->registerDestructionHelper(d->m_texture, &QShaderImage::setTexture, d->m_texture);

    Q_EMIT textureChanged(texture);
}

void QShaderImage::setLayered(bool layered)
{
    Q_D(QShaderImage);
    if (layered == d->m_layered)
        return;
    d->m_layered = layered;
    Q_EMIT layeredChanged(layered);
}

void QShaderImage::setMipLevel(int mipLevel)
{
    Q_D(QShaderImage);
    if (mipLevel == d->m_mipLevel)
        return;
    d->m_mipLevel = mipLevel;
    Q_EMIT mipLevelChanged(mipLevel);
}

void QShaderImage::setLayer(int layer)
{
    Q_D(QShaderImage);
    if (layer == d->m_layer)
        return;
    d->m_layer = layer;
    Q_EMIT layerChanged(layer);
}

void QShaderImage::setAccess(QShaderImage::Access access)
{
    Q_D(QShaderImage);
    if (access == d->m_access)
        return;
    d->m_access = access;
    Q_EMIT accessChanged(access);
}

void QShaderImage::setFormat(QShaderImage::ImageFormat format)
{
    Q_D(QShaderImage);
    if (format == d->m_format)
        return;
    d->m_format = format;
    Q_EMIT formatChanged(format);
}

Qt3DCore::QNodeCreatedChangeBasePtr Qt3DRender::QShaderImage::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QShaderImageData>::create(this);
    QShaderImageData &data = creationChange->data;
    Q_D(const QShaderImage);
    data.textureId = Qt3DCore::qIdForNode(d->m_texture);
    data.layer = d->m_layer;
    data.mipLevel = d->m_mipLevel;
    data.access = d->m_access;
    data.format = d->m_format;
    data.layered = d->m_layered;
    return creationChange;

}

} // namespace Qt3DRender

QT_END_NAMESPACE
