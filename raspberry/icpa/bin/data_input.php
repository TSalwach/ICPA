<?php
    //for timing calculations
    //file_put_contents("/tmp/emon-timing",microtime(true)."\n",FILE_APPEND);
    error_reporting(E_ALL);


    $file=file_get_contents("/icpa/tmp/data");
    $data=json_decode($file);
    if (!$data) exit();

    $last_kwh=@file_get_contents("/icpa/tmp/last_kwh");
    if (!$last_kwh) $last_kwh=0;
    if ($last_kwh > $data->kwh) $last_kwh=0;
    $kwh=($data->kwh - $last_kwh);
    $last_kwh=$data->kwh;
    file_put_contents("/icpa/tmp/last_kwh",$last_kwh);
    $data->kwh=$kwh;

    //tariff -  G12g 
    //0: 7.00  - 14.00, 16.00 - 22.00 oraz sobota 7.00 - 14.00
    //1: 14.00 - 16.00, 22.00 - 7.00 oraz od soboty od godziny 14.00 do poniedziaku do godz. 7.00

    $now=time();
    $now+=180; //meter offset

    $tariff=0;
    $tm=localtime($now,true);
    if ((strtotime("14:00")<$now)&&($now<strtotime("16:00"))) $tariff=1;
    if ((strtotime("22:00")<$now)&&($now<strtotime("23:59"))) $tariff=1;
    if ((strtotime("00:00")<$now)&&($now<strtotime("07:00"))) $tariff=1;
    if (($tm['tm_wday']==6) && (strtotime("14:00")<$now)) $tariff=1; //saturday
    if ($tm['tm_wday']==0) $tariff=1; //sunday

    $totals=explode(",",file_get_contents("/icpa/tmp/totals"));
    $totals[$tariff]=$totals[$tariff]+$data->kwh;
    file_put_contents("/icpa/tmp/totals",implode(",",$totals));


    $data->pwr=$data->real_pwr0+$data->real_pwr1+$data->real_pwr2;
    $data->tariff=$tariff;

    $data->tar0=$totals[0];
    $data->tar1=$totals[1];

    unset($data->volt0);
    unset($data->volt1);
    unset($data->volt2);
    unset($data->cur0);
    unset($data->cur1);
    unset($data->cur2);

    $now=time();
    require("data_sqlite.php");
    open_sqlite();
    add_data($data);
    close_sqlite();

    file_put_contents("/icpa/tmp/data_live",json_encode($data,JSON_NUMERIC_CHECK));

?>