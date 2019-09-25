#include <EtherCard.h>
#include <Ultrasonic.h>

#define pino_trigger 4
#define pino_echo 5
#define pinoLedA 3
#define pinoLedB 2
#define STATIC 1 

#if STATIC
// Endereço IP estático a ser definido a interface Ethernet
static byte myip[] = { 192,168,0,20};
// Endereço IP do Gateway da sua rede
static byte gwip[] = { 192,168,0,1 };
#endif

// MAC Address da interface ethernet - deve ser único na sua rede local
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500];

static BufferFiller bfill;  // used as cursor while filling the buffer

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

void setup()
{
  
  Serial.begin(9600);   // monitor serial 9600 Bps
 
  pinMode(pinoLedA, OUTPUT); 
  pinMode(pinoLedB, OUTPUT);
  digitalWrite(pinoLedA, LOW);
  digitalWrite(pinoLedB, LOW);
  
  Serial.println("\n[webserver]");

   if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
      Serial.println( "Failed to access Ethernet controller");
   #if STATIC
      ether.staticSetup(myip, gwip);
   #else
      if (!ether.dhcpSetup())
         Serial.println("DHCP failed");
   #endif

   ether.printIp("IP: ", ether.myip);
   ether.printIp("GW: ", ether.gwip);
   ether.printIp("DNS: ", ether.dnsip);

}
 
static word getDistancia(char distancia[] ) {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
      "HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Pragma: no-cache\r\n"
      "\r\n" 
      "$S"
       ),
      distancia);
  return bfill.position();
}
 
void loop()
{

  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);

  if(cmMsec > 10 ){
      digitalWrite(pinoLedA, HIGH);
      digitalWrite(pinoLedB, LOW);
  }else{
    digitalWrite(pinoLedA, LOW);
      digitalWrite(pinoLedB, HIGH);
  }

  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);

 float valAnt = 0;

  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);

  if(cmMsec > valAnt)
  valAnt = cmMsec;

  char distancia[50];
  String stringOne2 =  String(valAnt) ;
  stringOne2.toCharArray(distancia, 50);

  if(strstr((char *)Ethernet::buffer + pos, "GET /?distancia=on") != 0) {
     ether.httpServerReply(getDistancia(distancia));
  }
}
