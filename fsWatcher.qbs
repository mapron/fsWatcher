import qbs;

Project {
    StaticLibrary {
        name:"directoryWatcher";
        Depends {name: "cpp"}
        Depends {name: "Qt"; submodules: ['core', 'gui', 'widgets']}

        files:  ['lib/*.cpp', 'lib/*.h']
        excludeFiles: qbs.targetOS.contains('windows')  ? 'lib/*_unix.*' : 'lib/*_win.*';
        Export {
            Depends {name: "cpp"}
            Depends {name: "Qt"; submodules: ['core', 'gui', 'widgets']}
            cpp.includePaths: ['lib/'];
        }
    }

    CppApplication {
        name: "app_watcher";
        Depends {name:"directoryWatcher";}
        cpp.minimumWindowsVersion: '5.1';
        files: ['app/*.cpp', 'app/*.h']
        consoleApplication: false
        Group {
            fileTagsFilter: 'application'
            qbs.install:true
        }
    }

    StaticLibrary {
        name:"testUtils";
        Depends {name:"directoryWatcher";}
        Depends {name: "Qt"; submodules: ['test']}

        files:  ['tests/utils/*.cpp', 'tests/utils/*.h']
        Export {
            Depends {name: "cpp"}
            Depends {name: "Qt"; submodules: ['test']}
            cpp.includePaths: ['tests/utils/'];
        }
    }

    CppApplication {
        name: "test_auto";
        Depends {name:"testUtils";}
        Depends {name:"directoryWatcher";}
        cpp.minimumWindowsVersion: '5.1';
        files: ['tests/auto/*.cpp', 'tests/auto/*.h']
        Group {
            fileTagsFilter: 'application'
            qbs.install:true
        }

    }
    CppApplication {
        name: "test_manual";
        Depends {name:"testUtils";}
        Depends {name:"directoryWatcher";}
        cpp.minimumWindowsVersion: '5.1';
        files: ['tests/manual/*.cpp', 'tests/manual/*.h']
        Group {
            fileTagsFilter: 'application'
            qbs.install:true
        }
    }

}
