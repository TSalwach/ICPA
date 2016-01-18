<?php

    require("sqlite.php");
    open_sqlite();

    $o=array();
    $res = $sq->query('SELECT time*1000, freq FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) {
	$o[]=$r;
    }
    close_sqlite();
    $data=json_encode($o,JSON_NUMERIC_CHECK);
    
    require("json_out.php");
?>