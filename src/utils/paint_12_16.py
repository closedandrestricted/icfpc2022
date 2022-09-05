#!/usr/bin/python3
import collections
import json
import os
import re
import numpy as np
import png
import pprint


this_dir = os.path.dirname(os.path.abspath(__file__))

Block = collections.namedtuple('Block', 'x1 y1 x2 y2')


def find_block(blocks, x, y):
    for b_id, block in blocks.items():
        if block.x1 <= x < block.x2 and block.y1 <= y < block.y2:
            return b_id
    print(blocks, x, y)
    raise AssertionError('')


def cut_y(blocks, b0, y):
    block = blocks[b0]
    assert block.y1 < y <= block.y2
    blocks[b0 + '.0'] = Block(block.x1, block.y1, block.x2, y)
    blocks[b0 + '.1'] = Block(block.x1, y, block.x2, block.y2)
    del blocks[b0]


def cut_x(blocks, b0, x):
    block = blocks[b0]
    assert block.x1 < x <= block.x2
    blocks[b0 + '.0'] = Block(block.x1, block.y1, x, block.y2)
    blocks[b0 + '.1'] = Block(x, block.y1, block.x2, block.y2)
    del blocks[b0]


def cut_xy(blocks, b0, x, y):
    block = blocks[b0]
    assert block.x1 < x <= block.x2
    assert block.y1 < y <= block.y2
    blocks[b0 + '.0'] = Block(block.x1, block.y1, x, y)
    blocks[b0 + '.1'] = Block(x, block.y1, block.x2, y)
    blocks[b0 + '.2'] = Block(x, y, block.x2, block.y2)
    blocks[b0 + '.3'] = Block(block.x1, y, x, block.y2)
    del blocks[b0]


def merge(blocks, *args):
    x1, y1, x2, y2 = 1e10, 1e10, -1e10, -1e10
    assert len(args) == 2
    for arg in args:
        x1 = min(x1, blocks[arg].x1)
        y1 = min(y1, blocks[arg].y1)
        x2 = max(x2, blocks[arg].x2)
        y2 = max(y2, blocks[arg].y2)
        del blocks[arg]
    global last_idx
    last_idx += 1
    blocks[str(last_idx)] = Block(x1, y1, x2, y2)


def swap(blocks, b1, b2):
    blocks[b1], blocks[b2] = blocks[b2], blocks[b1]


PROBLEMS = range(1, 31)

SZ = 400
CHUNK_SZ = 100
SQUARE_SZ = 20

assert SZ % CHUNK_SZ == 0

last_idx = 0
square_colors = []
chunk_stddev = []
chunk_colors = []


def generate_assemble(blocks, result, tgt_chx, tgt_chy, loc_chx, loc_chy):
    steps = CHUNK_SZ // SQUARE_SZ
    sq_off_x = tgt_chx * steps
    sq_off_y = tgt_chy * steps
    for i in range(steps):
        for j in range(steps):
            x_cut = CHUNK_SZ * loc_chx + i * SQUARE_SZ
            y_cut = CHUNK_SZ * loc_chy + j * SQUARE_SZ
            b0 = find_block(blocks, 0, 0)
            # cut
            cut_xy(blocks, b0, x_cut, y_cut)
            result.append('cut [%s] [%d,%d]' % (b0, x_cut, y_cut))
            # color
            b0 = find_block(blocks, SZ-1, SZ-1)
            color = square_colors[sq_off_x + i][sq_off_y + j]
            result.append('color [%s] [%d,%d,%d,%d]' % ((b0,) + tuple(color)))
            # merge
            generate_merge_all(blocks, result)


def block_size(block):
    return (block.x2 - block.x1) * (block.y2 - block.y1)


def generate_merge_all(blocks, result, verbose=False):
    while len(blocks) > 1:
        best_pair = ()
        best_size = 0
        for b1 in blocks.keys():
            for b2 in blocks.keys():
                if b1 == b2:
                    continue
                block1 = blocks[b1]
                block2 = blocks[b2]
                ok = False
                if block1.x1 == block2.x1 and block1.x2 == block2.x2 and (block1.y1 == block2.y2 or block1.y2 == block2.y1):
                    ok = True
                if block1.y1 == block2.y1 and block1.y2 == block2.y2 and (block1.x1 == block2.x2 or block1.x2 == block2.x1):
                    ok = True
                if not ok:
                    continue
                size = max(block_size(block1), block_size(block2))
                if (size > best_size):
                    best_size = size
                    best_pair = (b1, b2)
        if not best_pair:
            pprint.pprint(blocks)
            assert best_pair
        b1, b2 = best_pair
        if verbose:
            print('merge %s %s %s %s' % (b1, b2, blocks[b1], blocks[b2]))
        merge(blocks, b1, b2)
        result.append('merge [%s] [%s]' % (b1, b2))


def generate_move(blocks, result, tgt_chx, tgt_chy, loc_chx, loc_chy):
    if tgt_chx == loc_chx and tgt_chy == loc_chy:
        return
    x_cuts = {
        CHUNK_SZ * loc_chx,
        CHUNK_SZ * loc_chx + CHUNK_SZ,
        CHUNK_SZ * tgt_chx,
        CHUNK_SZ * tgt_chx + CHUNK_SZ,
    }
    x_cuts -= {0, SZ}
    for x in sorted(x_cuts):
        b0 = find_block(blocks, SZ-1, SZ-1)
        cut_x(blocks, b0, x)
        result.append('cut [%s] [X] [%d]' % (b0, x))

    for x in {CHUNK_SZ * tgt_chx, CHUNK_SZ * loc_chx}:
        y_cuts = set()
        if x == CHUNK_SZ * tgt_chx:
            y_cuts |= {CHUNK_SZ * tgt_chy, CHUNK_SZ * tgt_chy + CHUNK_SZ}
        if x == CHUNK_SZ * loc_chx:
            y_cuts |= {CHUNK_SZ * loc_chy, CHUNK_SZ * loc_chy + CHUNK_SZ}
        y_cuts -= {0, SZ}
        for y in sorted(y_cuts, key=lambda y: -abs(y - SZ/2)):
            b0 = find_block(blocks, x+1, y)
            cut_y(blocks, b0, y)
            result.append('cut [%s] [Y] [%d]' % (b0, y))
    b1 = find_block(blocks, CHUNK_SZ * loc_chx + 1, CHUNK_SZ * loc_chy + 1)
    b2 = find_block(blocks, CHUNK_SZ * tgt_chx + 1, CHUNK_SZ * tgt_chy + 1)
    swap(blocks, b1, b2)
    result.append('swap [%s] [%s]' % (b1, b2))
    generate_merge_all(blocks, result)


def compute_stddev_rank():
    xs = []
    for i in range(SZ // CHUNK_SZ):
        for j in range(SZ // CHUNK_SZ):
            xs.append((chunk_stddev[i][j], (i, j)))
    xs.sort(reverse=True)
    assert len(xs) == 16
    return [x[1] for x in xs[:13]]


def generate_perm(blocks, result, perm):
    b0 = find_block(blocks, 0, 0)
    cut_xy(blocks, b0, SZ//2, SZ//2)
    result.append('cut [%s] [%d,%d]' % (b0, SZ//2, SZ//2))

    for i in range(2):
        for j in range(2):
            x = CHUNK_SZ * (2 * i + 1)
            y = CHUNK_SZ * (2 * j + 1)
            b0 = find_block(blocks, x, y)
            cut_xy(blocks, b0, x, y)
            result.append('cut [%s] [%d,%d]' % (b0, x, y))

    for i in range(4):
        for j in range(4):
            if (i, j) not in perm:
                continue
            # perm: original to current location
            i0, j0 = perm[(i, j)]
            if i == i0 and j == j0:
                continue
            b1 = find_block(blocks, CHUNK_SZ * i + 1, CHUNK_SZ * j + 1)
            b2 = find_block(blocks, CHUNK_SZ * i0 + 1, CHUNK_SZ * j0 + 1)
            swap(blocks, b1, b2)
            result.append('swap [%s] [%s]' % (b1, b2))
            for ij0 in perm.keys():
                if perm[ij0] == (i, j):
                    perm[ij0] = (i0, j0)
    # last step: fill 3 last blocks
    for i in range(4):
        for j in range(4):
            if (i, j) in perm:
                continue
            b0 = find_block(blocks, CHUNK_SZ * i + 1, CHUNK_SZ * j + 1)
            result.append('color [%s] [%d,%d,%d,%d]' %
                          ((b0,) + tuple(chunk_colors[i][j])))


def generate_move_simple(blocks, result, tgt_chx, tgt_chy, loc_chx, loc_chy):
    if tgt_chx == loc_chx and tgt_chy == loc_chy:
        return

    b0 = find_block(blocks, 0, 0)
    cut_xy(blocks, b0, SZ//2, SZ//2)
    result.append('cut [%s] [%d,%d]' % (b0, SZ//2, SZ//2))

    x1 = CHUNK_SZ * (2 * loc_chx - 1)
    y1 = CHUNK_SZ * (2 * loc_chy - 1)

    x2 = CHUNK_SZ * (2 * (tgt_chx//2) + 1)
    y2 = CHUNK_SZ * (2 * (tgt_chy//2) + 1)

    for x, y in {(x1, y1), (x2, y2)}:
        b0 = find_block(blocks, x, y)
        cut_xy(blocks, b0, x, y)
        result.append('cut [%s] [%d,%d]' % (b0, x, y))

    b1 = find_block(blocks, CHUNK_SZ * loc_chx + 1, CHUNK_SZ * loc_chy + 1)
    b2 = find_block(blocks, CHUNK_SZ * tgt_chx + 1, CHUNK_SZ * tgt_chy + 1)
    swap(blocks, b1, b2)
    result.append('swap [%s] [%s]' % (b1, b2))
    generate_merge_all(blocks, result)


for idx in PROBLEMS:
    last_idx = 0
    square_colors = []
    chunk_stddev = []
    chunk_colors = []
    w, h, rowit, info = png.Reader('../../problems/%s.png' % idx).read()
    assert w == h == SZ
    image_2d = np.vstack(map(np.uint16, rowit))
    image_3d = np.reshape(image_2d, (SZ, SZ, 4))

    for i in range(SZ // SQUARE_SZ):
        row_colors = []
        x1 = i * SQUARE_SZ
        x2 = x1 + SQUARE_SZ
        for j in range(SZ // SQUARE_SZ):
            y2 = (SZ // SQUARE_SZ - j) * SQUARE_SZ
            y1 = y2 - SQUARE_SZ
            square = image_3d[y1:y2, x1:x2]
            med_color = np.median(square, axis=[0, 1])
            row_colors.append(med_color)
        square_colors.append(row_colors)

    for i in range(SZ // CHUNK_SZ):
        chunk_stds = []
        row_colors = []
        x1 = i * CHUNK_SZ
        x2 = x1 + CHUNK_SZ
        for j in range(SZ // CHUNK_SZ):
            y2 = (SZ // CHUNK_SZ - j) * CHUNK_SZ
            y1 = y2 - CHUNK_SZ
            chunk = image_3d[y1:y2, x1:x2]
            chunk_std = np.std(chunk, axis=(0, 1))
            chunk_stds.append(sum(chunk_std))
            med_color = np.median(chunk, axis=[0, 1])
            row_colors.append(med_color)
        chunk_stddev.append(chunk_stds)
        chunk_colors.append(row_colors)

    # print(image_3d)
    result = []

    blocks = {"0": Block(0, 0, SZ, SZ)}

    ijs = []

    ijs = compute_stddev_rank()
    pos = [
        (0, 0),
        (0, 1),
        (1, 0),
        (1, 1),

        (0, 2),
        (0, 3),
        (1, 2),

        (2, 0),
        (3, 0),
        (2, 1),

        (3, 1),
        (1, 3),
        (2, 2),
    ]

    perm = {}

    for rank, ij in enumerate(ijs):
        if rank < 4:
            loc_chx, loc_chy = 1, 1
        elif rank < 7:
            loc_chx, loc_chy = 1, 2
        elif rank < 10:
            loc_chx, loc_chy = 2, 1
        else:
            loc_chx, loc_chy = 2, 2
        i, j = pos[rank]
        perm[ij] = (i, j)
        generate_assemble(blocks, result, ij[0], ij[1], loc_chx, loc_chy)
        if rank != 12:
            generate_move_simple(blocks, result, i, j, loc_chx, loc_chy)

    generate_perm(blocks, result, perm)

    with open(os.path.join(this_dir, '../../solutions/paint_12_16/%s.txt' % idx), 'w') as g:
        g.write('\n'.join(result))
