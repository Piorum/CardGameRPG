#include <iostream> // std::cin, std::cout
#include <cstdlib> // std::srand, std::rand, std::exit, std::sleep
#include <fstream> // std::ifstream, std::ofstream
#include <sstream> // std::stringstream
#include <string> // std::string
#include <cmath> // std::trunc
#include <ctime> // std:time

//rewrote without "using namepsace std;"
//using namespace std;

//notes-----------------------------------------------------------------------
//
// fix save system so savefile is not deleted every time
// add highscore
// add enemy ai
// redraw to five after defeating enemy
// show enemy moves in real time
// only allow 3 cards per turn

//creating global variables/arrays------------------------------------------------------------------------------------------------
//temp variables for storing random values
int i;
int temp[3];
std::string tempString;

//display value
bool debuffInfo;

//score/counter variables
int score;
int roundCounter = 0;

//[0] stores current health
//[1] stores max health
int playerHealth[2] = { 50, 50 };
int enemyHealth[2];

//variable to hold mana values
int playerMana;
int enemyMana;

//[0][0] stores the amount of effects
//[x 1-98][0] stores the debuff/buff type
//[x 1-98][1] stores the debuff/buff duration
int playerEffects[99][2];
int enemyEffects[99][2];

//effects values
//sets values of the effect durations in parallel with other effect arrays
const int effectsDuration[2] = {5, 10};
//sets values of the effect damages in parallel with other effect arrays
const int effectsDamage[2] = {2, 1};
//sets debuff names in parallel with other effect arrays
const std::string debuffNames[2] = { "Fire","Poison" };

//[0][0] stores amount of cards
//[1-98][0] stores card type, damage/heal/effect(debuff/buff)
//[1-98][1] stores card value, damage/heal ammount / debuff/buff id
//[1-98][2] stores card cost - mana value - to be added
int playerCardsArray[99][3];
int enemyCardsArray[99][3];

//card type names stored in parallel to values stored in other arrays
const std::string cardTypes[3] = { "Damage","Heal","Debuff" };

//output variables
//stores all information about what the enemy does during their turn
int outputLog[99][2];
//stores information about damage taken from effects
int outputLog2[2];

//function declarations-----------------------------------------------------------------------------------------------------------
//core functions
void exit();
void pause();
bool fileCheck(const std::string& filename);
void clear();
int power(int num, int power);
std::string tolower(std::string inputString);
std::string input(int user);
int stoint(std::string inputString);
int len(std::string inputString);
std::string intost(int inputInt);
//display functions
void help(); 
void menu();
void display();
//save and load functions 
void save();
void loadArray(int temparray[][3], std::stringstream& tempbuffer, std::string tempstring[]);
void loadArray(int temparray[][2], std::stringstream& tempbuffer, std::string tempstring[]);
void load();
//generation functions
void generateEnemy();
void generateCard(int cardArray[][3], int cardn);
//gameplay functions
void draw(int cardArray[][3], int& mana);
void discard(int cardArray[][3], int user);
void restoreCard(int cardArray[][3]);
void discardCard(int cardArray[][3], int cardn);
void drawCard(int cardArray[][3]);
void playCard(int cardArray[][3], int cardn, int friendlyHealth[2], int friendlyEffects[][2], int opponentHealth[2], int opponentEffect[][2], int& mana);
//combat functions
void resolveEffects(int effectArray[][2], int health[2]);
void enemyCombat();
void playerCombat();
void combat();
void death();

//main----------------------------------------------------------------------------------------------------------------------------
int main() {

	//infinite loop, program will only stop once player types exit
	while (true) {

		//creates main menu
		menu();

		//loads game variables
		load();

		//loops combat until player dies
		combat();

		//this should really be moved to combat
		//calls function to handle player death
		death();
	}

}

//core functions------------------------------------------------------------------------------------------------------------------

//function to exit the game
void exit() {
	clear();
	std::cout << "'S' Save and Exit or 'E' Exit?\n";
	tempString = tolower(input(0));

	if (tempString == "s") {
		save();
		std::cout << "Saved.";
		std::exit(0);
	}
	else if (tempString == "e") {
		std::exit(0);
	}
	else {
		display();
		std::cout << "Invalid Input\n";
	}
}

//pause function pauses the game until user input
void pause() {

	//waits for user to input anything before exiting game
	std::cout << "\nInput anything to continue.\n";
	std::cin >> tempString;

}

//function to check if a file exists
bool fileCheck(const std::string& filename) {

	//opens file name specified
	std::ifstream input_file(filename);

	//checks if eof is reached on input operation
	return input_file.good();

}

//function to clear console regardless of OS i think
void clear() {
	std::system("clear || cls");
}

//function to set a number to a positive power
int power(int num, int power) {
	//stores initial number
	int inum = num;
	//runs loop multiplying the number by itself to the correct power
	for (int i = 1; i < power; i++) {
		num *= inum;
	}
	//conditions for the power is 0
	if (power == 0) {
		num = 1;
	}
	//returns number - if power is zero no math is ever applied and it returns the original number
	return num;
}

//function that takes string as an input and converts each uppercase character to lowercase then returns the string
std::string tolower(std::string inputString) {
	int counter;

	//finds length of string and creates a loop to check every value
	for (counter = 0; counter < len(inputString); counter++) {
		//checking if the value is in range of uppercase letters
		if (inputString[counter] < 91 && inputString[counter] > 64) {
			//adds 32 to change ascii value to the lowercase equivilant
			inputString[counter] += 32;
		}
	}

	return inputString;
}

//function to get input - 0 for player, 1 for enemy
std::string input(int user) {
	//declaring local variable
	std::string input;

	//switch case 0 = player input, case 1 enemy AI input
	switch (user) {
	case 0:

		//gets user input
		std::cin >> input;

		//returns string
		return input;
		break;
	case 1:

		//sets user input (in future will be set to a variable set by the AI before running the input function)
		input = "1";

		//returns string
		return input;
		break;
	}
}

//function to convert a string to an int
int stoint(std::string inputString) {
	//initialize counter, negative flag, and length
	int i;
	bool negative = false;
	int length = len(inputString);
	//checks if string starts with '-'
	if (inputString[0] == '-') {
		//flips negative flag value
		negative = true;
		//removes one from the length
		length--;
		//shifts every value in the string one down
		for (i = 0; i < length; i++) {
			inputString[i] = inputString[i + 1];
		}
	}
	//sets Pow to the correct power and zeros out outputInt
	int Pow = power(10, length - 1);
	int outputInt = 0;
	//loops for as many characters as the string is long
	for (i = 0; i < length; i++) {
		//checks if value is outside ascii range of numbers
		if (inputString[i] > 57 && inputString[i] < 48) {
			//sets outputInt to failure variable and returns
			outputInt = -999;
			return outputInt;
		}
		//adds to outputInt the numbers in the string from left to right times the multiplicity that it should be
		outputInt += (inputString[i] - 48) * Pow;
		//reduces the multiplicity by 1
		Pow /= 10;
	}
	//checks for negative flag
	if (negative == true) {
		//converts int to negative
		outputInt -= 2 * outputInt;
	}
	return outputInt;
}

//function to get length of a string
int len(std::string inputString) {
	//sets counter
	int i;
	//loops adding 1 to the counter untill inputString reaches the end character
	for (i = 0; inputString[i] != '\0'; i++) {
	}
	//sets length to the amount of characters counted
	int length = i;
	return length;
}

//function used to convert a int to string
std::string intost(int inputInt) {
	//declaring local variables
	int counter;
	int counter2 = 1;
	int tempInt;
	int tempInt2 = 0;
	std::string outputString;
	int multiplicity = 10;
	bool negativeflag = false;

	if (inputInt < 0) {
		negativeflag = true;
		inputInt -= (2 * inputInt);
	}

	//checking to see how many zeros are behind the first number
	for (counter = 0; counter != -1;) {
		//sets tempInt to the input value
		tempInt = inputInt;
		//uses loss of data to check if number is below zero when divided by a certain multiplicity
		tempInt /= multiplicity;
		//checks to see if number is below 1 after being divided by multiplicity
		if (tempInt != 0) {
			//increases multiplicity
			multiplicity *= 10;
			//adds to the counter tracking character length of the int
			counter2++;
		}
		else {
			//reduces multiplicity
			multiplicity /= 10;
			//sets exit flag
			counter = -1;
		}

	}

	//resizes string to the size of the integer
	outputString.resize(counter2);

	//loops for how every many characters long the int is
	for (counter = 0; counter < counter2; counter++) {
		tempInt = inputInt;
		//divides int by multiplicity to get the correct number
		tempInt /= multiplicity;
		//subtracts tempInt2 (if the number was 5382 and this was the second loop the number about be 50 making 53 into 3 the correct number)
		tempInt -= tempInt2;
		//continuously adding to tempInt2 so that the correct number can keep being selected.
		if (tempInt2 == 0) {
			tempInt2 = tempInt;
			tempInt2 *= 10;
		}
		else {
			tempInt2 += tempInt;
			tempInt2 *= 10;
		}
		outputString[counter] = tempInt + 48;
		//reduces multiplicity
		multiplicity /= 10;
	}

	//swaps all values to the end of the resizes array and add - to the front if number is negative
	if (negativeflag == true) {
		outputString.resize(counter2 + 1);
		for (counter = 0; counter < counter2; counter++) {
			outputString[counter2 - counter] = outputString[counter2 - counter - 1];
		}
		outputString[0] = '-';
	}


	return outputString;
}

//display functions---------------------------------------------------------------------------------------------------------------

//function to display help information
void help() {
	clear();
	std::cout << "Cards can be choosen by entering the card number shown on the left of the console.\n";
	std::cout << "Every card has a mana cost, effects cards always cost 1, damage/heal cards cost atleast 1 or their value divided by 3.\n";
	std::cout << "Each card has information displayed in the order listed below.\n";
	std::cout << "Card Type (Damage/Heal/Debuff) - Damage/Heal Value / Effect Name - Mana Cost\n";
	std::cout << "Enemy health is random but will have a higher base value as you defeat more enemies.\n";
	std::cout << "Score raises exponentially as you defeat enemies and is currently just represents how many enemies you've defeated.\n";
	std::cout << "If you don't have enough mana a card or draw you can enter 'Pass' to end your turn and refill your mana.\n";
	std::cout << "Discard allows you to remove 3 cards and get 1 more for free.\n";
	std::cout << "To save you must have 7 mana\n\n";
	std::cout << "Typing save exits the game and stores your current gamestate to a file for next startup.\n\n";
	pause();
	display();

}

//function that generates menu
void menu() {

	bool validInput = false;
	while (!validInput) {

		clear();

		//short-ish help prompt
		std::cout << "Pick cards to fight enemies, upon defeat your score will increase\n";
		std::cout << "You can play a card/draw/pass/discard, costs are listed and draw costs 1 mana.\n";
		std::cout << "Discard lets you remove 3 cards for 1 new one.\n";
		std::cout << "If you don't have enough mana to play type 'Pass' to move on\n";
		std::cout << "Every enemy move will be listed underneath your cards\n\n";

		//need to add mini help prompt at the start and access to help menu
		//prompts user for input
		std::cout << "Please type Start, Exit, or Help\n";
		tempString = input(0);

		//sets inputstring to all lowercase
		tempString = tolower(tempString);

		//if user types start
		if (tempString == "start") {
			validInput = true;
		}
		//if user types exit
		else if (tempString == "exit") {
			std::exit(0);
		}
		//if user types help
		else if (tempString == "help") {
			help();
			clear();
		}
		//if user types something else
		else {
			std::cout << "Invalid Input";
			pause();
		}
	}

}

//function to display all relevant data needed
void display() {

	//calls function to clear console
	clear();

	//sets health back to zero if it ever goes below
	if (playerHealth[0] < 0) {
		playerHealth[0] = 0;
	}
	if (enemyHealth[0] < 0) {
		enemyHealth[0] = 0;
	}

	//print player health
	std::cout << "PlayerHP: [";
	for (i = 0; i < (playerHealth[0] * 10) / playerHealth[1]; i++) {
		std::cout << "|";
	}
	for (i = 0; i < 10 - ((playerHealth[0] * 10) / playerHealth[1] ); i++) {
		std::cout << " ";
	}
	std::cout << "]";
	//prints enemy health
	std::cout << " -- EnemyHP: [";
	for (i = 0; i < (enemyHealth[0] * 10) / enemyHealth[1]; i++) {
		std::cout << "|";
	}
	for (i = 0; i < 10 - ((enemyHealth[0] * 10) / enemyHealth[1]); i++) {
		std::cout << " ";
	}
	std::cout << "]";
	//prints player mana
	std::cout << "\nMana: [";
	for (i = 0;i < playerMana; i++) {
		std::cout << "|";
	}
	for (i = 0; i < 7 - playerMana; i++) {
		std::cout << " ";
	}
	std::cout << "]";
	//prints card count
	std::cout << " -- Cards: " << playerCardsArray[0][0] << "/10\n\n";

	//prints players cards
	for (i = 1; i <= playerCardsArray[0][0]; i++) {
		//uses a switch statment to print damage/heal type cards different then debuff cards
		switch (playerCardsArray[i][0]) {
		case 0:
		case 1:
			//for printing damage/heal cards, prints - Card x: card type - damage/heal value
			std::cout << "Card " << i << ": " << cardTypes[playerCardsArray[i][0]] << " - " << playerCardsArray[i][1] << " - Cost " << playerCardsArray[i][2] << "\n";
			break;
		case 2:
			// for printing debuff cards, prints - Card x: card type - debuff name
			std::cout << "Card " << i << ": " << cardTypes[playerCardsArray[i][0]] << " - " << debuffNames[playerCardsArray[i][1]] << " - Cost " << playerCardsArray[i][2] << "\n";
			break;
		}
	}

	std::cout << "\n";

	for (i = 0; i < outputLog[0][0]; i++) {
		switch (outputLog[i + 1][0]) {
		//output case for heal/damage
		case 0:
		case 1:
			std::cout << "The enemy played " << cardTypes[outputLog[i + 1][0]] << " for " << outputLog[i + 1][1] << ".\n";
			break;
		//output case for effects
		case 2:
			std::cout << "The enemy afflicted you with " << debuffNames[outputLog[i + 1][1]] << ".\n";
			break;
		}
	}

	if (debuffInfo == true) {

		if (outputLog2[0] != 0) {
			std::cout << "You took " << outputLog2[0] << " from debuffs.\n";
		}
		if (outputLog2[1] != 0) {
			std::cout << "You dealt " << outputLog2[1] << " damage with debuffs.\n";
		}

	}

}

//save and load functions---------------------------------------------------------------------------------------------------------

//function for saving the game
void save() {
	//defining local variable
	std::string saveString;

	//creating save file
	std::ofstream fout("savefile.txt");

	//adding variables to saveString
	saveString += intost(score) + ' ';
	saveString += intost(roundCounter) + ' ';
	saveString += intost(playerHealth[0]) + ' ';
	saveString += intost(playerHealth[1]) + ' ';
	saveString += intost(enemyHealth[0]) + ' ';
	saveString += intost(enemyHealth[1]) + ' ';
	saveString += intost(playerMana) + ' ';
	saveString += intost(enemyMana) + ' ';

	//adding amount of player/enemy cards and effects
	saveString += intost(playerCardsArray[0][0]) + ' ';
	saveString += intost(enemyCardsArray[0][0]) + ' ';
	saveString += intost(playerEffects[0][0]) + ' ';
	saveString += intost(enemyEffects[0][0]) + ' ';

	//adding each card and effect value to the save string
	for (i = 0; i < playerCardsArray[0][0]; i++) {
		saveString += intost(playerCardsArray[i + 1][0]) + ' ';
		saveString += intost(playerCardsArray[i + 1][1]) + ' ';
		saveString += intost(playerCardsArray[i + 1][2]) + ' ';
	}
	for (i = 0; i < enemyCardsArray[0][0]; i++) {
		saveString += intost(enemyCardsArray[i + 1][0]) + ' ';
		saveString += intost(enemyCardsArray[i + 1][1]) + ' ';
		saveString += intost(enemyCardsArray[i + 1][2]) + ' ';
	}
	for (i = 0; i < playerEffects[0][0]; i++) {
		saveString += intost(playerEffects[i + 1][0]) + ' ';
		saveString += intost(playerEffects[i + 1][1]) + ' ';
	}
	for (i = 0; i < enemyEffects[0][0]; i++) {
		saveString += intost(enemyEffects[i + 1][0]) + ' ';
		saveString += intost(enemyEffects[i + 1][1]) + ' ';
	}

	fout << saveString;
}

//function for loading an array of size [][3]
void loadArray(int temparray[][3], std::stringstream& tempbuffer, std::string tempstring[]) {

	//gets value of how many total cards there are loops to refill all the data
	for (i = 0; i < temparray[0][0]; i++) {
		//adds the next three values in the buff to the tempstring array
		for (temp[0] = 0; temp[0] < 3; temp[0]++) {
			tempbuffer >> tempString;
			tempstring[temp[0]] = tempString;
		}
		//adds the three values to the cardn based off the number of loops
		temparray[i + 1][0] = stoint(tempstring[0]);
		temparray[i + 1][1] = stoint(tempstring[1]);
		temparray[i + 1][2] = stoint(tempstring[2]);
	}

}

//function for loading an array of size [][2]
void loadArray(int temparray[][2], std::stringstream& tempbuffer, std::string tempstring[]) {

	for (i = 0; i < temparray[0][0]; i++) {
		//adds the next three values in the buff to the savefileText array
		for (temp[0] = 0; temp[0] < 3; temp[0]++) {
			tempbuffer >> tempString;
			tempstring[temp[0]] = tempString;
		}
		//adds the three values to the cardn based off the number of loops
		temparray[i + 1][0] = stoint(tempstring[0]);
		temparray[i + 1][1] = stoint(tempstring[1]);
	}

}

//load function that loads save file or sets starting variables
void load() {
	std::srand(std::time(nullptr));

	//if no save file
	if (!fileCheck("savefile.txt")) {

		playerCardsArray[0][0] = 0;
		playerEffects[0][0] = 0;
		enemyCardsArray[0][0] = 0;
		enemyEffects[0][0] = 0;
		//loops 5 times to generate cards for the player
		for (i = 0; i < 5; i++) {
			//calls function to select a playercard 5 times
			drawCard(playerCardsArray);
		}

		//generates enemy and gives it one card
		generateEnemy();
		drawCard(enemyCardsArray);

		//sets mana values so they are displayed on first load
		playerMana = 7;
		enemyMana = 7;

	}

	//needs rework to store variable card amounts
	//if there is save file
	else if (fileCheck("savefile.txt")) {

		//declaring variables
		std::string savefileText[99];

		//open savefile
		std::ifstream fin("savefile.txt");

		//error if file fails to open
		if (!fin.is_open()) {
			std::cout << "Error opening savefile." << std::endl;
		}

		//not really happy with this ----------
		//saving savefile to a string array
		std::stringstream buffer;
		buffer << fin.rdbuf();
		for (i = 0; i < 12; i++) {
			buffer >> tempString;
			savefileText[i] = tempString;
		}
		//-------------------------------------


		//order the values are stored is always the same and so can be reset in order
		//storing values from file to variables
		score = stoint(savefileText[0]);
		roundCounter = stoint(savefileText[1]);
		playerHealth[0] = stoint(savefileText[2]);
		playerHealth[1] = stoint(savefileText[3]);
		enemyHealth[0] = stoint(savefileText[4]);
		enemyHealth[1] = stoint(savefileText[5]);
		playerMana = stoint(savefileText[6]);
		enemyMana = stoint(savefileText[7]);

		//sets values for the number of cards/effects in each array
		playerCardsArray[0][0] = stoint(savefileText[8]);
		enemyCardsArray[0][0] = stoint(savefileText[9]);
		playerEffects[0][0] = stoint(savefileText[10]);
		enemyEffects[0][0] = stoint(savefileText[11]);

		//recreating arrays
		//loops that goes for as many cards as were stored in the save file
		loadArray(playerCardsArray, buffer, savefileText);
		loadArray(enemyCardsArray, buffer, savefileText);
		loadArray(playerEffects, buffer, savefileText);
		loadArray(enemyEffects, buffer, savefileText);

		//closes file
		fin.close();

		//deletes old savefile
		if (fileCheck("savefile.txt")) {
			std::remove("savefile.txt");
		}

	}

}

//generation functions------------------------------------------------------------------------------------------------------------

//function to generate initial enemy variables
void generateEnemy() {

	//generate random health value for enemy between 15 and 40
	enemyHealth[0] = std::rand() % 25 + 16;
	//multipies the round count by 10 and then again by 1-1.5x then adds the value to the base health to scale difficulty of the game.
	enemyHealth[0] += std::trunc((roundCounter * 10) * ((std::rand() % 5 + 11.0) / 10.0));
	//sets a health value equal to the max health that the enemy can't heal above
	enemyHealth[1] = enemyHealth[0];


}

//function to generate cards for player or enemy
void generateCard(int cardArray[][3], int cardn) {

	//generates random number to determine cardtype between 0-2
	temp[1] = std::rand() % 3;

	//sets cardtype, 0=Damage,1=Heal,2=Debuff
	cardArray[cardn][0] = temp[1];

	//switch to determine card type, 0-1 for heal/damage and 2 for debuffs
	switch (temp[1]) {
	case 0:
	case 1:
		//generates number for heal or damage value between 1-10
		temp[1] = std::rand() % 10 + 1;
		//saves heal or damage value to card number
		cardArray[cardn][1] = temp[1];
		//sets mana value of card
		cardArray[cardn][2] = std::trunc(temp[1] / 3);
		//sets mana value to 1 if damage value was too low for a cost
		if (cardArray[cardn][2] == 0) {
			cardArray[cardn][2] = 1;
		}
		break;
	case 2:

		//debuff generation needs to be reworked to work with effects array
		//generates debuff id between 0-1
		temp[1] = std::rand() % 2;
		//saves debuff id to card number
		cardArray[cardn][1] = temp[1];
		//sets mana value of card
		cardArray[cardn][2] = 2;
		break;
	}

}

//gameplay functions--------------------------------------------------------------------------------------------------------------

//player draw function
void draw(int cardArray[][3], int& mana) {

	if (cardArray[0][0] < 10){

		//calls function to draw another card
		drawCard(cardArray);

		//updates mana
		mana -= 1;

		//refreshes display
		display();
	}
	else {
		display();
		std::cout << "You already have too many cards.\n";
	}
}

//player discard function
void discard(int cardArray[][3], int user) {
	//checks to make sure they have enough cards to discard
	if (cardArray[0][0] > 2) {
		//display update
		display();
		std::cout << "Please input 3 card numbers to discard or Cancel\n";
		//sets a check to see if discarding gets canceled
		temp[1] = 0;
		//loops 3 times to delete 3 cards
		for (temp[0] = 0; temp[0] < 3; temp[0]++) {
			//sets user input to tempString
			tempString = input(user);
			//attempts to convert string to int
			if (stoint(tempString) != -999) {
				discardCard(cardArray, stoint(tempString));
			}
			//checks if string is equal to cancel if converting to int fails
			else {
				if (tolower(tempString) == "cancel") {
					//restores cards that the player discards
					for (temp[1] = 0; temp[1] < temp[0]; temp[1]++) {
						restoreCard(cardArray);
					}
					//sets variable to 3 to exit loop
					temp[0] = 3;
					//sets check to show loop was canceled
					temp[1] = 1;
				}
				else {
					//tells user they inputed something invalid and decrements loop counter
					display();
					std::cout << "Invalid Input - Please type a number or cancel\n";
					temp[0]--;
					pause();

				}
			}
			//display update showing remaining cards needed to be discarded
			display();
			std::cout << "Please select " << 2 - temp[0] << " more cards.\n";
		}
		//draws new card if loop was not cancelled
		if (temp[1] == 0) {
			drawCard(cardArray);
		}
		//display update
		display();
	}
	else {
		//display update informing user of an error
		display();
		std::cout << "You need 3 cards to discard\n";
	}
}

//function to restore card
void restoreCard(int cardArray[][3]) {
	cardArray[0][0]++;
}

//function used to remove a card from a deck
void discardCard(int cardArray[][3], int cardn) {

	//for loop that makes it so the current cards always take up the first spots in the array
	for (i = 0; i < (cardArray[0][0] - cardn); i++) {
		std::swap(cardArray[cardn + i][0], cardArray[cardn + i + 1][0]);
		std::swap(cardArray[cardn + i][1], cardArray[cardn + i + 1][1]);
		std::swap(cardArray[cardn + i][2], cardArray[cardn + i + 1][2]);
	}

	cardArray[0][0] -= 1;

}

//function used to draw a new card
void drawCard(int cardArray[][3]) {

	generateCard(cardArray, cardArray[0][0] + 1);
	cardArray[0][0] += 1;

}

//function to play a card
void playCard(int cardArray[][3], int cardn, int friendlyHealth[2], int friendlyEffects[][2], int opponentHealth[2], int opponentEffect[][2], int& mana) {

	//resolving effect from selected card
	//switch to play the different card types
	switch (cardArray[cardn][0]) {

		//case for damage card type subtracts enemy health by the value stored in the associated card
	case 0:
		opponentHealth[0] -= cardArray[cardn][1];

		//needs to return damage value
		break;

		//case for heal card type adds value associated with the card to player health
	case 1:
		friendlyHealth[0] += cardArray[cardn][1];

		//needs to return heal value

		//resets playerHealth if it goes over the max value
		if (friendlyHealth[0] > friendlyHealth[1]) {
			friendlyHealth[0] = friendlyHealth[1];
		}
		break;
		//case for debuff type card, sets enemy debuffed to true
	case 2:

		//sets values in effects array
		//adds 1 to the number of effects
		opponentEffect[0][0]++;
		//sets value of the most current effect equal to the debuff id of the cardn played
		opponentEffect[opponentEffect[0][0]][0] = cardArray[cardn][1];
		//sets value of the most current effect equal to the duration associated with the debuff id of the cardn played
		opponentEffect[opponentEffect[0][0]][1] = effectsDuration[cardArray[cardn][1]];

		//needs to send to effects function
		break;
	}

	//updates mana
	mana -= cardArray[cardn][2];

	//removes card from play
	discardCard(cardArray, cardn);
}

//combat functions----------------------------------------------------------------------------------------------------------------

//function to calculate damage from effects
void resolveEffects(int effectArray[][2], int health[2]) {

	//resolving effects
	for (i = 0; i < effectArray[0][0]; i++) {
		//gets the damage value for the debuff id associated with every debuff and subtracts that from the players health
		health[0] -= effectsDamage[effectArray[i + 1][0]];
		//removes 1 from the debuff's duration
		effectArray[i + 1][1]--;
		//checks if debuff duration has run out then moving the debuff values off the list and lowering the debuff count
		if (effectArray[i + 1][1] == 0) {
			//for loop that makes it so the current cards always take up the first spots in the array
			for (temp[2] = 0; temp[2] < effectArray[0][0] - i; temp[2]++) {
				std::swap(effectArray[temp[2]][0], effectArray[temp[2] + 1][0]);
				std::swap(effectArray[temp[2]][1], effectArray[temp[2] + 1][1]);
			}
			//removes 1 from the amount of effects on the player
			effectArray[0][0]--;
		}
	}


}

//function to resolve enemy combat - needs to control enemy AI
void enemyCombat() {

	//resets enemy mana
	enemyMana = 5;

	//enemy main loop
	while (enemyMana >= enemyCardsArray[1][2]) {

		//adds the card type used and value to the output log
		outputLog[outputLog[0][0] + 1][0] = enemyCardsArray[1][0];
		outputLog[outputLog[0][0] + 1][1] = enemyCardsArray[1][1];

		//will add basic decision tree later but for now it plays its 1 card then draws a new one
		playCard(enemyCardsArray, 1, enemyHealth, enemyEffects, playerHealth, playerEffects, enemyMana);
		drawCard(enemyCardsArray);

		outputLog[0][0]++;

		if (playerHealth[0] < 1) {
			enemyMana = 0;
		}
	}


}

//function to resolve player combat
void playerCombat() {

	//resets mana
	playerMana = 7;

	//turns on debuff info
	debuffInfo = true;

	//refreshes display to show enemy actions
	display();

	//resets output log counter
	outputLog[0][0] = 0;

	//player main loop
	//while loop that continously prompts user for input until they enter a valid option
	while (playerMana > 0) {

		temp[0] = 0;

		//loop that prints a message asking you to a select a card adding a number for each card you can select
		std::cout << "Please select a card ";
		for (i = 0; i < playerCardsArray[0][0]; i++) {
			std::cout << "'" << i + 1 << "', ";
		}
		std::cout << "Draw/Pass/Discard or Help/Save/Exit\n";
		//gets input and puts it into tempString
		tempString = input(0);

		//turns of debuff info
		debuffInfo = false;

		//trys to convert input to int
		temp[0] = stoint(tempString);

		//sets the inputstring to all lowercase
		tempString = tolower(tempString);

		//playcard action
		//checks if all values are within the ranges
		if (temp[0] > 0 && temp[0] <= playerCardsArray[0][0] && playerCardsArray[temp[0]][2] <= playerMana) {

			//calls function resolving effect from selected player card
			playCard(playerCardsArray, temp[0], playerHealth, playerEffects, enemyHealth, enemyEffects, playerMana);

			//refreshes display
			display();

			if (enemyHealth[0] < 1) {
				playerMana = 0;
			}
		}
		else if (tempString == "draw" && playerMana > 0) {
			draw(playerCardsArray, playerMana);
		}
		else if (tempString == "pass") {
			//updates mana
			playerMana = 0;
		}
		else if (tempString == "discard") {
			//runs discard function
			discard(playerCardsArray, 0);
		}
		else if (tempString == "save" && playerMana == 7) {
			//runs save function
			save();

			//informing user game is saved
			std::cout << "Saved\n";
		}
		else if (tempString == "help") {
			//runs help function
			help();
		}
		else if (tempString == "exit") {
			//runs exit function
			exit();
		}
		else {
			//display update informing user of an error
			display();
			std::cout << "Invalid Input or Not enough mana\n";
		}
	}

}

//function that loops until player death, handles enemy regeneration and switching between player and enemy turns
void combat() {

	while (playerHealth[0] > 0) {

		//starts players turn
		//calls function to resolve player actions
		playerCombat();

		//resolves effects on the enemy
		outputLog2[1] = enemyHealth[0];
		resolveEffects(enemyEffects, enemyHealth);
		outputLog2[1] -= enemyHealth[0];

		//starts enemys turn if they are not dead
		//calls function to start enemys turn
		//checks if enemy is alive.
		if (enemyHealth[0] > 0) {
			enemyCombat();
		}
		else {
			//display refresh with enemy beat notification
			display();
			std::cout << "You beat an enemy!\n";
			//increments roundcounter
			roundCounter++;
			//regenerates enemy
			generateEnemy();

			//resets effects counters
			playerEffects[0][0] = 0;
			enemyEffects[0][0] = 0;

			//adds one to score
			score += roundCounter;

			//resets player health
			playerHealth[0] = 50;

			save();

			//informing user game autosaved
			std::cout << "autosaved\n";

			//calls pause function
			pause();

			//calls function to refresh display so that new enemy is shown
			display();

		}

		//sets outputlog to the current health value, resolves effects damage, then subtracts current health value to get damage taken
		//resolves effects on the player
		outputLog2[0] = playerHealth[0];
		resolveEffects(playerEffects, playerHealth);
		outputLog2[0] -= playerHealth[0];


	}
}

//function that handles player death
void death() {

	//refreshes display so that playerhealth is shown at 0 or below
	display();

	//outputs score and informs that you lost
	std::cout << "You lost :(\nYour score is " + std::to_string(score);

	//need to replace this eventually with just deleting a savefile containing information related only to the current game state
	//this way another savefile can be used to store score values / newgame+ information eventually maybe
	//checks if save file exists and deletes it
	if (fileCheck("savefile.txt")) {
		std::remove("savefile.txt");
	}

	//calls pause function before restarting
	pause();

	//resets output log
	for (i = 0; i < outputLog[0][0]; i++) {
		outputLog[i + 1][0] = 0;
		outputLog[i + 1][1] = 0;
	}
	outputLog[0][0] = 0;

	//resets health
	playerHealth[0] = 50;

}
