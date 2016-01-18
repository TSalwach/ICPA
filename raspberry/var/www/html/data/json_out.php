<?php

ob_start("ob_gzhandler");


if(array_key_exists('callback', $_GET)){

//    header('Content-Type: text/javascript; charset=utf8');
    header('Content-Type: application/json; charset=utf8');

    header('Access-Control-Allow-Origin: *');
    header('Access-Control-Max-Age: 0');
    header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');

    $callback = $_GET['callback'];
    echo $callback.'('.$data.');';

}else{
    // normal JSON string
    header('Content-Type: application/json; charset=utf8');

    echo $data;
}

?>
