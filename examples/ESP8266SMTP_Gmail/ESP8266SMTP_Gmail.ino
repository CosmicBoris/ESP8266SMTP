#include <ESP8266WiFi.h>
#include "ESP8266SMTP.h"

char* ssid = "";            			   // WIFI network name
char* password = "";        			   // WIFI network password
uint8_t connection_state = 0;              // Connected to WIFI or not
uint16_t reconnect_interval = 10000;       // If not connected wait time to try again

uint8_t WiFiConnect(char* ssID, char* nPassword)
{
	static uint16_t attempt = 0;
	Serial.print("Connecting to ");
	Serial.println(ssID);
	WiFi.begin(ssID, nPassword);

	uint8_t i = 0;
	while(WiFi.status() != WL_CONNECTED && i++ < 50) {
		delay(200);
		Serial.print(".");
	}
	++attempt;
	Serial.println("");
	if(i == 51) {
		Serial.print(F("Connection: TIMEOUT on attempt: "));
		Serial.println(attempt);
		if(attempt % 2 == 0)
			Serial.println(F("Check if access point available or SSID and Password are correct\r\n"));
		return false;
	}
	Serial.println(F("Connection: ESTABLISHED"));
	Serial.print(F("Got IP address: "));
	Serial.println(WiFi.localIP());
	return true;
}

void Awaits(uint16_t interval)
{
	uint32_t ts = millis();
	while(!connection_state){
		delay(50);
		if(!connection_state && millis() > (ts + interval)){
			connection_state = WiFiConnect(ssid, password);
			ts = millis();
		}
	}
}

void setup()
{
	Serial.begin(115200);
	delay(2000);
	
	connection_state = WiFiConnect(ssid, password);

	if(!connection_state) {  				// if not connected to WIFI
		Awaits(reconnect_interval);         // constantly trying to connect
	}

	uint32_t startTime = millis();
	
	SMTP.setEmail("YOUR_EMAIL_ADDRESS@gmail.com")
		.setPassword("YOUR_PASSWORD")
		.Subject("ESP8266SMTP Gmail test")
		.setFrom("ESP8266SMTP")
		.setForGmail();						// simply sets port to 465 and setServer("smtp.gmail.com");						
																   // message text from http://www.blindtextgenerator.com/lorem-ipsum
	if(SMTP.Send("Recipient Email, use comma for multi recipient", "The European languages are members of the same family. Their separate existence is a myth. For science, music, sport, etc, Europe uses the same vocabulary. The languages only differ in their grammar, their pronunciation and their most common words. Everyone realizes why a new common language would be desirable: one could refuse to pay expensive translators. To achieve this, it would be necessary to have uniform grammar, pronunciation and more common words. If several languages coalesce, the grammar of the resulting language is more simple and regular than that of the individual languages. The new common language will be more simple and regular than the existing European languages. It will be as simple as Occidental; in fact, it will be Occidental. To an English person, it will seem like simplified English, as a skeptical Cambridge friend of mine told me what Occidental is. The European languages are members of the same family. Their separate existence is a myth. For science, music, sport, etc, Europe uses the same vocabulary. The languages only differ in their grammar, their pronunciation and their most common words. Everyone realizes why a new common language would be desirable: one could refuse to pay expensive translators. To achieve this, it would be necessary to have uniform grammar, pronunciation and more common words. If several languages coalesce, the grammar of the resulting language is more simple and regular than that of the individual languages. The new common language will be more simple and regular than the existing European languages. It will be as simple as Occidental; in fact, it will be Occidental. To an English person, it will seem like simplified English, as a skeptical Cambridge friend of mine told me what Occidental is. The European languages are members of the same family. Their separate existence is a myth. For science, music, sport, etc, Europe uses the same vocabulary. The languages only differ in their grammar, their pronunciation and their most common words. Everyone realizes why a new common language would be desirable: one could refuse to pay expensive translators. To achieve this, it would be necessary to have uniform grammar, pronunciation and more common words. If several languages coalesce, the grammar of the resulting language is more simple and regular than that of the individual languages. The new common language will be more simple and regular than the existing European languages. It will be as simple as Occidental; in fact, it will be Occidental. To an English person, it will seem like simplified English, as a skeptical Cambridge friend of mine told me what Occidental is.The European languages are members of the same family. Their separate existence is a myth. For science, music, sport, etc, Europe uses the same vocabulary. The languages only differ in their grammar, their pronunciation and their most common words. Everyone realizes why a new common language would be desirable: one could refuse to pay expensive translators. To achieve this, it would be necessary to have uniform grammar, pronunciation and more common ")) {
		Serial.println(F("Message sent"));
	} else {
		Serial.print(F("Error sending message: "));
		Serial.println(SMTP.getError());
	} 
	
	Serial.println(millis() - startTime);
}

void loop()
{}
