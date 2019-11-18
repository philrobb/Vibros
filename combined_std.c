#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <wiringPi.h>

#include <iostream>
#include <fstream>
/* For using memcpy. */
#include <string.h>
/* For using floor() and ceil(). */
#include <math.h>
/* For using time(NULL). */
#include <time.h>

using namespace std;

#define DEBUG 0
#define TRIES_LIMIT 1000
#define WARMUP 0

/* Cell positions. */

/* For no vibration. */
#define NO 0
/* Right leg. */
#define R_FRONT 1
#define R_RIGHT 2
#define R_BACK 3
#define R_LEFT 4
/* Left leg. */
#define L_FRONT 5
#define L_RIGHT 6
#define L_BACK 7
#define L_LEFT 8

/* The GPIO pin that each motor cell is connected to. */
/* Currently, position_leg so R_FRONT would be the cell
 * at the front of right leg. */
#define R_FRONT_PIN 21
#define R_RIGHT_PIN 22
#define R_BACK_PIN 23
#define R_LEFT_PIN 25

#define L_FRONT_PIN 0
#define L_RIGHT_PIN 2
#define L_BACK_PIN 3
#define L_LEFT_PIN 7
/* We want to create an appropriate delay, so we just toggle a pin
 * that is not connected to any motor. */
#define NO_PIN 8

/* Experiment parameters. */
#define BUZZ_DURATION 0.5
#define NUMBER_OF_CELLS 9
#define NUMBER_OF_FREQS 4
#define REPEATS 4
#define NUMBER_OF_COMBNS (NUMBER_OF_CELLS*NUMBER_OF_FREQS)
#define SEQUENCE_LENGTH (REPEATS*NUMBER_OF_COMBNS)
#define FREQ1 10
#define FREQ2 50
#define FREQ3 100
#define FREQ4 200

ofstream response_text;
ofstream response_csv;
ofstream freq_cell_sequence;

void setup(){				    		//This function sets the GPIOS to output, ready to buzz.
	wiringPiSetup();
	pinMode(R_FRONT_PIN, OUTPUT);                                         
	pinMode(R_RIGHT_PIN, OUTPUT);
	pinMode(R_BACK_PIN, OUTPUT);
	pinMode(R_LEFT_PIN, OUTPUT);
	
	pinMode(L_FRONT_PIN, OUTPUT);                                         
	pinMode(L_RIGHT_PIN, OUTPUT);
	pinMode(L_BACK_PIN, OUTPUT);
	pinMode(L_LEFT_PIN, OUTPUT);
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

int get_response(int counter){
	
	response_text << "Subject's response: ";
	printf("Your response: ");
	string response;
	cin >> response;
		if (response == "6"){
				response_text << "Front\n";
				response_csv << R_FRONT << endl;
				cout << "Your response was: " << "Right leg, Front\n";
		}else if (response =="4"){
				response_text << "Back\n";	
				response_csv << R_BACK << endl;  
				cout << "Your response was: " << "Right leg, Back\n";
		}else if (response == "5"){
				response_text << "Left\n"; 
				response_csv << R_LEFT << endl;
				cout << "Your response was: " << "Right leg, In\n";
		}else if (response == "2"){
				response_text <<"Right\n";
				response_csv << R_RIGHT << endl;		  
				cout << "Your response was: " << "Right leg, Out\n";
		}else if (response == "9"){
				response_text << "Front\n";
				response_csv << L_FRONT << endl;
				cout << "Your response was: " << "Left leg, Front\n";
		}else if (response =="7"){
				response_text << "Back\n";	
				response_csv << L_BACK << endl;  
				cout << "Your response was: " << "Left leg, Back\n";
		}else if (response == "8"){
				response_text << "Left\n"; 
				response_csv << L_LEFT << endl;
				cout << "Your response was: " << "Left leg, In\n";
		}else if (response == "61"){
				response_text <<"Right\n";
				response_csv << L_RIGHT << endl;		  
				cout << "Your response was: " << "Left leg, Out\n";
		}else if (response == "0"){
				response_text <<"No vibration was felt.\n";
				response_csv << 0 << endl;
				cout << "Your response was: " << "No vibration was felt.\n";	
		}else if (response == "X"){
				response_csv << "X" << endl;
				return -1;
				
		}else{
				printf("Invalid key. Try again.\n");
				return 1;
		}
		#if WARMUP
		usleep(2000000);
		#endif
		printf("\n");
		return 0;	
}


void buzz_sequence(int* freq, int* sequence, int pattern_length){ 			  	    //This function buzzes the cells at the calculated frequency delay.
	float fd_delay; //Get frequency delay.
	int buzz_duration;			
	/* Which motor cell to activate. For the right leg, for example,
	 * cell_position = 1 -> front
	 * 2 -> right
	 * 3 -> back
	 * 4 -> left */
	int cell_position;	
	int ret_val;
	response_csv << "Number, " << "Frequency, " << "Cell, " << "Response\n";	
	/* This loop goes over the entries in pattern_length (i.e. activates the motors 16 (pattern_length) times as defined
	 * in pattern1 or pattern2 arrays. */
	for(int counter=0; counter<pattern_length; counter++){	  
		printf("\033[H\033[J");  
		cell_position=sequence[counter];
		fd_delay = freqtodelay(freq[counter]);
		buzz_duration = BUZZ_DURATION*freq[counter]; //Duration of 1 buzz ex (5*freq= five seconds)
		response_csv << counter + 1 << ", " << freq[counter] << ", " << sequence[counter] << ", ";
		printf("\n#%d Activating cell.\n", counter+1);

		switch(cell_position){
			
			case NO:
				response_text << "\nNo cell activated.\n";
				for(int i=0; i<buzz_duration; i++){	
					buzz_single_cell(NO_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
				
			case R_FRONT:
				response_text << "\nActivating cell: Front, Right leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(R_FRONT_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
		
			case R_RIGHT:
				response_text << "\nActivating cell: Right, Right leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(R_RIGHT_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
				
			case R_BACK:
				response_text << "\nActivating cell: Back, Right leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(R_BACK_PIN, fd_delay);	
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
	
			case R_LEFT:
				response_text << "\nActivating cell: Left, Left leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(R_LEFT_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
					
			case L_FRONT:
				response_text << "\nActivating cell: Front, Left leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(L_FRONT_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
			
			case L_RIGHT:	
				response_text << "\nActivating cell: Right, Left leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(L_RIGHT_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
				
			case L_BACK:
				response_text << "\nActivating cell: Back, Left leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(L_BACK_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;			
				
			case L_LEFT:
				response_text << "\nActivating cell: Left, Left leg\n";
				for(int i=0; i<buzz_duration; i++){
					buzz_single_cell(L_LEFT_PIN, fd_delay);
				}
				while ((ret_val = get_response(counter)) == 1);
				if (ret_val == -1)
					return;
				break;
			
		}
	}
	printf("Trial complete.\n");
	return;
}

/* A "buzz" has two charactristics: location (cell) and frequency. */
struct buzz{
	int cell = 0;
	int freq = 0;
};

/* Random permutation of n numbers, each repeated m<=n times, without the same number two times in the row. */
/* Problem with my implementation: I think satifying the condition above is not always feasible. For example, consider the
 * case when the randomization is such that we have only 2 repeats of say combination 17 left at the end. */
/* Solution: - To detect when we're stuck, try a limited number of times (TRIES_LIMIT) and then return -1. 
 * - In main(), have while(generate_data()==-1) untill you get an answer.*/
int generate_data(buzz* sequence){

	buzz combns[NUMBER_OF_COMBNS];
	int cells[NUMBER_OF_CELLS] = {NO,R_FRONT,R_RIGHT,R_BACK,R_LEFT,L_FRONT,L_RIGHT,L_BACK,L_LEFT};
	int freqs[NUMBER_OF_FREQS] = {FREQ1,FREQ2,FREQ3,FREQ4};
	int c = 0;
	/* Create (NUMBER_OF_CELLS*NUMBER_OF_FREQS) structures that represent all combinations of 
	 * frequency and cell. */
	for (int i=0;i<=NUMBER_OF_CELLS-1;i++){
		for (int j=0;j<=NUMBER_OF_FREQS-1;j++){
			combns[c].cell = cells[i];
			combns[c].freq = freqs[j];
			c++;
		}
	}
		
	int repeats[NUMBER_OF_COMBNS];
	/* Each combination should only repeated REPEATS times. */
	for (int i=0;i<=NUMBER_OF_COMBNS-1;i++)
		repeats[i] = REPEATS;
	
	c = 0;
	int prev_cell = -1;
	int i = 0;
	int tries = 0;
	srand(time(NULL));
	while (i <= (SEQUENCE_LENGTH-1)){
		c = rand() % NUMBER_OF_COMBNS;
		#if DEBUG
		printf("c=%d\n", c);
		for (int i=0;i<=NUMBER_OF_COMBNS-1;i++)
			printf("%d ", repeats[i]);
		printf("\n");
		#endif
		/* Only accept a combination which still has repeats left and
		 * is not in the same location as the previous vibration. */
		if (repeats[c] != 0 && combns[c].cell != prev_cell){
			sequence[i].cell = combns[c].cell;
			sequence[i].freq = combns[c].freq;
			repeats[c]--;
			prev_cell = sequence[i].cell;
			i++;
		}else{
			tries++;
			if (tries == TRIES_LIMIT)
				return -1;
		}
		
	}
	
	return 0;
	
}

int main () {	
    
    printf("\033[H\033[J"); 
    
	string subject_name;
	#if WARMUP
	subject_name = "test";
	#else
	printf("Enter a name for the response file (ex. Jacob1): ");
	cin >> subject_name;
	#endif
	response_text.open(subject_name);
	
	string name_csv = subject_name + "_CSV";
	response_csv.open(name_csv);
    
    string name_sequence = subject_name + "_Sequence";
    freq_cell_sequence.open(name_sequence);
    
	setup();

	buzz sequence[SEQUENCE_LENGTH];
	int cell_sequence[SEQUENCE_LENGTH];
	int freq_sequence[SEQUENCE_LENGTH];
	
	while (generate_data(sequence) == -1);
	
	freq_cell_sequence << "Number, " << "Frequency, " << "Cell\n";
	
	for (int i=0;i<=SEQUENCE_LENGTH-1;i++){
		freq_sequence[i] = sequence[i].freq;
		cell_sequence[i] = sequence[i].cell;
		freq_cell_sequence << i+1 << ", " << freq_sequence[i] << ", " << cell_sequence[i] << endl;
	}
		
	printf("Press S to start.\n");
	char keystart;
	while(1){
		/* Choose a key that is NOT availabe on the keypad. */
		cin >> keystart;
		if(keystart == 'S'){
			break;
		}
	}
	
	buzz_sequence(freq_sequence, cell_sequence, SEQUENCE_LENGTH);
	printf("\nExiting the program.\n");	
	response_text.close();
	response_csv.close();
	
}
