<?php
    $file="../log.csv";
    $csv= file_get_contents($file);
    $array = array_map("str_getcsv", explode("\n", $csv));
    array_shift($array);
    $json = json_encode($array);
    echo $json;
?>