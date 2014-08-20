#ifndef BAJA_MIDI_HPP
#define BAJA_MIDI_HPP

#ifdef _WIN32

#include "midi/MIDIInDevice.h"
#include "midi/ShortMsg.h"
#include <queue>

// My class for receiving MIDI messages
class MyReceiver : public midi::CMIDIReceiver
{
public:
	HANDLE mainThread;

    // Receives short messages
    void ReceiveMsg(DWORD Msg, DWORD TimeStamp);

    // Receives long messages
    void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp){}

    // Called when an invalid short message is received
	void OnError(DWORD Msg, DWORD TimeStamp);
    //{ std::cout << "Invalid short message received!\n"; }

    // Called when an invalid long message is received
    void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp){}
};


class BajaMidi{
	private:
		midi::CMIDIInDevice *InDevice;
		MyReceiver *Receiver;
		
		
	public:
		std::queue<midi::CShortMsg*> messageQueue;
		HANDLE queueMutex;

		bool init();
		bool scriptInit();

		void perFrame();

		BajaMidi();
		~BajaMidi();
};

extern BajaMidi bajaMidi;

#else

class BajaMidi{
private:
	
public:
	
	bool init();
	bool scriptInit();
	
	void perFrame();
	
	BajaMidi();
	~BajaMidi();
};

extern BajaMidi bajaMidi;

#endif
#endif