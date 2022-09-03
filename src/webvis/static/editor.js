



function colorRegion(x, w, y, h, r, g, b, a) {
    return {
        x: x,
        w: w,
        y: y,
        h: h,
        r: r,
        g: g,
        b: b,
        a: a,
    }
}

function fill(block, r, g, b, a) {
    block.crs = [colorRegion(0, block.w, 0, block.h, r, g, b, a)]
}

function translate(crs, x, y) {
    return crs.map(cr => colorRegion(cr.x + x, cr.w, cr.y + y, cr.h, cr.r, cr.g, cr.b, cr.a))
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

    return colorRegion(x1, x2 - x1, y1, y2 - y1, cr.r, cr.g, cr.b, cr.a)
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
    fill(blocks[id], r, g, b, a);
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
            let [x, y] = [ev.offsetX, ev.offsetY]
            x /= 2
            y = (2 * h - 1 - y) / 2
            let clicked = []
            blockarr.forEach((b0) => {
                if (b0.x <= x && x <= b0.x + b0.w && b0.y <= y && y <= b0.y + b0.h) {
                    clicked.push(b0)
                }
            })
            if (clicked) {
                insertTextCommand("[" + clicked[0].id + "]");
            }
            console.log(clicked);
        })

    var crs = [];
    var crss = [];
    for (const [key, block] of Object.entries(blocks)) {
        if (key != "last_id") {
            blockarr.push(block);
            crs.push(colorRegion(block.x, block.w, block.y, block.h, 0, 0, 0, 255));
            crss.push(translate(block.crs, block.x, block.y));
        }
    }

    crs = crs.concat(...crss);

    d3.select("#svg")
        .selectAll(".rect")
        .data(crs)
        .enter()
        .append("rect")
        .attr("x", d => 2 * d.x)
        .attr("y", d => 2 * (h - d.y - d.h))
        .attr("width", d => 2 * d.w - 1)
        .attr("height", d => 2 * d.h - 1)
        .attr("fill", d => "rgba(" + d.r + "," + d.g + "," + d.b + "," + d.a + ")")
}

function get_w_h() {
    let w = d3.select("#png-problem").node().naturalWidth;
    let h = d3.select("#png-problem").node().naturalHeight;
    return [w, h];
}

function apply_solution(text) {
    lines = text.split("\n")

    let [w, h] = get_w_h();

    var blocks = {
        "last_id": 0,
        "0": newblock("0", 0, w, 0, h, []),
    };
    fill(blocks[0], 255, 255, 255, 255);

    lines.forEach(line => {
        line = line.replace(/\s/g, '');
        if (line == "" || line[0] == '#') return

        match = line.match(/cut\[([\d.]+)\]\[(\d+),(\d+)\]/i)
        if (match) {
            let [_, id, x, y] = match;
            apply_cut4(blocks, id, +x, +y);
        }

        match = line.match(/cut\[([\d.]+)\]\[([XxYy])\]\[(\d+)\]/i)
        if (match) {
            let [_, id, orient, xORy] = match;
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
            apply_color(blocks, id, +r, +g, +b, +a);
        }

        match = line.match(/swap\[([\d.]+)\]\[([\d.]+)\]/i)
        if (match) {
            let [_, id1, id2] = match;
            apply_swap(blocks, id1, id2);
        }

        match = line.match(/merge\[([\d.]+)\]\[([\d.]+)\]/i)
        if (match) {
            let [_, id1, id2] = match;
            apply_merge(blocks, id1, id2);
        }

    });

    draw_regions(blocks);
}

var canvas = document.createElement('canvas');


function insertTextCommand(text) {
    let node = d3.select("#commands").node();
    b = node.selectionEnd
    node.value = node.value.substring(0, b) + text + node.value.substring(b)
}

function set_problem() {
    var id = d3.select("#problem_id").node().value;

    d3.image("/problem?id=" + id).then(img => {
        d3.select("#prb-td")
            .selectAll("img")
            .remove()
        d3.select("#prb-td")
            .node()
            .appendChild(img)
        d3.select("#prb-td")
            .select("img")
            .attr("id", "png-problem")
            .attr("style", "border: 3px solid #000")
        var sol_folder = d3.select("#solution_folder").node().value || "best";

        var img = document.getElementById('png-problem');
        canvas.width = img.width;
        canvas.height = img.height;
        canvas.getContext('2d').drawImage(img, 0, 0, img.width, img.height);

        d3.text("/solution?id=" + id + "&kind=" + sol_folder).then(function (text) {
            d3.select("#commands").text(text)
            d3.select("#commands")
                .on("mouseout", () => {
                    apply_solution(d3.select("#commands").node().value)
                })
                .on("select", (e) => {
                    let node = d3.select("#commands").node();
                    b = node.selectionEnd
                    apply_solution(node.value.substring(0, b))
                })
            d3.select("#png-problem")
                .on("mousedown", (ev) => {
                    var rgba = canvas.getContext('2d').getImageData(ev.offsetX, ev.offsetY, 1, 1).data;
                    insertTextCommand("[" + rgba + "]");
                })


            // d3.select("#commands")
            //     .selectAll("li")
            //     .data(idxes)
            //     .enter()
            //     .append("li")
            //     .text(i => lines[i])
            //     .on("mouseover", (_, i) => {
            //         apply_solution(lines.slice(0, i))
            //     })
            // d3.select("#commands")
            //     
            apply_solution(text)
        });
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
        set_problem();
    })
}


d3.select(window).on("load", onload);
