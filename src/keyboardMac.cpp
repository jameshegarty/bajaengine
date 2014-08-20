
#include "keyboard.h" 
#include "conf.h"

#include "mouse.h"
#include "script.hpp"
#include "Helperlib/Log.hpp"

#include "Carbon/Carbon.h"

#include "endian.hpp"

#define VK_OPTION 244
#define VK_ALT 245
#define VK_CONTROL 246
#define VK_LEFT 247
#define VK_UP 248
#define VK_RIGHT 249
#define VK_DOWN 250
#define VK_INSERT 251
#define VK_HOME 252
#define VK_END 252
#define VK_PRIOR 253
#define VK_NEXT 254
#define VK_SHIFT 255

KEYBOARD keyboard;
bool KEYBOARD::update(){
	for(int li=0; li<256; li++){
		key_down[li]=false;
		key_up[li]=false;
	}
	
	KeyMap km;
	GetKeys(km);
	
	long km1;
	
	#ifdef __BIG_ENDIAN__
		km1=km[1];
	#else
		km1=km[1].bigEndianValue;
	#endif
	
	if(endian((unsigned int)km1) & 0x8000000){
		keyDownMessage[VK_CONTROL]=true;
		keyUpMessage[VK_CONTROL]=false;
	}else{
		keyDownMessage[VK_CONTROL]=false;
		keyUpMessage[VK_CONTROL]=true;
	}
	
	if(endian((unsigned int)km1) & 0x4000000){
		keyDownMessage[VK_OPTION]=true;
		keyUpMessage[VK_OPTION]=false;
	}else{
		keyDownMessage[VK_OPTION]=false;
		keyUpMessage[VK_OPTION]=true;
	}
	
	for(int li=0; li<256; li++){
		if(keyDownMessage[li] && !key[li]){
			//if you need to find the keycode of something, make a breakpoint here
			key_down[li]=true;
			key[li]=true;
		}

		if(keyUpMessage[li]){
			key_up[li]=true;
			key[li]=false;
		}
	}
	
	for(int li=0; li<256; li++){
		keyUpMessage[li]=false;
		keyDownMessage[li]=false;
	}


	bool key_number_hit=false;
	for(int li=0; li<256; li++){
		if(key[li]){
  			current_key_number=li;
  			key_number_hit=true;
     	}
	}
	
	
	if(!key_number_hit){current_key_number=-1;}


	tab=key['	'];
	tab_up=key_up['	'];
	tab_down=key_down['	'];
	
	ctrl=key[VK_CONTROL];
	ctrl_up=key_up[VK_CONTROL];
	ctrl_down=key_down[VK_CONTROL];
	
	escape=key[27];
	escape_up=key_up[27];
	escape_down=key_down[27];
	
	shift=key[VK_SHIFT];
	shift_up=key_up[VK_SHIFT];
	shift_down=key_down[VK_SHIFT];
	
	option=key[VK_OPTION];
	option_up=key_up[VK_OPTION];
	option_down=key_down[VK_OPTION];

	enter=key['\r'];
	enter_up=key_up['\r'];
	enter_down=key_down['\r'];

	period=(key['.'] );
	period_up=(key_up['.'] );
	period_down=(key_down['.'] );


	greater=key['>'];
	greaterUp=(key_up['>']);
	greaterDown=(key_down['>']) ;


	subtract=key['-'];
	subtractUp=key_up['-'];
	subtractDown=key_down['-'];

	
	add=key['+'];
	addUp=key_up['+'];
	addDown=key_down['+'];


	arrow_up=key[VK_UP];
	arrow_up_up=key_up[VK_UP];
	arrow_up_down=key_down[VK_UP];

	arrow_down=key[VK_DOWN];
	arrow_down_up=key_up[VK_DOWN];
	arrow_down_down=key_down[VK_DOWN];
	
	arrow_left=key[VK_LEFT];
	arrow_left_up=key_up[VK_LEFT];
	arrow_left_down=key_down[VK_LEFT];
	
	arrow_right=key[VK_RIGHT];
	arrow_right_up=key_up[VK_RIGHT];
	arrow_right_down=key_down[VK_RIGHT];
	
	page_up=key[VK_PRIOR];
	page_up_up=key_up[VK_PRIOR];
	page_up_down=key_down[VK_PRIOR];
	
	page_down=key[VK_NEXT];
	page_down_up=key_up[VK_NEXT];
	page_down_down=key_down[VK_NEXT];
	
	page_down=key[VK_NEXT];
	page_down_up=key_up[VK_NEXT];
	page_down_down=key_down[VK_NEXT];
	
	space=key[' '];
	space_up=key_up[' '];
	space_down=key_down[' '];
	
	home=key[VK_HOME];
	home_up=key_up[VK_HOME];
	home_down=key_down[VK_HOME];
	
	end=key[VK_END];
	end_up=key_up[VK_END];
	end_down=key_down[VK_END];
	
	bracketOpen=key['['];
	bracketOpenUp=key_up['['];
	bracketOpenDown=key_down['['];

	bracketClose=key[']'] ;
	bracketCloseUp=key_up[']'] ;
	bracketCloseDown=key_down[']'];

	parenthesisOpen=key['('];
	parenthesisOpenUp=key_up['('];
	parenthesisOpenDown=key_down['('];

	parenthesisClose=key[')'];
	parenthesisCloseUp=key_up[')'];
	parenthesisCloseDown=key_down[')'];

	equal=key['='];
	equalUp=key_up['='];
	equalDown=key_down['='];

	underscore=(key['_']);
	underscoreUp=(key_up['_']);
	underscoreDown=(key_down['_']);

	comma=key[','];
	commaUp=key_up[','];
	commaDown=key_down[','];

	less=key['<'];
	lessUp=key_up['<'];
	lessDown=key_down['<'];

	quote=key['\"'];
	quoteUp=key_up['\"'];
	quoteDown=key_down['\"'];

	backspace=key[127];
	backspaceUp=key_up[127];
	backspaceDown=key_down[127];
	
	semicolon=key[';'];
	semicolonUp=key_up[';'];
	semicolonDown=key_down[';'];

	colon=key[':'];
	colonUp=key_up[':'];
	colonDown=key_down[':'];

	multiply=key['*'];
	multiplyUp=key_up['*'];
	multiplyDown=key_down['*'];

	devide=key['/'];
	devideUp=key_up['/'];
	devideDown=key_down['/'];

	slash=devide;
	slashUp=devideUp;
	slashDown=devideDown;

	backslash=key[220] && !shift;
	backslashUp=key_up[220] && !shift;
	backslashDown=key_down[220] && !shift;

	exclamation=key[49] && shift;
	exclamationUp=key_up[49] && shift;
	exclamationDown=key_down[49] && shift;

	at=key['@'];
	atUp=key_up['@'];
	atDown=key_down['@'];


	question=key['?'];
	questionUp=key_up['?'];
	questionDown=key_down['?'];

	tilde=key['~'];
	tildeUp=key_up['~'];
	tildeDown=key_down['~'];

	prime=key['`'];
	primeUp=key_up['`'];
	primeDown=key_down['`'];


	pound=key['#'];
	poundUp=key_up['#'];
	poundDown=key_down['#'];

	dollar=key['$'];
	dollarUp=key_up['$'];
	dollarDown=key_down['$'];

	percent=key['%'];
	percentUp=key_up['%'];
	percentDown=key_down['%'];

	carrot=key['^'];
	carrotUp=key_up['^'];
	carrotDown=key_down['^'];

	ampersand=key['&'];
	ampersandUp=key_up['&'];
	ampersandDown=key_down['&'];

	pipe=key['|'];
	pipeUp=key_up['|'];
	pipeDown=key_down['|'];
		
	bool letter_hit=false;
	bool letter_down_hit=false;
	
	for(int li=65; li < 91; li++){
		if(key[li]){
  			current_letter=li;
     		letter_hit=true;
       	}
       	
       	if(key_down[li]){
       		current_letter_down=li;
     		letter_down_hit=true;
       	}
	}

	for(int li=97; li < 123; li++){
		if(key[li]){
  			current_letter=li;
     		letter_hit=true;
       	}
       	
       	if(key_down[li]){
       		current_letter_down=li;
     		letter_down_hit=true;
       	}
	}
	
	if(!letter_hit){current_letter=-1;}
	if(!letter_down_hit){current_letter_down=-1;}
	
	
	bool number_hit=false;
	number_down_hit=false;
	
	for(int li=48; li < 58; li++){
		if(key[li]){
  			current_number=li-48;
     		number_hit=true;
   		}
   		
   		if(key_down[li]){
  			current_number_down=li-48;
     		number_down_hit=true;
   		}
	}
	
	if(!number_hit){current_number=-1;}
	if(!number_down_hit){current_number_down=-1;}

	if(space_down){
		currentStringDown=String(" ");
		currentStringDownExtended=String("space");
	}else if(shift && bracketOpenDown){
		currentStringDown=String("{");
		currentStringDownExtended=String("{");
	}else if(shift && bracketCloseDown){
		currentStringDown=String("}");
		currentStringDownExtended=String("}");
	}else if(!shift && bracketOpenDown){
		currentStringDown=String("[");
		currentStringDownExtended=String("[");
	}else if(!shift && bracketCloseDown){
		currentStringDown=String("]");
		currentStringDownExtended=String("]");
	}else if(shift && parenthesisOpenDown){
		currentStringDown=String("(");
		currentStringDownExtended=String("(");
	}else if(shift && parenthesisCloseDown){
		currentStringDown=String(")");
		currentStringDownExtended=String(")");
	}else if(!shift && period_down){
		currentStringDown=String(".");
		currentStringDownExtended=String(".");
	}else if(greaterDown){
		currentStringDown=String(">");
	}else if(commaDown){
		currentStringDown=String(",");
	}else if(questionDown){
		currentStringDown=String("?");
	}else if(tildeDown){
		currentStringDown=String("~");
	}else if(primeDown){
		currentStringDown=String("`");
	}else if(lessDown){
		currentStringDown=String("<");
	}else if(addDown){
		currentStringDown=String("+");
	}else if(subtractDown){
		currentStringDown=String("-");
	}else if(equalDown){
		currentStringDown=String("=");
	}else if(shift && quoteDown){
		currentStringDown=String("\"");
	}else if(!shift && quoteDown){
		currentStringDown=String("\'");
	}else if(colonDown){
		currentStringDown=String(":");
	}else if(semicolonDown){
		currentStringDown=String(";");
	}else if(multiplyDown){
		currentStringDown=String("*");
	}else if(devideDown){
		currentStringDown=String("/");
	}else if(backslashDown){
		currentStringDown=String("\\");
	}else if(atDown){
		currentStringDown=String("@");
	}else if(poundDown){
		currentStringDown=String("#");
	}else if(dollarDown){
		currentStringDown=String("$");
	}else if(percentDown){
		currentStringDown=String("%");
	}else if(carrotDown){
		currentStringDown=String("^");
	}else if(ampersandDown){
		currentStringDown=String("&");
	}else if(pipeDown){
		currentStringDown=String("|");
	}else if(exclamationDown){
		currentStringDown=String("!");
	}else if(underscoreDown){
		currentStringDown=String("_");
	}else if(arrow_left_down){
		currentStringDownExtended=String("arrowLeft");
	}else if( arrow_right_down){
		currentStringDownExtended=String("arrowRight");
	}else if( arrow_up_down){
		currentStringDownExtended=String("arrowUp");
	}else if( arrow_down_down){
		currentStringDownExtended=String("arrowDown");
	}else if(current_letter_down!=-1){
		currentStringDown=keyboard.current_letter_down;
		currentStringDownExtended=keyboard.current_letter_down;
	}else if(current_number_down!=-1){
		currentStringDown=String(keyboard.current_number_down);
		currentStringDownExtended=String(keyboard.current_number_down);
	}else{
		currentStringDown="";
		currentStringDownExtended="";
	}
	
	if(key[48] || key[96]){one=true;}else{one=false;}
	if(key[48] || key[96]){two=true;}else{two=false;}
	if(key[48] || key[96]){three=true;}else{three=false;}
	if(key[48] || key[96]){four=true;}else{four=false;}
	if(key[48] || key[96]){five=true;}else{five=false;}
	if(key[48] || key[96]){six=true;}else{six=false;}
	if(key[48] || key[96]){seven=true;}else{seven=false;}
	if(key[48] || key[96]){eight=true;}else{eight=false;}
	if(key[48] || key[96]){nine=true;}else{nine=false;}

	if(key_down[48] || key_down[96]){oneDown=true;}else{oneDown=false;}
	if(key_down[48] || key_down[96]){twoDown=true;}else{twoDown=false;}
	if(key_down[48] || key_down[96]){threeDown=true;}else{threeDown=false;}
	if(key_down[48] || key_down[96]){fourDown=true;}else{fourDown=false;}
	if(key_down[48] || key_down[96]){fiveDown=true;}else{fiveDown=false;}
	if(key_down[48] || key_down[96]){sixDown=true;}else{sixDown=false;}
	if(key_down[48] || key_down[96]){sevenDown=true;}else{sevenDown=false;}
	if(key_down[48] || key_down[96]){eightDown=true;}else{eightDown=false;}
	if(key_down[48] || key_down[96]){nineDown=true;}else{nineDown=false;}

	if(key_up[48] || key_up[96]){oneUp=true;}else{oneUp=false;}
	if(key_up[48] || key_up[96]){twoUp=true;}else{twoUp=false;}
	if(key_up[48] || key_up[96]){threeUp=true;}else{threeUp=false;}
	if(key_up[48] || key_up[96]){fourUp=true;}else{fourUp=false;}
	if(key_up[48] || key_up[96]){fiveUp=true;}else{fiveUp=false;}
	if(key_up[48] || key_up[96]){sixUp=true;}else{sixUp=false;}
	if(key_up[48] || key_up[96]){sevenUp=true;}else{sevenUp=false;}
	if(key_up[48] || key_up[96]){eightUp=true;}else{eightUp=false;}
	if(key_up[48] || key_up[96]){nineUp=true;}else{nineUp=false;}

	if(key[65] || key[97]){a=true;}else{a=false;}
	if(key[66] || key[98]){b=true;}else{b=false;}
	if(key[67] || key[99]){c=true;}else{c=false;}
	if(key[68] || key[100]){d=true;}else{d=false;}
	if(key[69] || key[101]){e=true;}else{e=false;}
	if(key[70] || key[102]){f=true;}else{f=false;}
	if(key[71] || key[103]){g=true;}else{g=false;}
	if(key[72] || key[104]){h=true;}else{h=false;}
	if(key[73] || key[105]){i=true;}else{i=false;}
	if(key[74] || key[106]){j=true;}else{j=false;}
	if(key[75] || key[107]){k=true;}else{k=false;}
	if(key[76] || key[108]){l=true;}else{l=false;}
	if(key[77] || key[109]){m=true;}else{m=false;}
	if(key[78] || key[110]){n=true;}else{n=false;}
	if(key[79] || key[111]){o=true;}else{o=false;}
	if(key[80] || key[112]){p=true;}else{p=false;}
	if(key[81] || key[113]){q=true;}else{q=false;}
	if(key[82] || key[114]){r=true;}else{r=false;}
	if(key[83] || key[115]){s=true;}else{s=false;}
	if(key[84] || key[116]){t=true;}else{t=false;}
	if(key[85] || key[117]){u=true;}else{u=false;}
	if(key[86] || key[118]){v=true;}else{v=false;}
	if(key[87] || key[119]){w=true;}else{w=false;}
	if(key[88] || key[120]){x=true;}else{x=false;}
	if(key[89] || key[121]){y=true;}else{y=false;}
	if(key[90] || key[122]){z=true;}else{z=false;}

	if(key_down[65] || key_down[97]){aDown=true;}else{aDown=false;}
	if(key_down[66] || key_down[98]){bDown=true;}else{bDown=false;}
	if(key_down[67] || key_down[99]){cDown=true;}else{cDown=false;}
	if(key_down[68] || key_down[100]){dDown=true;}else{dDown=false;}
	if(key_down[69] || key_down[101]){eDown=true;}else{eDown=false;}
	if(key_down[70] || key_down[102]){fDown=true;}else{fDown=false;}
	if(key_down[71] || key_down[103]){gDown=true;}else{gDown=false;}
	if(key_down[72] || key_down[104]){hDown=true;}else{hDown=false;}
	if(key_down[73] || key_down[105]){iDown=true;}else{iDown=false;}
	if(key_down[74] || key_down[106]){jDown=true;}else{jDown=false;}
	if(key_down[75] || key_down[107]){kDown=true;}else{kDown=false;}
	if(key_down[76] || key_down[108]){lDown=true;}else{lDown=false;}
	if(key_down[77] || key_down[109]){mDown=true;}else{mDown=false;}
	if(key_down[78] || key_down[110]){nDown=true;}else{nDown=false;}
	if(key_down[79] || key_down[111]){oDown=true;}else{oDown=false;}
	if(key_down[80] || key_down[112]){pDown=true;}else{pDown=false;}
	if(key_down[81] || key_down[113]){qDown=true;}else{qDown=false;}
	if(key_down[82] || key_down[114]){rDown=true;}else{rDown=false;}
	if(key_down[83] || key_down[115]){sDown=true;}else{sDown=false;}
	if(key_down[84] || key_down[116]){tDown=true;}else{tDown=false;}
	if(key_down[85] || key_down[117]){uDown=true;}else{uDown=false;}
	if(key_down[86] || key_down[118]){vDown=true;}else{vDown=false;}
	if(key_down[87] || key_down[119]){wDown=true;}else{wDown=false;}
	if(key_down[88] || key_down[120]){xDown=true;}else{xDown=false;}
	if(key_down[89] || key_down[121]){yDown=true;}else{yDown=false;}
	if(key_down[90] || key_down[122]){zDown=true;}else{zDown=false;}

	if(key_up[65] || key_up[97]){aUp=true;}else{aUp=false;}
	if(key_up[66] || key_up[98]){bUp=true;}else{bUp=false;}
	if(key_up[67] || key_up[99]){cUp=true;}else{cUp=false;}
	if(key_up[68] || key_up[100]){dUp=true;}else{dUp=false;}
	if(key_up[69] || key_up[101]){eUp=true;}else{eUp=false;}
	if(key_up[70] || key_up[102]){fUp=true;}else{fUp=false;}
	if(key_up[71] || key_up[103]){gUp=true;}else{gUp=false;}
	if(key_up[72] || key_up[104]){hUp=true;}else{hUp=false;}
	if(key_up[73] || key_up[105]){iUp=true;}else{iUp=false;}
	if(key_up[74] || key_up[106]){jUp=true;}else{jUp=false;}
	if(key_up[75] || key_up[107]){kUp=true;}else{kUp=false;}
	if(key_up[76] || key_up[108]){lUp=true;}else{lUp=false;}
	if(key_up[77] || key_up[109]){mUp=true;}else{mUp=false;}
	if(key_up[78] || key_up[110]){nUp=true;}else{nUp=false;}
	if(key_up[79] || key_up[111]){oUp=true;}else{oUp=false;}
	if(key_up[80] || key_up[112]){pUp=true;}else{pUp=false;}
	if(key_up[81] || key_up[113]){qUp=true;}else{qUp=false;}
	if(key_up[82] || key_up[114]){rUp=true;}else{rUp=false;}
	if(key_up[83] || key_up[115]){sUp=true;}else{sUp=false;}
	if(key_up[84] || key_up[116]){tUp=true;}else{tUp=false;}
	if(key_up[85] || key_up[117]){uUp=true;}else{uUp=false;}
	if(key_up[86] || key_up[118]){vUp=true;}else{vUp=false;}
	if(key_up[87] || key_up[119]){wUp=true;}else{wUp=false;}
	if(key_up[88] || key_up[120]){xUp=true;}else{xUp=false;}
	if(key_up[89] || key_up[121]){yUp=true;}else{yUp=false;}
	if(key_up[90] || key_up[122]){zUp=true;}else{zUp=false;}

	return true;

}

KEYBOARD::KEYBOARD(){

}
bool KEYBOARD::scriptInit(){
	LuaType* mLT=script().add("keyboard");
	
	script().add("keyDown",&currentStringDown,mLT);
	script().add("keyNumber",&current_key_number,mLT);
	script().add("keyDownExtended",&currentStringDownExtended,mLT);
	
	script().add("enter",&enter,mLT);
	script().add("enterDown",&enter_down,mLT);
	script().add("enterUp",&enter_up,mLT);
	
	script().add("space",&space,mLT);
	script().add("spaceDown",&space_down,mLT);
	script().add("spaceUp",&space_up,mLT);
	
	script().add("backspace",&backspace,mLT);
	script().add("backspaceDown",&backspaceDown,mLT);
	script().add("backspaceUp",&backspaceUp,mLT);
	
	script().add("escape",&escape,mLT);
	script().add("escapeDown",&escape_down,mLT);
	script().add("escapeUp",&escape_up,mLT);
	
	script().add("shift",&shift,mLT);
	script().add("shiftDown",&shift_down,mLT);
	script().add("shiftUp",&shift_up,mLT);
	
	script().add("arrowDown",&arrow_down,mLT);
	script().add("arrowDownDown",&arrow_down_down,mLT);
	script().add("arrowDownUp",&arrow_down_up,mLT);
	
	script().add("arrowUp",&arrow_up,mLT);
	script().add("arrowUpDown",&arrow_up_down,mLT);
	script().add("arrowUpUp",&arrow_up_up,mLT);
	
	script().add("arrowLeft",&arrow_left,mLT);
	script().add("arrowLeftDown",&arrow_left_down,mLT);
	script().add("arrowLeftUp",&arrow_left_up,mLT);
	
	script().add("arrowRight",&arrow_right,mLT);
	script().add("arrowRightDown",&arrow_right_down,mLT);
	script().add("arrowRightUp",&arrow_right_up,mLT);
	
	script().add("ctrl",&ctrl,mLT);
	script().add("ctrlDown",&ctrl_down,mLT);
	script().add("ctrlUp",&ctrl_up,mLT);
	
	script().add("option",&option,mLT);
	script().add("optionDown",&option_down,mLT);
	script().add("optionUp",&option_up,mLT);
	
	script().add("alt",&alt,mLT);
	script().add("altDown",&alt_down,mLT);
	script().add("altUp",&alt_up,mLT);
	
	script().add("tab",&tab,mLT);
	script().add("tabDown",&tab_down,mLT);
	script().add("tabUp",&tab_up,mLT);
	
	script().add("a",&a,mLT);
	script().add("aDown",&aDown,mLT);
	script().add("aUp",&aUp,mLT);

	script().add("b",&b,mLT);
	script().add("bDown",&bDown,mLT);
	script().add("bUp",bUp,mLT);

	script().add("c",&c,mLT);
	script().add("cDown",&cDown,mLT);
	script().add("cUp",&cUp,mLT);

	script().add("d",&d,mLT);
	script().add("dDown",&dDown,mLT);
	script().add("dUp",&dUp,mLT);

	script().add("e",&e,mLT);
	script().add("eDown",&eDown,mLT);
	script().add("eUp",&eUp,mLT);

	script().add("f",&f,mLT);
	script().add("fDown",&fDown,mLT);
	script().add("fUp",&fUp,mLT);

	script().add("g",&g,mLT);
	script().add("gDown",&gDown,mLT);
	script().add("gUp",&gUp,mLT);

	script().add("h",&h,mLT);
	script().add("hDown",&hDown,mLT);
	script().add("hUp",&hUp,mLT);

	script().add("i",&i,mLT);
	script().add("iDown",&iDown,mLT);
	script().add("iUp",&iUp,mLT);

	script().add("j",&j,mLT);
	script().add("jDown",&jDown,mLT);
	script().add("jUp",&jUp,mLT);

	script().add("k",&k,mLT);
	script().add("kDown",&kDown,mLT);
	script().add("kUp",&kUp,mLT);

	script().add("l",&l,mLT);
	script().add("lDown",&lDown,mLT);
	script().add("lUp",&lUp,mLT);

	script().add("m",&m,mLT);
	script().add("mDown",&mDown,mLT);
	script().add("mUp",&mUp,mLT);

	script().add("n",&n,mLT);
	script().add("nDown",&nDown,mLT);
	script().add("nUp",&nUp,mLT);

	script().add("o",&o,mLT);
	script().add("oDown",&oDown,mLT);
	script().add("oUp",&oUp,mLT);

	script().add("p",&p,mLT);
	script().add("pDown",&pDown,mLT);
	script().add("pUp",&pUp,mLT);

	script().add("q",&q,mLT);
	script().add("qDown",&qDown,mLT);
	script().add("qUp",&qUp,mLT);

	script().add("r",&r,mLT);
	script().add("rDown",&rDown,mLT);
	script().add("rUp",&rUp,mLT);

	script().add("s",&s,mLT);
	script().add("sDown",&sDown,mLT);
	script().add("sUp",&sUp,mLT);

	script().add("t",&t,mLT);
	script().add("tDown",&tDown,mLT);
	script().add("tUp",&tUp,mLT);

	script().add("u",&u,mLT);
	script().add("uDown",&uDown,mLT);
	script().add("uUp",&uUp,mLT);

	script().add("v",&v,mLT);
	script().add("vDown",&vDown,mLT);
	script().add("vUp",&vUp,mLT);

	script().add("w",&w,mLT);
	script().add("wDown",&wDown,mLT);
	script().add("wUp",&wUp,mLT);

	script().add("x",&x,mLT);
	script().add("xDown",&xDown,mLT);
	script().add("xUp",&xUp,mLT);

	script().add("y",&y,mLT);
	script().add("yDown",&yDown,mLT);
	script().add("yUp",&yUp,mLT);

	script().add("z",&z,mLT);
	script().add("zDown",&zDown,mLT);
	script().add("zUp",&zUp,mLT);

	return true;
}

