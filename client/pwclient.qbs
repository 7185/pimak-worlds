import qbs

QtApplication {
    name: "pwclient"
    Depends { name: "Qt"; submodules: ["core", "network", "widgets"] }
    cpp.includePaths: ["./include", "/usr/include/OGRE", "/usr/include/OGRE/Overlay"]
    cpp.libraryPaths: ["/usr/lib"]
    cpp.dynamicLibraries: ["OgreMain", "OgreTerrain", "OgreOverlay", "boost_system"]
    files: [
        "include/*.h",
        "src/*.cpp",
        "ui/*.ui",
        "lang/*.ts",
        "pwclient.qrc"
    ]
    Group {
        name: "Executable"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "."
    }
}
