var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);

connection.onopen = function() {
    connection.send('Connect ' + new Date());
};

connection.onerror = function(error) {
    console.log('WebSocket Error ', error);
};

// Update screen info fields on data send from server/esp
connection.onmessage = function(e) {
    //console.log('Server: ', e.data);
    var myData = e.data.substr(1);
    switch (e.data[0]) {
        case 'f':
            document.getElementById('v_filename').innerHTML = myData;
            break;
        case 's':
            document.getElementById('v_size').innerHTML = myData;
            break;
        case 'p':
            document.getElementById('v_status').innerHTML = myData;
            break;
    };
}
