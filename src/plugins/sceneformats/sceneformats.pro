TEMPLATE = subdirs

old_importer {
    SUBDIRS = bezier 3ds obj
} else {
    SUBDIRS = bezier assimp
}
