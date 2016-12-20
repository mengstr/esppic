
var configItems = {
    "1": {
        desc: "Always 1",
        values: {
            "1": "Unused",
            "0": "ERROR"
        }
    },

    "FCMEN": {
        desc: "Fail-Safe Clock Monitor Enable",
        values: {
            "1": "Fail-Safe Clock Monitor and internal/external switchover are both enabled.",
            "0": "Fail-Safe Clock Monitor is disabled"
        }
    },

    "IESO": {
        desc: "Internal External Switchover",
        values: {
            "1": "Internal/External Switchover mode is enabled",
            "0": "Internal/External Switchover mode is disabled"
        }
    },

    "/CLKOUTEN": {
        desc: "Clock Out Enable",
        values: {
            "1": "CLKOUT function is disabled. I/O function on the CLKOUT pin.",
            "0": "CLKOUT function is enabled on the CLKOUT pin"
        }
    },

    "BOREN": {
        desc: "Brown-out Reset Enable",
        values: {
            "11": "BOR enabled",
            "10": "BOR enabled during operation and disabled in Sleep",
            "01": "BOR controlled by SBOREN bit of the BORCON register",
            "00": "BOR disabled"
        }
    },

    "CP": {
        desc: "Code Protection",
        values: {
            "1": "Program memory code protection is disabled",
            "0": "Program memory code protection is enabled"
        }
    },

    "MCLRE": {
        desc: "MCLR/VPP Pin Function Select",
        values: {
            "1": "MCLR/VPP pin function is MCLR; Weak pull-up enabled",
            "0": "MCLR/VPP pin function is digital input; MCLR internally disabled; Weak pull-up under control ofWPUA3 bit"
        }
    },

    "PWRTE": {
        desc: "Power-up Timer Enable",
        values: {
            "1": "PWRT disabled",
            "0": "PWRT enabled"
        }
    },

    "WDTE": {
        desc: "Watchdog Timer Enable",
        values: {
            "11": "WDT enabled",
            "10": "WDT enabled while running and disabled in Sleep",
            "01": "WDT controlled by the SWDTEN bit in the WDTCON register",
            "00": "WDT disabled"
        }
    },

    "FOSC": {
        desc: "Oscillator Selection",
        values: {
            "111": "ECH: External Clock, High-Power mode (4-20 MHz): device clock supplied to CLKIN pin",
            "110": "ECM: External Clock, Medium-Power mode (0.5-4 MHz): device clock supplied to CLKIN pin",
            "101": "ECL: External Clock, Low-Power mode (0-0.5 MHz): device clock supplied to CLKIN pin",
            "100": "INTOSC oscillator: I/O function on CLKIN pin",
            "011": "EXTRC oscillator: External RC circuit connected to CLKIN pin",
            "010": "HS oscillator: High-speed crystal/resonator connected between OSC1 and OSC2 pins",
            "001": "XT oscillator: Crystal/resonator connected between OSC1 and OSC2 pins",
            "000": "LP oscillator: Low-power crystal connected between OSC1 and OSC2 pins"
        }
    },

    "LVP": {
        desc: "Low-Voltage Programming Enable",
        values: {
            "1": "Low-voltage programming enabled",
            "0": "High-voltage on MCLR must be used for programming"
        }
    },

    "/DEBUG": {
        desc: "In-Circuit Debugger Mode",
        values: {
            "1": "In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins",
            "0": "In-Circuit Debugger enabled, ICSPCLK and ICSPDAT are dedicated to the debugger"
        }
    },

    "/LPBOR": {
        desc: "Low-Power BOR Enable",
        values: {
            "1": "Low-Power Brown-out Reset is disabled",
            "0": "Low-Power Brown-out Reset is enabled"
        }
    },

    "BORV": {
        desc: "Brown-out Reset Voltage Selection",
        values: {
            "1": "Brown-out Reset voltage (V BOR ), low trip point selected",
            "0": "Brown-out Reset voltage (V BOR ), high trip point selected"
        }
    },

    "STVREN": {
        desc: "Stack Overflow/Underflow Reset Enable",
        values: {
            "1": "Stack Overflow or Underflow will cause a Reset",
            "0": "Stack Overflow or Underflow will not cause a Reset"
        }
    },

    "PLLEN": {
        desc: "PLL Enable",
        values: {
            "1": "4xPLL enabled",
            "0": "4xPLL disabled"
        }
    },

    "ZCDDIS": {
        desc: "ZCD Disable",
        values: {
            "1": "ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON",
            "0": "ZCD always enabled"
        }
    },

    "PPS1WAY": {
        desc: "PPSLOCK Bit One-Way Set Enable bit",
        values: {
            "1": "The PPSLOCK bit can only be set once after an unlocking sequence is executed; once PPSLOCK is set, all future changes to PPS registers are prevented ",
            "0": "The PPSLOCK bit can beset and cleared as needed(provided an unlocking sequence is executed)"
        }
    },

    "WRT": {
        desc: "Flash Memory Self-Write Protection",
        values: {
            "11": "Write protection off",
            "10": "0000h to 1FFh write protected, 0200h to 1FFFh may be modified by PMCON control",
            "01": "0000h to 0FFFh write protected, 1000h to 1FFFh may be modified by PMCON control",
            "00": "0000h to 1FFFh write protected, no addresses may be modified by PMCON control"
        }
    },
}


// no=config word number, bs=bit start, be=bit end, na=bit name 
var configSpec = [
    { no: 1, bs: 13, be: 13, na: "FCMEN" },
    { no: 1, bs: 12, be: 12, na: "IESO" },
    { no: 1, bs: 11, be: 11, na: "/CLKOUTEN" },
    { no: 1, bs: 10, be: 9, na: "BOREN" },
    { no: 1, bs: 8, be: 8, na: "1" },
    { no: 1, bs: 7, be: 7, na: "CP" },
    { no: 1, bs: 6, be: 6, na: "MCLRE" },
    { no: 1, bs: 5, be: 5, na: "PWRTE" },
    { no: 1, bs: 4, be: 3, na: "WDTE" },
    { no: 1, bs: 2, be: 0, na: "FOSC" },
    { no: 2, bs: 13, be: 13, na: "LVP" },
    { no: 2, bs: 12, be: 12, na: "/DEBUG" },
    { no: 2, bs: 11, be: 11, na: "/LPBOR" },
    { no: 2, bs: 10, be: 10, na: "BORV"  },
    { no: 2, bs: 9, be: 9, na: "STVREN" },
    { no: 2, bs: 8, be: 8, na: "PLLEN" },
    { no: 2, bs: 7, be: 7, na: "ZCDDIS" },
    { no: 2, bs: 6, be: 6, na: "1" },
    { no: 2, bs: 5, be: 5, na: "1" },
    { no: 2, bs: 4, be: 4, na: "1" },
    { no: 2, bs: 3, be: 3, na: "1" },
    { no: 2, bs: 2, be: 2, na: "PPS1WAY" },
    { no: 2, bs: 1, be: 0, na: "WRT" },
];  


var config = [0xC9C4, 0xfFff];
UpdateConfigArea(config);

function ToBinary14(data) {
    var tmp = "";
    tmp += (data & 0x2000 ? '1' : '0');
    tmp += (data & 0x1000 ? '1' : '0');
    tmp += (data & 0x800 ? '1' : '0');
    tmp += (data & 0x400 ? '1' : '0');
    tmp += (data & 0x200 ? '1' : '0');
    tmp += (data & 0x100 ? '1' : '0');
    tmp += (data & 0x80 ? '1' : '0');
    tmp += (data & 0x40 ? '1' : '0');
    tmp += (data & 0x20 ? '1' : '0');
    tmp += (data & 0x10 ? '1' : '0');
    tmp += (data & 0x8 ? '1' : '0');
    tmp += (data & 0x4 ? '1' : '0');
    tmp += (data & 0x2 ? '1' : '0');
    tmp += (data & 0x1 ? '1' : '0');
    return tmp;
}

function ToHex4(i) {
    return (i+0x10000).toString(16).substr(-4).toUpperCase();
}

function UpdateConfigArea(confs) {
    var myconf = [ToBinary14(confs[0]), ToBinary14(confs[1])];
    // Update headings
    for (var i=1; i<3; i++) {
        var e=document.getElementById('config'+i+'hex');
        e.innerHTML=e.innerHTML.substr(0,e.innerHTML.length-4)+ToHex4(confs[i-1]);
        var s="";
        for (var j=0; j<14; j++) {
            s+=myconf[i-1].charAt(j)+' ';
        }
        document.getElementById('config'+i+'bits').innerHTML = s;
    }
    // Update rows
    for (var i = 0; i < configSpec.length; i++) {
        var a = configSpec[i];
        for (var j = a.be; j <= a.bs; j++) {
            if (j == a.be) {
                var myval = "";
                for (var k = a.bs; k >= a.be; k--) {
                    myval += myconf[a.no - 1].charAt(13 - k);
                }
                document.getElementById('cfg' + a.no + '_' + j).innerHTML = "(" + myval + ") " + a.na + ' : ' + configItems[a.na].values[myval];
                document.getElementById('cfg' + a.no + '_' + j).title = a.na + ' - ' + configItems[a.na].desc + '\x0A' + configItems[a.na].values[myval];
                document.getElementById('cfl' + a.no + '_' + j).innerHTML = '––'
            } else {
                document.getElementById('cfg' + a.no + '_' + j).innerHTML = '';
                document.getElementById('cfl' + a.no + '_' + j).innerHTML = '&nbsp&nbsp;'
            }
        }
    }
}
