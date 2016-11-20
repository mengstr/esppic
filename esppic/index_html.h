const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESPPIC - Upload file</title></head>
<style>
  body {
    background-color:#494949;
    color: #ffffff;
  }
  a {
    color: #8080ff;
  }
</style>
<script>
</script>
</head>

<body>
<img src="/logo.png">
<form method="POST" action="/upload" enctype="multipart/form-data">
Device:<select name="device">
<option value="16F1705" selected>PIC16F1705</option>
</select>
<br/>
Hex file:<input type="file" name="filename" accept=".hex" />
<br/>
<input type="submit" name="submit" value="Upload" />
</form>

<br/>
<a href="/readconfigs">Read config areas</a><br/>
<a href="/flash">Flash</a><br/>
Target control: <a href="/reset/yes">Reset</a>&nbsp; 
<a href="/reset/no">Run</a>&nbsp
<a href="/reset/pulse">Reboot</a><br/>

</body>
</html> 
)=====";

