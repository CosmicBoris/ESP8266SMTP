/* 
*  ESP8266SMTPHelper class helps send e-mails
*  using Arduino core for ESP8266 WiFi chip
*  by Boris Shobat
*  May 11 2017
*/

#ifndef _ESP8266SMTPHelper_H
#define _ESP8266SMTPHelper_H

#include <WiFiClientSecure.h>

//#define GS_SERIAL_LOG_LEVEL_1      // Print to Serial only server responce
//#define GS_SERIAL_LOG_LEVEL_2      // Print to Serial client commands and server responce

const char HTML_HEAD[] PROGMEM  = "Mime-Version: 1.0\r\nContent-Type: text/html; charset=\"UTF-8\";\r\nContent-Transfer-Encoding: 7bit;\r\n\r\n<!DOCTYPE html><html lang=\"en\">";
const char HTML_END[] PROGMEM   = "</html>\r\n.";
const char SMTP_HELO[] PROGMEM = "HELO friend";
const char SMTP_AUTH[] PROGMEM = "AUTH LOGIN";
const char SMTP_FROM[] PROGMEM = "MAIL FROM:<$>";
const char SMTP_RCPT[] PROGMEM = "RCPT TO:<$>";
const char SMTP_SUB[] PROGMEM  = "Subject: ";

class ESP8266SMTPHelper
{
	private:
	uint16_t _smtp_port = 465;
	const char *_error = nullptr;
	char
		*_smtp_server     = nullptr,
		*_emailAddress    = nullptr,
		*_base64_login    = nullptr,
		*_base64_password = nullptr,
		*_from            = nullptr,
		*_subject         = nullptr;
	String _serverResponce;
	bool AwaitSMTPResponse(WiFiClientSecure &client, const String &resp = "", uint16_t timeOut = 10000);

	public:
	ESP8266SMTPHelper() = default;
	ESP8266SMTPHelper(const char*, const char*);
	~ESP8266SMTPHelper();
	ESP8266SMTPHelper
		&setPort(uint16_t),
		&setServer(const char*),
		&Subject(const char*),
		&setFrom(const char*),
		&setEmail(const char*),
		&setPassword(const char*),
		&setForGmail();
	char* getBase64Email();
	char* getBase64Password();

	String getLastResponce();
	const char* getError();

	bool Send(const String &to, const String &message);
};

extern ESP8266SMTPHelper SMTP;
#endif
