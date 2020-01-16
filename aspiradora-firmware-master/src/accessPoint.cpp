#include <Accesspoint.h>

extern String estado;

//Pines de los motores
int PinIN1 = 5;
int PinIN2 = 4;
int PinIN3 = 0;
int PinIN4 = 2;

//Pin del Relay (Aspiradora)
int PinRelay= 1;

const char ssid[] = "NodeMCU-ESP8266";
WiFiServer server(80); 

//Variable para request de los botones
String request = "";

//----------------------------------------------------------//
//---------HTML + CSS + JAVASCRIPT-------------------------//
//--------------------------------------------------------//

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0' />
    <meta charset='utf-8'>
    <style>
      body {
      font-size: 140%;
      }
      #main {
      display: table;
      margin: auto;
      padding: 0 10px 0 10px;
      }
      h2 {
      text-align: center;
      }
      body input[type=button]
      {
      padding: 10px 10px 10px 10px;
      width: 100%;
      background-color: #50FF50;
      font-size: 120%;
      }

    </style>
<script>
  function switchAdelante() 
  {
    var button_text = document.getElementById("Avanzar_button").value;
    if (button_text=="Avanzar")
    {
      document.getElementById("Avanzar_button").value = "Detener";
      ajaxLoad('GOON'); 
    }
    else
    {
      document.getElementById("Avanzar_button").value = "Avanzar";
      ajaxLoad('GOOFF');
    }
  }
  
  
  function switchRelay() 
  {
    var button_text = document.getElementById("Relay_button").value;
    if (button_text=="Aspirar")
    {
      document.getElementById("Relay_button").value = "No aspirar";
      ajaxLoad('RELAYON'); 
    }
    else
    {
      document.getElementById("Relay_button").value = "Aspirar";
      ajaxLoad('RELAYOFF');
    }
  }
  
  function switchAtras() 
  {
    var button_text = document.getElementById("Retroceder_button").value;
    if (button_text=="Retroceder")
    {
      document.getElementById("Retroceder_button").value = "Detenerse";
      ajaxLoad('BACKON'); 
    }
    else
    {
      document.getElementById("Retroceder_button").value = "Retroceder";
      ajaxLoad('BACKOFF');
    }
  }
  
   function switchDerecha() 
  {
    var button_text = document.getElementById("Derecha_button").value;
    if (button_text=="Girar derecha")
    {
      document.getElementById("Derecha_button").value = "Detenerse";
      ajaxLoad('DERON'); 
    }
    else
    {
      document.getElementById("Derecha_button").value = "Girar derecha";
      ajaxLoad('DEROFF');
    }
  }
  
   function switchIzquierda() 
  {
    var button_text = document.getElementById("Izquierda_button").value;
    if (button_text=="Girar izquierda")
    {
      document.getElementById("Izquierda_button").value = "Detenerse";
      ajaxLoad('IZQON'); 
    }
    else
    {
      document.getElementById("Izquierda_button").value = "Girar izquierda";
      ajaxLoad('IZQOFF');
    }
  }

  function switchAutomatico() 
  {
    var button_text = document.getElementById("Automatico_button").value;
    if (button_text=="AAutomatico")
    {
      document.getElementById("Automatico_button").value = "AManual";
      ajaxLoad('AutoON'); 
    }
    else
    {
      document.getElementById("Automatico_button").value = "AAutomatico";
      ajaxLoad('AutoOFF');
    }
  }
  
  var ajaxRequest = null;
  if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
  else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
  
  
  function ajaxLoad(ajaxURL)
  {
  if(!ajaxRequest){ alert("AJAX is not supported."); return; }
  
  ajaxRequest.open("GET",ajaxURL,true);
  ajaxRequest.onreadystatechange = function()
  {
   if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
   {
     var ajaxResult = ajaxRequest.responseText;
   }
  }
  ajaxRequest.send();
  }
  
</script>
<title>Walter</title>
</head>
<body>
  <div id='main'>
    <h2>TDPII - Robot Aspiradora</h2>
    )====="; 
    String html_2 = R"=====(  
    <input type="button" id = "Avanzar_button" onclick="switchAdelante()" value="Avanzar"/>
    <br><br>
    <input type="button" id = "Izquierda_button" onclick="switchIzquierda()" value="Girar izquierda" /> 
    <input type="button" id = "Derecha_button" onclick="switchDerecha()" value="Girar derecha" />  
    <br><br>
    <input type="button" id = "Retroceder_button" onclick="switchAtras()" value="Retroceder"/> 
    <br><br><br>
    <input type="button" id = "Relay_button" onclick="switchRelay()" value="Aspirar"/> 
    <br><br><br>
    )=====";
    String html_3 = R"=====( 
    <div>
      Log: <span id="CurrentState">)=====";String html_4 = R"=====(</span>
	  </div>
    <br><br><br>
    <input type="button" id = "Automatico_button" onclick="switchAutomatico()" value="AAutomatico"/> 
  </div>
</body>
</html>
)=====";

//----------------------------------------------------------//
//---------FIN DE HTML + CSS + JAVASCRIPT------------------//
//--------------------------------------------------------//

WiFiClient client;

void AccessPoint_Setup(){
    server.begin();                      
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid); 
    Serial.println("");
    Serial.print("Direccion IP Access Point - por defecto: ");      //Imprime la dirección IP
    Serial.println(WiFi.softAPIP()); 
    Serial.print("Direccion MAC Access Point: ");                   //Imprime la dirección MAC
    Serial.println(WiFi.softAPmacAddress());
}

Action_Type AccessPoint_CheckClientPetition(){
  Action_Type returnStatement = INIT;
  client = server.available();
  if (!client) {
    return returnStatement;
  }
  // Espera hasta que el cliente envía alguna petición
  Serial.println("nuevo cliente");
  while (!client.available()) {
    delay(1);
  }
  // Imprime el número de clientes conectados
  Serial.printf("Clientes conectados al Access Point: %dn", WiFi.softAPgetStationNum());

  // Leo la peticion del cliente
  request = client.readStringUntil('\r');
  Serial.print("request: ");
  Serial.println(request);

  //----Boton 1: Avanzar-----//
  if ( request.indexOf("GOON") > 0 ){
    returnStatement = AVANZAR;
    //Avanzar
  }
  else if  ( request.indexOf("GOOFF") > 0 ){ 
    returnStatement = DETENER;
  }
  else {
    boolean pinStatus = digitalRead(PinIN1);
    boolean pinStatus2 = digitalRead(PinIN3);
    if ((pinStatus==HIGH)& (pinStatus2==HIGH) ){
      html_2.replace("Avanzar","Detener");   
    }
    else { 
      html_2.replace("Detener","Avanzar");   
    }
  }
 
  //----Boton 2: Aspirar-----//
  if  ( request.indexOf("RELAYON") > 0 )  { 
    returnStatement = ASPIRAR;         
  }
  else if  ( request.indexOf("RELAYOFF") > 0 ) { 
    returnStatement = NOASPIRAR;  
  }
  else {
    boolean pinStatus3 = digitalRead(PinRelay);
    if (pinStatus3==HIGH) {
      html_2.replace("Aspirar","No aspirar");   
    }
    else { 
      html_2.replace("No aspirar","Aspirar");   
    }
  }

  //------Boton 3: Retroceder-----//
  if   ( request.indexOf("BACKON") > 0 )  { 
    returnStatement = RETROCEDER;
  }
  else if  ( request.indexOf("BACKOFF") > 0 ) { 
    returnStatement = DETENER;
  }
  else {
    boolean pinStatus4 = digitalRead(PinIN2);
    boolean pinStatus5 = digitalRead(PinIN4);
    if ((pinStatus4==HIGH)&(pinStatus5==HIGH) ){
      html_2.replace("Retroceder","Detenerse");   
    }
    else { 
      html_2.replace("Detenerse","Retroceder");   
    } 
  }

  //-------Boton 4: Girar a la Derecha------//
  if   ( request.indexOf("DERON") > 0 )  { 
    returnStatement = GIRARDER;    
  }
  else if  ( request.indexOf("DEROFF") > 0 ) { 
    returnStatement = DETENER;
  }
  else {
    boolean pinStatus6 = digitalRead(PinIN2);
    boolean pinStatus7 = digitalRead(PinIN3);
    if ((pinStatus6==HIGH)&(pinStatus7==HIGH) ){
      html_2.replace("Girar derecha","Detenerse");   
    }
    else { 
      html_2.replace("Detenerse","Girar derecha");   
    }
  }

  //-----Boton 5: Girar a la izquierda-----//
  if   ( request.indexOf("IZQON") > 0 )  { 
    returnStatement = GIRARIZQ;
  }
  else if  ( request.indexOf("IZQOFF") > 0 ) { 
    returnStatement = DETENER;
  }
  else{
    boolean pinStatus8 = digitalRead(PinIN1);
    boolean pinStatus9 = digitalRead(PinIN4);
    if ((pinStatus8==HIGH)&(pinStatus9==HIGH) ){
      html_2.replace("Girar izquierda","Detenerse");   
    }
    else { 
      html_2.replace("Detenerse","Girar izquierda");   
    }
  }

  //-----Boton 6: cambiar de modo-----//
  if   ( request.indexOf("AutoON") > 0 )  { 
    runMode = 1;
  }
  else if  ( request.indexOf("AutoOFF") > 0 ) { 
    runMode = 0;
    returnStatement = INIT;
  }
  
  return returnStatement;
}


void AccessPoint_Flush(){
  client.flush();
  client.print( header );
  client.print( html_1 );   
  client.print( html_2 );
  String html_aux = html_3 + estado + html_4; 
  client.print( html_aux ); 
  delay(5);
}
