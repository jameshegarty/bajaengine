#ifndef IO_H
#define IO_H

#include "HelperLibString.hpp"

class KEYBOARD{
	private:
  	bool key_temp[256];
	
	public:

	bool keyDownMessage[256];
	bool keyUpMessage[256];
	
	bool key[256];
	bool key_down[256];
	bool key_up[256];

	bool shift;
	bool shift_down;
	bool shift_up;
	
	bool tab;
	bool tab_down;
	bool tab_up;
	
	bool caps;
	bool caps_down;
 	bool caps_up;
 	
	bool alt;
	bool alt_down;
	bool alt_up;
	
	bool option;
	bool option_down;
	bool option_up;
	
  	bool ctrl;
  	bool ctrl_down;
	bool ctrl_up;
	
	bool backspace;
  	bool backspaceDown;
	bool backspaceUp;
	
	bool enter;
  	bool enter_down;
  	bool enter_up;
  	
  	bool period;
  	bool period_down;
  	bool period_up;

	bool colon;
  	bool colonDown;
  	bool colonUp;

	bool semicolon;
  	bool semicolonDown;
  	bool semicolonUp;
  	
	bool deleteKey;
  	bool deleteKeyDown;
  	bool deleteKeyUp;
  	

  	bool underscore;
  	bool underscoreDown;
  	bool underscoreUp;
  	
  	
  	bool arrow_up;
  	bool arrow_up_down;
  	bool arrow_up_up;
  	
  	bool arrow_down;
  	bool arrow_down_down;
  	bool arrow_down_up;
  	
  	bool arrow_left;
  	bool arrow_left_down;
  	bool arrow_left_up;
  	
  	bool arrow_right;
  	bool arrow_right_down;
  	bool arrow_right_up;
  	
  	bool escape;
  	bool escape_down;
  	bool escape_up;
  	
  	bool page_up;
  	bool page_up_down;
  	bool page_up_up;
  	
  	bool page_down;
  	bool page_down_down;
  	bool page_down_up;
  	
  	bool space;
  	bool space_down;
  	bool space_up;
  	
	bool home;
  	bool home_down;
  	bool home_up;
  	
	bool end;
  	bool end_down;
  	bool end_up;

	bool parenthesisOpen;
	bool parenthesisOpenUp;
	bool parenthesisOpenDown;

	bool parenthesisClose;
	bool parenthesisCloseUp;
	bool parenthesisCloseDown;

	bool bracketOpen;
	bool bracketOpenUp;
	bool bracketOpenDown;

	bool bracketClose;
	bool bracketCloseUp;
	bool bracketCloseDown;

	bool comma;
	bool commaUp;
	bool commaDown;


	bool question;
	bool questionUp;
	bool questionDown;

	bool tilde;
	bool tildeUp;
	bool tildeDown;

	bool prime;
	bool primeUp;
	bool primeDown;

	bool equal;
	bool equalUp;
	bool equalDown;

	bool add;
	bool addUp;
	bool addDown;

	bool subtract;
	bool subtractUp;
	bool subtractDown;

	bool less;
	bool lessUp;
	bool lessDown;

	bool greater;
	bool greaterUp;
	bool greaterDown;

	bool quote;
	bool quoteUp;
	bool quoteDown;

	bool devide;
	bool devideUp;
	bool devideDown;

	bool multiply;
	bool multiplyUp;
	bool multiplyDown;

	bool backslash;
	bool backslashUp;
	bool backslashDown;

	bool slash;
	bool slashUp;
	bool slashDown;

	bool exclamation;
	bool exclamationUp;
	bool exclamationDown;

	bool at;
	bool atUp;
	bool atDown;

	bool pound;
	bool poundUp;
	bool poundDown;

	bool percent;
	bool percentUp;
	bool percentDown;

	bool carrot;
	bool carrotUp;
	bool carrotDown;

	bool ampersand;
	bool ampersandUp;
	bool ampersandDown;

	bool pipe;
	bool pipeUp;
	bool pipeDown;

	bool dollar;
	bool dollarUp;
	bool dollarDown;

	bool a;
	bool aUp;
	bool aDown;

	bool b;
	bool bUp;
	bool bDown;

	bool c;
	bool cUp;
	bool cDown;

	bool d;
	bool dUp;
	bool dDown;

	bool e;
	bool eUp;
	bool eDown;

	bool f;
	bool fUp;
	bool fDown;

	bool g;
	bool gUp;
	bool gDown;

	bool h;
	bool hUp;
	bool hDown;

	bool i;
	bool iUp;
	bool iDown;

	bool j;
	bool jUp;
	bool jDown;

	bool k;
	bool kUp;
	bool kDown;

	bool l;
	bool lUp;
	bool lDown;

	bool m;
	bool mUp;
	bool mDown;

	bool n;
	bool nUp;
	bool nDown;

	bool o;
	bool oUp;
	bool oDown;

	bool p;
	bool pUp;
	bool pDown;

	bool q;
	bool qUp;
	bool qDown;

	bool r;
	bool rUp;
	bool rDown;

	bool s;
	bool sUp;
	bool sDown;

	bool t;
	bool tUp;
	bool tDown;

	bool u;
	bool uUp;
	bool uDown;

	bool v;
	bool vUp;
	bool vDown;

	bool w;
	bool wUp;
	bool wDown;

	bool x;
	bool xUp;
	bool xDown;

	bool y;
	bool yUp;
	bool yDown;

	bool z;
	bool zUp;
	bool zDown;

	bool zero;
	bool zeroUp;
	bool zeroDown;

	bool one;
	bool oneUp;
	bool oneDown;

	bool two;
	bool twoUp;
	bool twoDown;
	
	bool three;
	bool threeUp;
	bool threeDown;
	
	bool four;
	bool fourUp;
	bool fourDown;
	
	bool five;
	bool fiveUp;
	bool fiveDown;
	
	bool six;
	bool sixUp;
	bool sixDown;
	
	bool seven;
	bool sevenUp;
	bool sevenDown;
	
	bool eight;
	bool eightUp;
	bool eightDown;

	bool nine;
	bool nineUp;
	bool nineDown;
	
  	bool up;
  	bool down;
  	
	
	char current_letter;
	char current_letter_down;
	int current_number;
	int current_number_down;
	int current_key_number;
	bool number_down_hit;
	
	bool update();
	
	String currentStringDown;
	String currentStringDownExtended;

	bool scriptInit();
	
	KEYBOARD();
};

extern KEYBOARD keyboard;

#endif
