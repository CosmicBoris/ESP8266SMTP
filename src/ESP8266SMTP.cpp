#include "ESP8266SMTP.h"
#include <base64.h>

ESP8266SMTPHelper::ESP8266SMTPHelper(const char* login, const char* password) :
	_base64_login(strdup(login)),
	_base64_password(strdup(password)){}

ESP8266SMTPHelper::~ESP8266SMTPHelper()
{
	if(_subject) delete[] _subject;
	if(_from) delete[] _from;
	if(_base64_login) delete[] _base64_login;
	if(_base64_password) delete[] _base64_password;
	
}

ESP8266SMTPHelper& ESP8266SMTPHelper::Subject(const char* s)
{
	if(_subject) 
		delete[] _subject;

	_subject = strdup(s);
	return *this;
}

ESP8266SMTPHelper& ESP8266SMTPHelper::setPort(uint16_t port)
{
	_smtp_port = port;
	return *this;
}

ESP8266SMTPHelper& ESP8266SMTPHelper::setServer(const char *server)
{
	if(_smtp_server)
		delete[] _smtp_server;

	_smtp_server = strdup(server);
	return *this;
}

ESP8266SMTPHelper& ESP8266SMTPHelper::setFrom(const char* from)
{
	if(_from)
		delete[] _from;

	_from = strdup(from);
	return *this;
}

ESP8266SMTPHelper& ESP8266SMTPHelper::setEmail(const char* m)
{
	if(_emailAddress)
		delete[] _emailAddress;
	if(_base64_login)
		delete[] _base64_login;
	_emailAddress = strdup(m);
	_base64_login = strdup(base64::encode(m).c_str());

	return *this;
}

ESP8266SMTPHelper& ESP8266SMTPHelper::setPassword(const char* password)
{
	if(_base64_password) 
		delete[] _base64_password;

	_base64_password = strdup(base64::encode(password).c_str());
	return *this;
}

ESP8266SMTPHelper& ESP8266SMTPHelper::setForGmail()
{
	_smtp_port = 465;
	setServer("smtp.gmail.com");
	return *this;
}

char* ESP8266SMTPHelper::getBase64Email()
{
	return _base64_login;
}

char* ESP8266SMTPHelper::getBase64Password()
{
	return _base64_password;
}

const char* ESP8266SMTPHelper::getError()
{
	return _error;
}

String ESP8266SMTPHelper::getLastResponce()
{
	return _serverResponce;
}

bool ESP8266SMTPHelper::AwaitSMTPResponse(WiFiClientSecure &client, const String &resp, uint16_t timeOut)
{
	uint32_t timeStart = millis();
	while(!client.available()) {
		if(millis() > (timeStart + timeOut)) {
			_error = "SMTP Response TIMEOUT!";
			return false;
		}
	}
	_serverResponce = client.readStringUntil('\n');
#if defined(GS_SERIAL_LOG_LEVEL_1) || defined(GS_SERIAL_LOG_LEVEL_2) 
	Serial.println(_serverResponce);
#endif
	return !resp || _serverResponce.indexOf(resp) != -1;
}

bool ESP8266SMTPHelper::Send(const String &to, const String &message)
{
	if(!_smtp_server) {
		_error = "SMTP server not set.";
		return false;
	}

	WiFiClientSecure client;

#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.print(F("Connecting to: "));
	Serial.println(_smtp_server);
#endif
	if(!client.connect(_smtp_server, _smtp_port)) {
		_error = "Could not connect to mail server";
		return false;
	}
	if(!AwaitSMTPResponse(client, "220")) {
		_error = "Connection error";
		return false;
	}

#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println(FPSTR(SMTP_HELO));
#endif
	client.println(FPSTR(SMTP_HELO));
	if(!AwaitSMTPResponse(client, "250")) {
		_error = "identification error";
		return false;
	}

#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println(FPSTR(SMTP_AUTH));
#endif
	client.println(FPSTR(SMTP_AUTH));
	AwaitSMTPResponse(client);

#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println(F("BASE64_LOGIN:"));
#endif
	client.println(_base64_login);
	AwaitSMTPResponse(client);

#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println(F("BASE64_PASSWORD:"));
#endif
	client.println(_base64_password);
	if(!AwaitSMTPResponse(client, "235")) {
		_error = "SMTP AUTH error";
		return false;
	}

	String tmp = FPSTR(SMTP_FROM);
	tmp.replace("$", _emailAddress);
#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println(tmp);
#endif
	client.println(tmp);
	AwaitSMTPResponse(client);

	if(to.indexOf(',') == -1) {            // one recepient
		tmp = FPSTR(SMTP_RCPT);
		tmp.replace("$", to);
#if defined(GS_SERIAL_LOG_LEVEL_2)
		Serial.println(tmp);
#endif
		client.println(tmp);
		AwaitSMTPResponse(client);
	} else {
		char *toCopy = strdup(to.c_str()); // make copy becouse strtok modifyes original string
		char *sz_r = strtok(toCopy, ",");
		while(sz_r) {
			while(*sz_r == ' ')            // skip spaces after comma.
				++sz_r;
			tmp = FPSTR(SMTP_RCPT);
			tmp.replace("$", sz_r);
#if defined(GS_SERIAL_LOG_LEVEL_2)
			Serial.println(tmp);
#endif
			client.println(tmp);
			AwaitSMTPResponse(client);
			sz_r = strtok(NULL, ",");
		}
		delete[] toCopy;
	}

#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println("DATA");
#endif
	client.println("DATA");
	if(!AwaitSMTPResponse(client, "354")) {
		_error = "SMTP DATA error";
		return false;
	}

	tmp = F("From: <$>");
	if(_from != nullptr) 
		tmp.replace(" ", _from);
	tmp.replace("$",_emailAddress);
#if defined(GS_SERIAL_LOG_LEVEL_2)
	Serial.println(tmp);
#endif
	client.println(tmp);

	tmp = F("To: <$>");
	if(to.indexOf(',') == -1) {
		tmp.replace("$", to);
#if defined(GS_SERIAL_LOG_LEVEL_2)
		Serial.println(tmp);
#endif
		client.println(tmp);
	} else {
		char *rec = strtok((char*)to.c_str(), ",");
		while(rec) {
			while(*rec == ' ')           // skip spaces after comma.
				++rec;
			tmp.replace("$", rec);
#if defined(GS_SERIAL_LOG_LEVEL_2)
			Serial.println(tmp);
#endif
			client.println(tmp);
			rec = strtok(NULL, ",");
			tmp = F("Cc: <$>");
		}
	}

	tmp = FPSTR(SMTP_SUB);
	tmp += _subject;

	client.println(tmp);
	client.print(FPSTR(HTML_HEAD));
	client.print(message);
	client.println(FPSTR(HTML_END));

	if(!AwaitSMTPResponse(client, "250")) {
		_error = "Sending message error";
		return false;
	}
	client.println("QUIT");
	if(!AwaitSMTPResponse(client, "221")) {
		_error = "SMTP QUIT error";
		return false;
	}
	return true;
}

ESP8266SMTPHelper SMTP;
