<?php

    require("sqlite.php");


    print("<table>\n");
    print("<tr><th>date</th><th>peak</th><th>off-peak</th><th>cost</th><th>alt. tar. diff.</th></tr>\n");

    for ($i=-30;$i<=0;$i++){
	$t=time()+($i*86400);
	$peak=false;
	if ($a=get_stats($i)){
	    $peak=$a[0];
	    $off_peak=$a[1];

	}else {

	    if (!open_sqlite($i)) continue;
	    $res = @$sq->query('SELECT MIN(tar0),MAX(tar0),MIN(tar1),MAX(tar1) FROM edata');
	    if (!($res instanceof Sqlite3Result)){
	        close_sqlite();
	        continue;
	    }

	    while ($r = $res->fetchArray(SQLITE3_NUM)){
		//print(($r[1]-$r[0])*(0.2003+0.4144+0.0115)."\n");
		//print(($r[3]-$r[2])*(0.0691+0.2023+0.0115)."\n");
		$peak=$r[1]-$r[0];
		$off_peak=$r[3]-$r[2];
	    }
	    $res->finalize();
	    close_sqlite();
	    save_stats($i,array($peak,$off_peak));
	}
	if ($peak){
	    $tm=localtime($t,true);
	    if (($tm['tm_wday']==0)||($tm['tm_wday']==6)) $class="weekend"; else $class="";


	    $price_peak=$peak*           (0.4105+0.2023+0.0);
	    $price_off_peak=$off_peak*   (0.2006+0.0698+0.0);
	    $alt_price=($peak+$off_peak)*(0.3132+0.1654+0.0);
	    if ($alt_price<($price_peak+$price_off_peak)) $alt="red"; else $alt="green";

	    print("<tr class='".$class."'><td>".strftime("%d.%m.%Y",$t)."</td><td>".number_format($peak,2)."</td><td>".number_format($off_peak,2)."</td><td>".number_format($price_peak+$price_off_peak,2)." zł</td><td style='color: ".$alt."'>".number_format($alt_price-$price_peak-$price_off_peak,2)." zł</td></tr>\n");
	}

    }
    print("</table>\n");


/*
    $x=array();
    $res = $sq->query('SELECT time*1000, real_pwr0 FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) $x[]=$r;
    $res->finalize();


    $y=array();
    $res = $sq->query('SELECT time*1000, real_pwr1 FROM edata WHERE time NOT NULL');
    while ($r = $res->fetchArray(SQLITE3_NUM)) $y[]=$r;
    $res->finalize();


    close_sqlite();

    $o=array($x,$y);
    $data=json_encode($o,JSON_NUMERIC_CHECK);


    
    require("json_out.php");
*/
?>