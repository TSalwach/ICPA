<?php

if(array_key_exists('live', $_GET)) $data=file_get_contents("/icpa/tmp/data_live");
    else $data=file_get_contents("/icpa/tmp/data");

    require("json_out.php");
?>