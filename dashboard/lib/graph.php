<?php
    session_start();
    if (!isset($_SESSION["linecount"])) {
        $_SESSION["linecount"] = 0;
    }

    // $file = fopen("/home/pi/AER_MCU/log.csv", "r");
    $file = fopen("../log.csv", "r");
    $lines=array();
    $linecount = 0;
    while (($line = fgets($file)) !== false) {
        $line = trim($line);
        $line = explode(",", $line);
        array_push($lines, $line);
        $linecount += 1;
    }
    fclose($file);

    if ($_SESSION["linecount"] == 0) {
        $lines = array_slice($lines, 1);
    }
    else {
        $lines = array_slice($lines, $_SESSION["linecount"]);
    }
    
    $_SESSION["linecount"] = $linecount;

    echo json_encode($lines);
?>