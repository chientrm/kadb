from glob import glob
from dataclasses import dataclass, field

CC = 'gcc -Iheader -Werror'


@dataclass
class Rule:
    target: str
    sources: list = field(default_factory=list)
    commands: list = field(default_factory=list)

    def __str__(self):
        source = ' '.join(self.sources)
        lines = [f'{self.target}: {source}']
        lines.extend([f'\t{c}' for c in self.commands])
        return '\n'.join(lines) + '\n'


def get_object(source):
    return source.replace('.c', '.o').replace('lib/', 'build/release/')


def generate_makefile():
    libs = [lib[len('lib/'):] for lib in glob('lib/*')]
    archives = [f'build/release/{lib}.a' for lib in libs]

    rules = [
        Rule('all', ['dist/release/kadb']),
        Rule('clean', commands=['rm -rf build dist']),
        Rule('dist/release/kadb', ['main.c'] + archives,
             [
            'mkdir -p dist/release',
            f'{CC} -o $@ $^ -luring'
        ]),
    ]
    for lib in libs:
        public_sources = glob(f'lib/{lib}/*.c')
        private_sources = glob(f'lib/{lib}/private/*.c')
        sources = public_sources + private_sources
        rule = Rule(f'build/release/{lib}.a',
                    [get_object(source) for source in sources],
                    ['ar rcs $@ $^'])
        rules.append(rule)
        for source in public_sources:
            rule = Rule(
                get_object(source),
                [f'header/{lib}.h', f'header/{lib}.private.h', source],
                [
                    f'mkdir -p build/release/{lib}',
                    f'{CC} -c -o $@ {source}'
                ])
            rules.append(rule)
        for source in private_sources:
            rule = Rule(
                get_object(source),
                [
                    f'header/{lib}.h',
                    f'header/{lib}.private.h',
                    source
                ],
                [
                    f'mkdir -p build/release/{lib}/private',
                    f'{CC} -c -o $@ {source}'
                ])
            rules.append(rule)
    makefile = '\n'.join([str(rule) for rule in rules])
    with open('Makefile', 'w') as f:
        f.write(makefile)
    with open('Makefile.debug', 'w') as f:
        f.write(makefile
                .replace('-o $@', '-o $@ -g -O0')
                .replace('/release', '/debug'))


if __name__ == '__main__':
    generate_makefile()
