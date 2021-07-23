const char MAIN_page[] PROGMEM = R"=====(
<html>
    <head>
        <title>National Infotech's Webserver for Stabilizer data</title>
    </head>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\>
    <style>
        h1   {color: navy;
              text-align:center;
            margin-left: auto;
              margin-right: auto;
             
        }
        h2   {text-align:center;}
        img {
          display: block;
          margin-left: auto;
          margin-right: auto;
        margin-top: auto;
          margin-bottom: auto;
          width:50;height:50;
        }
        button {
          background-color: #008CBA; /* Green */
          border: none;
          color: white;
          padding: 15px 32px;
          text-align: center;
          text-decoration: none;
          display: inline-block;
          font-size: 16px;
        }
        </style>
    <body>
        <h1>National Infotech<img src="logo.png" alt="logo" width="128" height="120" style="float:left" /></h1>
        <center>
            <b>A Way to Power Electronics and Embeddded System Solutions...</b><br>
            <button type="button" onclick="sendData(1)">LED ON</button>
			<button type="button" onclick="sendData(0)">LED OFF</button><BR>
			ADC Value is : <span id="ADCValue">0</span><br>
    		LED State is : <span id="LEDState">NA</span>
        </center>
        
    <script>
		
		function sendData(led) {
  			var xhttp = new XMLHttpRequest();
  			xhttp.onreadystatechange = function() {
   		if (this.readyState == 4 && this.status == 200) {
      			document.getElementById("LEDState").innerHTML =
      			this.responseText;
   				}
  			};
  			xhttp.open("GET", "setLED?LEDstate="+led, true);
 			 xhttp.send();
		}
		setInterval(function() {
  										// Call a function repetatively with 2 Second interval
  			getData();
			}, 2000); //2000mSeconds update rate
	
		function getData() {
  			var xhttp = new XMLHttpRequest();
 			 xhttp.onreadystatechange = function() {
   		if (this.readyState == 4 && this.status == 200) {
      		document.getElementById("ADCValue").innerHTML =
      		this.responseText;
    			}
  			};
  			xhttp.open("GET", "readADC", true);
  			xhttp.send();
		}
	</script>
    </body>
</html>
)=====";
