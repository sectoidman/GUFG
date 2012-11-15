#ifdef TRAINER_INCLUDED
#define TRAINER_INCLUDED
class trainer : public controller{
public:
	int HP;
	organism * party[6];
};
#endif
