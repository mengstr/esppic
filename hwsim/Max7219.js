/*jshint -W097*/ // don't warn about "use strict"
/*jshint bitwise: false*/


var Max7219 = (function() {


    // var _id = 0;
    // var privateVars = [];

    // function Restaurant(name) {
    //     this.id = ++_id;
    //     this.name = name;
    //     privateVars[this.id] = {
    //         cooked: []
    //     };
    // }

    // Max7219.prototype.cook = function(food) {
    //         privateVars[this.id].cooked.push(food);
    // }

    // This is the bit mask for each segment, note that the order
    // is different from the common scheme
    const SEGMENT = {
        A: 0x40,
        B: 0x20,
        C: 0x10,
        D: 0x08,
        E: 0x04,
        F: 0x02,
        G: 0x01,
        DP: 0x80
    };

    // Segments for the built-in ASCII mode
    const AsciiTable = [
        0x7E, 0x30, 0x6D, 0x79, // 0 1 2 3
        0x33, 0x5B, 0x5F, 0x70, // 4 5 6 7
        0x7F, 0x7B, 0x01, 0x4F, // 8 9 - E
        0x37, 0x0E, 0x67, 0x00, // H L P 
    ];

    // Gradient of reds for the 16 intensity levels.
    // Made at http://www.perbang.dk/rgbgradient/
    const ColorTable = [
        '#400000', '#4C0000', '#590000', '#660000',
        '#720000', '#7F0000', '#8C0000', '#990000',
        '#A50000', '#B20000', '#BF0000', '#CC0000',
        '#D80000', '#E50000', '#F20000', '#FF0000',
    ];

    const MATRIXPITCH = 10;


    // // Automatically attach the css styles required
    // var style = document.createElement('style');
    // style.innerHTML = `/* Max7219 */`;
    // document.head.appendChild(style);


    //
    //
    //
    /** @constructor */
    function Max7219(canvas, typ) {
        this.canvas = canvas;
        this.type = typ;
        this.typeName = 'Max7219_' + typ;
        if (typ == Max7219.TYPE.SEGMENT) this.typeName = 'Max7219_segment';
        if (typ == Max7219.TYPE.MATRIX) this.typeName = 'Max7219_matrix';
        this.isSPI = true;
        this.regs = [0, 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0];
        this.firstByte = 0;
        this.secondByte = 0
        Refresh(this);
    }


    //
    //
    //
    Max7219.TYPE = {
        SEGMENT: 0,
        MATRIX: 1
    };


    //
    //
    // Max7219 commands (bits 8..15 of each 16 bit spi transmission) 
    Max7219.CMD = {
        Nop: 0x00,
        Digit0: 0x01,
        Digit1: 0x02,
        Digit2: 0x03,
        Digit3: 0x04,
        Digit4: 0x05,
        Digit5: 0x06,
        Digit6: 0x07,
        Digit7: 0x08,
        Mode: 0x09,
        Intensity: 0x0A,
        ScanLimit: 0x0B,
        Shutdown: 0x0C,
        Test: 0x0F
    };


    //
    //
    //
    Max7219.prototype.spi = function(data) {
        var holdByte = this.secondByte;
        this.secondByte = this.firstByte;
        this.firstByte = data;
        // console.log("spi data=" + data + " second byte=" + this.secondByte);
        return holdByte;
    };


    //
    //
    //
    Max7219.prototype.latch = function() {
        //        console.log("Max7219.latch display:" + this.canvas.id + " (" + this.secondByte + "=" + this.firstByte + ")");
        this.regs[this.secondByte] = this.firstByte;
        Refresh(this);
    };


    //
    //
    //
    Refresh = function(th) {
        var context = th.canvas.getContext('2d');
        if (!context) return;
        context.clearRect(0, 0, th.canvas.width, th.canvas.height);

        for (var i = 0; i < 8; i++) {
            var bits = th.regs[Max7219.CMD.Digit0 + i];
            if (th.regs[Max7219.CMD.Mode] & (1 << i)) { // Binary=0     Decode=1
                bits = AsciiTable[bits & 0x0F] | (bits & 0x80); // Lookup ascii and add back DP
            }
            if (i > th.regs[Max7219.CMD.ScanLimit]) bits = 0x00; // Turn off digits over scanlimit
            if (th.regs[Max7219.CMD.Test] & 1) bits = 0xFF; // Normal=0     Test=1
            if (!(th.regs[Max7219.CMD.Shutdown] & 1)) bits = 0x00; // Shutdown=0   Normal=1
            var color = ColorTable[th.regs[Max7219.CMD.Intensity] & 0x0F];
            if (th.type == Max7219.TYPE.SEGMENT) {
                // The digits is drawn in two steps. First all the unlit segments
                // by using a bit-inverted value. Then the lit segments. this is
                // done to reduce the code size a bit.
                Draw1DigitLayer(context, ~bits, i * 22, 0, '#202020');
                Draw1DigitLayer(context, bits, i * 22, 0, color);
            }
            if (th.type == Max7219.TYPE.MATRIX) {
                Draw1MatrixRow(context, bits, 0, i * MATRIXPITCH, color);
            }
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
    Draw1DigitLayer = function(ctx, bits, xo, yo, c) {
        var x = 7 + xo,
            y = 4 + yo;
        var linewidth = 3.5;
        var height = 11;
        var width = 12;

        ctx.lineCap = 'round';

        ctx.beginPath();
        ctx.strokeStyle = c;
        ctx.lineWidth = linewidth;

        if (bits & SEGMENT.A) {
            ctx.moveTo(4 + x, y);
            ctx.lineTo(4 + x + width, y);
        }

        if (bits & SEGMENT.B) {
            ctx.moveTo(4 + x + width, y);
            ctx.lineTo(2 + x + width, y + height * 1);
        }

        if (bits & SEGMENT.C) {
            ctx.moveTo(2 + x + width, y + height * 1);
            ctx.lineTo(x + width, y + height * 2);
        }

        if (bits & SEGMENT.D) {
            ctx.moveTo(x, y + height * 2);
            ctx.lineTo(x + width, y + height * 2);
        }

        if (bits & SEGMENT.E) {
            ctx.moveTo(2 + x, y + height * 1);
            ctx.lineTo(x, y + height * 2);
        }

        if (bits & SEGMENT.F) {
            ctx.moveTo(4 + x, y);
            ctx.lineTo(2 + x, y + height * 1);
        }

        if (bits & SEGMENT.G) {
            ctx.moveTo(2 + x, y + height * 1);
            ctx.lineTo(2 + x + width, y + height * 1);
        }

        ctx.stroke();

        if (bits & SEGMENT.DP) {
            ctx.beginPath();
            ctx.lineWidth = 1;
            ctx.fillStyle = c;
            ctx.arc(x + width * 1.4, y + height * 2, 2, 0, 2 * Math.PI, false);
            ctx.fill();
        }
    };

    //
    //
    //
    Draw1MatrixRow = function(ctx, bits, xo, yo, c) {
        var x = 12 + xo,
            y = 7 + yo;

        ctx.strokeStyle = 'white';
        ctx.lineWidth = 1;
        ctx.fillStyle = c;
        var mask = 1;
        for (var i = 0; i < 8; i++) {
            ctx.beginPath();
            ctx.arc(x + i * MATRIXPITCH, y, 4, 0, 2 * Math.PI, false);
            if (bits & mask) ctx.fill()
            else ctx.stroke();
            mask *= 2;
        }
    };

    return Max7219; // Return the constructor

})();