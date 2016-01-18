<?php
    $tmp_path="/icpa/tmp";

    $sq=false;

    function open_sqlite(){
	global $sq,$tmp_path;
        if ($sq) return;
	$filename=$tmp_path."/pwr-".strftime("%Y-%j");
	$sq=new SQLite3($filename,SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE);
    }
    function close_sqlite(){
        global $sq;
        $sq->close();
    }


   function add_data($data){
        global $sq;
        if (!$sq) return;
	$cols="";
	foreach ($data as $k => $v) { $x[]=$k; $y[]=$sq->escapeString($v); }
	$x[]="time";
	$y[]=time();
	$cols=implode(",",$x);
    	$vals=implode(",",$y);
	$query="INSERT INTO edata (".$cols.") VALUES (".$vals.");";
	//print($query)B;
        if (!$sq->exec($query)){
	    if (strstr($sq->lastErrorMsg(),"no such table")){
		foreach ($data as $k => $v) { $z[]=$k." DOUBLE"; }
		$cols2=implode(",",$z);
		$query2="CREATE TABLE edata (".$cols2.", time LONG); CREATE INDEX edata_t ON edata(time);";
		//print($query2);
		$sq->exec($query2);
		$sq->exec($query);
	    }
	}
    }


?>
