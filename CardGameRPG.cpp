#include <iostream> // std::cin, std::cout
#include <cstdlib> // std::srand, std::rand, std::exit
#include <fstream> // std::ifstream, std::ofstream
#include <sstream> // std::stringstream
#include <string> // std::string, std::stoi
#include <cmath> // std::trunc
#include <ctime> // std:time

//rewrote without "using namepsace std;" to improve scalability
//using namespace std;

//notes
//Note for Computer Science I CSC1060C05 and myself tbh-------------------------------
// I do not plan to finish all of what is listed in my notes by the end of the semester most of these notes I'm leaving for later the
// plan is to continue working on this going forward and keep reiterating as I learn new things so I can see how it changes over time.
// My expectation is that I will need to rework this program many times as I learn which things can be done better. I have also just 
// found this quite fun to work on every once in awhile, most other programs I've made were not things I wanted to make.
//------------------------------------------------------------------------------------

//General Notes-----------------------------------------------------------------------
//These are changes I plan to make to the program and can be removed.
//
// New Systems--------------
// add buffs
// add card discard system
// add deck system - deck with rotating set cards more like an actual card game - this will be needed more for later systems I want to add
// add system that lets you add a new card to your deck after defeating an enemy
// many more systems I have planned for later but these here are imo the most impactful
//
// Notes--------------------
// wait for more systems to balance combat
// move all cout calls to the display function so it can later be reworked to call graphics APIs for a better UI
// 
// Progression Path---------
//  -- WIP
//  -- continuously update visual errors that will be harder to fix later ( Ex. changing enemy health so it never displays as negative )
//  -- redesign to make use of classes
//  -- redesign to make use of header files to clean up main .cpp file and improve scalabilty/compile times going forward
//  -- consider work and benefit of a rewrite to C# -- currently leaning towards C++
//	-- add new systems relating to the combat
//  -- add new systems relating to newgame+
//  -- at this point the "game" should be finished and running well and next should be making the visual interface
//  -- start developing a structure to make use of graphics APIs to make a GUI - using DirectX12 is something i'm interested in
//------------------------------------------------------------------------------------

//creating global variables/arrays------------------------------------------------------------------------------------------------
//temp variables for storing random values
int i;
int temp[3];
std::string tempString;
std::string outputString;

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
std::string tolower(std::string inputString);
std::string input(int user);
//display functions
void help(); 
void menu();
void display();
//save and load functions 
void save();
void load();
//generation functions
void generateEnemy();
void generateCard(int cardArray[][3], int cardn);
//gameplay functions
void draw(int cardArray[][3], int mana);
void discard(int cardArray[][3], int user);
void restoreCard(int cardArray[][3]);
void discardCard(int cardArray[][3], int cardn);
void drawCard(int cardArray[][3]);
void playCard(int cardArray[][3], int cardn, int friendlyHealth[2], int friendlyEffects[][2], int opponentHealth[2], int opponentEffect[][2]);
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

		//calls function to handle player death
		death();
	}

}

//core functions------------------------------------------------------------------------------------------------------------------

//function to exit the game
void exit() {
	std::exit(0);
	//save();
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

//function that takes string as an input and converts each uppercase character to lowercase then returns the string
std::string tolower(std::string inputString) {

	//finds length of string and creates a loop to check every values
	for (i = 0; i < std::size(inputString); i++) {
		//checking if the value is in range of uppercase letters
		if (inputString[i] < 90 && inputString[i] > 64) {
			//adds 32 to change ascii value to the lowercase equivilant
			inputString[i] += 32;
		}
	}

	return inputString;
}

//function to get input
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

//display functions---------------------------------------------------------------------------------------------------------------

//currently empty function that should present help information and be exitable back to where the game was at.
void help() {
	clear();
	std::cout << "Cards can be choosen by entering the card number shown on the left of the console.\n";
	std::cout << "Every card has a mana cost, effects cards always cost 2, damage/heal cards cost atleast 1 or their value divided by 3.\n";
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

	clear();

	bool validInput = false;
	while (!validInput) {

		//short-ish help prompt
		std::cout << "Pick cards to fight enemies, upon defeat your score will increase\n";
		std::cout << "You can play a card/draw/pass/discard, costs are listed and draw costs 2 mana.\n";
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
			exit();
		}
		//if user types help
		else if (tempString == "help") {
			help();
			clear();
		}
		//if user types something else
		else {
			std::cout << "Invalid Input";
		}
	}

}

//function to display all relevant data needed
void display() {

	//calls function to clear console
	clear();

	//print player and enemy health
	std::cout << "HP: " << playerHealth[0] << "/50" << "||||||||||" << "Enemy: " << enemyHealth[0] << "/" << enemyHealth[1] << "\n";

	//prints player mana
	std::cout << "Mana: " << playerMana << "\n";

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
	saveString += std::to_string(score) + ' ';
	saveString += std::to_string(roundCounter) + ' ';
	saveString += std::to_string(playerHealth[0]) + ' ';
	saveString += std::to_string(playerHealth[1]) + ' ';
	saveString += std::to_string(enemyHealth[0]) + ' ';
	saveString += std::to_string(enemyHealth[1]) + ' ';
	saveString += std::to_string(playerMana) + ' ';
	saveString += std::to_string(enemyMana) + ' ';

	//adding amount of player/enemy cards and effects
	saveString += std::to_string(playerCardsArray[0][0]) + ' ';
	saveString += std::to_string(enemyCardsArray[0][0]) + ' ';
	saveString += std::to_string(playerEffects[0][0]) + ' ';
	saveString += std::to_string(enemyEffects[0][0]) + ' ';

	//adding each card and effect value to the save string
	for (i = 0; i < playerCardsArray[0][0]; i++) {
		saveString += std::to_string(playerCardsArray[i + 1][0]) + ' ';
		saveString += std::to_string(playerCardsArray[i + 1][1]) + ' ';
		saveString += std::to_string(playerCardsArray[i + 1][2]) + ' ';
	}
	for (i = 0; i < enemyCardsArray[0][0]; i++) {
		saveString += std::to_string(enemyCardsArray[i + 1][0]) + ' ';
		saveString += std::to_string(enemyCardsArray[i + 1][1]) + ' ';
		saveString += std::to_string(enemyCardsArray[i + 1][2]) + ' ';
	}
	for (i = 0; i < playerEffects[0][0]; i++) {
		saveString += std::to_string(playerEffects[i + 1][0]) + ' ';
		saveString += std::to_string(playerEffects[i + 1][1]) + ' ';
	}
	for (i = 0; i < enemyEffects[0][0]; i++) {
		saveString += std::to_string(enemyEffects[i + 1][0]) + ' ';
		saveString += std::to_string(enemyEffects[i + 1][1]) + ' ';
	}

	fout << saveString;

	//informing user game is saved and exiting
	std::cout << "Saved";
	exit();
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
		//saving savefile to a string
		std::stringstream buffer;
		buffer << fin.rdbuf();
		std::string fileContents;
		fileContents = buffer.str();

		//savefile always stores at least 12 values from 0-11
		//spliting savefileString into an array splitting by ' '
		std::stringstream buffer2(fileContents);
		for (i = 0; i < 12; i++) {
			buffer2 >> tempString;
			savefileText[i] = tempString;
		}
		//-------------------------------------


		//order the values are stored is always the same and so can be reset in order
		//storing values from file to variables
		score = std::stoi(savefileText[0]);
		roundCounter = std::stoi(savefileText[1]);
		playerHealth[0] = std::stoi(savefileText[2]);
		playerHealth[1] = std::stoi(savefileText[3]);
		enemyHealth[0] = std::stoi(savefileText[4]);
		enemyHealth[1] = std::stoi(savefileText[5]);
		playerMana = std::stoi(savefileText[6]);
		enemyMana = std::stoi(savefileText[7]);

		//sets values for the number of cards/effects in each array
		playerCardsArray[0][0] = std::stoi(savefileText[8]);
		enemyCardsArray[0][0] = std::stoi(savefileText[9]);
		playerEffects[0][0] = std::stoi(savefileText[10]);
		enemyEffects[0][0] = std::stoi(savefileText[11]);

		//recreating arrays
		//loops that goes for as many cards as were stored in the save file
		for (i = 0; i < playerCardsArray[0][0]; i++) {
			//adds the next three values in the buff to the savefileText array
			for (temp[0] = 0; temp[0] < 3; temp[0]++) {
				buffer2 >> tempString;
				savefileText[temp[0]] = tempString;
			}
			//adds the three values to the cardn based off the number of loops
			playerCardsArray[i + 1][0] = std::stoi(savefileText[0]);
			playerCardsArray[i + 1][1] = std::stoi(savefileText[1]);
			playerCardsArray[i + 1][2] = std::stoi(savefileText[2]);
		}
		for (i = 0; i < enemyCardsArray[0][0]; i++) {
			for (temp[0] = 0; temp[0] < 3; temp[0]++) {
				buffer2 >> tempString;
				savefileText[temp[0]] = tempString;
			}
			enemyCardsArray[i + 1][0] = std::stoi(savefileText[0]);
			enemyCardsArray[i + 1][1] = std::stoi(savefileText[1]);
			enemyCardsArray[i + 1][2] = std::stoi(savefileText[2]);
		}
		for (i = 0; i < playerEffects[0][0]; i++) {
			for (temp[0] = 0; temp[0] < 3; temp[0]++) {
				buffer2 >> tempString;
				savefileText[temp[0]] = tempString;
			}
			playerEffects[i + 1][0] = std::stoi(savefileText[0]);
			playerEffects[i + 1][1] = std::stoi(savefileText[1]);
		}
		for (i = 0; i < enemyEffects[0][0]; i++) {
			for (temp[0] = 0; temp[0] < 3; temp[0]++) {
				buffer2 >> tempString;
				savefileText[temp[0]] = tempString;
			}
			enemyEffects[i + 1][0] = std::stoi(savefileText[0]);
			enemyEffects[i + 1][1] = std::stoi(savefileText[1]);
		}

		//closes file
		fin.close();

		//deletes old savefile
		if (fileCheck("savefile.txt")) {
			std::remove("savefile.txt");
		}

	}

}

//generation functions------------------------------------------------------------------------------------------------------------

//function to generate enemy variables generates health and card if passed 0 and only card if passed 1
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
void draw(int cardArray[][3], int mana) {

	//calls function to draw another card
	drawCard(cardArray);

	//updates mana
	mana -= 2;

	//refreshes display
	display();
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
			try {
				discardCard(cardArray, std::stoi(tempString));
			}
			//checks if string is equal to cancel if converiting to int fails
			catch (...) {
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
		//draws new card if loop was no canceled
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

//will later be reworked to draw from the bottom of the deck
//function to restore card
void restoreCard(int cardArray[][3]) {
	cardArray[0][0]++;
}

//generic discard functoin
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

//generic draw function
//function used to draw a new card
void drawCard(int cardArray[][3]) {

	generateCard(cardArray, cardArray[0][0] + 1);
	cardArray[0][0] += 1;

}

//generic and player play card function
//function to play a card
void playCard(int cardArray[][3], int cardn, int friendlyHealth[2], int friendlyEffects[][2], int opponentHealth[2], int opponentEffect[][2]) {

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

	//removes card from play
	discardCard(cardArray, cardn);
}

//combat functions----------------------------------------------------------------------------------------------------------------

//function to calculate damage from effects
void resolveEffects(int effectArray[][2], int health[2]) {

	//resolving effects on player
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

	//resolving effects on enemy
	for (i = 0; i < enemyEffects[0][0]; i++) {

	}


}

//function to resolve enemy combat - will be mostly replaced by playCard function - should handle enemy AI
void enemyCombat() {
	//resolves effect of enemy card
	//switch to play different card types
	//sets i to zero so it can be used for output log
	i = 0;

	while (enemyMana >= enemyCardsArray[1][2]) {

		//removes mana for playing card
		enemyMana -= enemyCardsArray[1][2];

		//adds the card type used and value to the output log
		outputLog[outputLog[0][0] + 1][0] = enemyCardsArray[1][0];
		outputLog[outputLog[0][0] + 1][1] = enemyCardsArray[1][1];

		//will add basic decision tree later but for now it plays its 1 card then draws a new one
		playCard(enemyCardsArray, 1, enemyHealth, enemyEffects, playerHealth, playerEffects);
		drawCard(enemyCardsArray);

		outputLog[0][0]++;
	}


}

//function to resolve player combat - needs to be extended to include having the player choose what action to take - then resolving effects
void playerCombat() {

	//turns on debuff info
	debuffInfo = true;

	//refreshes display
	display();

	//resets output log
	for (i = 0; i < outputLog[0][0]; i++) {
		outputLog[i + 1][0] = 0;
		outputLog[i + 1][1] = 0;
	}
	outputLog[0][0] = 0;

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
		try {
			temp[0] = std::stoi(tempString);
		}
		catch (...) {
		}

		//sets the inputstring to all lowercase
		tempString = tolower(tempString);

		//checks if all values are within the ranges
		if (temp[0] > 0 && temp[0] <= playerCardsArray[0][0] && playerCardsArray[temp[0]][2] <= playerMana) {

			//updates mana
			playerMana -= playerCardsArray[temp[0]][2];

			//calls function resolving effect from selected player card
			playCard(playerCardsArray, temp[0], playerHealth, playerEffects, enemyHealth, enemyEffects);

			//refreshes display
			display();
		}
		else if (tempString == "draw" && playerMana >= 2) {
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

	//refreshes display
	display();
}

//function that loops until player death, handles enemy regeneration and switching between player and enemy turns
void combat() {

	while (playerHealth[0] > 0) {
		//checks if enemy health is below 0
		if (enemyHealth[0] < 1) {

			//regenerates enemy
			std::cout << "You beat an enemy!";
			roundCounter++;
			generateEnemy();

			//calls pause function
			pause();

			//adds one to score
			score += roundCounter;

			//calls function to refresh display so that new enemy is shown
			display();
		}
		//resets playerMana and starts players turn
		//calls function to resolve player actions
		playerMana = 7;
		playerCombat();

		//resets enemyMana and starts enemys turn
		//calls function to start enemys turn
		enemyMana = 5;
		enemyCombat();

		//sets outputlog to the current health value, resolves effects damage, then subtracts current health value to get damage taken
		//resolves effects on the player
		outputLog2[0] = playerHealth[0];
		resolveEffects(playerEffects, playerHealth);
		outputLog2[0] -= playerHealth[0];

		//resolves effects on the enemy
		outputLog2[1] = enemyHealth[0];
		resolveEffects(enemyEffects, enemyHealth);
		outputLog2[1] -= enemyHealth[0];


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
