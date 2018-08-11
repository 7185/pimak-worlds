import qbs

QtApplication {
    name: "pwserver"
    Depends { name: "Qt"; submodules: ["core", "network"] }
    cpp.includePaths: ["./include", "../common/include"]
    files: [
        "../common/include/*.h",
        "include/*.h",
        "src/*.cpp",
    ]
    Group {
        name: "Executable"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "."
    }
}
