#include "controller.h"
keySetting::keySetting()
{
	trigger.type = 0;
	effect = 0;
}

script * controller::patternMatch(std::vector<int> pos)
{
	for(unsigned int j = 0; j < macro.size(); j++){
		bool fail = false;
		for(int i = 0; i < 5; i++){
			if(((pos[i] == 1) << i) & pattern[j]){
				fail = true;
			}
		}
		if(!fail) return macro[j];
	}
	return NULL;
}

void controller::setKey(int effect)
{
	SDL_Event temp;
	bool configFlag = 0;
	while(SDL_PollEvent(&temp));
	while (!configFlag){
		if (SDL_PollEvent(&temp)) {
			for(int i = 0; i < input.size(); i++){
				if(input[i]->effect == effect) input.erase(input.begin()+i);
			}
			configFlag = setKey(effect, temp);
		}
	}
}

/*This function takes an event and a desired effect and maps them in the keySetting array*/
bool controller::setKey(int effect, SDL_Event temp)
{
	int workingIndex = -1;
	switch (temp.type){
	case SDL_JOYAXISMOTION:
		if(temp.jaxis.axis < 6 && temp.jaxis.value != 0){
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jaxis.which == temp.jaxis.which &&
				   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
				   input[i]->trigger.jaxis.value == temp.jaxis.value){
					input.erase(input.begin()+i);
					i = input.size();
				}
			}
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jaxis.which = temp.jaxis.which;
			input[workingIndex]->trigger.jaxis.axis = temp.jaxis.axis;
			input[workingIndex]->trigger.jaxis.value = temp.jaxis.value;
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.jbutton.which == temp.jbutton.which &&
			   input[i]->trigger.jbutton.button == temp.jbutton.button){
				input.erase(input.begin()+i);
				i = input.size();
			}
		}
		input.push_back(new keySetting);
		workingIndex = input.size() - 1;
		input[workingIndex]->trigger.type = temp.type;
		input[workingIndex]->trigger.jbutton.which = temp.jbutton.which;
		input[workingIndex]->trigger.jbutton.button = temp.jbutton.button;
		break;
	case SDL_KEYDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				input.erase(input.begin()+i);
				i = input.size();
			}
		}
		input.push_back(new keySetting);
		workingIndex = input.size() - 1;
		input[workingIndex]->trigger.type = temp.type;
		input[workingIndex]->trigger.key.keysym.sym = temp.key.keysym.sym;
		break;
	default:
		break;
	}
	if(workingIndex > -1){
		input[workingIndex]->effect = effect;
		return 1;
	} else return 0;
}

bool controller::same(SDL_Event temp)
{
	switch (temp.type){
	case SDL_KEYDOWN:
		if(input[0]->trigger.type == temp.type) return true;
	case SDL_JOYBUTTONDOWN:
		if(input[0]->trigger.type == SDL_JOYAXISMOTION && temp.jbutton.which == input[0]->trigger.jaxis.which) return true;
		else if(input[0]->trigger.type == SDL_JOYBUTTONDOWN && temp.jbutton.which == input[0]->trigger.jbutton.which) return true;
	case SDL_JOYAXISMOTION:
		if(input[0]->trigger.type == SDL_JOYAXISMOTION && temp.jaxis.which == input[0]->trigger.jaxis.which) return true;
		else if(input[0]->trigger.type == SDL_JOYBUTTONDOWN && temp.jaxis.which == input[0]->trigger.jbutton.which) return true;
	}
	return false;
}

void controller::swapKey(int effect, SDL_Event temp)
{
	int sameKey = -1, sameEffect = -1;
	for(unsigned int i = 0; i < input.size(); i++){
		if(input[i]->effect == effect){
			sameEffect = i;
			i = input.size();
		}
	}
	for(unsigned int i = 0; i < input.size(); i++){
		switch(temp.type){
		case SDL_KEYDOWN:
			if(input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				sameKey = i;
				i = input.size();
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if(input[i]->trigger.jbutton.button == temp.jbutton.button){
				sameKey = i;
				i = input.size();
			}
			break;
		case SDL_JOYAXISMOTION:
			if(input[i]->trigger.jaxis.axis == temp.jaxis.axis && input[i]->trigger.jaxis.value == temp.jaxis.value){
				sameKey = i;
				i = input.size();
			}
			break;
		default:
			return;
		}
	}
	if(sameEffect >= 0){
		if(sameKey >= 0){
			if(sameKey == sameEffect) return;
			input[sameEffect]->effect = input[sameKey]->effect;
			input[sameKey]->effect = effect;
		} else {
			setKey(effect, temp);
			input.erase(input.begin()+sameEffect);
		}
	} else {
		if(sameKey >= 0) input[sameKey]->effect = effect;
		else setKey(effect, temp);
	}
}

int controller::tap(SDL_Event temp)
{
	for(unsigned int i = 0; i < input.size(); i++){
		switch(temp.type){
		case SDL_KEYUP:
			if(input[i]->trigger.key.keysym.sym == temp.key.keysym.sym) 
				return -input[i]->effect;
		case SDL_KEYDOWN:
			if(input[i]->trigger.key.keysym.sym == temp.key.keysym.sym) 
				return input[i]->effect;
		case SDL_JOYBUTTONUP:
			if(input[i]->trigger.jbutton.which == temp.jbutton.which && 
			   input[i]->trigger.jbutton.button == temp.jbutton.button) 
				return -input[i]->effect;
		case SDL_JOYBUTTONDOWN:
			if(input[i]->trigger.jbutton.which == temp.jbutton.which && 
			   input[i]->trigger.jbutton.button == temp.jbutton.button) 
				return input[i]->effect;
		case SDL_JOYAXISMOTION:
			if(input[i]->trigger.jaxis.which == temp.jaxis.which &&
			   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
			   input[i]->trigger.jaxis.value == temp.jaxis.value)
				return input[i]->effect;
			if(input[i]->trigger.jaxis.which == temp.jaxis.which &&
			   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
			   input[i]->trigger.jaxis.value == 0)
				return -input[i]->effect;
		}
	}
	return 0;
}
