#!/usr/bin/php
<?php
error_reporting(E_ERROR | E_PARSE);
include 'uCal.php';

preg_match('/([0-9]+)-([0-9]+)-([0-9]+)/', $argv[1], $m);
$d = new uCal;
$h = $d->g2u($m[3], $m[2], $m[1]);
printf("%d-%02d-%02d\n", $h['year'], $h['month'], $h['day']);
?>
