<?php

 $tmp_path="/icpa/tmp";
 $storage_path="/icpa/data";

    $sq=false;

    function open_sqlite($day=0){
        global $sq,$tmp_path,$storage_path;
        if ($sq) return true;
	if ($day!=0) $filename=$storage_path."/pwr-".strftime("%Y-%j",time()+($day*86400));
	    else $filename=$tmp_path."/pwr-".strftime("%Y-%j");
	try {    	    
	    $sq=new SQLite3($filename,SQLITE3_OPEN_READONLY);
	} catch (Exception $e) {
	    //print($filename." open error\n");
	    return false;
	}
	return true;
    }

    function close_sqlite(){
	global $sq;
	$sq->close();
	$sq=false;
    }


    function get_stats($day=0){
        global $sq,$tmp_path,$storage_path;
	$filename=$storage_path."/stats/pwr-".strftime("%Y-%j",time()+($day*86400));
	$file=@file_get_contents($filename);
	if (!$file) return false;
	return @explode(",",$file);
    }

    function save_stats($day=0,$a){
        global $sq,$tmp_path,$storage_path;
	if ($day==0) return;
	$filename=$storage_path."/stats/pwr-".strftime("%Y-%j",time()+($day*86400));
	file_put_contents($filename,implode(",",$a));
    }


?>
