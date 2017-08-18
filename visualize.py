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
    ps = []
    for line in fh:
        if line.startswith('VISUALIZE: '):
            line = line.split()
            p = list(map(int, line[2 : ]))
            assert len(p) == int(line[1])
            ps += [ { 'p': p } ]
        elif line.startswith('MESSAGE: '):
            message = line[len('MESSAGE: ') : ]
            if 'message' in ps[-1]:
                ps[-1]['message'] += ' / ' + message
            else:
                ps[-1]['message'] = message
    return ps

def flood_fill(h, w, y, x, callback, pred):
    dy = [ -1, 1, 0, 0 ]
    dx = [ 0, 0, 1, -1 ]
    used = [ False ] * (h * w)
    stk = []
    if pred(y, x):
        used[y * w + x] = True
        stk += [ (y, x) ]
    while stk:
        y, x = stk[-1]
        stk.pop()
        callback(y, x)
        for i in range(4):
            ny = y + dy[i]
            nx = x + dx[i]
            if 0 <= ny < h and 0 <= nx < w and not used[ny * w + nx] and pred(ny, nx):
                used[ny * w + nx] = True
                stk += [ (ny, nx) ]

def calculate_score(s, matrix, p):
    used = [ False ] * (s ** 2)
    result = { 'score': - float('inf') }
    for y in range(s):
        for x in range(s):
            if not used[y * s + x] and matrix[p[y] * s + p[x]]:
                size = 0
                acc = 0
                component = [ False ] * (s ** 2)
                def callback(y, x):
                    nonlocal used, size, acc, component
                    used[y * s + x] = True
                    size += 1
                    acc += matrix[p[y] * s + p[x]]
                    component[y * s + x] = True
                def pred(y, x):
                    return not used[y * s + x] and matrix[p[y] * s + p[x]]
                flood_fill(s, s, y, x, callback, pred)
                score = acc * math.sqrt(size)
                if result['score'] < score:
                    result['score'] = score
                    result['size'] = size
                    result['sum'] = acc
                    result['used'] = component
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

def render_image(screen, s, matrix, p, scale):
    screen.fill(0x000000)
    for y in range(s):
        for x in range(s):
            width = 1
            rect = pygame.Rect( x * scale, y * scale, scale, scale )
            pygame.draw.rect(screen, [0xd3d3d3, 0x000000][p['used'][y * s + x]], rect)  # draw the border
            color = get_color(matrix[p['p'][y] * s + p['p'][x]], p['used'][y * s + x])
            rect = pygame.Rect( x * scale + width, y * scale + width, scale - 2 * width, scale - 2 * width )
            pygame.draw.rect(screen, color, rect)

def update_screen(screen, s, matrix, ps, ix, scale):
    p = ps[ix]
    if 'score' not in p:
        p.update(calculate_score(s, matrix, p['p']))
    render_image(screen, s, matrix, p, scale)
    caption = 'visualize %d of %d / score = %d' % (ix + 1, len(ps), p['score'])
    if 'message' in p:
        caption += '/ ' + p['message']
    pygame.display.set_caption(caption)
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
                if event.key == pygame.K_a:
                    for p in ps:
                        if 'score' not in p:
                            p.update(calculate_score(s, matrix, p['p']))
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
        ps = read_ps(sys.stdin)
    else:
        with open(args.output) as fh:
            ps = read_ps(fh)

    visualize(s, matrix, ps)
