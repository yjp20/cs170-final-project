<script>
	// @hmr:keep

	import { onMount } from "svelte";

	let canvas;
	let ctx;
	let input = ` 45
50
3
10
14 1
25 15
36 1
30 2
26 13
43 34
33 8
46 40
45 30
46 20
29 40
49 12
48 25
42 4
7 42
27 10
2 33
44 40
13 14
33 17
38 31
45 11
38 14
20 21
24 0
3 9
9 46
45 36
1 27
24 31
42 42
46 49
20 25
29 31
20 2
5 40
34 3
22 37
28 22
25 31
11 35
33 42
10 49
14 35
6 26`;
	let output = `27
1 11
1 31
3 26
5 42
9 48
11 1
12 14
13 33
18 23
21 0
22 40
25 12
27 31
28 25
31 42
33 1
33 2
33 5
36 17
38 17
38 30
43 40
44 2
45 33
47 12
48 22
49 49`;
	let testcase = ""

	let N, D, Rs, Rp, points = []
	let M, towers

	let moving = false;

	function readInput(input) {
		const lines = input.split("\n").filter(e => e[0] != "#")
		const N = Number(lines[0])
		const D = Number(lines[1])
		const Rs = Number(lines[2])
		const Rp = Number(lines[3])
		const points = []
		for (let i=0; i<N; i++) {
			const [a, b] = lines[i+4].split(" ")
			points.push([Number(a), Number(b)])
		}
		return [N,D,Rs,Rp,points]
	}

	function readOutput(output) {
		const lines = output.split("\n").filter(e => e[0] != "#")
		const M = Number(lines[0])
		const towers = []
		for (let i=0; i<M; i++) {
			const [a, b] = lines[i+1].split(" ")
			towers.push([Number(a), Number(b)])
		}
		return [M, towers]
	}

	function readGradient(csv) {

	}
	function readProgression(csv) {

	}

	function update() {
		fetch(`./inputs/${testcase}.in`).then(r => r.text()).then(readOut)
		fetch(`./outputs/${testcase}.out`).then(r => r.text()).then(readOut)
		fetch(`./outputs/${testcase}_gradient.csv`).then(r => r.text()).then(readGradient)
		fetch(`./outputs/${testcase}_progression.csv`).then(r => r.text()).then(readProgression)
	}

	const SPACING = 10;

	function draw(N,D,Rs,Rp,points,M,towers) {
		if (!ctx) return;
		ctx.clearRect(0, 0, canvas.width, canvas.height);

		for (let tower of towers) {
			ctx.beginPath();
			ctx.arc(SPACING * tower[0], SPACING * tower[1], Rp*SPACING, 0, 2 * Math.PI, false);
			ctx.strokeStyle = '#333333';
			ctx.stroke()
			ctx.closePath()
		}

		for (let tower of towers) {
			ctx.beginPath();
			ctx.arc(SPACING * tower[0], SPACING * tower[1], 3*SPACING, 0, 2 * Math.PI, false);
			ctx.fillStyle = '#FF333351';
			ctx.fill()
			ctx.closePath()
		}

		for (let tower of towers) {
			for (let other of towers) {
				const dist = Math.pow(tower[0] - other[0], 2) + Math.pow(tower[1] - other[1], 2)
				if (dist <= Rp*Rp) {
					drawLine(tower[0], tower[1], other[0], other[1], "blue", 3);
				}
			}
		}

		for (let point of points) {
			ctx.beginPath();
			ctx.arc(SPACING * point[0], SPACING * point[1], 3, 0, 2 * Math.PI, false);
			ctx.fillStyle = 'black';
			ctx.fill()
			ctx.closePath();
		}

		for (let i=0; i<D; i++) {
			drawLine(0, i, D-1, i, "#333333")
			drawLine(i, 0, i, D-1, "#333333")
		}
	}

	function onClick(e) {
		const x = Math.round(e.offsetX / SPACING)
		const y = Math.round(e.offsetY / SPACING)
		const index = towers.findIndex(([a,b]) => a==x && b==y);
		if (index != -1) {
			towers.splice(index, 1)
		} else {
			towers.push([x,y])
		}
		towers = towers;
	}



	function drawLine(x0, y0, x1, y1, color = "black", stroke_width = 1) {
		ctx.lineWidth = stroke_width
		ctx.strokeStyle = color

		ctx.beginPath()
		ctx.moveTo(x0 * SPACING, y0 * SPACING)
		ctx.lineTo(x1 * SPACING, y1 * SPACING)
		ctx.stroke()
		ctx.closePath()
	}

	function towersOut(towers) {
		let s = String(towers.length) + "\n"
		for (let tower of towers) {
			s += `${tower[0]} ${tower[1]}\n`
		}
		return s
	}

	onMount(() => {
		ctx = canvas.getContext("2d")
		draw(N,D,Rs,Rp,points,M,towers)
	})

	$: [N, D, Rs, Rp, points] = readInput(input)
	$: [M, towers] = readOutput(output)
	$: draw(N,D,Rs,Rp,points,M,towers)
</script>

<textarea id="input" bind:value={input} />
<textarea id="output" bind:value={output} />
<canvas bind:this={canvas} on:click={onClick} width=1002 height=1002 />
<pre>
	{towersOut(towers)}
</pre>

<style>
</style>
