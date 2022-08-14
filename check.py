from glob import glob


def check_public_source(lib, source):
    with open(source) as f:
        lines = f.readlines()
        assert lines[0] == f'#include "{lib}.h"\n', source
        assert lines[1] == f'#include "{lib}.private.h"\n', source
        assert lines[2] == '\n', source


def check_private_source(lib, source):
    with open(source) as f:
        lines = f.readlines()
        assert lines[0] == f'#include "{lib}.private.h"\n', source
        assert lines[1] == '\n', source


def check_lib(lib):
    public_sources = glob(f'lib/{lib}/*.c')
    private_sources = glob(f'lib/{lib}/private/*.c')
    [check_public_source(lib, source) for source in public_sources]
    [check_private_source(lib, source) for source in private_sources]


def check_header(header):
    with open(header) as f:
        lines = f.readlines()
        assert lines[0] == '#pragma once\n', header


def check():
    libs = [lib[len('lib/'):] for lib in glob('lib/*')]
    [check_lib(lib) for lib in libs]
    headers = glob('header/*.h')
    [check_header(header) for header in headers]


if __name__ == '__main__':
    check()
