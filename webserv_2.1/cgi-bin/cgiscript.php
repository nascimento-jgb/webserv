#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html; charset=UTF-8");
?>

<!DOCTYPE html>
<html>
<head>
    <title>Script CGI in PHP</title>
</head>
<body>
    <h1>¡Hello from a CGI script in php!</h1>
    <p>This is an example comming from CGI written in php</p>
    <p>But, Tell me, What time is it?: <?php echo date("Y-m-d H:i:s"); ?></p>
</body>
</html>