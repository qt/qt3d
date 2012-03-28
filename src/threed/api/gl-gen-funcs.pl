#!/usr/bin/perl
#############################################################################
##
## Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the Qt3D module of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## GNU Lesser General Public License Usage
## This file may be used under the terms of the GNU Lesser General Public
## License version 2.1 as published by the Free Software Foundation and
## appearing in the file LICENSE.LGPL included in the packaging of this
## file. Please review the following information to ensure the GNU Lesser
## General Public License version 2.1 requirements will be met:
## http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights. These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU General
## Public License version 3.0 as published by the Free Software Foundation
## and appearing in the file LICENSE.GPL included in the packaging of this
## file. Please review the following information to ensure the GNU General
## Public License version 3.0 requirements will be met:
## http://www.gnu.org/copyleft/gpl.html.
##
## Other Usage
## Alternatively, this file may be used in accordance with the terms and
## conditions contained in a signed written agreement between you and Nokia.
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################


#
# USAGE:
#
# perl gl-gen-funcs.pl < gl-functs.txt > output.txt
#
# This will automatically generate the qopenglfunctions.h/.cpp files for Qt3D
# based on the platform and available functions/capabilities.
#
# The file output.txt will contain the text for the header and implementation
# files - it is left to the user to manually cut and paste this content into
# their own qopenglfunctions.h and qopenglfunctions.cpp files.
#
# Note: this script is intended for perl version 5.10.1 or better, and has not
# been tested on previous versions of perl.
#

use strict;
use warnings;

use Data::Dumper;

# Read the function definitions from the input.
my @functions = ();
my @macros = ();
my %func_info = ();
while (<>)
{
    my ($returnType, $name, $argstr) =
        m/^(\w+|const GLubyte\*)\s+(\w+)\s*\(([^)]*)\)/;

    if (!$returnType) {
        my ($macroname, $macrovalue) = m/^#define\s+(\w+)\s+(.*)$/;
        if ($macroname) {
            my %macro_info = ();
            $macro_info{'name'} = $macroname;
            $macro_info{'value'} = $macrovalue;
            push @macros, { %macro_info };
            next;
        }
        my ($tag, $value) = m/^\s+(\w+)\s+(.*)$/;
        next unless $tag;
        $func_info{$tag} = $value;
        $func_info{'funcname'} = $value if ($tag eq "es_name");
        next;
    }

    if ($func_info{'name'}) {
        push @functions, { %func_info };
        %func_info = ();
    }

    $argstr =~ s/^\s+//;
    $argstr =~ s/\s+$//;
    my @args = split /,\s*/,$argstr;

    my @argNames = ();
    if ($argstr ne 'void') {
        foreach (@args) {
            my ($argType, $argName) = m/^(\w+|\w+.*\*)\s+(\w+)$/;
            push @argNames, $argName;
        }
    } else {
        $argstr = "";
    }
    my $argnamestr = join(', ', @argNames);

    $func_info{'name'} = $name;
    $func_info{'funcname'} = $name;
    $name =~ s/^gl//;
    $func_info{'varname'} = lcfirst($name);
    $func_info{'returnType'} = $returnType;
    $func_info{'argstr'} = $argstr;
    $func_info{'argnamestr'} = $argnamestr;
}
if ($func_info{'name'}) {
    push @functions, { %func_info };
}

# Generate the declarations for qopenglfunctions.h.
print "// qopenglfunctions.h\n\n";
print "#ifdef Q_WS_WIN\n";
print "#    define QT3D_GLF_APIENTRY APIENTRY\n";
print "#endif\n";
print "\n";
print "#ifndef Q_WS_MAC\n";
print "# ifndef QT3D_GLF_APIENTRYP\n";
print "#   ifdef QT3D_GLF_APIENTRY\n";
print "#     define QT3D_GLF_APIENTRYP QT3D_GLF_APIENTRY *\n";
print "#   else\n";
print "#     define QT3D_GLF_APIENTRY\n";
print "#     define QT3D_GLF_APIENTRYP *\n";
print "#   endif\n";
print "# endif\n";
print "#else\n";
print "# define QT3D_GLF_APIENTRY\n";
print "# define QT3D_GLF_APIENTRYP *\n";
print "#endif\n";
print "\n";
print "struct QOpenGLFunctionsPrivate;\n";
print "\n";

print "// Undefine any macros from GLEW, qglextensions_p.h, etc that\n";
print "// may interfere with the definition of QOpenGLFunctions.\n";
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    my $name = $_->{'funcname'};
    print "#undef $name\n";
}
print "\n";

# Output the prototypes into the QOpenGLFunctions class.
print "class Q_QT3D_EXPORT QOpenGLFunctions\n";
print "{\n";
print "public:\n";
print "    QOpenGLFunctions();\n";
print "    explicit QOpenGLFunctions(const QGLContext *context);\n";
print "    ~QOpenGLFunctions() {}\n";
print "\n";
print "    enum OpenGLFeature\n";
print "    {\n";
print "        Multitexture          = 0x0001,\n";
print "        Shaders               = 0x0002,\n";
print "        Buffers               = 0x0004,\n";
print "        Framebuffers          = 0x0008,\n";
print "        BlendColor            = 0x0010,\n";
print "        BlendEquation         = 0x0020,\n";
print "        BlendEquationSeparate = 0x0040,\n";
print "        BlendFuncSeparate     = 0x0080,\n";
print "        BlendSubtract         = 0x0100,\n";
print "        CompressedTextures    = 0x0200,\n";
print "        Multisample           = 0x0400,\n";
print "        StencilSeparate       = 0x0800,\n";
print "        NPOTTextures          = 0x1000\n";
print "    };\n";
print "    Q_DECLARE_FLAGS(OpenGLFeatures, OpenGLFeature)\n";
print "\n";
print "    QOpenGLFunctions::OpenGLFeatures openGLFeatures() const;\n";
print "    bool hasOpenGLFeature(QOpenGLFunctions::OpenGLFeature feature) const;\n";
print "\n";
print "    void initializeGLFunctions(const QGLContext *context = 0);\n";
print "\n";
my $last_shader_only = 0;
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $name = $_->{'funcname'};
    #print "#ifndef QT_OPENGL_ES_1\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n" if (!$shader_only && $last_shader_only);
    print "    $_->{'returnType'} $name($_->{'argstr'});\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;

print "\n";
print "private:\n";
print "    QOpenGLFunctionsPrivate *d_ptr;\n";
print "    static bool isInitialized(const QOpenGLFunctionsPrivate *d) { return d != 0; }\n";
print "};\n";
print "\n";
print "Q_DECLARE_OPERATORS_FOR_FLAGS(QOpenGLFunctions::OpenGLFeatures)\n";
print "\n";
print "struct QOpenGLFunctionsPrivate\n";
print "{\n";
print "    QOpenGLFunctionsPrivate(const QGLContext *context = 0);\n";
print "\n";
print "#ifndef QT_OPENGL_ES_2";
print "\n";

# Output the function pointers into the QOpenGLFunctionsPrivate class.
$last_shader_only = 0;
foreach ( @functions ) {
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    next if $inline && $inline eq 'all_diff';
    my $name = $_->{'varname'};
    #print "#ifndef QT_OPENGL_ES_1\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n" if (!$shader_only && $last_shader_only);
    print "    $_->{'returnType'} (QT3D_GLF_APIENTRYP $name)($_->{'argstr'});\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;

print "#endif\n";
print "};\n";
print "\n";

my %platform_defines = ();
$platform_defines{'es1'} = "defined(QT_OPENGL_ES_1)";
$platform_defines{'es2'} = "defined(QT_OPENGL_ES_2)";
$platform_defines{'es'} = "defined(QT_OPENGL_ES)";
$platform_defines{'desktop'} = "!defined(QT_OPENGL_ES)";

# Output the inline functions that call either the raw GL function
# or resolve via the function pointer.
$last_shader_only = 0;
foreach ( @functions ) {
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $funcname = $_->{'funcname'};
    my $varname = $_->{'varname'};
    my $is_void = ($_->{'returnType'} eq 'void');
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    #print "#ifndef QT_OPENGL_ES_1\n\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n\n" if (!$shader_only && $last_shader_only);
    print "inline $_->{'returnType'} QOpenGLFunctions::$funcname($_->{'argstr'})\n";
    print "{\n";
    if ($_->{'es_name'}) {
        # Functions like glClearDepth() that are inline, but named differently.
        print "#ifndef QT_OPENGL_ES\n";
        print "    ::$_->{'name'}($_->{'argnamestr'});\n";
        print "#else\n";
        print "    ::$_->{'es_name'}($_->{'argnamestr'});\n";
        print "#endif\n";
    } elsif ($inline && $inline eq 'all') {
        # Inlined on all platforms.
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "::$_->{'name'}($_->{'argnamestr'});\n";
    } elsif ($inline) {
        # Inlined only on certain platforms.
        my @platforms = split /,\s*/,$inline;
        my @defines = ();
        foreach (@platforms) {
            push @defines, $platform_defines{$_};
        }
        print "#if ";
        print join(' || ', @defines);
        print "\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "::$_->{'name'}($_->{'argnamestr'});\n";
        print "#else\n";
        print "    Q_ASSERT(QOpenGLFunctions::isInitialized(d_ptr));\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "d_ptr->$varname($_->{'argnamestr'});\n";
        print "#endif\n";
    } else {
        # Resolve on all platforms.
        print "    Q_ASSERT(QOpenGLFunctions::isInitialized(d_ptr));\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "d_ptr->$varname($_->{'argnamestr'});\n";
    }
    print "}\n\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;

# Output the macro definitions.
foreach ( @macros ) {
    my $name = $_->{'name'};
    my $value = $_->{'value'};
    print "#ifndef $name\n";
    print "#define $name $value\n";
    print "#endif\n";
}
print "\n";

print "// qopenglfunctions.cpp\n\n";

# Generate qdoc documentation for all of the functions.
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if $inline && $inline eq 'all';

    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');

    my $name = $_->{'funcname'};

    my $docargs = $_->{'argnamestr'};
    if (length($docargs) > 0) {
        $docargs =~ s/,/, \\a/g;
        $docargs =~ s/^/\\a /;
    }

    my $khronos_name = $_->{'es_name'};
    if (!$khronos_name) {
        $khronos_name = $_->{'name'};
    }

    print "/*!\n";
    print "    \\fn $_->{'returnType'} QOpenGLFunctions::$name($_->{'argstr'})\n";
    print "\n";
    if ($khronos_name eq $_->{'name'}) {
        print "    Convenience function that calls $khronos_name($docargs).\n";
    } else {
        print "    Convenience function that calls $_->{'name'}($docargs) on\n";
        print "    desktop OpenGL systems and $khronos_name($docargs) on\n";
        print "    embedded OpenGL/ES systems.\n";
    }
    print "\n";
    print "    For more information, see the OpenGL/ES 2.0 documentation for\n";
    print "    \\l{http://www.khronos.org/opengles/sdk/docs/man/$khronos_name.xml}{$khronos_name()}.\n";
    if ($shader_only) {
        print "\n";
        print "    This convenience function will do nothing on OpenGL/ES 1.x systems.\n";
    }
    print "*/\n\n";
}

# Generate the resolver functions.
print "#ifndef QT_OPENGL_ES_2\n\n";
$last_shader_only = 0;
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if $inline && $inline eq 'all';
    next if $inline && $inline eq 'all_diff';
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $name = $_->{'varname'};
    my $resolver_name = $_->{'name'};
    $resolver_name =~ s/^gl/qglfResolve/;
    my $special_name = $_->{'name'};
    $special_name =~ s/^gl/qglfSpecial/;
    my @platforms = split /,\s*/,$inline;
    $shader_only = 1 if @platforms ~~ 'es1';
    my $is_void = ($_->{'returnType'} eq 'void');
    my $special_handling = ($_->{'special_handling'} && $_->{'special_handling'} eq 'yes');
    #print "#ifndef QT_OPENGL_ES_1\n\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n\n" if (!$shader_only && $last_shader_only);

    if ($special_handling) {
        # Output special fallback implementations for certain functions.
        if ($name eq "getShaderPrecisionFormat") {
            print "static $_->{'returnType'} QT3D_GLF_APIENTRY $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(shadertype);\n";
            print "    Q_UNUSED(precisiontype);\n";
            print "    range[0] = range[1] = precision[0] = 0;\n";
            print "}\n\n";
        } elsif ($name eq "isProgram" || $name eq "isShader") {
            print "static $_->{'returnType'} QT3D_GLF_APIENTRY $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    return $_->{'argnamestr'} != 0;\n";
            print "}\n\n";
        } elsif ($name eq "releaseShaderCompiler") {
            print "static $_->{'returnType'} QT3D_GLF_APIENTRY $special_name($_->{'argstr'})\n";
            print "{\n";
            print "}\n\n";
        }
    }

    print "static $_->{'returnType'} QT3D_GLF_APIENTRY $resolver_name($_->{'argstr'})\n";
    print "{\n";
    my $type_name = "type_$_->{'name'}";
    print "    typedef $_->{'returnType'} (QT3D_GLF_APIENTRYP $type_name)($_->{'argstr'});\n\n";
    print "    const QGLContext *context = QGLContext::currentContext();\n";
    print "    QOpenGLFunctionsPrivate *funcs = qt_gl_functions(context);\n";
    print "\n";
    print "    funcs->$name = ($type_name)\n";
    print "        context->getProcAddress(QLatin1String(\"$_->{'name'}\"));\n";
    my @alt_names = ();
    if ($_->{'alt_name'}) {
        push @alt_names, $_->{'alt_name'};
    } else {
        push @alt_names, "$_->{'name'}OES";
        push @alt_names, "$_->{'name'}EXT";
        push @alt_names, "$_->{'name'}ARB";
    }
    foreach (@alt_names) {
        print "#ifdef QT_OPENGL_ES\n" if /OES/;
        print "    if (!funcs->$name) {\n";
        print "        funcs->$name = ($type_name)\n";
        print "            context->getProcAddress(QLatin1String(\"$_\"));\n";
        print "    }\n";
        print "#endif\n" if /OES/;
    }
    if ($special_handling) {
        print "\n";
        print "    if (!funcs->$name)\n";
        print "        funcs->$name = $special_name;\n\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "funcs->$name($_->{'argnamestr'});\n";
    } else {
        print "\n";
        print "    if (funcs->$name)\n";
        if ($is_void) {
            print ("        ");
        } else {
            print ("        return ");
        }
        print "funcs->$name($_->{'argnamestr'});\n";
        if ($is_void) {
            print "    else\n";
            print "        funcs->$name = $resolver_name;\n";
        } else {
            print "    funcs->$name = $resolver_name;\n";
            print "    return $_->{'returnType'}(0);\n";
        }
    }
    print "}\n\n";

    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;
print "#endif // !QT_OPENGL_ES_2\n\n";

# Generate the initialization code for QOpenGLFunctionsPrivate.
print "QOpenGLFunctionsPrivate::QOpenGLFunctionsPrivate(const QGLContext *)\n";
print "{\n";
print "#ifndef QT_OPENGL_ES_2\n";
$last_shader_only = 0;
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if $inline && $inline eq 'all';
    next if $inline && $inline eq 'all_diff';
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $name = $_->{'varname'};
    my $resolver_name = $_->{'name'};
    $resolver_name =~ s/^gl/qglfResolve/;
    my @platforms = split /,\s*/,$inline;
    $shader_only = 1 if @platforms ~~ 'es1';
    #print "#ifndef QT_OPENGL_ES_1\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n" if (!$shader_only && $last_shader_only);
    print "    $name = $resolver_name;\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;
print "#endif // !QT_OPENGL_ES_2\n";
print "}\n\n";

#print Dumper(\@functions);
