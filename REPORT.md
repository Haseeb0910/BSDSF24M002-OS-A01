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

## Feature 3: Static Library

### 1. Makefile differences for library creation

Building a static library requires two new things beyond the multi-file
build's rules:

1. A separate object list that excludes `main.o`, since the library
   should only contain the reusable utility functions, not the driver
   program:

       LIB_SRCS = $(SRC_DIR)/mystrfunctions.c $(SRC_DIR)/myfilefunctions.c
       LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))

2. A rule that archives those object files into `libmyutils.a` instead
   of linking them into an executable:

       $(STATIC_LIB): $(LIB_OBJS)
           ar rcs $(STATIC_LIB) $(LIB_OBJS)
           ranlib $(STATIC_LIB)

3. A separate link rule for the executable that links against the
   library rather than the raw object files, using `-L` (library
   search path) and `-l` (library name):

       $(STATIC_TARGET): $(OBJ_DIR)/main.o $(STATIC_LIB)
           $(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lmyutils -o $(STATIC_TARGET)

The core difference from Feature 2: instead of one link step that
consumes all object files at once, there are now two stages — archive,
then link against the archive.

### 2. Purpose of ar and ranlib

`ar` (archiver) bundles multiple `.o` object files into a single `.a`
archive file, without compression — conceptually similar to a `.zip`
but for object files specifically. The flags used, `rcs`:
- `r` — insert/replace the given files in the archive
- `c` — create the archive if it doesn't already exist
- `s` — write a symbol index into the archive

`ranlib` generates (or regenerates) that same symbol index for an
existing archive. The index maps each symbol (function) to the object
file inside the archive that defines it, so the linker can find the
right `.o` to pull in without scanning every object file in the
archive sequentially. Modern `ar rcs` already includes this indexing
(the `s` flag does what `ranlib` does), so running `ranlib` afterward
is redundant in practice but demonstrates the tool explicitly.

### 3. Symbol analysis with nm

Running `nm lib/libmyutils.a` shows each function from both source
files as type `T`, meaning defined in the text (code) section, e.g.:

    mystrfunctions.o:
    0000000000000000 T str_reverse
    000000000000023f T str_count_char

    myfilefunctions.o:
    0000000000000000 T file_word_count
    00000000000000a9 T file_line_count

External functions the code calls but doesn't define (like `strlen`,
`fopen`, `printf`) show as `U` (undefined) — expected, since those
come from libc, not this library.

Running `nm bin/client_static` on the final linked executable shows
the same functions (`str_reverse`, `file_word_count`, etc.) now with
type `T` inside the executable itself, not `U`. This confirms static
linking actually happened — the linker physically copied the object
code from the archive into the executable, rather than just recording
a reference to it. Interestingly, standard library functions like
`printf` and `fopen` still show as `U` with a `@GLIBC_...` version tag,
because only `libmyutils.a` was statically linked — glibc itself is
still linked dynamically, which is normal default behavior on Linux.

Comparing `bin/client` (Feature 2) and `bin/client_static` (Feature 3),
both come out to 17K — nearly identical size, since the same object
code ends up embedded in the executable either way; archiving it first
doesn't change the final linked size meaningfully.

## Feature 4: Dynamic Library

### 1. Position-Independent Code (PIC) requirements

A shared library can be loaded at a different memory address in every
program that uses it, and even at a different address on different
runs of the same program (due to address space layout randomization).
Regular compiled code can contain absolute memory addresses baked in
at compile time — which breaks if the code doesn't end up loaded where
the compiler assumed it would be.

Position-Independent Code solves this by having the compiler generate
code that only uses addresses *relative* to the current instruction
pointer, rather than absolute addresses. This is enabled with the
`-fPIC` flag at compile time. In this project, PIC object files were
built separately from the regular ones (`.pic.o` instead of `.o`) so
the static build (which doesn't need PIC) and the dynamic build (which
requires it) don't interfere with each other's object files:

    $(OBJ_DIR)/%.pic.o: $(SRC_DIR)/%.c
        $(CC) $(CFLAGS) $(PIC_FLAGS) -c $< -o $@

The shared library itself is then produced with the `-shared` flag:

    $(DYNAMIC_LIB): $(PIC_OBJS)
        $(CC) -shared -o $(DYNAMIC_LIB) $(PIC_OBJS)

### 2. Executable size differences

For this project, all three executables — `client` (multi-file),
`client_static`, and `client_dynamic` — came out to the same size
(17K). This is not the typical textbook result of "dynamic is
smaller," and it's worth explaining why:

The size benefit of dynamic linking comes from *not duplicating*
library code across many executables, or *not embedding* a large
library into one binary. `libmyutils` is very small — a handful of
short utility functions — so the actual code being included-or-not
only amounts to a few hundred bytes either way. That difference is
dwarfed by the fixed overhead every ELF executable carries regardless
of linking method (ELF headers, section tables, and — for the dynamic
build specifically — additional dynamic linking metadata like the
`.dynamic` section, PLT/GOT entries, and symbol tables needed to
resolve the library at runtime).

In other words: at this small scale, the extra bookkeeping the dynamic
build needs to carry outweighs the code it saves by not embedding the
library. Real-world size benefits of shared libraries appear with
much larger libraries (e.g. glibc itself, shared system-wide across
every program) or executables linking against many libraries at once
— not a small custom library like this one.

### 3. LD_LIBRARY_PATH and the dynamic loader

Running `client_dynamic` without any setup produced:

    error while loading shared libraries: libmyutils.so:
    cannot open shared object file: No such file or directory

This happens because a dynamically linked executable does not contain
the library's code — only a reference saying it needs `libmyutils.so`
at runtime. Before `main()` runs, a separate program, the dynamic
loader (`ld.so`), searches a specific set of directories for that
library. Since `libmyutils.so` lives in this project's own `lib/`
folder — not a directory the loader checks by default — it failed to
find it.

Setting `LD_LIBRARY_PATH` tells the loader to check that directory
first, before its default system paths:

    export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH

Running `ldd bin/client_dynamic` afterward confirmed the library was
now correctly resolved:

    libmyutils.so => .../lib/libmyutils.so (0x...)
    libc.so.6 => /usr/lib/x86_64-linux-gnu/libc.so.6 (0x...)

`ldd` lists every shared library dependency of an executable and shows
whether the dynamic loader can currently resolve it — a useful tool
for diagnosing exactly this kind of missing-library error.

## Feature 5: Documentation & Installation

### 1. Man pages (groff formatting)

Man pages are written in **groff**, using the `man` macro package —
a much terser markup than Markdown, built around dot-commands. Each
function got its own page in `man/man3/`, section 3 being specifically
for library calls (as opposed to section 1 for shell commands or
section 2 for system calls). Key macros used:

- `.TH` — title header (name, section, date, source, manual category)
- `.SH` — section heading (NAME, SYNOPSIS, DESCRIPTION, etc.)
- `.B` / `.I` / `.BI` — bold / italic / alternating bold-italic text,
  used for formatting function signatures
- `.nf` / `.fi` — turns automatic line-wrapping off/on, used to
  preserve formatting in code examples
- `.BR` — alternating bold-roman, used for `SEE ALSO` cross-references

Pages were tested locally with `man -l <file>.3` before installing,
which previews a page without needing it to be in a system man path.

### 2. Install target

The Makefile's `install` target copies build outputs to standard
system locations under `/usr/local` (the conventional prefix for
manually-installed, non-package-manager software):

    install: $(DYNAMIC_TARGET)
        install -d $(INSTALL_BIN)
        install -m 755 $(DYNAMIC_TARGET) $(INSTALL_BIN)/client
        install -d $(INSTALL_LIB)
        install -m 755 $(DYNAMIC_LIB) $(INSTALL_LIB)
        install -d $(INSTALL_MAN)
        install -m 644 man/man3/*.3 $(INSTALL_MAN)
        ldconfig

The `install` command (distinct from the Makefile target of the same
name) copies files with explicit permissions in one step: `755`
(read/write/execute for owner, read/execute for others) for the
executable and shared library, `644` (read/write for owner, read-only
for others) for man pages, since they don't need to be executable.

Both the executable and `libmyutils.so` needed to be installed
together — installing only the binary would reproduce the same
"cannot open shared object file" error from Feature 4, just system-wide
instead of local to the project directory.

`ldconfig` is run after installing the library so the system's shared
library cache is refreshed immediately, letting the dynamic loader
find `libmyutils.so` in `/usr/local/lib` without needing
`LD_LIBRARY_PATH` set manually. After running `sudo make install`,
both `client` and `man str_reverse` worked correctly from any
directory, with no environment variable required — confirming the
library was now resolvable through the system's standard search path.

An `uninstall` target reverses all of this, removing the installed
binary, library, and man pages, and re-running `ldconfig` to refresh
the cache again.
