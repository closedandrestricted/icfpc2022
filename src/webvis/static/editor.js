



// x, y denote coordinates relative to block
// pngx, pngy are x and y on PNG picture of the origin of the block
function colorRegion(x, w, y, h, rgba, pngx, pngy) {
    return {
        x: x,
        w: w,
        y: y,
        h: h,
        rgba: rgba,
        pngx: pngx + 0, // undefined to NaN
        pngy: pngy + 0, // undefined to NaN
    }
}

function fill(block, rgba) {
    block.crs = [colorRegion(0, block.w, 0, block.h, rgba)]
}

function translate(crs, x, y) {
    return crs.map(cr => colorRegion(cr.x + x, cr.w, cr.y + y, cr.h, cr.rgba, cr.pngx - x, cr.pngy + y))
}

function clip(x, lo, hi) {
    return Math.min(Math.max(x, lo), hi)
}

function trim(cr, w, h) {
    x1 = cr.x
    x1 = clip(x1, 0, w)

    x2 = cr.x + cr.w
    x2 = clip(x2, 0, w)

    y1 = cr.y
    y1 = clip(y1, 0, h)

    y2 = cr.y + cr.h
    y2 = clip(y2, 0, h)

    if (y2 < y1) throw cr
    if (x2 < x1) throw cr

    return colorRegion(x1, x2 - x1, y1, y2 - y1, cr.rgba, cr.pngx, cr.pngy)
}

function trim_all(crs, w, h) {
    return crs.map(cr => trim(cr, w, h)).filter(cr => cr.w > 0 && cr.h > 0)
}

function newblock(id, x, w, y, h, crs) {
    var block = {
        id: id,
        x: x,
        w: w,
        y: y,
        h: h,
        crs: trim_all(crs, w, h),
    }
    block.crs.forEach(cr => {
        if (cr.x < 0 || cr.y < 0 || cr.x + cr.w > block.w || cr.y + cr.h > block.h) {
            throw cr;
        }
    })
    return block
}

function apply_cut4(blocks, id, x, y) {
    let block = blocks[id]
    x -= block.x
    y -= block.y
    if (x <= 0 || x >= block.w) {
        console.log(block)
        throw x
    }
    if (y <= 0 || y >= block.h) {
        console.log(block)
        throw y
    }
    let b0 = newblock(id + ".0", block.x, x, block.y, y, block.crs);
    let b1 = newblock(id + ".1", block.x + x, block.w - x, block.y, y, translate(block.crs, -x, 0));
    let b2 = newblock(id + ".2", block.x + x, block.w - x, block.y + y, block.h - y, translate(block.crs, -x, -y));
    let b3 = newblock(id + ".3", block.x, x, block.y + y, block.h - y, translate(block.crs, 0, -y));
    delete blocks[id]
    blocks[b0.id] = b0
    blocks[b1.id] = b1
    blocks[b2.id] = b2
    blocks[b3.id] = b3
}

function apply_cutX(blocks, id, x) {
    let block = blocks[id]
    x -= block.x
    if (x <= 0 || x >= block.w) {
        console.log(block)
        throw x
    }
    let b0 = newblock(id + ".0", block.x, x, block.y, block.h, block.crs);
    let b1 = newblock(id + ".1", block.x + x, block.w - x, block.y, block.h, translate(block.crs, -x, 0));
    delete blocks[id]
    blocks[b0.id] = b0
    blocks[b1.id] = b1
}

function apply_cutY(blocks, id, y) {
    let block = blocks[id]
    y -= block.y
    if (y <= 0 || y >= block.h) {
        console.log(block)
        throw y
    }
    let b0 = newblock(id + ".0", block.x, block.w, block.y, y, block.crs);
    let b1 = newblock(id + ".1", block.x, block.w, block.y + y, block.h - y, translate(block.crs, 0, -y));
    delete blocks[id]
    blocks[b0.id] = b0
    blocks[b1.id] = b1
}

function apply_color(blocks, id, r, g, b, a) {
    fill(blocks[id], [r, g, b, a]);
}

function swap_props(obj1, obj2, name) {
    let tmp = obj1[name]
    obj1[name] = obj2[name]
    obj2[name] = tmp
}

function swap_keys(obj, key1, key2) {
    let tmp = obj[key1]
    obj[key1] = obj[key2]
    obj[key2] = tmp
}

function apply_swap(blocks, id1, id2) {
    var b1 = blocks[id1];
    var b2 = blocks[id2];
    if (b1.w != b2.w || b1.h != b2.h) {
        console.log(b1)
        console.log(b2)
        throw ("incompatible sizes")
    }
    swap_props(b1, b2, "crs");
    swap_props(b1, b2, "id");
    swap_keys(blocks, id1, id2);
}

function apply_merge(blocks, id1, id2) {
    var b1 = blocks[id1];
    var b2 = blocks[id2];
    var newid = "" + (blocks.last_id + 1)
    let b0 = undefined;
    if (b1.x + b1.w == b2.x && b1.y == b2.y && b1.h == b2.h) {
        b0 = newblock(newid, b1.x, b1.w + b2.w, b1.y, b1.h, [].concat(b1.crs, translate(b2.crs, b1.w, 0)));
    } else if (b1.y + b1.h == b2.y && b1.x == b2.x && b1.w == b2.w) {
        b0 = newblock(newid, b1.x, b1.w, b1.y, b1.h + b2.h, [].concat(b1.crs, translate(b2.crs, 0, b1.h)));
    } else if (b2.x + b2.w == b1.x && b2.y == b1.y && b2.h == b1.h) {
        b0 = newblock(newid, b2.x, b2.w + b1.w, b2.y, b2.h, [].concat(b2.crs, translate(b1.crs, b2.w, 0)));
    } else if (b2.y + b2.h == b1.y && b2.x == b1.x && b2.w == b1.w) {
        b0 = newblock(newid, b2.x, b2.w, b2.y, b2.h + b1.h, [].concat(b2.crs, translate(b1.crs, 0, b2.h)));
    } else {
        throw ("incompatible locations")
    }
    delete blocks[id1]
    delete blocks[id2]
    blocks[b0.id] = b0
    blocks.last_id = blocks.last_id + 1
}

function draw_crs(crs) {
    let [w, h] = get_w_h()

    d3.select("#svg").html("")

    let plain_crs = []
    let image_crs = []

    crs.forEach(cr => {
        if (cr.rgba) {
            plain_crs.push(cr);
        } else {
            image_crs.push(cr);
        }
    })

    d3.select("#svg")
        .selectAll(".rect")
        .data(plain_crs)
        .enter()
        .append("rect")
        .attr("x", d => 2 * d.x)
        .attr("y", d => 2 * (h - d.y - d.h))
        .attr("width", d => 2 * d.w - 1)
        .attr("height", d => 2 * d.h - 1)
        .attr("fill", d => "rgba(" + d.rgba[0] + "," + d.rgba[1] + "," + d.rgba[2] + "," + d.rgba[3] + ")")


    // console.log(image_crs)
    d3.select("#svg")
        .selectAll(".svg")
        .data(image_crs)
        .enter()
        .append("svg")
        .attr("x", d => 2 * d.x)
        .attr("y", d => 2 * (h - d.y - d.h))
        .attr("width", d => 2 * d.w - 1)
        .attr("height", d => 2 * d.h - 1)
        .append("image")
        .attr("x", d => -2 * d.pngx - 2 * d.x)
        .attr("y", d => -2 * d.pngy - 2 * (h - d.y - d.h))
        .attr("width", d => 2 * w - 1)
        .attr("height", d => 2 * h - 1)
        .attr("href", "/source?id=" + globProblemId)
}

function draw_regions(blocks) {

    let [w, h] = get_w_h()

    const xScale = d3.scaleLinear()
        .domain([0, w])
        .range([0, 2 * w - 1]);

    const yScale = d3.scaleLinear()
        .domain([0, h])
        .range([2 * h - 1, 0]);

    var blockarr = [].concat(blocks);

    d3.select("#svg-td").select("svg").remove()
    d3.select("#svg-td")
        .append("svg")
        .attr("id", "svg")
        .attr("width", 2 * w - 1)
        .attr("height", 2 * h - 1)
        .attr("style", "border: 1px solid #000")
        .on("mousedown", (ev) => {
            if (ev.altKey) {
                d3.select("#svg")
                    .html("")
                    .append("image")
                    .attr("xlink:href", "/problem?id=" + globProblemId)
                    .attr("style", "image-rendering: pixelated")
                    .attr("width", 2 * w - 1)
                    .attr("height", 2 * h - 1);
                return
            }
            let [x, y] = [ev.offsetX, ev.offsetY]
            x /= 2
            y = (2 * h - 1 - y) / 2
            let clicked = []
            blockarr.forEach((b0) => {
                if (rect_contains_xy(b0, x, y)) {
                    clicked.push(b0)
                }
            })
            if (clicked) {
                insertTextCommand("[" + clicked[0].id + "]");
            }
            var errcrs = []
            crss.forEach(crs => crs.forEach(cr => {
            }))
        })
        .on("mouseup", (ev) => {
            draw_crs(crs);
        })

    var crs = [];
    var crss = [];
    for (const [key, block] of Object.entries(blocks)) {
        if (key != "last_id" && key != "penalty") {
            blockarr.push(block);
            crs.push(colorRegion(block.x, block.w, block.y, block.h, [0, 0, 0, 255]));
            crss.push(translate(block.crs, block.x, block.y));
        }
    }

    crs = crs.concat(...crss);

    draw_crs(crs);

    let penalty_diff = diffPenalty(crss)
    d3.select("#penalty_diff").text(penalty_diff)
    d3.select("#penalty_ops").text(blocks.penalty)
    d3.select("#penalty_total").text(blocks.penalty + penalty_diff)
}

function get_w_h() {
    let w = d3.select("#png-problem").node().naturalWidth;
    let h = d3.select("#png-problem").node().naturalHeight;
    return [w, h];
}

function size(block) {
    return block.w * block.h
}

function cost(baseCost, w, h, block) {
    return Math.round(baseCost * w * h / size(block))
}

function rect_contains_xy(b0, x, y) {
    return (b0.x <= x && x <= b0.x + b0.w && b0.y <= y && y <= b0.y + b0.h)
}

function get_block_id_for_coords(blocks, x, y) {
    for (const [key, block] of Object.entries(blocks)) {
        if (rect_contains_xy(block, x, y)) {
            return key
        }
    }
    console.log(x, y)
    console.log(blockarr)
    throw "did not find block for coords"
}

// x, y are relative to block origin
function get_cr_color(cr, x, y) {
    try {
        if (cr.rgba) {
            return cr.rgba
        } else {
            return srcGlobPX[cr.pngx + x][cr.pngy + y]
        }
    } catch (e) {
        console.log(cr)
        console.log(x, y)
        throw (e)
    }
}

// get pixel color where x, y are absolute coordinates
function find_pixel_color(block, x, y) {
    for (let i = 0; i < block.crs.length; i++) {
        let cr = block.crs[i]
        if (rect_contains_xy(cr, x - block.x, y - block.y)) {
            return get_cr_color(cr, x - block.x, y - block.y)
        }
    }
    console.log(x, y)
    console.log(block)
    throw "did not find color region for coords"
}

function get_1d_paint_plan(rgbas, dx, w, h) {
    let cost = [0]
    let pcut = [-1]
    let clrs = [undefined]
    let kk = w / dx;
    for (let i = 0; i < rgbas.length; i++) {
        cost.push(1e20)
        pcut.push(-1)
        clrs.push(undefined)
        for (let j = 0; j <= i; j++) {
            let curcost = cost[j] + Math.round(8 * w * h / (dx * dx * (kk - j)))
            let rgba_num = [0, 0, 0, 0]
            let rgba_den = [0, 0, 0, 0]
            let rgba_avg = [0, 0, 0, 0]
            for (let k = j; k <= i; k++) {
                for (let c = 0; c < 4; c++) {
                    rgba_num[c] += rgbas[k][c];
                    rgba_den[c] += 1.0
                }
            }
            for (let c = 0; c < 4; c++) {
                rgba_avg[c] = Math.round(rgba_num[c] / rgba_den[c])
            }
            let imgdiff = 0
            for (let k = j; k <= i; k++) {
                for (let c = 0; c < 4; c++) {
                    let x = (rgbas[k][c] - rgba_avg[c])
                    imgdiff += x * x
                }
            }
            imgdiff = Math.sqrt(imgdiff)
            imgdiff = imgdiff * (i - j + 1) * dx * dx
            curcost += Math.round(0.005 * imgdiff)
            if (curcost < cost[i + 1]) {
                cost[i + 1] = curcost
                pcut[i + 1] = j
                clrs[i + 1] = rgba_avg
            }
        }
    }
    // console.log(cost)
    // console.log(pcut)
    // console.log(clrs)
    let i = rgbas.length
    let plan = {}
    while (i > 0) {
        plan[i - 1] = clrs[i]
        i = pcut[i]
    }
    return plan
}

function auto_solve(initial) {
    blocks = blocks_init(initial)
    let dx = blocks["0"].w
    let dy = blocks["0"].h
    text = ''
    // console.log(initial)
    for (let x = dx / 2; x <= initial.width; x += dx) {
        for (let y = dy / 2 + dy; y <= initial.height; y += dy) {
            apply_solution(blocks, text)
            b1_id = get_block_id_for_coords(blocks, x, y - dy)
            b2_id = get_block_id_for_coords(blocks, x, y)
            text += 'merge [' + b1_id + '] [' + b2_id + ']\n'
            blocks = blocks_init(initial)
        }
        apply_solution(blocks, text)
        let rgbas = []
        for (let y = dy / 2; y <= initial.height; y += dy) {
            let rgba_num = [0, 0, 0, 0]
            let rgba_den = [0, 0, 0, 0]
            for (let xx = x - dx / 2; xx < x + dx / 2; xx++) {
                for (let yy = y - dy / 2; yy < y + dy / 2; yy++) {
                    for (let i = 0; i < 4; i++) {
                        rgba_num[i] += globPX[xx][initial.height - yy - 1][i];
                        rgba_den[i] += 1
                    }
                }
            }
            let rgba = [0, 0, 0, 0]
            for (let i = 0; i < 4; i++) {
                rgba[i] = Math.round(rgba_num[i] / rgba_den[i])
            }
            rgbas.push(rgba)
        }
        blocks = blocks_init(initial)

        // console.log('RGBAS')
        // console.log(rgbas)

        plan = get_1d_paint_plan(rgbas, dx, initial.width, initial.height)
        for (let step = 0; step < rgbas.length; step++) {
            if (!plan[step]) continue
            apply_solution(blocks, text)
            let y = dy / 2 + dy * step
            b0_id = get_block_id_for_coords(blocks, x, y)
            text += 'color [' + b0_id + '] [' + plan[step] + ']\n'
            if (y + dy < initial.height) {
                text += 'cut [' + b0_id + '] [Y] [' + (y + dy / 2) + ']\n'
                prevcut = true
            }
            blocks = blocks_init(initial)
        }
        console.log("At x = " + x)
    }
    d3.select("#commands").node().value = text
    apply_solution(blocks, text)
}

function try_apply_solution(initial, text) {
    try {
        blocks = blocks_init(initial)
        apply_solution(blocks, text)
    } catch (e) {
        d3.select("#penalty_total").text("ERROR!!")
        console.log(e)
        throw (e)
    }
}

function blocks_init(initial) {
    var blocks = {
        "penalty": 0,
    };
    initial.blocks.forEach(jsonblock => {
        let [x, w, y, h] = [
            jsonblock.bottomLeft[0],
            jsonblock.topRight[0] - jsonblock.bottomLeft[0],
            jsonblock.bottomLeft[1],
            jsonblock.topRight[1] - jsonblock.bottomLeft[1]
        ]
        let rgba = undefined
        let pngx = undefined
        let pngy = undefined
        if (jsonblock.color) {
            rgba = jsonblock.color
        } else {
            pngx = jsonblock.pngBottomLeftPoint[0]
            pngy = jsonblock.pngBottomLeftPoint[1]
        }
        blocks[jsonblock.blockId] = newblock(
            jsonblock.blockId, x, w, y, h,
            [colorRegion(0, w, 0, h, rgba, pngx, pngy)])
    })
    blocks.last_id = initial.blocks.length - 1
    return blocks;
}

function apply_solution(blocks, text) {
    lines = text.split("\n")

    let [w, h] = get_w_h();

    lines.forEach(line => {
        line = line.replace(/\s/g, '');
        if (line == "" || line[0] == '#') return

        match = line.match(/cut\[([\d.]+)\]\[(\d+),(\d+)\]/i)
        if (match) {
            let [_, id, x, y] = match;
            blocks.penalty += cost(10, w, h, blocks[id])
            apply_cut4(blocks, id, +x, +y);
        }

        match = line.match(/cut\[([\d.]+)\]\[([XxYy])\]\[(\d+)\]/i)
        if (match) {
            let [_, id, orient, xORy] = match;
            blocks.penalty += cost(7, w, h, blocks[id])
            if (orient == "X" || orient == "x") {
                apply_cutX(blocks, id, +xORy);
            } else if (orient == "Y" || orient == "y") {
                apply_cutY(blocks, id, +xORy);
            } else {
                throw orient;
            }
        }

        match = line.match(/color\[([\d.]+)\]\[(\d+),(\d+),(\d+),(\d+)\]/i)
        if (match) {
            let [_, id, r, g, b, a] = match;
            blocks.penalty += cost(5, w, h, blocks[id])
            apply_color(blocks, id, +r, +g, +b, +a);
        }

        match = line.match(/swap\[([\d.]+)\]\[([\d.]+)\]/i)
        if (match) {
            let [_, id1, id2] = match;
            if (size(blocks[id1]) > size(blocks[id2])) {
                blocks.penalty += cost(3, w, h, blocks[id1])
            } else {
                blocks.penalty += cost(3, w, h, blocks[id2])
            }
            apply_swap(blocks, id1, id2);
        }

        match = line.match(/merge\[([\d.]+)\]\[([\d.]+)\]/i)
        if (match) {
            let [_, id1, id2] = match;
            if (size(blocks[id1]) > size(blocks[id2])) {
                blocks.penalty += cost(1, w, h, blocks[id1])
            } else {
                blocks.penalty += cost(1, w, h, blocks[id2])
            }
            apply_merge(blocks, id1, id2);
        }
    });

    draw_regions(blocks);
}

var globPX = []
var srcGlobPX = []

function setCanvas(img, globPX) {
    var canvas = document.createElement('canvas');
    canvas.width = img.width;
    canvas.height = img.height;
    canvas.getContext('2d').drawImage(img, 0, 0, img.width, img.height);
    while (globPX.length > 0) globPX.pop(); // clear
    let ctx = canvas.getContext('2d')
    for (let x = 0; x < img.width; x++) {
        let px = []
        for (let y = 0; y < img.height; y++) {
            let data = ctx.getImageData(x, y, 1, 1).data
            data = [data[0], data[1], data[2], data[3]]
            data.forEach(d => {
                if (isNaN(d)) {
                    console.log(data, x, y)
                    throw "NaN in Canvas"
                }
            })
            px.push(data)
        }

        globPX.push(px)
    }
}

function insertTextCommand(text) {
    let node = d3.select("#commands").node();
    b = node.selectionEnd
    node.value = node.value.substring(0, b) + text + node.value.substring(b)
}

// here all crss are block-less, i.e. their offsets are absolute
function diffPenalty(crss) {
    let result = 0
    let [w, h] = get_w_h()
    crss.forEach(crs => {
        crs.forEach(cr => {
            for (let dx = 0; dx < cr.w; dx++) {
                for (let dy = 0; dy < cr.h; dy++) {
                    let x = cr.x + dx
                    let y = h - cr.y - dy - 1
                    let rgba = get_cr_color(cr, x, y)
                    let px = globPX[x][y]
                    let dresult = 0
                    for (let i = 0; i < 4; i++) {
                        dresult += (px[i] - rgba[i]) * (px[i] - rgba[i])
                    }
                    dresult = Math.sqrt(dresult)
                    if (isNaN(dresult)) {
                        console.log(x, y)
                        console.log(globPX[x][y])
                        console.log(rgba)
                        throw cr
                    }
                    result += dresult
                }
            }
        })
    })
    return Math.round(result * 0.005)
}


async function set_solution() {
    var id = d3.select("#problem_id").node().value;
    var sol_folder = d3.select("#solution_folder").node().value || "best";

    let initial = await d3.json("/initial?id=" + id)

    if (initial.blocks["0"].pngBottomLeftPoint) {
        let srcImg = await d3.image("/source?id=" + globProblemId)
        d3.select("#source-hidden-div")
            .selectAll("img")
            .remove()
        d3.select("#source-hidden-div")
            .node()
            .appendChild(srcImg)
        d3.select("#source-hidden-div")
            .select("img")
            .attr("id", "source-hidden-img")

        setCanvas(document.getElementById('source-hidden-img'), srcGlobPX)
    }

    d3.select('#auto_solve').on('click', function (e) {
        e.stopPropagation();
        e.preventDefault();
        console.log('autosolve')
        auto_solve(initial)
    })

    let text = await d3.text("/solution?id=" + id + "&kind=" + sol_folder)

    d3.select("#commands").text(text)

    d3.select("#commands")
        .on("select", (e) => {
            let node = d3.select("#commands").node();
            b = node.selectionEnd
            try_apply_solution(initial, node.value.substring(0, b))
        })

    d3.select('#solution_draw').on("click", function (e) {
        e.stopPropagation();
        e.preventDefault();
        try_apply_solution(initial, d3.select("#commands").node().value)
    })
    d3.select("#png-problem")
        .on("mousedown", (ev) => {
            if (ev.altKey) {
                var [w, h] = get_w_h()
                const MAX = 10;
                let [bestx, besty, bestd] = [ev.offsetX, ev.offsetY, 0]
                for (let dx = -MAX; dx <= MAX; dx++) {
                    for (let dy = -MAX; dy <= MAX; dy++) {
                        var x = ev.offsetX + dx;
                        var y = ev.offsetY + dy;
                        if (x <= 0 || y <= 0 || x >= w || y >= h) {
                            continue;
                        }
                        let px = [];
                        px.push(globPX[x][y])
                        px.push(globPX[x][y - 1])
                        px.push(globPX[x - 1][y - 1])
                        px.push(globPX[x - 1][y])
                        let sumdist = 0
                        for (let i = 0; i < 4; i++) {
                            p1 = px[i]
                            p2 = px[(i + 1) % 4]
                            sumdist += Math.hypot(...p1.map((d, i) => d - p2[i]))
                        }
                        if (sumdist > bestd) {
                            bestx = x
                            besty = y
                            bestd = sumdist
                        }
                    }
                }
                insertTextCommand("[" + bestx + ", " + (h - besty - 1) + "]");
            } else {
                var rgba = globPX[ev.offsetX][ev.offsetY]
                insertTextCommand("[" + rgba + "]");
            }
        })
        .on("mousemove", (ev) => {
            if (ev.altKey) {
                d3.select("#color-coord-sel").select("i").text("COORDS")
            } else {
                d3.select("#color-coord-sel").select("i").text("COLOR")
            }
        })
    try_apply_solution(initial, text)
}

var globProblemId = undefined

function set_problem() {
    globProblemId = d3.select("#problem_id").node().value;

    d3.image("/problem?id=" + globProblemId).then(img => {
        d3.select("#prb-td")
            .selectAll("img")
            .remove()
        d3.select("#prb-td")
            .node()
            .appendChild(img)
        d3.select("#prb-td")
            .select("img")
            .attr("id", "png-problem")
            .attr("style", "image-rendering: crisp-edges")

        var img = document.getElementById('png-problem');
        setCanvas(img, globPX)
        set_solution()
    })
}

function onload() {
    d3.select("#problem_id").node().value = 1
    set_problem()
    d3.select('#submit').on('click', function (e) {
        e.stopPropagation();
        e.preventDefault();
        set_problem();
    })
    d3.select('#solution_load').on('click', function (e) {
        e.stopPropagation();
        e.preventDefault();
        set_solution();
    })
}


d3.select(window).on("load", onload);
