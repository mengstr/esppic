/*jshint -W097*/ // don't warn about "use strict"
/*jshint bitwise: false*/
'use strict';

Max7219.Nop = 0x00;
Max7219.Digit0 = 0x01;
Max7219.Digit1 = 0x02;
Max7219.Digit2 = 0x03;
Max7219.Digit3 = 0x04;
Max7219.Digit4 = 0x05;
Max7219.Digit5 = 0x06;
Max7219.Digit6 = 0x07;
Max7219.Digit7 = 0x08;
Max7219.Mode = 0x09;
Max7219.Intensity = 0x0A;
Max7219.ScanLimit = 0x0B;
Max7219.Shutdown = 0x0C;
Max7219.Test = 0x0F;

Max7219.AsciiTable = [
    0x7E, 0x30, 0x6D, 0x79, // 0123
    0x33, 0x5B, 0x5F, 0x70, // 4567
    0x7F, 0x7B, 0x01, 0x4F, // 89-E
    0x37, 0x0E, 0x67, 0x00 // HLP 
];

Max7219.SEGA = 0x40;
Max7219.SEGB = 0x20;
Max7219.SEGC = 0x10;
Max7219.SEGD = 0x08;
Max7219.SEGE = 0x04;
Max7219.SEGF = 0x02;
Max7219.SEGG = 0x01;
Max7219.SEGDP = 0x80;


// Gradient made at http://www.perbang.dk/rgbgradient/
Max7219.ColorTable = [
    '#400000', '#4C0000', '#590000', '#660000',
    '#720000', '#7F0000', '#8C0000', '#990000',
    '#A50000', '#B20000', '#BF0000', '#CC0000',
    '#D80000', '#E50000', '#F20000', '#FF0000',
];


// Automatically attach the css styles required
var style = document.createElement('style');
style.innerHTML = `/* Max7219 */`;
document.head.appendChild(style);


//
//
//
function Max7219(canvas) {
    this.canvas = canvas;
    this.type = 'Max7219'
    this.isSPI = true;
    this.regs = [0, 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0];
    this.firstByte = 0;
    this.secondByte = 0
    this.refresh();
}


//
//
//
Max7219.prototype.setRegister = function(register, value) {
    this.regs[register] = value;
    this.refresh();
};


//
//
//
Max7219.prototype.spi = function(data) {
    var holdByte = this.secondByte;
    this.secondByte = this.firstByte;
    this.firstByte = data;
    return holdByte;
};

//
//
//
Max7219.prototype.latch = function() {
    this.setRegister(this.secondByte, this.firstByte);
};


//
//
//
Max7219.prototype.refresh = function() {
    var context = this.canvas.getContext('2d');
    if (!context) return;
    context.clearRect(0, 0, this.canvas.width, this.canvas.height);

    for (var i = 0; i < 8; i++) {
        var bits = this.regs[Max7219.Digit0 + i];
        if (this.regs[Max7219.Mode] & (1 << i)) { // Binary=0     Decode=1
            bits = Max7219.AsciiTable[bits & 0x0F] | (bits & 0x80); // Lookup ascii and add back DP
        }
        if (i > this.regs[Max7219.ScanLimit]) bits = 0x00; // Turn off digits over scanlimit
        if (this.regs[Max7219.Test] & 1) bits = 0xFF; // Normal=0     Test=1
        if (!(this.regs[Max7219.Shutdown] & 1)) bits = 0x00; // Shutdown=0   Normal=1
        var color = Max7219.ColorTable[this.regs[Max7219.Intensity] & 0x0F];
        // The digits is drawn in two steps. First all the unlit segments
        // by using a bit-inverted value. Then the lit segments. This is
        // done to reduce the code size a bit.
        this.draw1DigitLayer(context, ~bits, i * 22, 0, '#202020');
        this.draw1DigitLayer(context, bits, i * 22, 0, color);
    }
};


//
// Draws the segments specified by 'v' with the requested color
// By some reason the MAX72xx have the order of segments reversed
// compared to the usual pattern
//
// Bit#   :  7 6 5 4 3 2 1 0
//---------------------------------------
// Normal : dp G F E D C B A
// Max72xx: dp A B C D E F G
//
Max7219.prototype.draw1DigitLayer = function(ctx, bits, xo, yo, c) {
    var x = 7 + xo,
        y = 4 + yo;
    var linewidth = 3.5;
    var height = 11;
    var width = 12;

    ctx.lineCap = 'round';

    ctx.beginPath();
    ctx.strokeStyle = c;
    ctx.lineWidth = linewidth;

    if (bits & Max7219.SEGA) {
        ctx.moveTo(4 + x, y);
        ctx.lineTo(4 + x + width, y);
    }

    if (bits & Max7219.SEGB) {
        ctx.moveTo(4 + x + width, y);
        ctx.lineTo(2 + x + width, y + height * 1);
    }

    if (bits & Max7219.SEGC) {
        ctx.moveTo(2 + x + width, y + height * 1);
        ctx.lineTo(x + width, y + height * 2);
    }

    if (bits & Max7219.SEGD) {
        ctx.moveTo(x, y + height * 2);
        ctx.lineTo(x + width, y + height * 2);
    }

    if (bits & Max7219.SEGE) {
        ctx.moveTo(2 + x, y + height * 1);
        ctx.lineTo(x, y + height * 2);
    }

    if (bits & Max7219.SEGF) {
        ctx.moveTo(4 + x, y);
        ctx.lineTo(2 + x, y + height * 1);
    }

    if (bits & Max7219.SEGG) {
        ctx.moveTo(2 + x, y + height * 1);
        ctx.lineTo(2 + x + width, y + height * 1);
    }

    ctx.stroke();

    if (bits & Max7219.SEGDP) {
        ctx.beginPath();
        ctx.lineWidth = 1;
        ctx.fillStyle = c;
        ctx.arc(x + width * 1.4, y + height * 2, 2, 0, 2 * Math.PI, false);
        ctx.fill();
    }
};