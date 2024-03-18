<?php
//mysql data
//error_reporting(0);
$data = array('server' => 'localhost',
			  'user'   => 'root',
			  'pass'   => 'root',
			  'db' 	   => 'ghostlulz');


// login info
$username = "admin";
$password = "admin";

//$conn = mysqli_connect($data['server'],$data['user'],$data['pass'],$data['db']); // or i can use this line instead of the below two lines

$conn = mysqli_connect($data['server'],$data['user'],$data['pass']);
mysqli_select_db($conn,$data['db']);
?>