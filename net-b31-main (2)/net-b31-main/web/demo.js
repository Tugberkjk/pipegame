const EMPTY = 0;
const ENDPOINT = 1;
const SEGMENT = 2;
const CORNER = 3;
const TEE = 4;
const CROSS = 5;
const NB_SHAPES = 6;

const NORTH = 0;
const EAST = 1;
const SOUTH = 2;
const WEST = 3;
const NB_DIRS = 4;

let gameInstance;
const cellSize = 50;

const shapeImageFiles = {
    1: "images/endpoint.png",
    2: "images/segment.png",
    3: "images/corner.png",
    4: "images/tee.png",
    5: "images/cross.png"
};

const shapeImages = {};
for (let shape in shapeImageFiles) {
    const img = new Image();
    img.src = shapeImageFiles[shape];
    shapeImages[shape] = img;
}

function drawPiece(ctx, shape, orient, x, y, size) {
    if (shape == EMPTY) return;

    const img = shapeImages[shape];
    if (!img || !img.complete) return;

    ctx.save();
    ctx.translate(x + size / 2, y + size / 2);
    ctx.rotate((orient * Math.PI) / 2);
    ctx.drawImage(img, -size / 2, -size / 2, size, size);
    ctx.restore();
}

function drawCanvas(game) {
    document.getElementById("winMessage").style.display = "none";
    const canvas = document.getElementById("gameCanvas");
    const ctx = canvas.getContext("2d");

    const rows = Module._nb_rows(game);
    const cols = Module._nb_cols(game);
    canvas.width = cols * cellSize;
    canvas.height = rows * cellSize;

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
            const shape = Module._get_piece_shape(game, r, c);
            const orient = Module._get_piece_orientation(game, r, c);
            drawPiece(ctx, shape, orient, c * cellSize, r * cellSize, cellSize);
        }
    }
}

function checkGameWon() {
    if (Module._won(gameInstance)) {
        document.getElementById("winMessage").style.display = "block";
    }
}

function start() {
    gameInstance = Module._new_default();
    drawCanvas(gameInstance);

    const canvas = document.getElementById("gameCanvas");
    canvas.addEventListener("click", function (e) {
        const rect = canvas.getBoundingClientRect();
        const col = Math.floor((e.clientX - rect.left) / cellSize);
        const row = Math.floor((e.clientY - rect.top) / cellSize);

        Module._play_move(gameInstance, row, col, 1);
        drawCanvas(gameInstance);
        checkGameWon();
    });

    document.getElementById("solveBtn").addEventListener("click", () => {
        Module._solve(gameInstance);
        drawCanvas(gameInstance);
        checkGameWon();
    });

    document.getElementById("randomBtn").addEventListener("click", () => {
        const rows = Math.floor(Math.random() * 3) + 5;  
        const cols = Math.floor(Math.random() * 3) + 5;  
        const wrapping = Math.random() < 0.5 ? 1 : 0;     
        const nb_empty = Math.floor(Math.random() * 5);  
        const nb_extra = Math.floor(Math.random() * 5);  
    
        gameInstance = Module._new_random(rows, cols, wrapping, nb_empty, nb_extra);
        Module._restart(gameInstance); 
        drawCanvas(gameInstance);
        document.getElementById("winMessage").style.display = "none";
    });
    document.getElementById("undo").addEventListener("click", () => {
        Module._undo(gameInstance);
        drawCanvas(gameInstance);
        checkGameWon();
    });
    document.getElementById("redo").addEventListener("click", () => {
        Module._redo(gameInstance);
        drawCanvas(gameInstance);
        checkGameWon();
    });
}
Module.onRuntimeInitialized = () => {
    start();
};
