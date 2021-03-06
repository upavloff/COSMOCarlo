#ifndef DEF_STATE
#define DEF_STATE


#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cmath>
#include <map>

//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
class State 
{

	protected :

	std::vector<int> sequence;		//pi - elimination sequence - maxsize = nbCandidates
	std::vector<int> candidatesLeft;	//candidate not yet eliminated	
	int nbCandidates;
	int nbVoters;
	int nbCandidatesLeft;
	std::vector<std::vector<int>> votersPreferences;	// size >> m*n -- maybe use an array rather than a vector
	//int nbPrefs;	//m*n -- maybe use an array rather than a vector
	uint hash = 0;		//use hash in play function
	std::vector<std::vector<uint>> hashTable;// = std::vector<std::vector<uint>>(2); 	//hashTable[O] for candidates et [1] for voters


	public :

	//int can be changed by using template if borda is not the only rule
	int virtual score()
	{
		int winner = candidatesLeft[0];
		
		int res = 0;
		for (int i = 0; i < nbVoters; ++i)
		{
			res += votersPreferences[i][winner] ;		// borda score
		}

		return res;
	}

	int virtual score(int winner)
	{
		int res = 0;
		for (int i = 0; i < nbVoters; ++i)
		{
			res += votersPreferences[i][winner] ;		// borda score
		}

		return res;
	}


	int virtual playout()	//sincere playout 
	{
		if (nbCandidatesLeft < 2) return score(candidatesLeft[0]);
		std::vector<int> candidatesLeftCopy = candidatesLeft;
		//for completing the sequence
		for (int i = sequence.size() - 1; i < nbCandidates-2; ++i)	//maybe nbCandidates - 1
		{
			int votersToChose = rand() % nbVoters;

			eraseLeastPreferedSincere(votersToChose, candidatesLeftCopy);
		}
		return score(candidatesLeftCopy[0]);
	}

	int virtual result()
	{
		return score();
	}


	//true if element find, index becomes the index of the element
	bool findElement(std::vector<int> &v, int key, int &index){
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			if (v[i] == key){
				index = i;
				return true;
			}
		}
		return false;
	}

	//return the index of the best element
	void findBestElement(std::vector<int> &v, int &index)
	{
		int max=-1;
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			if (v[i] > max){
				index = i;
				max = v[i];
			}
		}
	}

	void findWorstElement(std::vector<int> &v, int &index)
	{
		int min=10000000;
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			if (v[i] < min){
				index = i;
				min = v[i];
			}
		}
	}

	//return the index of the best element
	int findBestElement(std::map<int,uint> &m, std::map<int,uint>::iterator &it)
	{
		it = m.begin();
		uint max=it->second;
		int candidate = it->first;
		for (std::map<int,uint>::iterator i=++m.begin(); i!=m.end(); i++)
		{
			if (i->second > max){
				max = i->second;
				candidate = i->first;
				it = i;
			}
		}
		return candidate;
	}

	//change type of return with template
	int optimumScore(){
		int max = 0;
		int currenScore;
		for (int i = 0; i < nbCandidates; ++i)
		{
			currenScore = State::score(i);
			if (currenScore > max)
			{
				max = currenScore;
			}
		}
		return max;
	}

	//maximal score reachable, change value of its argument
	void optimumScore(int &bestCandidate, int &max){
		int currenScore;
		for (int i = 0; i < nbCandidates; ++i)
		{
			currenScore = State::score(i);
			if (currenScore > max)
			{
				max = currenScore;
				bestCandidate = i;
			}
		}
	}

	//add voter to the final sequence and erase his sincere worst candidate
	void addVoterToSequence(int voter){
		sequence.push_back(voter);
		int candidateElminated = eraseLeastPreferedSincere(voter, candidatesLeft);
		//change hash
		hash ^= hashTable[candidateElminated][voter] ;
	}

	void virtual action(int  choice)
	{
		addVoterToSequence(choice);
	}


	int eraseLeastPreferedSincere(int const &voter, std::vector<int> &candidatesLeft){
		int min = 10000000;
		int leastPreferedCandidate = -1;
		int indexLeastPreferedCandidate = -1;
		for (int i = 0; i < nbCandidatesLeft; ++i)
		{
			int & currentCandidate = candidatesLeft[i];
			if (votersPreferences[voter][currentCandidate] < min)
			{
				min = votersPreferences[voter][currentCandidate];
				leastPreferedCandidate = currentCandidate;
				indexLeastPreferedCandidate = i;
			}
		}
		candidatesLeft.erase(candidatesLeft.begin()+indexLeastPreferedCandidate);
		nbCandidatesLeft--;
		return leastPreferedCandidate;
	}

	int eraseLeastPreferedSincere(int const &voter){
		return eraseLeastPreferedSincere(voter, candidatesLeft);
	}

	int virtual eraseLeastPreferedSincere(){
		int voter = rand()%nbVoters;
		return eraseLeastPreferedSincere(voter);
	}

	//display a given vector
	template <typename T>
	static void displayVec(std::vector<T> &v){
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			std::cout << v[i] << " ";
		}
		std::cout << std::endl;
	}

	template <typename A, typename B>
	void displayMap(std::map<A,B> &m) const{
		for (auto const &elem : m )
		{
			std::cout << elem.first << ":" << elem.second << " ";
		}
		std::cout << std::endl;
	}

	//true if only there is only one candidate left
	bool terminal() const{
		if (nbCandidatesLeft>1) return false;
		return true;
	}

	//given a sequence eliminate round by round the least 
	//prefered candidate for each voter
	int executeSincereSequence( std::vector<int> &v ) {
		std::vector<int> candidatesLeft;
		for (std::size_t i = 0; i < v.size()+1; ++i)
		{
			candidatesLeft.push_back(i);
		}
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			eraseLeastPreferedSincere(v[i], candidatesLeft);
		}
		return candidatesLeft[0];
	}

	uint getHash() const{
		return hash;
	}

	int getNbVoters() const{
		return nbVoters;
	}

	int getNbCandidates() const{
		return nbCandidates;
	}

	int getNbCandidatesLeft() const{
		return nbCandidatesLeft;
	}

	std::vector<int> getCandidatesLeft(){
		return candidatesLeft;
	}

	std::vector<int> getSequence() const{
		return sequence;
	}

	int virtual getNbChild() const
	{
		return nbVoters;
	}

	//State virtual getChlid(int index);

};

#endif
