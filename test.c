#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <wiringPi.h>
#include <iostream>

#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

using namespace std;

#define REPEATS 4
#define BUZZ_DURATION 1
#define NUMBER_OF_FREQS 4
#define FREQ1 10
#define FREQ2 50
#define FREQ3 100
#define FREQ4 200

void setup_right(){				    		//This function sets the GPIOS to output, ready to buzz.
	wiringPiSetup();
	pinMode(21, OUTPUT);                                         
	pinMode(22, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(25, OUTPUT);
}

void setup_left(){				    		//This function sets the GPIOS to output, ready to buzz.
	wiringPiSetup();
	pinMode(0, OUTPUT);                                         
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(7, OUTPUT);
}

float freqtodelay(float Hz){		    //This function calculates the frequency delay based on inputted frequency.
	float fd_delay=(1/(Hz)*1000000*0.5);
	return fd_delay;
}

void buzz_single_cell(int cell_pin, float fd_delay){
	digitalWrite(cell_pin,HIGH); 		
	delayMicroseconds(fd_delay);
	digitalWrite(cell_pin, LOW);
	delayMicroseconds(fd_delay);
}

struct buzz{
	int cell = 0;
	int freq = 0;
};


int main () {
	
printf("\033[H\033[J"); 
	
setup_right();
setup_left();


float freq;
float fd_delay;
int buzz_duration;
int right_leg = 0;
int left_leg = 0;

vector<int> freqs = {FREQ1,FREQ2,FREQ3,FREQ4};

printf("Enter R or L.\n");
	char keystart;
	while(1){
		
		cin >> keystart;
		if(keystart == 'R'){
			right_leg = 1;
			break;
		}else if(keystart == 'L'){
			left_leg = 1;
			break;
		}
	}
	
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
auto rng = default_random_engine {};
shuffle(begin(freqs), end(freqs), default_random_engine(seed));

cout << freqs.at(0) << " " << freqs.at(1) << " " << freqs.at(2) << " " << freqs.at(3) << " " << endl;

int k = 0;

if (right_leg)
{
	while (k<REPEATS){
				
		freq = freqs.at(k % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;			
		for(int i=0; i<buzz_duration; i++){	
			fd_delay = freqtodelay(freq);
			buzz_single_cell(21, fd_delay);			
		}
					
		freq = freqs.at((k+1) % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);
			buzz_single_cell(22, fd_delay);
		}
					
		freq = freqs.at((k+2) % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);
			buzz_single_cell(23, fd_delay);
		}
					
		freq = freqs.at((k+3) % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);
			buzz_single_cell(25, fd_delay);
		}
	k++;
	usleep(1000000);
	}	
}

k = 0;

if(left_leg){
	while (k<REPEATS){
		
		freq = freqs.at(k % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;	
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);
			buzz_single_cell(0, fd_delay);
		}
			
		freq = freqs.at((k+1) % NUMBER_OF_FREQS);	
		buzz_duration = BUZZ_DURATION*freq;
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);
			buzz_single_cell(2, fd_delay);
		}
				
		freq = freqs.at((k+2) % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);			
			buzz_single_cell(3, fd_delay);
		}
				
		freq = freqs.at((k+3) % NUMBER_OF_FREQS);
		buzz_duration = BUZZ_DURATION*freq;		
		for(int i=0; i<buzz_duration; i++){
			fd_delay = freqtodelay(freq);
			buzz_single_cell(7, fd_delay);
		}
	k++;
	usleep(1000000);
	}	
}
	

}
