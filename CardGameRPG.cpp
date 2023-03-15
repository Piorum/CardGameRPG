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
int i;
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
string tempString;
string outputString;
const string cardTypes[3] = { "Damage","Heal","Debuff" }; 
const string debuffNames[2] = { "Fire","Poison" };

//core functions------------------------------------------------------------------------------------------------------------------

//pause function pauses the game until user input
void pause() {

	//waits for user to input anything before exiting game
	cout << "\nInput anything to continue.\n";
	cin >> tempString;

}

//function to check if a file exists
bool fileCheck(const string& filename) {

	//opens file name specified
	ifstream input_file(filename);

	//checks if eof is reached on input operation
	return input_file.good();

}

//function to clear console regardless of OS i think
void clear() {
	system("clear || cls");
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

	//saving variables to file with seperator ' '
	fout << score << ' ' << playerHealth << ' ' << enemyHealth << ' ' << enemyHealthLocked << ' '
		<< enemyDebuffed << ' ' << turnCounter << ' ' << saveString;

	//informing user game is saved and exiting
	cout << "Saved";
	exit(2);
}

//function to display all relevant data needed
void display() {

	//calls function to clear console
	clear();

	//print player and enemy health
	cout << "HP: " << playerHealth << "/50" << "||||||||||" << "Enemy: " << enemyHealth << "/" << enemyHealthLocked << "\n";

	//prints players cards
	for (i = 0; i < 5; i++) {
		//uses a switch statment to print damage/heal type cards different then debuff cards
		switch (playerCardsArray[i][0]) {
		case 0:
		case 1:
			//for printing damage/heal cards, prints - Card x: card type - damage/heal value
			cout << "Card " << i + 1 << ": " << cardTypes[playerCardsArray[i][0]] << " - " << playerCardsArray[i][1] << "\n";
			break;
		case 2:
			// for printing debuff cards, prints - Card x: card type - debuff name
			cout << "Card " << i + 1 << ": " << cardTypes[playerCardsArray[i][0]] << " - " << debuffNames[playerCardsArray[i][1]] << "\n";
			break;
		}
	}

}

//gameplay functions--------------------------------------------------------------------------------------------------------------

//function to generate enemy variables generates health and card if passed 0 and only card if passed 1
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

	//switch to determine card type, 0-1 for heal/damage and 2 for debuffs
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

//function to resolve enemy combat
void enemyCombat() {

	//resolves effect of enemy card
	//switch to play different card types
	switch (enemyCard[0]) {
	//case for damage cards
	case 0:
		playerHealth -= enemyCard[1];
		//append to outputString letting user know enemy damaged you
		outputString = outputString + "\nThe enemy damaged you for " + to_string(enemyCard[1]) + "!";
		break;
	//case for healing cards
	case 1:
		enemyHealth += enemyCard[1];
		//append to outputString letting user know the enemy healed
		outputString = outputString + "\nThe enemy healed for " + to_string(enemyCard[1]) + "!";
		//resets enemyHealth to the max if it goes over
		if (enemyHealth > enemyHealthLocked) {
			enemyHealth = enemyHealthLocked;
		}
		break;
	}

}

//function to resolve player combat
void playerCombat(int cardn) {

	//removing 1 from passed variable to align with array values
	--cardn;
	
	//resolving effect from selected player card
	//switch to play the different card types
	switch (playerCardsArray[cardn][0]) {
		//case for damage card type subtracts enemy health by the value stored in the associated card
	case 0:
		enemyHealth -= playerCardsArray[cardn][1];
		//append to outputString letting user know they damaged the enemy
		outputString = "You damaged the enemy for " + to_string(playerCardsArray[cardn][1]) + " !";
		break;
		//case for heal card type adds value associated with the card to player health
	case 1:
		playerHealth += playerCardsArray[cardn][1];
		//append to outputString letting user know they healed
		outputString = "You healed for " + to_string(playerCardsArray[cardn][1]) + " !";
		//resets playerHealth if it goes over the max value
		if (playerHealth > 50) {
			playerHealth = 50;
		}
		break;
		//case for debuff type card, sets enemy debuffed to true
	case 2:
		++enemyDebuffed;
		//append to outputString letting user know they applied a debuff
		outputString = "You applied " + debuffNames[playerCardsArray[cardn][1]] + " to the enemy!";
		break;
	}

	//resolving damage from debuffs
	//calculate debuff damage
	enemyHealth -= enemyDebuffed;
	//append to outputString letting user know enemy was damaged from debuffs
	outputString = outputString + "\nEnemy was damaged by " + to_string(enemyDebuffed) + " from debuffs.";
}

//primary function used to handle combat, enemy/player attacks
void combat() {

	//reseting outputString and temp
	outputString = "";
	temp = 0;

	//while loop that continously prompts user for input until they enter a valid option
	bool validInput = false;
	while (!validInput) {
		cout << "Please select a card 1-5 or Save" << "\n";
		cin >> tempString;

		//trys to convert input to int
		try {
			temp = stoi(tempString);
		}
		catch (const invalid_argument& ia) {

		}

		//checks if input is save or a valid number and runs save function if input matches
		if (tempString == "save" || tempString == "Save") {
			save();
		}
		else if (0 < temp && temp < 6) {
			validInput = true;
		}
		else {
			tempString = "";
			cout << "Please input a number '1','2','3','4','5', or 'Save'";
		}
	}

	//calls function to resolve player actions
	playerCombat(temp);
	//calls function to resolve the enemy play
	enemyCombat();

	//Increase turnCounter by 1
	++turnCounter;
	//check to see if game needs to refresh selection of cards
	if (turnCounter > 2) {
		//loops 5 times to generate new cards for the player
		for (i = 0; i < 5; i++) {
			//calls function to generate a playercard
			generateCard(i);
		}
		//appends to the outputString to let user know new cards were generated
		outputString = outputString + "\nNew cards were generated";
		//resets turnCounter
		turnCounter = 0;
	}

	//regenerates player card that was used subtracting 1 to align with array values
	generateCard(temp - 1);
	//regenerates enemy card by passing value 1
	generateEnemy(1);

	//refreshes display and outputs all changes
	display();
	//outputs the outputString which is constructed throughout the combat function
	cout << outputString << "\n";

}

//main----------------------------------------------------------------------------------------------------------------------------
int main() {

	//calls function to clear console
	clear();

	//creates menu----------------------------------------------------------------------------------------------------------------

	//while loop asking for input until the user types exit or start
	bool validInput = false;
	while (!validInput) {
		//prompts user for input
		cout << "Please type Start or Exit\n";
		cin >> tempString;

		//if user types start
		if (tempString == "start" || tempString == "Start") {
			validInput = true;
		}
		//if user types exit
		else if (tempString == "exit" || tempString == "Exit") {
			exit(0);
		}
		//if user types something else
		else {
			cout << "Invalid Input";
		}
	}

	//loads games, checks for save file or generates fresh variables--------------------------------------------------------------

	//seeding random number generator
	srand(time(nullptr));

	//if no save file
	if (!fileCheck("savefile.txt")) {

		//loops 5 times to generate cards for the player
		for (i = 0; i < 5; i++) {
			//calls function to generate a playercard
			generateCard(i);
		}

		//calls function to set enemy health AND card by passing the 0 value
		generateEnemy(0);

	}
	//if there is save file
	else if (fileCheck("savefile.txt")) {

		//declaring variables
		string savefileText[18];

		//open savefile
		ifstream fin("savefile.txt");

		//error if file fails to open
		if (!fin.is_open()) {
			std::cout << "Error opening savefile." << std::endl;
		}

		//saving savefile to a string
		stringstream buffer;
		buffer << fin.rdbuf();
		string fileContents;
		fileContents = buffer.str();

		//savefile always stores exactly 18 values from 0-17
		//spliting savefileString into an array splitting by ' '
		stringstream buffer2(fileContents);
		for (i = 0; i < 18; i++) {
			buffer2 >> tempString;
			savefileText[i] = tempString;
		}

		//order the values are stored is always the same
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

		//closes file
		fin.close();

		//deletes old savefile
		if (fileCheck("savefile.txt")) {
			remove("savefile.txt");
		}

	}

	//calls function to refresh the 'display' console in this case
	display();

	//main gameloop, loops until player death or exit-----------------------------------------------------------------------------

	//loops combat while playerHealth is above 0
	while (playerHealth > 0) {
		//checks if enemy health is below 0
		if (enemyHealth < 1) {

			//regenerates enemy
			cout << "You beat an enemy!";
			generateEnemy(0);

			//calls pause function
			pause();

			//adds one to score
			++score;

			//calls function to refresh display
			display();
		}
		//calls combat function
		combat();
	}
	//loop ends when you die

	//outputs score and informs that you lost
	cout << "You lost :(\nYour score is " + to_string(score);

	//checks if save file exists and deletes it
	if (fileCheck("savefile.txt")) {
		remove("savefile.txt");
	}

	//calls pause function before exit
	pause();

}
