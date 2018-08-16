import qbs

QtApplication {
    name: "pwserver"
    Depends { name: "Qt"; submodules: ["core", "network"] }
    cpp.includePaths: ["./include", "../common/include"]
    cpp.dynamicLibraries: ["msgpackc"]
    files: [
        "../common/include/*.h",
        "include/*.h",
        "../common/src/*.cpp",
        "src/*.cpp",
    ]
    Group {
        name: "Executable"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "."
    }
}
