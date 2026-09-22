# REPORT.md

## Feature 2: Multi-file Build

### 1. Makefile linking rules vs library linking

In this feature, the Makefile compiles each `.c` file in `src/` into a
`.o` object file, then links all the object files directly into a single
executable:

    $(TARGET): $(OBJS)
        $(CC) $(OBJS) -o $(TARGET)

Here, the linker pulls the machine code from every object file straight
into the final binary. There's no intermediate packaging step — the
executable is built from exactly the object files it depends on.

Library linking (used in Features 3 and 4) works differently. Instead of
linking object files directly, they are first packaged into a library
(a `.a` static archive or a `.so` shared object). The executable is then
linked *against* that library using the `-L` flag (tells the linker
which directory to search for libraries) and the `-l` flag (tells the
linker which library to link, by name, without the `lib` prefix or file
extension — e.g. `-lmyutils` for `libmyutils.a`).

With a static library, the linker still copies the needed object code
into the final executable at link time — similar to direct object
linking, but the code came from an archive instead of standalone `.o`
files. With a dynamic library, the linker does not copy any code in;
it only records that the executable depends on the shared library,
and the actual code is loaded from `.so` file at runtime by the
dynamic linker.

### 2. Git tags: simple vs annotated

A **simple (lightweight) tag** is just a named pointer to a specific
commit, with no extra metadata:

    git tag v0.1.1-multifile

An **annotated tag** stores additional information as a full object in
git's history — the tagger's name and email, the date, and a message:

    git tag -a v0.1.1-multifile -m "Feature 2: Multi-file build with Makefile"

Annotated tags are the standard choice for marking releases because
they are traceable (you know who tagged it, when, and why) and because
GitHub Releases are built from annotated tags — the release page shows
the tag message and links back to the exact commit it points to.

### 3. GitHub releases and binary distribution

A GitHub Release packages a specific tagged commit together with
release notes and, optionally, attached files ("assets") — in this
case, the compiled `bin/client` executable.

This separates two different things a project can distribute:
- **Source distribution**: what git tracks — the `.c`/`.h` files
  someone would need to clone and build the project themselves.
- **Binary distribution**: a pre-compiled, ready-to-run executable,
  attached directly to the release, that someone can download and run
  without needing a compiler or build toolchain at all.

For this feature, the release at tag `v0.1.1-multifile` includes the
compiled `bin/client`, so anyone can grab a working binary directly
from the Releases page rather than building from source.
