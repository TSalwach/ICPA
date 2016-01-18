<?php

    require("sqlite.php");
    open_sqlite();

    $p=array();
    $res = $sq->query("SELECT time*1000, pwr FROM edata WHERE time > ".(time()-300));
    while ($r = $res->fetchArray(SQLITE3_NUM)) $p[]=$r;
    $res->finalize();


    close_sqlite();

    $data=json_encode($p,JSON_NUMERIC_CHECK);


    
    require("json_out.php");
?>