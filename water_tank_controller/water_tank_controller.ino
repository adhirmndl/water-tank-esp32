#include <WiFi.h>

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

const int output_led[] = {16, 17, 18, 19, 21, 22, 23, 25, 26, 27};
const int num_leds = sizeof(output_led) / sizeof(output_led[0]);

int relay_output[] = {32, 33};

void sync_led()
{
	const int num_channels = sizeof(water_label) / sizeof(water_label[0]);

	for (int channel = 0; channel < num_channels; channel++)
	{
		uint32_t led_mask = 0; // Initialize the LED mask as all zeros

		if (water_label[channel] < 10)
		{
			// no light
		}
		else if (water_label[channel] < 25)
		{
			led_mask = 1; // Turn on LED 1
		}
		else if (water_label[channel] < 50)
		{
			led_mask = 3; // Turn on LED 1 and LED 2
		}
		else if (water_label[channel] < 75)
		{
			led_mask = 7; // Turn on LED 1, LED 2, and LED 3
		}
		else if (water_label[channel] < 95)
		{
			led_mask = 15; // Turn on LED 1, LED 2, LED 3, and LED 4
		}
		else
		{
			led_mask = 31; // Turn on all LEDs (1 to 5)
		}

		// Set the LED outputs based on the led_mask
		for (int i = 0; i < sizeof(output_led) / sizeof(output_led[0]); i++)
		{
			digitalWrite(output_led[i], (led_mask >> i) & 1);
		}
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
	print("GPIO initialized for output");
}

void setup()
{
	Serial.begin(115200);
	print("board on setup mode\n logging service start");

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

	// calculation status
	sync_led();

	// cron
}
