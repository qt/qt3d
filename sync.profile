%modules = ( # path to module name map
    "Qt3DCore" => "$basedir/src/core",
    "Qt3DRenderer" => "$basedir/src/core/render",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
    "qtbase" => "refs/heads/stable",
    "qtxmlpatterns" => "refs/heads/stable",
    "qtdeclarative" => "refs/heads/stable",
);
