#!/usr/bin/env python3
import math
import subprocess

def generate_input(seed):
    cmd = [ 'java', '-jar', 'tester.jar', '-seed', str(seed), '-exec', './generate.py.sh' ]
    proc = subprocess.run(cmd, stdout=subprocess.PIPE)
    lines = proc.stdout.decode().splitlines()
    s = int(math.sqrt(int(lines[0])))
    assert lines[-1].startswith('Score = ')
    matrix = list(map(int, lines[1 : -1]))
    assert len(matrix) == s ** 2
    return matrix

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('seed', type=int)
    args = parser.parse_args()

    matrix = generate_input(args.seed)
    print(len(matrix))
    for it in matrix:
        print(it)
