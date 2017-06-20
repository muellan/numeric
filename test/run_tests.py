#!/usr/bin/python

import re
import os
import shutil
from os import path
from os import system
from sys import stdout
from sys import argv
from sets import Set

#default settings
builddir   = "../build_test"
incpaths   = ["", "../include/", "../src/"]
macros     = ["NO_DEBUG", "NDEBUG"]
compiler   = "g++"
compileopt = "-std=c++14 -O3 -Wall -Wextra -Wpedantic -Wno-unknown-pragmas"
tuext      = "cpp"


# [ (needs compilation, dependency regex) ]
deprxp = [re.compile('^\s*#pragma\s+test\s+needs\(\s*"(.+\..+)"\s*\)\s*$'),
          re.compile('^\s*#include\s+\<(.+\..+)\>\s*$'),
          re.compile('^\s*#include\s+"(.+\..+)"\s*$')]

testrxp = re.compile('(.+)\.' + tuext)

def dependencies(source, searchpaths = [], sofar = Set()):
    """ return set of dependencies (found by DFS)
        dependencies look like this:
          - #include "path/to/file.ext"
          - #include <path/to/file.ext>
          - #pragma test needs("path/to/file.ext")
    """
    result = Set()

    if not path.exists(source):
        return result

    curpath = path.dirname(source) + "/"

    with open(source, 'r') as file:
        for line in file:
            dep = ""
            res = None
            for rxp in deprxp:
                # if line encodes dependency
                res = rxp.match(line)
                if res is not None:
                    dep = res.group(1)
                    if dep != "":
                        if not path.exists(dep):
                            # try same path as current dependency
                            if path.exists(curpath + dep):
                                dep = curpath + dep
                            else: # try include paths
                                for sp in searchpaths:
                                    if path.exists(sp + dep):
                                        dep = sp + dep
                                        break

                        result.add(dep)
                        if dep not in sofar:
                            result.update(dependencies(dep, searchpaths, result.union(sofar)))
                        break

    result.add(source)
    return result


# initialize
onwindows = os.name == "nt"

artifactext = ""
if onwindows: artifactext = ".exe"

paths = []
sources = []
includes = []
showDependencies = False
haltOnFail = True
recompile = False

# gather source file names
if len(argv) > 1:
    for i in range(1,len(argv)):
        arg = argv[i]
        if arg == "-h" or arg == "--help":
            print "usage: " + argv[0] + \
                " [--clean]" \
                " [-r|--recompile]" \
                " [-h|--help]" \
                " [-d|--show-dependecies]" \
                " [-c|--compiler EXEC]" \
                " [{-m|--macro MACRO}]" \
                " [{-i|--include INCLUDE_PATH}]" \
                " [--continue-on-fail]" \
                " [{directory|file}]"
            exit()
        elif arg == "--clean":
            if os.path.exists(builddir):
                shutil.rmtree(builddir)
            print "-- clean build --"
        elif arg == "-r" or arg == "--recompile":
            recompile = True
        elif arg == "-d" or arg == "--show-dependencies":
            showDependencies = True
        elif arg == "--continue-on-fail":
            haltOnFail = False
        elif arg == "-c" or arg == "--compiler":
            if i+1 < len(argv): compiler = argv[i+1]
        elif arg == "-i" or arg == "--include":
            if i+1 < len(argv): incpats.add(argv[i+1])
        elif arg == "-m" or arg == "--macro":
            if i+1 < len(argv): macros.add(argv[i+1])
        else:
            paths.append(arg)

if len(paths) < 1:
    paths = [os.getcwd()]

for p in paths:
    if p.endswith("." + tuext):
        sources.append(p)
    else:
        sources.extend([path.join(root, name)
             for root, dirs, files in os.walk(p)
             for name in files
             if name.endswith("." + tuext)])

if len(sources) < 1:
    print "ERROR: no source files found"
    exit()

# make build directory
if not os.path.exists(builddir): os.makedirs(builddir)

# compile and run tests

print "----------------------------------------------------------------------"

compilecmd = compiler + " " + compileopt
for m in macros:
    if m != "": compilecmd = compilecmd + " -D" + m

for ip in incpaths:
    if ip != "": compilecmd = compilecmd + " -I " + ip

#print compilecmd

for source in sources:
    res1 = testrxp.match(source)
    res2 = testrxp.match(path.basename(source))
    if res1 is not None and res2 is not None:
        tname = res1.group(1)
        sname = res2.group(1)
        stdout.write("testing " + tname + " > ")
        stdout.flush()
        artifact = builddir + "/" + sname + artifactext

        srcdeps = dependencies(source, incpaths)

        if showDependencies:
            print ""
            for dep in srcdeps: print "    needs " + dep
            stdout.write("    ")
            stdout.flush()

        doCompile = recompile or not path.exists(artifact)
        if not doCompile:
             for dep in srcdeps:
                if path.exists(dep):
                    if str(path.getmtime(artifact)) < str(path.getmtime(dep)):
                        doCompile = True
                        break
                else:
                    print "ERROR: dependency " + dep + " could not be found!"
                    exit()

        if doCompile:
            stdout.write("compiling > ")
            stdout.flush()

            if path.exists(artifact):
                os.remove(artifact)

            tus = ""
            for dep in srcdeps:
                 if dep.endswith("." + tuext):
                     tus = tus + " " + dep

            system(compilecmd + " " + tus + " -o " + artifact)
            if not path.exists(artifact):
                print "FAILED!"
                if haltOnFail: exit();

        #execute test; backslashes make sure that it works with cmd.exe
        if onwindows:
            artifact = artifact.replace("/", "\\")

        stdout.write("running > ")
        stdout.flush()
        runres = system(artifact)
        if runres == 0:
            print "passed."
        else:
            print "FAILED!"
            if haltOnFail : exit()

print "----------------------------------------------------------------------"
print "All tests passed."
