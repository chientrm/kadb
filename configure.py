import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from glob import glob
import argparse
from dataclasses import dataclass, field


parser = argparse.ArgumentParser(description='Makefile helper')
parser.add_argument('--watch', action='store_true')

args = parser.parse_args()


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
    print('Detecting change...')
    libs = [lib[len('lib/'):] for lib in glob('lib/*')]
    archives = [f'build/release/{lib}.a' for lib in libs]

    rules = [
        Rule('all', ['dist/release/server']),
        Rule('clean', commands=['rm -rf build dist']),
        Rule('dist/release/server', ['main.c'] + archives,
             [
            'mkdir -p dist/release',
            'gcc -Werror -Iheader -o $@ $^ -luring'
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
                [f'header/{lib}.h', source],
                [
                    f'mkdir -p build/release/{lib}',
                    f'gcc -c -Werror -Iheader -o $@ {source}'
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
                    f'gcc -c -Werror -Iheader -o $@ {source}'
                ])
            rules.append(rule)
    makefile = '\n'.join([str(rule) for rule in rules])
    with open('Makefile', 'w') as f:
        f.write(makefile)
    with open('Makefile.debug', 'w') as f:
        f.write(makefile
                .replace('-o $@', '-o $@ -g')
                .replace('/release', '/debug'))
    print('Makefile generated.')


class Handler(FileSystemEventHandler):

    @staticmethod
    def on_any_event(event):
        if event.is_directory:
            return None
        if event.event_type in ['created', 'deleted', 'moved']:
            generate_makefile()


if __name__ == '__main__':
    generate_makefile()
    if args.watch:
        observer = Observer()
        observer.schedule(Handler(), "lib", recursive=True)
        observer.start()
        try:
            while True:
                time.sleep(5)
        except:
            observer.stop()
            print("Error")
        observer.join()
