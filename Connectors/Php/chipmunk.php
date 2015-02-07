<?php
class searchParameter {
	protected $value;
	protected $condition;
	protected $key;

	function __construct($key, $condition, $value) {
		$this->condition = $condition;
		$this->value = $value;
		$this->key = $key;
		return $this;
	}

	public function __toString() {
		return $this->key . ":" . $this->condition . ":" . $this->value;
	}
}
class chipmunk {
	
	function get($identifier)
	{
		$socket = socket_create(AF_INET,SOCK_DGRAM,SOL_UDP);
		$msg = "G" . $identifier;
		socket_sendto($socket,$msg,strlen($msg)+1,0,"localhost",8906);
		
		$from = '';
		$port = 0;
		socket_recvfrom($socket, $msg, 1024, 0, $from, $port);
		if($msg !== "NODATA")
		{
			return unserialize($msg);
		} else {
			return NULL;
		}
	}
        function findOr($metadata, $subset = null)
        {
                $socket = socket_create(AF_INET,SOCK_DGRAM,SOL_UDP);
                $first = true;
                $metadataParsed = "";
                foreach($metadata as $value) {
                        if($first === true) {
                                $first = false;
                        } else {
                                $metadataParsed .= "#";
                        }

                        $metadataParsed .= (string)$value;
                }

                $msg = "O" . $metadataParsed;
				
				$msg .= "\n";
				if($subset !== null)
				{
					$msg .= implode('#',$subset);
				}
				echo $msg;
				
                socket_sendto($socket,$msg,strlen($msg)+1,0,"localhost",8906);

                $from = '';
                $port = 0;
				
				
                socket_recvfrom($socket, $msg, 1024, 0, $from, $port);
                if($msg !== "NODATA")
                {
                        return explode('|',$msg);
                } else {
                        return NULL;
                }
        }
        function findAnd($metadata, $subset= null)
        {
                $socket = socket_create(AF_INET,SOCK_DGRAM,SOL_UDP);
                $first = true;
                $metadataParsed = "";
                foreach($metadata as $value) {
                        if($first === true) {
                                $first = false;
                        } else {
                                $metadataParsed .= "#";
                        }

                        $metadataParsed .= (string)$value;
                }

                $msg = "A" . $metadataParsed;
				$msg .= "\n";
				if($subset !== null)
				{
					$msg .= implode('#',$subset);
				}
                socket_sendto($socket,$msg,strlen($msg)+1,0,"localhost",8906);

                $from = '';
                $port = 0;
				
				
                socket_recvfrom($socket, $msg, 1024, 0, $from, $port);
                if($msg !== "NODATA")
                {
                        return explode('|',$msg);
                } else {
                        return NULL;
                }
        }		
	function set($identifier,$metadata,$data,$ensure=false)
	{
		$socket = socket_create(AF_INET,SOCK_DGRAM,SOL_UDP);
		$first = true;
		$metadataParsed = "";
		foreach($metadata as $key => $value) {
			if($first === true) {
				$first = false;			
			} else {
				$metadataParsed .= "#";
			}
			
			$metadataParsed .= $key . ":" . $value;
		} 

		$msg = "S" . $identifier . "|" . $metadataParsed . "|" . serialize($data);
		socket_sendto($socket,$msg,strlen($msg)+1,0,"localhost",8906);
		
		if($ensure === true)
		{
			$from = '';
			$port = 0;
			socket_recvfrom($socket, $msg, 1024, 0, $from, $port);			
			if($msg !=="SAVED")
				return true;
			else return false;
		}
		
		return true;
	}
}
