import qbs

QtApplication {
    name: "pwserver"
    Depends { name: "Qt"; submodules: ["core", "network"] }
    cpp.includePaths: ["./include"]
    files: [
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
