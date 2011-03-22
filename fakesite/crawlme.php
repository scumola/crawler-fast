<?php
	$depth=$_GET['depth'];
	$numurls=$_GET['numurls'];
	$code =$_GET['code'];

	if (!isset($numurls)) {
		$numurls=100;
	}
	if (!isset($depth)) {
		$depth=2;
	}

	function genRandomString() {
		global $depth;
		$length = $depth;
		$characters = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
		$string = '';
		for ($p = 0; $p < $length; $p++) {
			$string .= $characters[mt_rand(0, strlen($characters)-1)];
		}
		return $string;
	}

	$a = 0;
	while ($a < $numurls) {
		$newcode = genRandomString();
		print ("<a href='crawlme.php?numurls=$numurls&depth=$depth&code=$newcode'>$newcode</a> ");
		$a++;
	}

?>
