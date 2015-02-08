<?php

/**
* Chipmunk Metadata Search Parameter
*
* @package chipmunk
* @version 0.1.0
*
* @copyright Bartosz Pachołek
* @copyleft Bartosz pachołek
* @author Bartosz Pachołek
* @license http://opensource.org/licenses/MIT (The MIT License)
*
* Copyright (c) 2014, IDCT IdeaConnect Bartosz Pachołek (http://www.idct.pl/)
* Copyleft (c) 2014, IDCT IdeaConnect Bartosz Pachołek (http://www.idct.pl/)
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
* OR OTHER DEALINGS IN THE SOFTWARE.
*/

class searchParameter {

    /**
    * value expected (or not: depending on the condition) in the metadata for the given key.
    * WARNING: line breaks, pipes, hash symbols are forbidded
    *
    * @var string
    */
	protected $value;

    /**
    * condition: EQUALS (=) or NOT (!) for the comaprison in the metadata search
    *
    * @var string
    */
	protected $condition;

    /**
    * key (identifier) of the metadata entry to compare
    * WARNING: line breaks, pipes, hash symbols are forbidded
    *
    * @var string
    */
	protected $key;

    /**
    * Constructor of the search parameter.
    *
    * @param string Key (identifier) of the metadata entry to compare. WARNING: line breaks, pipes, hash symbols are forbidden.
    * @param string Condition (symbol) EQUALS (=) or NOT (!) for the comaprison in the metadata search
    * @param string Value expected (or not: depending on the condition) in the metadata for the given key. WARNING: line breaks, pipes, hash symbols are forbidden.
    */
	function __construct($key, $condition, $value) {
		$this->condition = $condition;
		$this->value = $value;
		$this->key = $key;
		return $this;
	}

    /**
    * Sets the key, identifier for the metadata search
    *
    * @param string Key (identifier) of the metadata entry to compare. WARNING: line breaks, pipes, hash symbols are forbidden.
    */
    public function setKey($key) {
        $this->key = $key;

        return $this;
    }

    /**
    * Sets the condition for the comparison
    *
    * @param string Condition (symbol) EQUALS (=) or NOT (!) for the comaprison in the metadata search
    */
    public function setCondition($condition) {
        $this->condition = $condition;

        return $this;
    }

    /**
    * Sets the value for the comparison
    *
    * @param string Value expected (or not: depending on the condition) in the metadata for the given key. WARNING: line breaks, pipes, hash symbols are forbidden.
    */
    public function setValue($value) {
        $this->key = $key;

        return $this;
    }

    /**
    * Returns the search parameter in a format ready for use within the connector
    *
    * @return string
    */
	public function __toString() {
		return $this->key . ":" . $this->condition . ":" . $this->value;
	}
}

/**
* Chipmunk Database Connector
*
* @package chipmunk
* @version 0.1.0
*
* @copyright Bartosz Pachołek
* @copyleft Bartosz pachołek
* @author Bartosz Pachołek
* @license http://opensource.org/licenses/MIT (The MIT License)
*
* Copyright (c) 2014, IDCT IdeaConnect Bartosz Pachołek (http://www.idct.pl/)
* Copyleft (c) 2014, IDCT IdeaConnect Bartosz Pachołek (http://www.idct.pl/)
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
* OR OTHER DEALINGS IN THE SOFTWARE.
*/

class chipmunk {

    protected $buf = 1024;
    /**
    * Socket used for DB connections
    *
    * @var resource
    */

    protected function sendRequest($msg)
    {
        $buf = $this->buf;
        $socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
        socket_connect($socket, "localhost",8906);
        $maxLength = strlen($msg);
        $maxBuf = ceil($maxLength / $buf);
        $position = 0;

        while(empty($msg)== false) {
            $msg = substr($msg,$position);
            if(empty($msg) ==false ) {
                $position += $buf;

                $len = ($buf > $maxLength) ? $maxLength : $buf;
                $s = socket_send($socket, $msg, $len, 0);
                $maxLength -= $position;
            }
        }
        return $socket;
    }


    protected function parseMetadata($metadata)
    {
        $first = true;
        $metadataParsed = "";
        foreach($metadata as $key => $value) {
                if($first === true) {
                        $first = false;
                } else {
                        $metadataParsed .= "#";
                }

                if($value instanceof searchParameter) {
                    $metadataParsed .= (string)$value;
                } else {
                    $metadataParsed .= $key . ":" . $value;
                }

        }

        return $metadataParsed;
    }

    protected function getResponse($socket)
    {
        $buf = $this->buf;
        $response = "";
    	while (true) {
            $mesg = "";
            $n = @socket_recv($socket, $mesg, $buf, MSG_PEEK);
		    if ($n < 1)
			    break;
		    $n = @socket_recv($socket, $mesg, $n, MSG_WAITALL);
		    $response .= $mesg;
		    if ($n < 1)
			    break;
	    }

        socket_close($socket);

        return $response;
    }

    /**
    * Gets the data from under the given identifier
    * TODO: host and port modification
    *
    * @param string identifier
    * @return mixed|NULL response. NULL on failure or no data
    */
	public function get($identifier)
	{
		$msg = "G" . $identifier;
		$socket = $this->sendRequest($msg);
        $response = $this->getResponse($socket);
		return ($response !== "NODATA") ? unserialize($response) : null;
	}

    /**
    * Performs an OR search for the given metadata parameters within the given subset of identifiers (or NULL for search within all db entries)
    *
    * @param array metadata
    * @param array|NULL subset of identifiers or NULL to search within all
    * @return array array of matched identifiers
    */
    public function findOr($metadata, $subset = null)
    {
        $metadataParsed = $this->parseMetadata($metadata);
        $msg = "O" . $metadataParsed;
		$msg .= "\n";
		if($subset !== null)
		{
			$msg .= implode('#',$subset);
		}
		$socket = $this->sendRequest($msg);
        $response = $this->getResponse($socket);
        return ($response !== "NODATA") ? explode('|',$response) : null;
    }

    /**
    * Performs an AND search for the given metadata parameters within the given subset of identifiers (or NULL for search within all db entries)
    *
    * @param array metadata
    * @param array|NULL subset of identifiers or NULL to search within all
    * @return array array of matched identifiers
    */
    public function findAnd($metadata, $subset= null)
    {
        $metadataParsed = $this->parseMetadata($metadata);

        $msg = "A" . $metadataParsed;
		$msg .= "\n";
		if($subset !== null)
		{
			$msg .= implode('#',$subset);
		}
        $socket = $this->sendRequest($msg);
        $response = $this->getResponse($socket);
        return ($response !== "NODATA") ? explode('|',$response) : null;
    }

    /**
    * Sets the data under the given identifier with set metadata
    * TODO: host and port modification
    *
    * @param string identifier
    * @param array array with metadata
    * @param mixed data to be saved (must be serializable)
    */
	public function set($identifier, $metadata, $data)
	{
		$metadataParsed = $this->parseMetadata($metadata);

		$msg = "S" . $identifier . "|" . $metadataParsed . "|" . serialize($data);
		$socket = $this->sendRequest($msg);
		socket_recvfrom($socket, $msg, $this->buf, 0, $from = '', $port = 0);
        socket_close($socket);
		return ($msg === "SAVED") ? true : false;
	}
}
