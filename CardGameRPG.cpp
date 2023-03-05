#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

//creating global variables/arrays
int temp;
int temp2;
int score;
int playerHealth = 50;
int enemyHealth;
int enemyHealthLocked;
int enemyDebuffed;
int playerCardsArray[5][2];
int enemyCard[2];
int turnCounter = 0;
string outputString;
string cardTypes[3] = { "Damage","Heal","Debuff" }; 
string debuffNames[2] = { "Fire","Poison" };

//function to generate enemy variables
void generateEnemy(int x) {

	//switch so that function can be used to generate enemy card, or enemy card and health
	switch (x) {
	case 0:
		//generate random health value for enemy between 15 and 40
		enemyHealth = rand() % 25 + 15;
		//sets a health value the enemy can't heal above
		enemyHealthLocked = enemyHealth;
		enemyDebuffed = 0;
	case 1:
		//setting card type to damage or heal by generating number between 0-1
		enemyCard[0] = rand() % 2;
		//saves heal or damage value to card number between 1 and 10
		enemyCard[1] = rand() % 10 + 1;
		break;
	}
}

//function to generate cards for the player
void generateCard(int cardn) {

	//generates random number to determine cardtype between 0-2
	temp2 = rand() % 3;

	//sets cardtype, 0=Damage,1=Heal,2=Debuff
	playerCardsArray[cardn][0] = temp2;

	switch (temp2) {
	case 0:
	case 1:
		//generates number for heal or damage value between 1-10
		temp2 = rand() % 10 + 1;
		//saves heal or damage value to card number
		playerCardsArray[cardn][1] = temp2;
		break;
	case 2:
		//generates debuff id between 0-1
		temp2 = rand() % 2;
		//saves debuff id to card number
		playerCardsArray[cardn][1] = temp2;
		break;
	}

}

//function to check if a file exists
bool fileCheck(const std::string& filename) {

	//opens file name specified
	ifstream input_file(filename);

	//checks if eof is reached on input operation
	return input_file.good();

}

//function to load the game
void loadGame() {
	//if no save file
	if (!fileCheck("savefile.txt")) {

		//loops 5 times to generate cards for the player
		temp = 0;
		while (temp < 5) {
			//calls function to generate a playercard
			generateCard(temp);
			++temp;
		}

		//calls function to set enemy health and card
		generateEnemy(0);

	}
	//if there is save file
	else if (fileCheck("savefile.txt")) {

		//declaring variables
		string savefileText[18];
		string tempString;

		//open savefile
		ifstream fin("savefile.txt");

		//error if file fails to open
		if (!fin.is_open()) {
			std::cout << "Error opening file." << std::endl;
		}

		//saving savefile to a string
		stringstream buffer;
		buffer << fin.rdbuf();
		string fileContents;
		fileContents = buffer.str();

		//spliting savefileString into an array splitting by ' '
		stringstream buffer2(fileContents);
		temp = 0;
		while (temp < 18) {
			buffer2 >> tempString;
			savefileText[temp] = tempString;
			++temp;
		}

		//storing values from file to variables
		score = stoi(savefileText[0]);
		playerHealth = stoi(savefileText[1]);
		enemyHealth = stoi(savefileText[2]);
		enemyHealthLocked = stoi(savefileText[3]);
		enemyDebuffed = stoi(savefileText[4]);
		turnCounter = stoi(savefileText[5]);
		enemyCard[0] = stoi(savefileText[16]);
		enemyCard[1] = stoi(savefileText[17]);

		//recreating playerCardsArray
		temp = 6;
		while (temp < 11) {
			playerCardsArray[temp - 6][0] = stoi(savefileText[temp]);
			++temp;
		}
		while (temp < 16) {
			playerCardsArray[temp - 11][1] = stoi(savefileText[temp]);
			++temp;
		}

		//
		cardTypes[0] = "Damage";

		//closes file
		fin.close();

		//deletes old savefile
		if (fileCheck("savefile.txt")) {
			remove("savefile.txt");
		}

	}
	//critical error
	else {

		cout << "Critical Error";

	}

}

//function to clear console regardless of OS i think
void clear() {
	system("clear || cls");
}

//function to display all relevant data needed
void display() {
	//declaring local variables
	string filler = "||||||||||";

	//calls function to clear console
	clear();

	//print player and enemy health
	cout << "HP: " << playerHealth << "/50" << filler << "Enemy: " << enemyHealth << "/" << enemyHealthLocked << "\n";

	//prints players cards
	temp = 0;
	while (temp < 5) {

		//uses a switch statment to print damage/heal type cards different then debuff cards
		switch (playerCardsArray[temp][0]) {
		case 0:
		case 1:
			//for printing damage/heal cards, prints - Card x: card type - damage/heal value
			cout << "Card " << temp + 1 << ": " << cardTypes[playerCardsArray[temp][0]] << " - " << playerCardsArray[temp][1] << "\n";
			++temp;
				break;
		case 2:
			// for printing debuff cards, prints - Card x: card type - debuff name
			cout << "Card " << temp + 1 << ": " << cardTypes[playerCardsArray[temp][0]] << " - " << debuffNames[playerCardsArray[temp][1]] << "\n";
			++temp;
				break;
		}
	}

}

//function to resolve enemy combat
void enemyCombat() {

	//switch to play different card types
	switch (enemyCard[0]) {
	//case for damage cards
	case 0:
		playerHealth -= enemyCard[1];
		outputString = outputString + "\nThe enemy damaged you for " + to_string(enemyCard[1]) + "!";
		break;
	//case for healing cards
	case 1:
		enemyHealth += enemyCard[1];
		outputString = outputString + "\nThe enemy healed for " + to_string(enemyCard[1]) + "!";
		//resets enemyHealth to the max if it goes over
		if (enemyHealth > enemyHealthLocked) {
			enemyHealth = enemyHealthLocked;
		}
		break;
	}

	//regenerates enemy card
	generateEnemy(1);

	//refreshes display and outputs all changes
	display();
	cout << outputString << "\n";

}

//function to resolve a card being played
void playcard(int x) {

	//removing 1 from x
	--x;

	//reseting outputString
	outputString = "";

	//switch to play the different card types
	switch (playerCardsArray[x][0]) {
		//case for damage card type subtracts enemy health by the value stored in the associated card
	case 0:
		enemyHealth -= playerCardsArray[x][1];
		outputString = "You damaged the enemy for " + to_string(playerCardsArray[x][1]) + " !";
		break;
		//case for heal card type adds value associated with the card to player health
	case 1:
		playerHealth += playerCardsArray[x][1];
		outputString = "You healed for " + to_string(playerCardsArray[x][1]) + " !";
		if (playerHealth > 50) {
			playerHealth = 50;
		}
		break;
		//case for debuff type card, sets enemy debuffed to true
	case 2:
		++enemyDebuffed;
		outputString = "You applied " + debuffNames[playerCardsArray[x][1]] + " to the enemy!";
		break;
	}
	
	//calculate debuff damage
	enemyHealth -= enemyDebuffed;
	outputString = outputString + "\nEnemy was damaged by " + to_string(enemyDebuffed) + " from debuffs.";

	//Increase turnCounter by 1
	++turnCounter;
	if (turnCounter > 2) {
		//loops 5 times to generate new cards for the player
		temp = 0;
		while (temp < 5) {
			//calls function to generate a playercard
			generateCard(temp);
			++temp;
		}
		outputString = outputString + "\nNew cards were generated";
		turnCounter = 0;
	}
	generateCard(x);
}

//function for saving the game
void save() {
	//defining local variable
	string saveString;

	//creating save file
	ofstream fout("savefile.txt");

	//adding each individual value in the array to a string with seperator ' '
	for (int i = 5 - 1; i >= 0; i--)
		saveString = saveString + to_string(playerCardsArray[i][0]) + ' ';
	for (int i = 5 - 1; i >= 0; i--)
		saveString = saveString + to_string(playerCardsArray[i][1]) + ' ';
	for (int i = 2 - 1; i >= 0; i--)
		saveString = saveString + to_string(enemyCard[i]) + ' ';

	//saving variables to file with seperator b
	fout << score << ' ' << playerHealth << ' ' << enemyHealth << ' ' << enemyHealthLocked << ' '
		<< enemyDebuffed << ' ' << turnCounter << ' ' << saveString;

	cout << "Saved";
	exit(2);
}

//primary function used to handle combat, enemy/player attacks
void combat() {

	//declaring local variables
	string input;
	int input2 = 0;
	bool validInput = false;

	//while loop that continously prompts user for input until they enter a valid option
	while (!validInput) {
		cout << "Please select a card 1-5 or Save" << "\n";
		cin >> input;

		//trys to convert input to int
		try {
			input2 = stoi(input);
		}
		catch (const invalid_argument& ia) {

		}

		//checks if input is save or a valid number and runs save function if input matches
		if (input == "save" || input == "Save") {
			save();
		}
		else if (0 < input2 && input2 < 6) {
			validInput = true;
		}
		else {
			input = "";
			cout << "Please input a number '1','2','3','4','5', or 'Save'";
		}
	}

	//calls function to play the card selected and converts the input string into a integer
	playcard(input2);
	enemyCombat();
}

//main
int main() {

	//declaring local variables
	string input;
	bool validInput = false;

	//seeding random number generator
	srand(time(nullptr));

	//calls function to clear console
	clear();

	//while loop running menu function until the user exits or starts the game
	while (!validInput) {
		//prompts user for input
		cout << "Please type Start or Exit\n";
		cin >> input;

		//if user types start
		if (input == "start" || input == "Start") {
			validInput = true;
		}
		//if user types exit
		else if (input == "exit" || input == "Exit") {
			exit(0);
		}
		//if user types something else
		else {
			cout << "Invalid Input";
		}
	}
	//resets validInput
	//validInput = false;

	//calls function to load game
	loadGame();
	//calls function to refresh the 'display' console in this case
	display();
	//loops combat while playerHealth is above 0
	while (playerHealth > 0) {
		//checks if enemy health is below 0
		if (enemyHealth < 1) {

			//regenerates enemy
			cout << "You beat an enemy!";
			generateEnemy(0);

			//waits for user to press enter before refreshing display
			cout << "\nInput anything to continue\n";
			string tempString;
			cin >> tempString;

			//adds one to score
			++score;

			//calls function to refresh display
			display();
		}
		//calls combat function
		combat();
	}
	//outputs score and informs that you lost
	cout << "You lost :(\nYour score is " + to_string(score);
	//checks if save file exists and deletes it
	if (fileCheck("savefile.txt")) {
		remove("savefile.txt");
	}
}