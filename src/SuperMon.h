/*


  OK, ya ready for some fun HTML + CSS styling + javascript all in and undebuggable environment

  one trick I've learned to how to debug HTML and CSS code.

  get all your HTML code (from html to /html) and past it into this test site
  muck with the HTML and CSS code until it's what you want
  https://www.w3schools.com/html/tryit.asp?filename=tryhtml_intro

  No clue how to debug javascrip other that write, compile, upload, refresh, guess, repeat

  I'm using class designators to set styles and id's for data updating
  for example:
  the CSS class .tabledata defines with the cell will look like
  <td><div class="tabledata" id = "switch"></div></td>

  the XML code will update the data where id = "switch"
  java script then uses getElementById
  document.getElementById("switch").innerHTML="Switch is OFF";


  .. now you can have the class define the look AND the class update the content, but you will then need
  a class for every data field that must be updated, here's what that will look like
  <td><div class="switch"></div></td>

  the XML code will update the data where class = "switch"
  java script then uses getElementsByClassName
  document.getElementsByClassName("switch")[0].style.color=text_color;


  the main general sections of a web page are the following and used here

  <html>
    <style>
    // dump CSS style stuff in here
    </style>
    <body>
      <header>
      // put header code for cute banners here
      </header>
      <main>
      // the buld of your web page contents
      </main>
      <footer>
      // put cute footer (c) 2021 xyz inc type thing
      </footer>
    </body>
    <script>
    // you java code between these tags
    </script>
  </html>


*/

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
  <title>Tacho / Odometer</title>

  <style>
    body,
    html {
      height: 100%;
    }

    table {

      width: 100%;
      border-spacing: 0px;
    }

    /* tr {
      border: 0px solid white;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 10;
    } 
        th {
      height: 20px;
      padding: 3px 15px;
      background-color: #343a40;
      color: #ffffff !important;
    }
    */
    td {
      height: 20px;
      padding: 3px 15px;
    }
    .btn {
      background-color: #444444;
      border: none;
      
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      cursor: pointer;
    }
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 28px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .valueSmall {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 40px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .valueBig {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 100px;
      line-height: 150px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }

  </style>

  <!-- <style>






    .fanrpmslider {
      width: 30%;
      height: 55px;
      outline: none;
      height: 25px;
    }
    .bodytext {
      font-family: "Verdana", "Arial", sans-serif;
      /* font-size: clamp(1rem, -0.8750rem + 8.3333vw, 3.5rem); */

      text-align: left;
      font-weight: light;
      border-radius: 5px;
      display: inline;
    }
    .navbar {
      width: 100%;
      height: 50px;
      margin: 0;
      padding: 10px 0px;
      background-color: #fff;
      color: #000000;
      border-bottom: 5px solid #293578;
    }
    .fixed-top {
      position: fixed;
      top: 0;
      right: 0;
      left: 0;
      z-index: 1030;
    }
    .navtitle {
      float: left;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 50px;
      font-weight: bold;
      line-height: 50px;
      padding-left: 20px;
    }
    .navheading {
      position: fixed;
      left: 60%;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      font-weight: bold;
      line-height: 20px;
      padding-right: 20px;
    }
    .navdata {
      justify-content: flex-end;
      position: fixed;
      left: 70%;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      font-weight: bold;
      line-height: 20px;
      padding-right: 20px;
    }
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 32px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .heading {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: normal;
      font-size: 28px;
      text-align: left;
    }

    .btn {
      background-color: #444444;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      cursor: pointer;
    }
    .foot {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      position: relative;
      height: 30px;
      text-align: center;
      color: #aaaaaa;
      line-height: 20px;
    }
    .container {
      max-width: 1800px;
      margin: 0 auto;
    }
    table tr:first-child th:first-child {
      border-top-left-radius: 5px;
    }
    table tr:first-child th:last-child {
      border-top-right-radius: 5px;
    }
    table tr:last-child td:first-child {
      border-bottom-left-radius: 5px;
    }
    table tr:last-child td:last-child {
      border-bottom-right-radius: 5px;
    }
  </style> -->


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
    <div style="height: 100%" class="fullscreen">
      <table>
        <tr>
          <td>
            <table border=5 bordercolor = red>
                <tr>
                  <td>
                <div class="valueSmall" id="time">    Uhrzeit </div>
                <div class="valueSmall" id="date">    Date</div>
                <div class="valueSmall" id="position">    Position</div>
            
           
                    

                  </td>
                </tr>
                <tr>
                  <td>
                    <table border=5 bordercolor = red>
                        <tr>
                            <td><div class="category"> Distanz A </div></td>
                            <td><div class="valueSmall" id="distA"> 88.88 km/h</div></td>
                            <td> <button type="button" class = "btn" id = "btn0" onclick="ButtonResetDistA()">Reset</button></td>
                        <tr>
                            <td><div class="category"> Distanz B </div></td>
                            <td><div class="valueSmall" id="distB"> 88.88 km/h</div></td>
                            <td> <button type="button" class = "btn" id = "btn1" onclick="ButtonResetDistB()">Reset</button></td>
                        <tr>    
                            <td><div class="category"> GesamtKM </div></td>
                            <td><div class="valueSmall" id="totalDist">88.88 km/h</div></td>
                            <td></td>
                        </td>
                        <tr>
                            <td><div class="category"> GesamtH </div></td>
                            <td><div class="valueSmall" id="totalTime">88.88 h</div></td>
                            <td></td> 
                    </table>
                  </td>
                </tr>
            </table>

          </td>
          <td>
            <table border=5 bordercolor = red>
              <tr>
                <td>
                   <div class="valueBig" id="speed">88.88</div>
                </td>
              </tr>
              <tr>
                <td>
                    kmh
                </td>
              </tr>
              <tr>
                <td>
                    <table border=5 bordercolor = red>
                        <tr>
                          <td><div class="category">Fahrzeit </div></td>
                          <td><div class="valueSmall"  id="dailyTime"> 88.88 h</div></td>
                        </tr>
                        <tr>
                          <td><div class="category" >TagesKM </div></td>
                          <td><div class="valueSmall" id="dailyDist"> 88.88 km/h </div></td>
                        </tr>
                      </table>

                  </td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
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
