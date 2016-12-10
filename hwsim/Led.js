/*jshint -W097*/ // don't warn about "use strict"
/*jshint bitwise: false*/
'use strict';


// Automatically attach the css styles required
var style = document.createElement('style');
style.innerHTML = `/* Led */`;
document.head.appendChild(style);


//
//
//
//
window['Led'] = Led;
//
/** @constructor */
function Led(canvas) {
    this.canvas = canvas;
    this.type = 2;
    this.typeName = 'Led_1';
    this.isSPI = false;
    this.on = false;
    this.refresh();
}


//
//
//
Led.prototype['state'] = Led.prototype.state;
//
Led.prototype.state = function(data) {
    this.on = data;
    this.refresh();
};


//
//
//
Led.prototype['refresh'] = Led.prototype.refresh;
//
Led.prototype.refresh = function() {
    var context = this.canvas.getContext('2d');
    if (!context) return;
    context.clearRect(0, 0, this.canvas.width, this.canvas.height);

    this.draw1Led(context, 0, 0, this.on);
};


//
//
//
Led.prototype['draw1Led'] = Led.prototype.draw1Led;
//
Led.prototype.draw1Led = function(ctx, xo, yo, c) {
    var x = xo + 20,
        y = yo + 15;
    var size = 6;

    ctx.beginPath();
    ctx.strokeStyle = 'white';
    ctx.lineWidth = 1;
    if (c) ctx.fillStyle = 'green';
    ctx.arc(x, y, size, 0, 2 * Math.PI, false);
    if (c) ctx.fill();
    ctx.stroke();
};