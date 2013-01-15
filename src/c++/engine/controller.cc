#include "controller.h"
keySetting::keySetting()
{
	trigger.type = 0;
	effect = 0;
}

void controller::setKey(int effect) 
{
	SDL_Event temp;
	bool configFlag = 0;

	while(SDL_PollEvent(&temp));
	while (!configFlag){
		if (SDL_PollEvent(&temp)) {
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
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jaxis.which = temp.jaxis.which;
			input[workingIndex]->trigger.jaxis.axis = temp.jaxis.axis;
			input[workingIndex]->trigger.jaxis.value = temp.jaxis.value;
		}
		break;
	case SDL_JOYBUTTONDOWN:
		input.push_back(new keySetting);
		workingIndex = input.size() - 1;
		input[workingIndex]->trigger.type = temp.type;
		input[workingIndex]->trigger.jbutton.which = temp.jbutton.which;
		input[workingIndex]->trigger.jbutton.button = temp.jbutton.button;
		break;
	case SDL_KEYDOWN:
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
