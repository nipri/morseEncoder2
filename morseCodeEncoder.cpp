#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <errno.h>
#include "morseCodeEncoder.h"

using namespace std;

class morseCode
{

	public:

		double timeElement;
		double wpm;		

		unsigned int morseCodeTable[36][8] = 
		{
			{'a', DIT, DAH, END}, //a
			{'b', DAH, DIT, DIT, DIT, END}, //b
			{'c', DAH, DIT, DAH, DIT, END}, //c
			{'d', DAH, DIT, DIT, END}, //d
			{'e', DIT, END}, //e
			{'f', DIT, DIT, DAH, DIT, END}, //f
			{'g', DAH, DAH, DIT, END}, //g
			{'h', DIT, DIT, DIT, DIT, END}, //h
			{'i', DIT, DIT, END}, //i
			{'j', DIT, DAH, DAH, DAH, END}, //j
			{'k', DAH, DIT, DAH, END}, //k
			{'l', DIT, DAH, DIT, DIT, END}, //l
			{'m', DAH, DAH, END}, //m
			{'n', DAH, DIT, END}, //n
			{'o', DAH, DAH, DAH, END}, //o
			{'p', DIT, DAH, DAH, DIT, END}, //p
			{'q', DAH, DAH, DIT, DAH, END}, //q
			{'r', DIT, DAH, DIT, END}, //r
			{'s', DIT, DIT, DIT, END}, //s
			{'t', DAH, END}, //t
			{'u', DIT, DIT, DAH, END}, //u
			{'v', DIT, DIT, DIT, DAH, END}, //v
			{'w', DIT, DAH, DAH, END}, //w
			{'x', DAH, DIT, DIT, DAH, END}, //x
			{'y', DAH, DIT, DAH, DAH, END}, //y
			{'z', DAH, DAH, DIT, DIT, END}, //z
			{'1', DIT, DAH, DAH, DAH, DAH, END}, //1
			{'2', DIT, DIT, DAH, DAH, DAH, END}, //2
			{'3', DIT, DIT, DIT, DAH, DAH, END}, //3
			{'4', DIT, DIT, DIT, DIT, DAH, END}, //4
			{'5', DIT, DIT, DIT, DIT, DIT, END}, //5
			{'6', DAH, DIT, DIT, DIT, DIT, END}, //6
			{'7', DAH, DAH, DIT, DIT, DIT, END}, //7
			{'8', DAH, DAH, DAH, DIT, DIT, END}, //8
			{'9', DAH, DAH, DAH, DAH, DIT, END}, //9
			{'0', DAH, DAH, DAH, DAH, DAH, END} //0
	
		};	
};



int main(void)
{

	FILE *fp;
	char keypress;
	string text;
	string sendString;
	ofstream putText;
	ofstream gpio;
	ifstream getText;
	int i, k, sendElement;
	double  newWPM;

	morseCode code;

	cout << "HELLO\r\n\r\n";

// Initialize to 5 WPM
	code.wpm = 5;
	code.timeElement = 240000; // 5 WPM
//	code.timeElement = 92308;  // 13 WPM
//	code.timeElement = 60000;  // 20 WPM	

	double oneTimeUnit = code.timeElement;
	double threeTimeUnits = oneTimeUnit * 3;
	double sevenTimeUnits = oneTimeUnit * 7;

	// Export and configure GPIO23
	fp = fopen("/sys/class/gpio/export", "w");
	fprintf(fp, "18");
	fclose(fp);

	usleep(500000);

	fp = fopen("/sys/class/gpio/gpio18/direction", "w");

	if (!fp)
	{
		perror("Can't Open file!: ");
		cout << "\r\n";
	}
	else
	{
		fprintf(fp, "out");
		fclose(fp);
	}

/*
	fp = fopen("/sys/class/gpio/gpio18/value", "w");
	fprintf(fp, "1");
	fclose(fp);

	usleep(2000000);

	fp = fopen("/sys/class/gpio/gpio18/value", "w");
	fprintf(fp, "0");
	fclose(fp);
*/

	while (keypress != 'q')
	{
		system("clear");
		cout << "press 'e' to enter a line of text\r\n";
		cout << "Press 'c' to chagne the code speed\r\n";
		cout << "Press 's' to  send\r\n";
		cout << "Press 'q' to quit\r\n\r\n";

		cin >> keypress;
		cin.ignore(256, '\n');

		if (keypress == 'c')
		{
			cout << "Enter new code speed in WPM\r\n";
			cin >> newWPM;

			double newSpeedFactor = newWPM / code.wpm;

			oneTimeUnit /= newSpeedFactor;
			threeTimeUnits = oneTimeUnit  * 3;
			sevenTimeUnits = oneTimeUnit * 7;

			code.wpm = newWPM;

			cout << newSpeedFactor << " " << oneTimeUnit << endl;
		}

		else if (keypress == 'e')
		{
			getline(cin, text);

			putText.open("sendText.txt", ios::out | ios::trunc);

			if (!putText.is_open())
			{
				cout << "Error opening file for writing!\r\n";
				return -1;
			}

			else 
			{
				putText << text;
				putText.close();
			}
		}
		else if (keypress == 's')
		{
			getText.open("sendText.txt");

			if (!getText.is_open())
			{
				cout << "Error opening file for reading\r\n";
				return (-1);
			}
			else
			{
				getline(getText, sendString);
				getText.close();

				int stringLength = sendString.length();

				cout << "Sending: " << flush;

				for (i=0; i<stringLength; i++)
				{
					// Get next character
					cout <<  sendString[i] << flush;

					if (i == stringLength-1)
						cout << "\r\n\r\n";

					// If the next char is a space
					if (sendString[i] == ' ')
					{
						usleep(sevenTimeUnits);
					}

					else
					{
						k = 0;
						//Look it up; k will represent the line we want to send
						do
						{
							k++;
						} while (sendString[i] != code.morseCodeTable[k-1][0]);

						// Send the char

						int l = 1;
						do
						{
							sendElement = code.morseCodeTable[k-1][l];

							if (sendElement == DIT)
							{
								fp = fopen("/sys/class/gpio/gpio18/value", "w");
								fprintf(fp, "1");
								fclose(fp);
								usleep(oneTimeUnit);
							}

							else if (sendElement == DAH)
							{
								fp = fopen("/sys/class/gpio/gpio18/value", "w");
								fprintf(fp, "1");
								fclose(fp);
								usleep(threeTimeUnits);
							}

							else
							{
							}

							fp = fopen("/sys/class/gpio/gpio18/value", "w");
							fprintf(fp, "0");
							fclose(fp);		

							usleep(oneTimeUnit);
							l++;

						} while (sendElement != END);

					usleep(threeTimeUnits);
					} // end else
				} // end for

 

//				cout << "Send String IS: " << " " << sendString << "\r\n";
//				sleep(code.timeElement);
//				cout << "Second char is: " << " " << sendString[1] << "\r\n";

			}
		}
		else
		{

		}
	} // end while()
	return (0);
}