<?php
include "chipmunk.php";

function  headline($msg) {
	echo "\n\n" . $msg ."\n"."==========================================\n";
}

$wss = new chipmunk();

$mval = "";
for($i = 0 ; $i < 5 ;$i++)
{
    $mval .= ":" .$i;
}

$data = array('somedata'=>'somevalue','someotherdata'=>'somevalue','test'=>$mval);



$response = $wss->set("tester1",array('test1'=>'value1','test2'=>'value5'),$data,$ensureSave=true);

$response = $wss->set("tester2",array('test1'=>'value2','test2'=>'value6'),$data,$ensureSave=true);

$response = $wss->set("tester2e",array('test1'=>'value1','test2'=>'value6'),$data,$ensureSave=true);

$response = $wss->set("tester3",array('test1'=>'value1','test2'=>'value5'),$data,$ensureSave=true);
$response = $wss->set("tester4",array('test1'=>'value1','test2'=>'value5'),$data,$ensureSave=true);


$response = $wss->get('tester1');
for($i = 0 ; $i < 10000 ; $i++) {
    $response = $wss->set("tester$i",array('test1'=>'value1','test2'=>'value5'),$data,$ensureSave=true);
    usleep(100*1000);
}
 
headline("OR with subset: should return tester3, tester4");
var_dump($wss->findOr(array(
								new searchParameter('test1','=','value1')
    					   ),
					  array('tester3','tester4')
					 )
		);

headline("OR with subset: should return tester4");
var_dump($wss->findOr(array(
								new searchParameter('test1','=','value1')
    					   ),
					  array('tester2','tester4')
					 )
		);

headline("OR without subset: should return tester1, tester3, tester4");
var_dump($wss->findOr(array(
								new searchParameter('test1','=','value1')
    					   )
					 )
		);

headline("AND without subset: should return tester1, tester3, tester4");
var_dump($wss->findAnd(array(
								new searchParameter('test1','=','value1')
    					   )
					 )
		);

headline("AND without subset: should return tester1, tester3, tester4");
var_dump($wss->findAnd(array(
								new searchParameter('test1','=','value1'),
								new searchParameter('test2','=','value5'),
    					   )
					 )
		);

headline("AND without subset: should return tester2e");
var_dump($wss->findAnd(array(
								new searchParameter('test1','=','value1'),
								new searchParameter('test2','=','value6'),
    					   )
					 )
		);

headline("AND without subset: should return tester1, tester3, tester4");
var_dump($wss->findAnd(array(
								new searchParameter('test1','=','value1'),
								new searchParameter('test2','!','value6'),
    					   )
					 )
		);

headline("AND with subset: should return tester1, tester4");
var_dump($wss->findAnd(array(
								new searchParameter('test1','=','value1'),
								new searchParameter('test2','!','value6'),
    					   )	,
					   array('tester1','tester4')
					 )
		);
