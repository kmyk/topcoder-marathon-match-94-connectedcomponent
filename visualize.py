#!/usr/bin/env python3
import random
import math
import subprocess
import sys
import pygame
import time

def read_matrix(path):
    with open(path) as fh:
        lines = fh.read()
    lines = lines.splitlines()
    s = int(math.sqrt(int(lines[0])))
    matrix = list(map(int, lines[1 : ]))
    assert len(matrix) == s ** 2
    return matrix

def read_ps(fh):
    for line in fh:
        if line.startswith('VISUALIZE: '):
            line = line.split()
            p = list(map(int, line[2 : ]))
            assert len(p) == int(line[1])
            yield p

def flood_fill(h, w, y, x, callback):
    dy = [ -1, 1, 0, 0 ]
    dx = [ 0, 0, 1, -1 ]
    used = [ False ] * (h * w)
    stk = [ (y, x) ]
    used[y * w + x] = True
    while stk:
        y, x = stk[-1]
        stk.pop()
        if callback(y, x):
            for i in range(4):
                ny = y + dy[i]
                nx = x + dx[i]
                if 0 <= ny < h and 0 <= nx < w and not used[ny * w + nx]:
                    used[ny * w + nx] = True
                    stk += [ (ny, nx) ]

def calculate_score(s, matrix, p):
    used = [ False ] * (s ** 2)
    result = { 'score': -1 }
    for y in range(s):
        for x in range(s):
            if not used[y * s + x]:
                size = 0
                acc = 0
                def callback(y, x):
                    nonlocal used, size, acc
                    value = matrix[p[y] * s + p[x]]
                    if value == 0:
                        return False
                    used[y * s + x] = True
                    size += 1
                    acc += value
                    return True
                flood_fill(s, s, y, x, callback)
                score = int(acc * math.sqrt(size))
                if result['score'] < score:
                    result['score'] = score
                    result['size'] = size
                    result['sum'] = acc
                    result['y'] = y
                    result['x'] = x
    return result

def get_color(m, highlight):
    if m == 0:
        return (0xff, 0xff, 0xff)
    elif m > 0:
        base = [0x000011, 0x001100][bool(highlight)]
        return (0xF - m * 0x1) * base
    else:
        base = [0x111111, 0x110000][bool(highlight)]
        return (0xF - m * 0x1) * base

def render_image(screen, s, matrix, p, result, scale):
    screen.fill(0x000000)
    font = pygame.font.SysFont(None, 12)
    used = [ False ] * (s ** 2)
    def callback(y, x):
        nonlocal used
        if matrix[p[y] * s + p[x]] == 0:
            return False
        used[y * s + x] = True
        return True
    flood_fill(s, s, result['y'], result['x'], callback)
    for y in range(s):
        for x in range(s):
            if not used[y * s + x]:
                color = get_color(matrix[p[y] * s + p[x]], False)
                rect = pygame.Rect( x * scale, y * scale, scale, scale )
                pygame.draw.rect(screen, color, rect)
    for y in range(s):
        for x in range(s):
            if used[y * s + x]:
                width = 2
                rect = pygame.Rect( x * scale - width, y * scale - width, scale + 2 * width, scale + 2 * width )
                pygame.draw.rect(screen, 0x000000, rect)  # draw the border
    for y in range(s):
        for x in range(s):
            if used[y * s + x]:
                color = get_color(matrix[p[y] * s + p[x]], True)
                rect = pygame.Rect( x * scale, y * scale, scale, scale )
                pygame.draw.rect(screen, color, rect)

def update_screen(screen, s, matrix, ps, ix, scale):
    result = calculate_score(s, matrix, ps[ix])
    render_image(screen, s, matrix, ps[ix], result, scale)
    pygame.display.set_caption('visualize %d / %d: score = %d' % (ix + 1, len(ps), result['score']))
    pygame.display.update()

def visualize(s, matrix, ps):
    scale = 1024 // s
    ix = len(ps) - 1
    pygame.init()
    screen = pygame.display.set_mode((s * scale, s * scale))
    update_screen(screen, s, matrix, ps, ix, scale)
    quit = False
    while not quit:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                quit = True
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RIGHT:
                    ix = (ix + 1) % len(ps)
                    update_screen(screen, s, matrix, ps, ix, scale)
                elif event.key == pygame.K_LEFT:
                    ix = (ix - 1) % len(ps)
                    update_screen(screen, s, matrix, ps, ix, scale)
        time.sleep(1 / 60)
    pygame.quit()

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('input')
    parser.add_argument('output', nargs='?')
    args = parser.parse_args()

    matrix = read_matrix(args.input)
    s = int(math.sqrt(len(matrix)))

    if args.output is None:
        ps = list(read_ps(sys.stdin))
    else:
        with open(args.output) as fh:
            ps = list(read_ps(fh))

    visualize(s, matrix, ps)
