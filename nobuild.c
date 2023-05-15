#define NOBUILD_IMPLEMENTATION
#include "nobuild.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define CXX "g++"
#define COMMON_CXXFLAGS                                                                            \
    "-Wall", "-Wextra", "-pedantic", "-std=c++2a", "-Iinclude", "-Iinclude/vendor"
#define CXXFLAGS_DEBUG "-g"
#define CXXFLAGS_RELEASE "-O3"
#ifdef _WIN32
    #define LDFLAGS "-Llib", "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm"
#else
    #define LDFLAGS "-lraylib", "-lm"
#endif

#define OBJ_DEBUG_DIR PATH("obj", "debug")
#define OBJ_RELEASE_DIR PATH("obj", "release")
#define BIN_DEBUG_DIR PATH("bin", "debug")
#define BIN_RELEASE_DIR PATH("bin", "release")
#define BIN_NAME "cegui"

typedef enum { Debug, Release, Both } CompilationMode;

bool getModTimeDiff(const char* filenameA, const char* filenameB, long* diff) {
    if (!PATH_EXISTS(filenameA)) {
        return false;
    }
    if (!PATH_EXISTS(filenameB)) {
        return false;
    }
    struct stat fileAStat;
    struct stat fileBStat;
    if (!stat(filenameA, &fileAStat) && !stat(filenameB, &fileBStat)) {
        time_t fileATime = fileAStat.st_mtime;
        time_t fileBTime = fileBStat.st_mtime;
        *diff = fileATime - fileBTime;
        return true;
    }
    return false;
}

const char* getCorrectDir(CompilationMode cm, bool getObjDir) {
    const char* dir;
    if (getObjDir) {
        if (cm == Debug)
            dir = OBJ_DEBUG_DIR;
        else
            dir = OBJ_RELEASE_DIR;
    } else {
        if (cm == Debug)
            dir = BIN_DEBUG_DIR;
        else
            dir = BIN_RELEASE_DIR;
    }
    return dir;
}

void buildObj(const char* filename, CompilationMode cm) {
    const char* objDir = getCorrectDir(cm, true);
    CMD(CXX, COMMON_CXXFLAGS, CXXFLAGS_DEBUG, "-c", PATH("src", filename), "-o",
        PATH(objDir, CONCAT(NOEXT(filename), ".o")));
}

void shouldRebuild(const char* file, const char* objDir, CompilationMode cm) {
    if (ENDS_WITH(file, ".cpp")) {
        long diff = 0;
        bool success =
            getModTimeDiff(PATH(objDir, CONCAT(NOEXT(file), ".o")), PATH("src", file), &diff);
        if (!success) {
            buildObj(file, cm);
        } else {
            if (diff < 0)
                buildObj(file, cm);
        }
    }
}

void buildAllObj(CompilationMode cm) {
    const char* objDir = getCorrectDir(cm, true);
    int count = 0;
    FOREACH_FILE_IN_DIR(file, "src", {
        shouldRebuild(file, objDir, cm);
        count++;
    });
}

void buildBin(CompilationMode cm) {
    const char* objDir = getCorrectDir(cm, true);
    const char* binDir = getCorrectDir(cm, false);
    Cstr_Array ca = {0};
    FOREACH_FILE_IN_DIR(file, objDir, {
        if (ENDS_WITH(file, ".o")) {
            Cstr filepath = PATH(objDir, file);
            ca = cstr_array_append(ca, filepath);
        }
    });
    Cstr final = cstr_array_join(" ", ca);
    Cstr cmdStr = JOIN(" ", CXX, final, "-o", PATH(binDir, BIN_NAME), LDFLAGS);
    INFO("CMD: %s", cmdStr);
    system(cmdStr);
}

void setup(CompilationMode cm) {
    if (!PATH_EXISTS("obj"))
        MKDIRS("obj");
    if (!PATH_EXISTS("bin"))
        MKDIRS("bin");
    switch (cm) {
    case Debug:
        if (!PATH_EXISTS(OBJ_DEBUG_DIR))
            MKDIRS("obj/debug");
        if (!PATH_EXISTS(BIN_DEBUG_DIR))
            MKDIRS("bin/debug");
        break;
    case Release:
        if (!PATH_EXISTS(OBJ_RELEASE_DIR))
            MKDIRS("obj/release");
        if (!PATH_EXISTS(BIN_RELEASE_DIR))
            MKDIRS("bin/release");
        break;
    case Both:
        setup(Debug);
        setup(Release);
        break;
    }
}

void buildBoth() {
    // DEBUG
    buildAllObj(Debug);
    buildBin(Debug);
    // RELEASE
    buildAllObj(Release);
    buildBin(Release);
}

void clean() {
    RM("obj");
    RM("bin");
}

int main(int argc, char** argv) {
    GO_REBUILD_URSELF(argc, argv);
    // The default mode of compilation is debug and release
    CompilationMode cm = Both;
    if (argc >= 2) {
        if (!strcmp(argv[1], "debug"))
            cm = Debug;
        else if (!strcmp(argv[1], "release"))
            cm = Release;
        if (!strcmp(argv[1], "clean")) {
            clean();
            return EXIT_SUCCESS;
        }
    }

    setup(cm);
    if (cm == Both) {
        buildBoth();
    } else {
        buildAllObj(cm);
        buildBin(cm);
    }
}
