import qbs 1.0

QtApplication {
    name: "pwclient"
    Depends { name: "Qt"; submodules: ["core", "network", "widgets"] }
    cpp.includePaths: ["./include", "../common/include", "/usr/include/OGRE", "/usr/include/OGRE/Bites", "/usr/include/OGRE/Overlay"]
    cpp.libraryPaths: ["/usr/lib"]
    cpp.dynamicLibraries: ["boost_system", "OgreMain", "OgreBites", "OgreOverlay", "OgreTerrain", "msgpackc"]
    files: [
        "../common/include/*.h",
        "include/*.h",
        "src/*.cpp",
        "../common/src/*.cpp",
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
