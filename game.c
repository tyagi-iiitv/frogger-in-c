/*
 * game.c
 *
 *  Created on: 23-Oct-2014
 *      Author: anjul
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <term.h>
#include <unistd.h>
#include <sys/types.h>
//#include <math.h>
#define width 10//width is the number of lanes in the game
#define length 20// length is the length of the screen on which the game is played
//#define RAND_MAX 32767
int p = width-1; //These cordinates are the starting cordinates of the player
int q = length/2-1;
//int lane_counter[width]; //array which keeps track of cars to be produced in the lanes
int game_over = 0;
struct termios initialrsettings, newrsettings; // These settings are the keyboard settings
struct str{
	int pos;    //pos=0: Object is not there ; pos=1: Object is there
	int speed;  //speed=n means the object has to move ahead by n block with every clock tick
};
typedef struct str vehicle;
vehicle lane[width][length];
/*int init_lane_counter_null(){
	int i;
	for(i=0;i<width;i++){
		lane_counter[i]=-1;
	}
}*/
void init_null(){  //initialises the lane array to zero
	int i,j;
	for(i=0;i<width;i++){
		for(j=0;j<length;j++){
			lane[i][j].pos=0;
			lane[i][j].speed=0;
		}
	}
}
void init_keyboard(){ // Initialise the terminal to wait for 1/10th of a second for the input and continue otherwise
	tcgetattr(0,&initialrsettings);  // get the attributes of the initial settings of the terminal and save it in the initialrsettings variable
	newrsettings = initialrsettings; // initialise the newrsettings with the current terminal settings
	newrsettings.c_lflag &= ~ICANON; // set the terminal settings to non canonical form of input
	newrsettings.c_lflag &= ~ECHO; // set the terminal settings to the non echo form
	newrsettings.c_cc[VMIN] = 0; // wait until a character is input by the user
	newrsettings.c_cc[VTIME] = 1; // wait only for 1/10th of a second
	tcsetattr(0, TCSANOW, &newrsettings); //apply the newrsettings to the terminal
}
void close_keyboard(){ //close the current terminal settings and set the original terminal settings back
	tcsetattr(0, TCSANOW, &initialrsettings);
}
void exit_failure(){
	printf("OOPs..!! You are hit by a car. Better Luck next time\n");
	close_keyboard();
	exit(0);
}
void exit_success(){
	printf("Great work, You have successfully completed the challenge\n");
	close_keyboard();
	exit(0);
}
int produce(){  //return 0 or 1 corresponding to not produce or produce a car
	int a = rand()%8;
	if (a == 1){
		return 1;
	}
	else{
		return 0;
	}
}
void new_init(int arr[]){ //this function initialises the new cars produced into the lane array
	int i;
	for(i=0;i<width;i++){
		if(arr[i]==1){
			lane[i][0].pos = 1;
			lane[i][0].speed = 1;
		}
	}
}
void display_lane(){ //to display the game on to the screen
	int i,j;
	for(i=0;i<width;i++){
		for(j=0;j<length;j++){
			printf("******");
		}
		printf("\n");
		for(j=0;j<length;j++){
			if(i==p && j==q && lane[i][j].pos == 0){
				printf("  /*\\ ");
			}
			else if(i==p && j==q && lane[i][j].pos == 1){
				printf("CRASH");
				game_over = 1;
			}
			else if(lane[i][j].pos == 0){
				printf("      ");
			}
			else{
				printf("[][][]");
			}
		}
		printf("\n");
		for(j=0;j<length;j++){
			printf("******");
		}
		printf("\n");
	}
	if(game_over == 1){
		exit_failure();
	}
	if(p == -1){
		exit_success();
	}
}
void shift_lane(){ //for shifting the cars one step ahead
	int i,j;
	int arr[length];
	for(i=0;i<width;i++){
		for(j=0;j<length;j++){
			arr[j]=0;
		}
		for(j=0;j<length;j++){
			if(lane[i][j].pos==1){
				if(j+lane[i][j].speed < length){
					arr[j+lane[i][j].speed]=1;
					lane[i][j+lane[i][j].speed].speed = lane[i][j].speed;
				}
			}
		}
		if( i< width/2 ){
			for(j=0;j<length;j++){
				lane[i][j].pos=arr[j];
			}
		}
		else{
			for(j=0;j<length;j++){
				lane[i][j].pos=arr[length-1-j];
			}
		}
	}
}
void reshift_lane(){ //In a two way traffic, the lane array had to be inverted. This function is used to again correct the lane array.
	int i,j;
	int temp;
	for(i=width/2;i<width;i++){
		for(j=0;j<length/2;j++){
			temp=lane[i][j].pos;
			lane[i][j].pos=lane[i][length-1-j].pos;
			lane[i][length-1-j].pos=temp;
		}
	}
}
void print_lanes(){ //this is the most important function which controls the game
	int new_car[width]; //if new_car[i]==1 then produce a new car in that lane otherwise not
	int i,j;
	//init_lane_counter_null();
	for(i=0;i<width;i++){
		new_car[i]=produce();
	}
	new_init(new_car); //Inserts the new cars into the lane array
	shift_lane(); // Shifts the cars one step ahead so as to make them moving
	display_lane(); // Displays the cars onto the screen
	reshift_lane(); //corrects the lane array in a two way traffic
}
int main(){
	//char ab;
	//printf("Press enter to start the Game\n");
	//ab=getchar();
	init_keyboard();
	init_null();
	srand(time(NULL));
	system("clear");
	char a;
	while(a!='q'){
		a = 'o';
		printf("Enter w,s,a,d to move and q to quit\n");
		//printf("P = %d : Q = %d\n",p,q);
		print_lanes();
		usleep(100000);
		scanf("%c",&a);
		if(a == 'a'){
			if(q-1 >= 0){
				q--;
			}
		}
		if(a == 'd'){
			if(q+1 <= length-1){
				q++;
			}
		}
		if(a == 'w'){
			if(p-1 >= -1){
				p--;
			}
		}
		if(a == 's'){
			if(p+1 <= width-1){
				p++;
			}
		}
		system("clear");
	}
	close_keyboard();
	return 0;
}
