#include <iostream>
#include <ctime>

using namespace std;

/**Simple Welcome Message*/
void printIntroduction(int difficulty){
	//Welcome message
	cout<<"You are a secret agent breaking into a level "<<difficulty<<" secure server room . . ."<<endl;
	cout<<endl;
	cout<<"Enter the correct code to continue . . ."<<endl;
}

/**Array value setting, used by sort3NumbersSequence*/
void assignArray(int array[3], int x0, int x1, int x2){
	array[0]=x0;
	array[1]=x1;
	array[2]=x2;
}

/**Decision Tree based Insertion Sort, for a 3 numbers sequence sorting*/
void sort3NumbersSequence(int sequence[3], int sortedSequence[3]){
	if(sequence[0]<=sequence[1]){
		if(sequence[1]>sequence[2]){
			if(sequence[0]<=sequence[2])
				assignArray(sortedSequence, sequence[0], sequence[2], sequence[1]);
			else
				assignArray(sortedSequence, sequence[2], sequence[0], sequence[1]);
		}
		else
			assignArray(sortedSequence, sequence[0], sequence[1], sequence[2]);	
	}
	else{
		if(sequence[1]<=sequence[2]){
			if(sequence[0]<=sequence[2])
				assignArray(sortedSequence ,sequence[1], sequence[0], sequence[2]);
			else
				assignArray(sortedSequence ,sequence[1], sequence[2], sequence[0]);
		}
		else
				assignArray(sortedSequence ,sequence[2], sequence[1], sequence[0]);
	}
}

/**index based check on the sequence*/
bool isSameCode(int sequence1[3], int sequence2[3]){
	for(int i=0; i<3; i++){
		if(sequence1[i]!=sequence2[i]) return false;
	}
	return true;
}

/**Effective match function.
 * Sequences are sorted first to allow a index based test*/
bool theyMatch(int sequence1[3], int sequence2[3]){
	int sorted1[3]={0,0,0}, sorted2[3]={0,0,0};
	sort3NumbersSequence(sequence1, sorted1);
	sort3NumbersSequence(sequence2, sorted2);
	/*cout<<"\t\t\t\t playerGuess "<<sequence1[0]<<" "<<sequence1[1]<<" "<<sequence1[2]<<endl;
	cout<<"\t\t\t\t secretCode "<<sequence2[0]<<" "<<sequence2[1]<<" "<<sequence2[2]<<endl;
	cout<<"\t\t\t\t SortedPlayerGuess "<<sorted1[0]<<" "<<sorted1[1]<<" "<<sorted1[2]<<endl;
	cout<<"\t\t\t\t SortedSecretCode "<<sorted2[0]<<" "<<sorted2[1]<<" "<<sorted2[2]<<endl;*/
	return isSameCode(sorted1, sorted2);
}

//				Generators.. L=level. (/) is the balance of Probability between the first half and the second of the interval
	
	/**First Generator: UNFAIR RANDOM
	 * rand in [1; 2L];
	 * random value Unfair in [0; L] which gives an extra chance to the last (Unfair) values in [1; 2L]
	 * (/) IN [1; 2]
	*/
	int getUnfairRandom(int difficulty, int unfairness){
		const int range = (2*difficulty)+unfairness;
		int ret = 1+rand()%range;
		if( ret > 2*difficulty )
			return ret-unfairness;
		else
			return ret;
		
	}

	void unfairRandomGeneration(int code[3], int difficulty){
		int unfairness = rand()%(difficulty+1);
		for(int i=0; i<3; i++)
			code[i]=getUnfairRandom(difficulty, unfairness);
	}


	//Summatory generators	
	int sommatory_1_n(int n){
		return (n*(n+1))/2;
	}

	int binarySearchOnSommatory_i_n(int x, int i, int n){
		if(i!=n){
			int middlePoint = (i+n)/2;
			int middlePointSummatory = sommatory_1_n(middlePoint);
			//cout<<endl<<"middlePoint ="<<middlePoint<<" midSumm="<<middlePointSummatory<<" x="<<x<<" i="<<i<<" n="<<n<<endl;
			if(x>middlePointSummatory){
				//cout<<"x>middlePointSumm. return binarySearchOnSommatory_i_n("<<x<<", "<<middlePoint+1<<", "<<n<<")"<<endl;
				return binarySearchOnSommatory_i_n(x, middlePoint+1, n);
			}
			else if(x<middlePointSummatory){
				//cout<<"x<middlePointSumm. return binarySearchOnSommatory_i_n("<<x<<", "<<i<<", "<<middlePoint <<")"<<endl;
				return binarySearchOnSommatory_i_n(x, i, middlePoint);
			}
			else{
				return middlePoint;
			}
		}
		return i;
	}


	/**Second Generator: SUMMATORY DISPOSITION
	 * rand in [1; 2L];
	 * each value v has Σ1_v possibilities on Σ1_2L
	 * (/) IN [2; 2.66..]
	*/
	int getSommatoryRandom(int difficulty){
		int random = 1+rand()%sommatory_1_n(2*difficulty);
		random = binarySearchOnSommatory_i_n(random, 1, 2*difficulty);
		return random;
	}

	void SummatoryDispositionGeneration(int code[3], int difficulty){
		for(int i=0; i<3; i++)
			code[i]=getSommatoryRandom(difficulty);
	}


	/**Third Generator: SUMMATORY DISPOSITION UNFAIR CUCSHIONING
	 * rand in [1; 2L];
	 * each value v has Σ1_v possibilities on Σ1_2L
	 * random value Unfair in [0; L] which gives an extra chance to the first (Unfair) values in [1; 2L]
	 * (/) IN [1; 2.66..]
	*/
	int getSommatoryUnfairCushioningRandom(int difficulty, int unfairness){
		const int summatory=sommatory_1_n(2*difficulty);
		int random = 1+rand()%(summatory+unfairness);
		if(random > summatory) //Unfair Cushioning
			return random-summatory;
		else
			return binarySearchOnSommatory_i_n(random, 1, 2*difficulty);
	}

	void SummatoryDispositionUnfairCushioningGeneration(int code[3], int difficulty){
		for(int i=0; i<3; i++){
			int unfairness = rand()%(difficulty+1);
			code[i]=getSommatoryUnfairCushioningRandom(difficulty, unfairness);
		}
	}


	/**Fourth Generator: SUMMATORY DISPOSITION CONSTANT AND UNFAIR CUCSHIONING
	 * rand in [1; 2L];
	 * if v is in [L+1; 2L]: Σ1_v chances
	 * else: 1+Σ1_v chances but..
	 * random value Unfair in [0; L] which gives an extra chance to the first (Unfair) values in [1; 2L]
	 * so there are: unfair+L+Σ1_v chances
	 * (/) IN [0.66..; 2]
	*/
	int getSommatoryConstantAndUnfairCushioningRandom(int difficulty, int unfairness){
		const int summatory=sommatory_1_n(2*difficulty);
		int random = 1+rand()%(summatory+difficulty+unfairness);
		if(random > summatory){ //Cushioning
			if(random-summatory<=difficulty)
				return random-summatory;
			else
				return (random-summatory)-difficulty;
		}
		else
			return binarySearchOnSommatory_i_n(random, 1, 2*difficulty);
	}

	void SummatoryDispositionConstantAndUnfairCushioningGeneration(int code[3], int difficulty){
		for(int i=0; i<3; i++){
			int unfairness = rand()%(difficulty+1);
			code[i]=getSommatoryConstantAndUnfairCushioningRandom(difficulty, unfairness);
		}
	}

void generateRandomCode(int secretCode[3], int difficulty, int generator=0){
	if(generator==0) generator=1+rand()%4;
	switch (generator)
	{
	case 1:
		unfairRandomGeneration(secretCode, difficulty);
		break;
	case 2:
		SummatoryDispositionGeneration(secretCode, difficulty);
		break;
	case 3:
		SummatoryDispositionUnfairCushioningGeneration(secretCode, difficulty);
		break;
	case 4:
		SummatoryDispositionConstantAndUnfairCushioningGeneration(secretCode, difficulty);
		break;
	}
}

bool playGame(int difficulty, int *HP){
	printIntroduction(difficulty);

	//Declaring secret code
	int secretCode[3];
	generateRandomCode(secretCode, difficulty, 4);
	int secretCodeSum = secretCode[0] + secretCode[1] + secretCode[2];
	int secretCodeProduct = secretCode[0] * secretCode[1] * secretCode[2];

	//Puzzle message
	cout<<"	The secret code is made of 3 numbers..."<< endl;
	cout<<"	Those numbers add-up to: "<< secretCodeSum << endl;
	cout<<"	and multiply to give: "<< secretCodeProduct << endl;

	//Player Guess
	int playerGuess[3];
	cout<<" - - You carefully digit ";
	cin>>playerGuess[0];
	cin>>playerGuess[1];
	cin>>playerGuess[2];
	system("cls");
	cout<<"\tYou carefully digited "<<playerGuess[0]<<" "<<playerGuess[1]<<" "<<playerGuess[2]<<endl;
	int playerGuessSum = playerGuess[0] + playerGuess[1] + playerGuess[2];
	int playerGuessProduct = playerGuess[0] * playerGuess[1] * playerGuess[2];

	//Check result
	if(playerGuessSum==secretCodeSum && playerGuessProduct==secretCodeProduct){
		if(isSameCode(playerGuess, secretCode)){
			 (*HP)++;
			 cout<<"Greetings! ";
			 return true;
		}
		else if(theyMatch(playerGuess, secretCode)){
			cout<<"Congratulations! ";
			return true;
		}
		else{
			cout<<"How unlucky! sum and product match but the codes are not the same"<<endl;
			cout<<"\tYou digited "<<playerGuess[0]<<" "<<playerGuess[1]<<" "<<playerGuess[2]<<" but the code was "<<secretCode[0]<<" "<<secretCode[1]<<" "<<secretCode[2]<<endl;
			return false;
		}
	}
	else{
		cout<<"So sad.. ";
		if(playerGuessSum!=secretCodeSum)
			cout<<"The sum is not "<<secretCodeSum<<" because "<<playerGuess[0]<<"+"<<playerGuess[1]<<"+"<<playerGuess[2]<<"="<<playerGuessSum<<endl;
		if(playerGuessProduct!=secretCodeProduct){
			if(playerGuessSum!=secretCodeSum) cout<<"Moreover.. ";
			cout<<"The product is not "<<secretCodeProduct<<" because "<<playerGuess[0]<<"*"<<playerGuess[1]<<"*"<<playerGuess[2]<<"="<<playerGuessProduct<<endl;
		}
		return false;
	}
}

bool isAlive(int HP){
	return HP>0;
}

bool areThereLevelsLeft(int level, int maxLevel){
	return level<=maxLevel;
}

bool continueGame(int HP, int level, int maxLevel){
	return isAlive(HP) && areThereLevelsLeft(level, maxLevel);
}

int main(){
	system("cls");
	srand(time(NULL));
	int levelDifficulty=1;
	const int maxDifficulty = 5;
	bool levelComplete=true;
	int healthPoints=1;
	while(continueGame(healthPoints, levelDifficulty, maxDifficulty)){
		if(playGame(levelDifficulty, &healthPoints)){
			levelDifficulty++;
			if(areThereLevelsLeft(levelDifficulty, maxDifficulty))
				cout<<"You deciphred the secret code and a hidden door opens showing you the access to level "<<levelDifficulty<<endl;
			else
				cout<<"You deciphred the last code."<<endl;
		}
		else{
			healthPoints--;
			cout<<"The alarm expolde loudly and you hear footstep approaching."<<endl;
			cout<<" You hide beneath the desk and pray they don't find you.\n";
			if(isAlive(healthPoints)){
				cout<<" As the guards break in the room you hold your breath; the look rapidly in the room but seeing no one they leave."<<endl;
				cout<<"In a few minutes the alarm stops and you are alone in the silent room again\n";
			}
		}
		cin.clear();
		cin.ignore();
	}
	if(isAlive(healthPoints))
		cout<<"\tSuddenly a message appear inside your eyeglasses:\n\t\t\t\t Mission Completed Agent!\n";
	else{
		cout<<"Once again you hold your breath; The doors slowly opens but no man seem to be outside"<<endl;
		cout<<"Suddenly you notice some marks on the wood of the desk: \"You can't hide under a desk.\""<<endl;
		cout<<"Horrified, you focus to hold a gasp but is useless effort. No more sound will come out of your mouth.."<<endl;
		cout<<"Before your memories gently drift away you notice a stack of corpse in the corner of the room, and you are being brought there too."<<endl;
	}
	cout<<endl<<"Game over.. "<<endl<<endl;
	system("pause");
	return 0;
}