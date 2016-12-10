"use strict";

// http://lisperator.net/uglifyjs/
// https://www.sanwebe.com/2014/02/css3-buttons-examples-with-effects-animations/comment-page-2
// http://www.crockford.com/javascript/private.html

var LED_1 = 2;


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
function Send1ByteToSpiChain(parts, data) {
    for (var i = 0; i < parts.length; i++) {
        if (parts[i].isSPI) {
            if (data === undefined || data.length == 0) {
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

// 
// 76543210 76543210 76543210 ... 
//-------------------------------------------------
// cciiiiii i_______ dddddddd ... 
//
window['HandleCmd00'] = HandleCmd00;
//
function HandleCmd00(a) {
    var id = ((a[0] & 0x3F) << 1) | (a[1] >> 7);
    var data = [];
    for (var i = 2; i < a.length; i++) {
        data.push(a[i]);
    }
    // console.log("HandleCmd00 id=" + id + " (dev=" + parts[id].typeName + ") data=" + data + " ( " + data.length + " bytes)")

    switch (parts[id].type) {
        case Max7219.TYPE.MATRIX:
        case Max7219.TYPE.SEGMENT:
            if (data.length > 0) {
                for (var i = 0; i < data.length; i++) {
                    Send1ByteToSpiChain(parts, data[i]);
                }
            } else {
                Send1ByteToSpiChain(parts)
            }
            break;
        case LED_1:
            parts[id].state(true);
            break;
    }
}

// 76543210 76543210 76543210 76543210 ... 76543210 
//-------------------------------------------------
// cciiiiii idddddxx xxxyyyyy llllllll ... llllllll
//
window['HandleCmd01'] = HandleCmd01;
//
function HandleCmd01(a) {
    var id = ((a[0] & 0x3F) << 1) | (a[1] >> 7);
    var dev = (a[1] >> 2) & 0x1F;
    var x = ((a[1] & 0x03) << 3) | (a[2] >> 5);
    var y = a[2] & 0x1F;
    var label = "";
    for (var i = 0; i < 8 && a[i + 3]; i++) {
        label += String.fromCharCode(a[i + 3]);
    }
    //console.log("id=" + id + " dev=" + dev + " x=" + x + " y=" + y + " label=[" + label + "]")
    switch (dev) {
        case Max7219.TYPE.SEGMENT:
            parts.push(new Max7219(CreatePart(id, label, x, y, 4, 1), Max7219.TYPE.SEGMENT));
            break;
        case Max7219.TYPE.MATRIX:
            parts.push(new Max7219(CreatePart(id, label, x, y, 2, 2), Max7219.TYPE.MATRIX));
            break;
        case LED_1:
            parts.push(new Led(CreatePart(id, label, x, y, 1, 1)));
            break;
    }
}