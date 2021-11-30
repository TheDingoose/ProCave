#pragma once
#include <vector>

enum Stores {
	EnvironmentCollision = 0,
	End
};


static class Raccoon
{
public:
	Raccoon() {
		//When do I initialize the vector??

		//This may not work
		for (int i = 0; i < Stores::End; i++) { Store.push_back(new std::vector<void*>); }
	};

	~Raccoon() {}; //Make sure to do this!

	std::vector<std::vector<void*>*> Store;

	unsigned int Put(Stores aStore, void* Adress) {
		Store[aStore]->push_back(Adress);
	}

	template <class t>
	t* Get(Stores aStore, unsigned int Place) {
		return Store[aStore][Place];
	}

	void Delete(Stores aStore, unsigned int Place) {
		Store[aStore]->erase(Store[aStore]->begin() + Place);
		return;
	}

	//static/singleton
	//Have the array of arrays -> void*
	//Functions to get, set, delete
	




};

