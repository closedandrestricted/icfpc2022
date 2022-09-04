#!/usr/bin/python3
import collections
import json
import os
import re

_PROBLEM_TO_ORIGINAL = {
    26: 5,
    27: 2,
    28: 10,
    29: 18,
    30: 11,
    31: 24,
    32: 9,
    33: 15,
    34: 7,
    35: 25,
}


this_dir = os.path.dirname(os.path.abspath(__file__))

Block = collections.namedtuple('Block', 'x1 y1 x2 y2')


def to_blocks(initial):
    res = {}
    for block in initial['blocks']:
        res[block['blockId']] = Block(
            block['bottomLeft'][0], block['bottomLeft'][1], block['topRight'][0], block['topRight'][1])
    return res, initial['width'], initial['height']


def find_block(blocks, x, y):
    for b_id, block in blocks.items():
        if block.x1 <= x < block.x2 and block.y1 <= y < block.y2:
            return b_id
    print(blocks, x, y)
    raise AssertionError('')


last_idx = 0


def increment_idx(line):
    line = line.replace(' ', '')
    if line == '' or line[0] == '#':
        return line
    steps = {
        'cut': 1,
        'color': 1,
        'swap': 2,
        'merge': 2,
    }
    count = steps[line[0:line.index('[')]]

    def repl(match):
        return '[' + str(last_idx + int(match.group(0)[1:]))
    return re.sub(r'\[\d+', repl, line, count)


def merge(blocks, *args):
    x1, y1, x2, y2 = 1e10, 1e10, -1e10, -1e10
    for arg in args:
        x1 = min(x1, blocks[arg].x1)
        y1 = min(y1, blocks[arg].y1)
        x2 = max(x2, blocks[arg].x2)
        y2 = max(y2, blocks[arg].y2)
        del blocks[arg]
    global last_idx
    last_idx += 1
    blocks[str(last_idx)] = Block(x1, y1, x2, y2)


def cut(blocks, b0, y):
    block = blocks[b0]
    assert block.y1 < y <= block.y2
    blocks[b0 + '.0'] = Block(block.x1, block.y1, block.x2, y)
    blocks[b0 + '.1'] = Block(block.x1, y, block.x2, block.y2)
    del blocks[b0]


I = 5

for idx, idx_orig in _PROBLEM_TO_ORIGINAL.items():
    with open(os.path.join(this_dir, '../../problems/%s.initial.json' % idx)) as f:
        blocks, w, h = to_blocks(json.loads(f.read()))
    last_idx = len(blocks) - 1
    with open(os.path.join(this_dir, '../../solutions/best/%s.txt' % idx_orig)) as f:
        solution = f.read().split('\n')
    dx = blocks['0'].x2 - blocks['0'].x1
    dy = blocks['0'].y2 - blocks['0'].y1
    result = []
    for i in range(I):
        for j in range(1, h // dy):
            x = dx // 2 + dx * i
            y = dy // 2 + dy * j
            b1 = find_block(blocks, x, y)
            b2 = find_block(blocks, x, y - dy)
            merge(blocks, b1, b2)
            result.append('merge [%s] [%s]' % (b1, b2))
        if i:
            x = dx // 2 + dx * i
            y = dy // 2
            b1 = find_block(blocks, x - dx, y)
            b2 = find_block(blocks, x, y)
            merge(blocks, b1, b2)
            result.append('merge [%s] [%s]' % (b1, b2))

    for j in range(1, h // dy):
        x = dx // 2
        y = dy * j
        b0 = find_block(blocks, x, y)
        cut(blocks, b0, y)
        result.append('cut [%s] [Y] [%d]' % (b0, y))

    for i in range(I, w // dx):
        for j in range(h // dy):
            x = dx // 2 + dx * i
            y = dy // 2 + dy * j
            b1 = find_block(blocks, x - dx, y)
            b2 = find_block(blocks, x, y)
            merge(blocks, b1, b2)
            result.append('merge [%s] [%s]' % (b1, b2))

    for j in range(1, h // dy):
        x = dx // 2
        y = dy // 2 + dy * j
        b1 = find_block(blocks, x, y)
        b2 = find_block(blocks, x, y - dy)
        merge(blocks, b1, b2)
        result.append('merge [%s] [%s]' % (b1, b2))

    result.append('color [%s] [255,255,255,255]' % last_idx)

    for line in solution:
        result.append(increment_idx(line))

    with open(os.path.join(this_dir, '../../solutions/ontopof/%s.txt' % idx), 'w') as g:
        g.write('\n'.join(result))
