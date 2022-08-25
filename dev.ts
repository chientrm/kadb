import chokidar from 'chokidar'
import { exec, spawn, execSync, ChildProcess } from 'child_process'

let p: ChildProcess

chokidar.watch('main.c').on('all', () => {
    console.clear()
    try {
        if (p) {
            p.kill('SIGINT');
        }
        execSync("gcc main.c -o main")
        p = exec("./main");
        p.stdout.pipe(process.stdout)
        p.stderr.pipe(process.stderr)
    } catch (e) { }
})
