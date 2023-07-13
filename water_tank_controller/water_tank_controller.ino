#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "HeartBeat_Main";
const char *password = "connected";
IPAddress local_IP(192, 168, 2, 91); // set your desired static IP address
IPAddress gateway(192, 168, 2, 1);	 // set your router's gateway IP address
IPAddress subnet(255, 255, 255, 0);	 // set your subnet mask
IPAddress dns(8, 8, 8, 8);			 // set your desired DNS server
// int blueLedPin = 4;					 // set your blue LED pin number
AsyncWebServer server(80);
const int blueLedPin = 2;
bool is_blue_led_on = false;
int status_led_count = 4;
int water_label[] = {95, 95};
bool relay_state[] = {false, false};
bool is_enable_channel[] = {true, true};
int tank_size_in_cm[] = {120, 60};

HTTPClient http;

const int output_led[] = {16, 17, 18, 19, 21, 22, 23, 25, 26, 27};
const int num_leds = sizeof(output_led) / sizeof(output_led[0]);

int relay_output[] = {13, 14};

void sync_led()
{
	uint32_t led_mask = 0; // Initialize the LED mask as all zeros

	print("--------------------------------");
	if (is_enable_channel[0])
	{
		if (water_label[0] < 15)
		{
			Serial.print("_ _ _ _ _");
			// no light
			digitalWrite(output_led[0], 0);
			digitalWrite(output_led[1], 0);
			digitalWrite(output_led[2], 0);
			digitalWrite(output_led[3], 0);
			digitalWrite(output_led[4], 0);
		}
		else if (water_label[0] < 25)
		{
			Serial.print("# _ _ _ _");
			digitalWrite(output_led[0], 1); // Turn on LED 1
			digitalWrite(output_led[1], 0);
			digitalWrite(output_led[2], 0);
			digitalWrite(output_led[3], 0);
			digitalWrite(output_led[4], 0);
		}
		else if (water_label[0] < 50)
		{
			Serial.print("# # _ _ _");
			digitalWrite(output_led[0], 1);
			digitalWrite(output_led[1], 1); // Turn on LED 1 and LED 2
			digitalWrite(output_led[2], 0);
			digitalWrite(output_led[3], 0);
			digitalWrite(output_led[4], 0);
		}
		else if (water_label[0] < 75)
		{
			Serial.print("# # # _ _");
			digitalWrite(output_led[0], 1);
			digitalWrite(output_led[1], 1);
			digitalWrite(output_led[2], 1);
			// Turn on LED 1, LED 2, and LED 3
			digitalWrite(output_led[3], 0);
			digitalWrite(output_led[4], 0);
		}
		else if (water_label[0] < 95)
		{
			Serial.print("# # # # _");
			digitalWrite(output_led[0], 1);
			digitalWrite(output_led[1], 1);
			digitalWrite(output_led[2], 1);
			digitalWrite(output_led[3], 1);
			// Turn on LED 1, LED 2, LED 3, and LED 4
			digitalWrite(output_led[4], 0);
		}
		if (water_label[0] >= 95)
		{
			Serial.print("# # # # #");
			digitalWrite(output_led[0], 1);
			digitalWrite(output_led[1], 1);
			digitalWrite(output_led[2], 1);
			digitalWrite(output_led[3], 1);
			digitalWrite(output_led[4], 1);
			// Turn on all LEDs (1 to 5)
		}
		print("");
	}

	// channel 2:
	if (is_enable_channel[1])
	{
		if (water_label[1] < 10)
		{
			Serial.print("_ _ _ _ _");
			// no light
			digitalWrite(output_led[5], 0);
			digitalWrite(output_led[6], 0);
			digitalWrite(output_led[7], 0);
			digitalWrite(output_led[8], 0);
			digitalWrite(output_led[9], 0);
		}
		else if (water_label[1] < 25)
		{
			Serial.print("# _ _ _ _");
			digitalWrite(output_led[5], 1); // Turn on LED 1
			digitalWrite(output_led[6], 0);
			digitalWrite(output_led[7], 0);
			digitalWrite(output_led[8], 0);
			digitalWrite(output_led[9], 0);
		}
		else if (water_label[1] < 50)
		{
			Serial.print("# # _ _ _");
			digitalWrite(output_led[5], 1);
			digitalWrite(output_led[6], 1); // Turn on LED 1 and LED 2
			digitalWrite(output_led[7], 0);
			digitalWrite(output_led[8], 0);
			digitalWrite(output_led[9], 0);
		}
		else if (water_label[1] < 75)
		{
			Serial.print("# # # _ _");
			digitalWrite(output_led[5], 1);
			digitalWrite(output_led[6], 1);
			digitalWrite(output_led[7], 1);
			// Turn on LED 1, LED 2, and LED 3
			digitalWrite(output_led[8], 0);
			digitalWrite(output_led[9], 0);
		}
		else if (water_label[1] < 95)
		{
			Serial.print("# # # # _");
			digitalWrite(output_led[5], 1);
			digitalWrite(output_led[6], 1);
			digitalWrite(output_led[7], 1);
			digitalWrite(output_led[8], 1);
			// Turn on LED 1, LED 2, LED 3, and LED 4
			digitalWrite(output_led[9], 0);
		}
		if (water_label[1] >= 95)
		{
			Serial.print("# # # # #");
			digitalWrite(output_led[5], 1);
			digitalWrite(output_led[6], 1);
			digitalWrite(output_led[7], 1);
			digitalWrite(output_led[8], 1);
			digitalWrite(output_led[9], 1);
			// Turn on all LEDs (1 to 5)
		}
	}
}

void parseAndAssignValues(const String &response)
{
	// Find the position of the comma in the response
	int commaPos = response.indexOf(',');

	if (commaPos != -1)
	{
		// Extract the substrings before and after the comma
		String aStr = response.substring(0, commaPos);
		String bStr = response.substring(commaPos + 1);

		// Convert the substrings to integers
		water_label[0] = aStr.toInt();
		water_label[1] = bStr.toInt();
	}
	else
	{
		Serial.println("Invalid response format: " + response);
	}
}

void get_water_status()
{
	// Send GET request
	int httpResponseCode = http.GET();
	if (httpResponseCode == HTTP_CODE_OK)
	{
		// Read the response
		String response = http.getString();
		Serial.print("\nreponse: ");
		Serial.print(response);
		parseAndAssignValues(response);
		print("");
	}
	else
	{
		Serial.print("Error accessing API. Error code: ");
		Serial.print(httpResponseCode);
	}
}

void turn_on_blue_light(bool enabled)
{
	if (is_blue_led_on == enabled)
		return;
	pinMode(blueLedPin, OUTPUT); // Set the pin as an output

	if (enabled)
	{
		digitalWrite(blueLedPin, HIGH); // Turn on the blue LED
		is_blue_led_on = true;
	}
	else
	{
		digitalWrite(blueLedPin, LOW); // Turn off the blue LED
		is_blue_led_on = false;
	}
}

void print(char *str)
{
	Serial.print(str);
	Serial.print("\n");
}

void init_gpio()
{
	for (int i = 0; i < num_leds; i++)
	{
		pinMode(output_led[i], OUTPUT);
	}
	// relay
	pinMode(relay_output[0], OUTPUT);
	digitalWrite(relay_output[0], HIGH);
	pinMode(relay_output[1], OUTPUT);
	digitalWrite(relay_output[1], HIGH);

	// SR04 sensor 1
	// pin 32 -echo pin 33 -trigger
	pinMode(33, OUTPUT);
	pinMode(32, INPUT);
	print("GPIO initialized for output");
}

int get_sensor_readings(int ECHO_PIN, int TRIG_PIN, int tank_size)
{
	float duration_us, distance_cm;
	// generate 10-microsecond pulse to TRIG pin
	digitalWrite(TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG_PIN, LOW);

	// measure duration of pulse from ECHO pin
	duration_us = pulseIn(ECHO_PIN, HIGH);

	// calculate the distance
	distance_cm = 0.017 * duration_us;

	// print the value to Serial Monitor
	Serial.print("\ndistance: ");
	Serial.print(distance_cm);
	Serial.println(" cm");

	// calculate the percentage
	if (distance_cm > tank_size)
		distance_cm = tank_size;
	int percentage = 100 - int((distance_cm * 100) / tank_size);

	Serial.print("water level percentage: ");
	Serial.print(percentage);
	Serial.println(" %");

	return percentage;
}

void cron()
{
	// channel 1;
	if (is_enable_channel[0])
	{
		if (relay_state[0] && water_label[0] >= 95)
		{
			digitalWrite(relay_output[0], HIGH); // off = HIGH
			relay_state[0] = false;
			print("relay 1: OFF");
		}
		if (!relay_state[0] && water_label[0] < 20)
		{
			digitalWrite(relay_output[0], LOW); // ON = LOW
			relay_state[0] = true;
			print("relay 1: ON");
		}
	}
	if (is_enable_channel[1])
	{
		if (relay_state[1] && water_label[1] >= 95)
		{
			digitalWrite(relay_output[1], HIGH); // off = HIGH
			relay_state[1] = false;
			print("relay 2: OFF");
		}
		if (!relay_state[1] && water_label[1] < 20)
		{
			digitalWrite(relay_output[1], LOW); // ON = LOW
			relay_state[1] = true;
			print("relay 2: ON");
		}
	}
}

void init_web_server()
{
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/html", "<!DOCTYPE html><html><head><style>.toggle{display:inline-block;width:60px;height:34px;position:relative;cursor:pointer}.toggle input[type=checkbox]{display:none}.slider{position:absolute;top:0;left:0;right:0;bottom:0;background-color:#ccc;border-radius:34px;transition:.4s}.slider:before{position:absolute;content:"
												";height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;border-radius:50%;transition:.4s}input[type=checkbox]:checked+.slider{background-color:#2196F3}input[type=checkbox]:checked+.slider:before{transform:translateX(26px)}</style></head><body><div class=toggle><input type=checkbox id=toggle1 onclick=toggleButton(0,this.checked)><label class=slider for=toggle1></label></div><div class=toggle><input type=checkbox id=toggle2 onclick=toggleButton(1,this.checked)><label class=slider for=toggle2></label></div><script>function toggleButton(button,state){const url=`http://192.168.2.91/${button}/${state?1:0}`;fetch(url).then(response=>{}).catch(error=>{});}</script></body></html>"); });
	server.on("/1/0", HTTP_GET, [](AsyncWebServerRequest *request)
			  { relay_state[1] = false;digitalWrite(relay_output[1], HIGH);request->send(200, "text/plain", "done"); });
	server.on("/1/1", HTTP_GET, [](AsyncWebServerRequest *request)
			  { relay_state[1] = true;digitalWrite(relay_output[1], LOW);request->send(200, "text/plain", "done"); });
	server.on("/0/1", HTTP_GET, [](AsyncWebServerRequest *request)
			  { relay_state[0] = true;digitalWrite(relay_output[0], LOW);request->send(200, "text/plain", "done"); });
	server.on("/0/0", HTTP_GET, [](AsyncWebServerRequest *request)
			  { relay_state[0] = false;digitalWrite(relay_output[0], HIGH);request->send(200, "text/plain", "done"); });
	server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request)
			  {String relay1State = relay_state[0] ? "ON" : "OFF";String relay2State = relay_state[1] ? "ON" : "OFF";String response = "relay_1: " + relay1State + ", relay_2: " + relay2State;request->send(200, "text/plain", response); });

	server.begin();
	print("Web server started on port 80");
}

void connect_wifi()
{
	// connect to Wi-Fi
	WiFi.begin(ssid, password);
	// Serial.print("Connecting to Wi-Fi...");
	print("Connecting to Wi-Fi...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		print("sleep 1s and retrying ...");
	}
	Serial.println("Connected to Wi-Fi successfully");

	// set static IP address and DNS server
	if (!WiFi.config(local_IP, gateway, subnet, dns))
	{
		print("Failed to configure static IP and DNS");
	}
}

void setup()
{

	Serial.begin(115200);
	// delay(4000);
	print("board on setup mode\n logging service start");
	http.begin("https://heartbt.xyz/api/test_api");

	init_gpio();

	// configure blue LED pin as output
	pinMode(blueLedPin, OUTPUT);
	print("blue light pin config set");

	connect_wifi();

	init_web_server();
}

void loop()
{
	// turn on blue LED if Wi-Fi is connected
	if (WiFi.status() == WL_CONNECTED)
		turn_on_blue_light(true);
	else
		turn_on_blue_light(false);

	// your code here
	water_label[0] = get_sensor_readings(32, 33, tank_size_in_cm[0]); // pin 32 -echo pin 33 -trigger

	// TODO: get water lavel status
	// get_water_status();

	// calculation status
	sync_led();

	// cron
	cron();

	// delay
	delay(500);
}
