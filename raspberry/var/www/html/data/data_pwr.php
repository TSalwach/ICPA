<?php

    require("sqlite.php");
    open_sqlite();

    $p=array();
    $res = $sq->query('SELECT time*1000, pwr FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) $p[]=$r;
    $res->finalize();

    $x=array();
    $res = $sq->query('SELECT time*1000, real_pwr0 FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) $x[]=$r;
    $res->finalize();


    $y=array();
    $res = $sq->query('SELECT time*1000, real_pwr1 FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) $y[]=$r;
    $res->finalize();


    $z=array();
    $res = $sq->query('SELECT time*1000, real_pwr2 FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) $z[]=$r;
    $res->finalize();

    close_sqlite();

    $o=array($p,$x,$y,$z);
    $data=json_encode($o,JSON_NUMERIC_CHECK);


    
    require("json_out.php");
?>