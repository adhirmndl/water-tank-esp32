#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "HeartBeat_Main";
const char *password = "connected";
IPAddress local_IP(192, 168, 2, 91); // set your desired static IP address
IPAddress gateway(192, 168, 2, 1);	 // set your router's gateway IP address
IPAddress subnet(255, 255, 255, 0);	 // set your subnet mask
IPAddress dns(8, 8, 8, 8);			 // set your desired DNS server
// int blueLedPin = 4;					 // set your blue LED pin number
const int blueLedPin = 2;
bool is_blue_led_on = false;
int status_led_count = 4;
int water_label[] = {95, 95};
bool relay_state[] = {false, false};
bool is_enable_channel[] = {true, false};

HTTPClient http;

const int output_led[] = {16, 17, 18, 19, 21, 22, 23, 25, 26, 27};
const int num_leds = sizeof(output_led) / sizeof(output_led[0]);

int relay_output[] = {32, 33};

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
	pinMode(relay_output[1], OUTPUT);
	print("GPIO initialized for output");
}

void cron()
{
	// channel 1;
	if (is_enable_channel[0])
	{
		if (relay_state[0] && water_label[0] >= 95)
		{
			digitalWrite(relay_output[0], 0);
			relay_state[0] = false;
			print("relay 1: OFF");
		}
		if (!relay_state[0] && water_label[0] < 20)
		{
			digitalWrite(relay_output[0], 1);
			relay_state[0] = true;
			print("relay 1: ON");
		}
	}
	if (is_enable_channel[1])
	{
		if (relay_state[1] && water_label[1] >= 95)
		{
			digitalWrite(relay_output[1], 0);
			relay_state[1] = false;
			print("relay 2: OFF");
		}
		if (!relay_state[1] && water_label[1] < 20)
		{
			digitalWrite(relay_output[1], 0);
			relay_state[1] = true;
			print("relay 2: ON");
		}
	}
}
void setup()
{
	Serial.begin(115200);
	print("board on setup mode\n logging service start");
	http.begin("https://heartbt.xyz/api/test_api");

	init_gpio();

	// configure blue LED pin as output
	pinMode(blueLedPin, OUTPUT);
	print("blue light pin config set");

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

void loop()
{
	// turn on blue LED if Wi-Fi is connected
	if (WiFi.status() == WL_CONNECTED)
		turn_on_blue_light(true);
	else
		turn_on_blue_light(false);

	// your code here

	// TODO: get water lavel status
	get_water_status();

	// calculation status
	sync_led();

	// cron
	cron();

	// delay
	delay(1000);
}
