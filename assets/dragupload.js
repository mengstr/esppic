    //----------- START OF DRAG AND DROP FROM http://html5demos.com/dnd-upload ---------

var dropzone = document.getElementById('dropzone'),
    fileupload = document.getElementById('upload');

function readfiles(files) {
    // debugger;
    var formData = new FormData();
    for (var i = 0; i < files.length; i++) {
        formData.append('file', files[i]);
    }

    // now post a new XHR request
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/upload');

    xhr.onload = function() {};

    xhr.upload.onprogress = function(event) {
        if (event.lengthComputable) {
            var complete = (event.loaded / event.total * 100 | 0);
            document.getElementById('v_progress').innerHTML = 'Uploading - ' + complete + '%';
        }
    }

    xhr.send(formData);
}

dropzone.ondragover = function() {
    this.className = 'hover';
    return false;
};

dropzone.ondragend = function() {
    this.className = '';
    return false;
};

dropzone.ondrop = function(e) {
    this.className = '';
    e.preventDefault();
    readfiles(e.dataTransfer.files);
}
