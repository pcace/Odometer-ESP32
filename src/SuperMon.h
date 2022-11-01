const char PAGE_MAIN[] PROGMEM = R"=====(


<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
  <title>Tacho / Odometer</title>
  <style>
    html {
      padding: 0;
      height: 100%;
      width: 100%;
    }
    body,
    .outer {
      height: 100%;
      width: 100%;
    }
    .inner {
      height: 100%;
      width: 100%;
    }
    table {
      height: 100%;
      width: 100%;
    }
    div {
      padding: 0px 0px;
    }
    .btn {
      background-color: #000000;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 5vmin;
      margin: 4px 2px;
      cursor: pointer;
    }
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 2vmin;
      padding: 0px 0px 0px 0px;
      color: #000000;
    }
    .valueSmall {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 8vmin;
      padding: 0px 0px 0px 0px;
      color: #000000;
    }
    .valueBig {
      font-family: "Verdana", "Arial", sans-serif;
      vertical-align: top;
      font-weight: bold;
      font-size: 20vmin;
      padding: 0px 0px 0px 0px;
      margin: 0px 0px 0px 0px;
      color: #000000;
    }
  </style>
  <!-- 
    ids: 
    distA
    distB
    totalDist
    totalTime
    dailyTime
    dailyDist
    speed
   -->
  <body style="background-color: #efefef" onload="process()">
    <!-- div container fullscreen -->
    <div class="outer">
      <div class="inner">
        <table style="background-color: rgb(151, 151, 151)">
          <tr style="background-color: rgb(210, 210, 210)">
            <td style="width: 40%">
              <table>
                <tr>
                  <td>
                    <div class="valueSmall" id="time">time</div>
                    <div class="valueSmall" id="date">date</div>
                    <div class="valueSmall" id="position">position</div>
                  </td>
                </tr>
                <tr>
                  <td>
                    <table>
                      <tr>
                        <td><div class="category">dist A</div></td>
                        <td>
                          <div class="valueSmall" id="distA">88.88</div>
                        </td>
                        <td>
                          <button
                            type="button"
                            class="btn"
                            id="btn0"
                            onclick="ButtonResetDistA()"
                          >
                            Reset
                          </button>
                        </td>
                      </tr>
                      <tr>
                        <td><div class="category">dist B</div></td>
                        <td>
                          <div class="valueSmall" id="distB">88.88</div>
                        </td>
                        <td>
                          <button
                            type="button"
                            class="btn"
                            id="btn1"
                            onclick="ButtonResetDistB()"
                          >
                            Reset
                          </button>
                        </td>
                      </tr>
                      <tr>
                        <td><div class="category">totalDist</div></td>
                        <td>
                          <div class="valueSmall" id="totalDist">88.88</div>
                        </td>
                        <td></td>
                      </tr>
                      <tr>
                        <td><div class="category">totalTime</div></td>
                        <td>
                          <div class="valueSmall" id="totalTime">88.88</div>
                        </td>
                        <td></td>
                      </tr>
                    </table>
                  </td>
                </tr>
              </table>
            </td>
            <td>
              <table>
                <tr>
                  <td>
                    <div class="valueBig" id="speed">88.88</div>
                  </td>
                </tr>
                <tr>
                  <td><div class="category"></div></td>
                </tr>
                <tr>
                  <td>
                    <table>
                      <tr>
                        <td><div class="category">mvtime</div></td>
                        <td>
                          <div class="valueSmall" id="dailyTime">88.88</div>
                        </td>
                      </tr>
                      <tr>
                        <td><div class="category">dailyDist</div></td>
                        <td>
                          <div class="valueSmall" id="dailyDist">88.88</div>
                        </td>
                      </tr>
                    </table>
                  </td>
                </tr>
              </table>
            </td>
          </tr>
        </table>
      </div>
    </div>
  </body>
  <script type="text/javascript">
    // global variable visible to all java functions
    var xmlHttp = createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject() {
      if (window.XMLHttpRequest) {
        xmlHttp = new XMLHttpRequest();
      } else {
        xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonResetDistA() {
      var xhttp = new XMLHttpRequest();
      var message;
      // if you want to handle an immediate reply (like status from the ESP
      // handling of the button press use this code
      // since this button status from the ESP is in the main XML code
      // we don't need this
      // remember that if you want immediate processing feedbac you must send it
      // in the ESP handling function and here
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          message = this.responseText;
          // update some HTML data
        }
      }
      */

      xhttp.open("PUT", "BUTTON_0", false);
      xhttp.send();
    }

    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonResetDistB() {
      var xhttp = new XMLHttpRequest();
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("button1").innerHTML = this.responseText;
        }
      }
      */
      xhttp.open("PUT", "BUTTON_1", false);
      xhttp.send();
    }


    // function to handle the response from the ESP
    function response() {
      var message;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();


      xmlResponse = xmlHttp.responseXML;
if(xmlResponse){
      // console.log("xmlResponse", xmlResponse)

      // get host date and time
      document.getElementById("time").innerText = dt.toLocaleTimeString();
      document.getElementById("date").innerText = dt.toLocaleDateString();


      xmldoc = xmlResponse.getElementsByTagName("SPEED");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("speed").innerText = message;
 }
      xmldoc = xmlResponse.getElementsByTagName("DISTA");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("distA").innerText = message;
 }
      xmldoc = xmlResponse.getElementsByTagName("DISTB");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("distB").innerText = message;
 }
      xmldoc = xmlResponse.getElementsByTagName("TOTALDIST");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("totalDist").innerText = message;
 }
      xmldoc = xmlResponse.getElementsByTagName("TOTALTIME");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("totalTime").innerText = message;
 }
      xmldoc = xmlResponse.getElementsByTagName("DAILYTIME");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("dailyTime").innerText = message;
}
 
      xmldoc = xmlResponse.getElementsByTagName("DAILYDIST");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("dailyDist").innerText = message;
 }
      xmldoc = xmlResponse.getElementsByTagName("POSITION");
      if(xmldoc){
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("position").innerText = message;
 }}
    }

    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process() {
      if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
        xmlHttp.open("PUT", "xml", true);
        xmlHttp.onreadystatechange = response;
        xmlHttp.send(null);
      }
      // you may have to play with this value, big pages need more porcessing time, and hence
      // a longer timeout
      setTimeout("process()", 100);
    }
  </script>
</html>

)=====";
