"use strict";

// http://lisperator.net/uglifyjs/
// https://www.sanwebe.com/2014/02/css3-buttons-examples-with-effects-animations/comment-page-2
// http://www.crockford.com/javascript/private.html



// Automatically attach the css styles required
var style = document.createElement('style');
style.innerHTML =
    `.part {
background-color: #333;
border: 1px solid rgba(255, 255, 255, 0.2);
border-radius: 5px;
padding: 0;
margin: 0;
}

legend {
font-size: 60%;
}`;

document.head.appendChild(style);

var parts = [];

//
// Creates a "part" consisting of a labelled fieldset
// with a canvas inside that will hold the actual component
//
// The function returns the canvas named "part"<partNo>
//
function CreatePart(partNo, label, x, y, xsize, ysize) {
    const XGRID = 50;
    const YGRID = 50;

    var part = document.createElement('fieldset');
    part.setAttribute('class', 'part');
    part.style.position = 'absolute';
    part.style.top = y * YGRID + 'px';
    part.style.left = x * XGRID + 'px';
    part.style.width = (xsize * XGRID - 8) + 'px';
    part.style.height = (ysize * YGRID - 5) + 'px';
    //
    var legend = document.createElement('legend');
    legend.innerHTML = label;
    part.appendChild(legend);
    //
    var canvas = document.createElement('canvas');
    canvas.setAttribute('id', 'part' + partNo);
    canvas.setAttribute('width', (xsize * XGRID - 8));
    canvas.setAttribute('height', (ysize * YGRID - 5));
    part.appendChild(canvas);
    //
    document.body.appendChild(part);
    return canvas;
}

//
// Outputs data to all SPI-enabled devices
// This is currently specially made for Max7219-style
// daisy-chained devices with a strobe/latch input
// that here is triggered by calling this function
// without (or undefined) data
//
function SendToSpiChain(parts, data) {
    for (var i = 0; i < parts.length; i++) {
        if (parts[i].isSPI) {
            if (data === undefined) {
                parts[i].latch();
            } else {
                data = parts[i].spi(data);
            }
        }
    }
}

//
//
//
function RemoveAllParts() {
    var part = document.getElementsByClassName('part');
    while (part[0]) {
        part[0].parentNode.removeChild(part[0]);
    }
}