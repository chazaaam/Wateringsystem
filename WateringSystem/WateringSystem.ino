/*
 Name:		WateringSystem.ino
 Created:	21.07.2017 19:01:34
 Author:	Stefan
*/

#include <servo.h>

//DIO
bool buttonDuration;
bool buttonDryness;
const int waterpin = 3;
const int buttonDurationpin = 2;
const int buttonDrynesspin = 2;
int LEDpin[10];

//AIO
long soilValue[3];
long bucketValue;

//Values
long dryValue = 0;
const long emptyValue = 0;

//Timer
long pumpTimer;
long modeTimer;
long looptimer;
int ledTimer;
const long timerValue = 20000;
const long modeTimeValue = 1000;
const long loopValue = 10000;
const int ledTimerValue = 100;

//misc
int state;
int modeDryness;
int modeDuration;
int lednumber;

//Servo
Servo sweeper;
int plant[3];
int pos;

enum State_enum
{
	ST_WAIT,
	ST_VALUE_CHECK,
	ST_PUMP,
	ST_SERVO,
	ST_BUCKET,
	ST_DRYNESS_MODE,
	ST_DURATION_MODE
};

void setup() {
	Serial.begin(9600);

	for (int i = 0; i < 5; i++)
	{
		LEDpin[i] = 13 - i;
		pinMode(LEDpin[i], OUTPUT);
	}

	pinMode(waterpin, OUTPUT);
	pinMode(buttonDrynesspin, INPUT);
	pinMode(buttonDurationpin, INPUT);

	plant[0] = 45;
	plant[1] = 90;
	plant[2] = 135;


	sweeper.attach(8);
}

void loop() {
	switch (state)
	{
	case ST_WAIT:
		looptimer--;

		if (digitalRead(buttonDurationpin))
			state = ST_DURATION_MODE;
		if (digitalRead(buttonDrynesspin))
			state = ST_DRYNESS_MODE;

		if (looptimer <= 0)
		{
			looptimer = loopValue;
			state = ST_VALUE_CHECK;
		}
		break;

	case ST_VALUE_CHECK:
		Serial.write("Checking Value \n");
		soilValue[0] = analogRead(A0);
		soilValue[1] = analogRead(A1);
		soilValue[2] = analogRead(A2);
		bucketValue = analogRead(A5);
		Serial.print(state);

		for (int i = 0; i < 2; i++)
		{
			pos = plant[i];
			pumpTimer = timerValue;
			state = ST_SERVO;
		}


		if (bucketValue > emptyValue)
		{
			state = ST_BUCKET;
		}


		state = ST_WAIT;
		break;

	case ST_SERVO:
		sweeper.write(pos);
		if (sweeper.read() == pos)
			state = ST_PUMP;
		break;

	case ST_PUMP:
		Serial.write("Starting pump \n");
		while (pumpTimer > 0)
		{
			digitalWrite(waterpin, HIGH);
		}
		digitalWrite(waterpin, LOW);
		state = ST_WAIT;
		break;

	case ST_BUCKET:
		Serial.write("Bucket empty \n");
		ledTimer--;
		if (ledTimer <= 0)
		{
			if (lednumber != 4)
				lednumber++;
			else
				lednumber = 0;
		}

		digitalWrite(LEDpin[lednumber], HIGH);

		if (bucketValue > emptyValue)
		{

			for (int i = 0; i < 5; i++)
			{
				digitalWrite(LEDpin[i], LOW);
			}
			state = ST_WAIT;
		}
		break;

	case ST_DRYNESS_MODE:
		modeTimer = modeTimeValue;
		while (modeTimer == 0)
		{
			if (digitalRead(buttonDrynesspin))
			{
				if (modeDryness == 4)
					modeDryness = 0;
				else
					modeDryness++;
				modeTimer = modeTimeValue;
			}

			for (int i = 0; i < 5; i++)
			{
				if (i != modeDryness)
					digitalWrite(LEDpin[i], LOW);
				else
					digitalWrite(LEDpin[i], HIGH);
			}

			modeTimer--;
		}
		break;

	case ST_DURATION_MODE:
		modeTimer = modeTimeValue;
		while (modeTimer == 0)
		{
			if (digitalRead(buttonDurationpin))
			{
				if (modeDuration == 4)
					modeDuration = 0;
				else
					modeDuration++;
				modeTimer = modeTimeValue;
			}

			for (int i = 0; i < 5; i++)
			{
				if (i != modeDuration)
					digitalWrite(LEDpin[i], LOW);
				else
					digitalWrite(LEDpin[i], HIGH);
			}

			modeTimer--;
		}
		break;
	}
}




