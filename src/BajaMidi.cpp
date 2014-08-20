#include "BajaMidi.hpp"
#include "script.hpp"

BajaMidi bajaMidi;

#ifdef _WIN32

#include "midi/ShortMsg.h"

using midi::CMIDIInDevice;
using midi::CMIDIReceiver;

VOID CALLBACK mainThreadCallback(
  __in  ULONG_PTR dwParam
  ){

	  int lol=0;
}



// Function called to receive short messages
void MyReceiver::ReceiveMsg(DWORD Msg, DWORD TimeStamp){
	midi::CShortMsg *ShortMsg=new midi::CShortMsg(Msg, TimeStamp);

	WaitForSingleObject(bajaMidi.queueMutex, INFINITE);
	bajaMidi.messageQueue.push(ShortMsg);
	ReleaseMutex(bajaMidi.queueMutex);
}

void MyReceiver::OnError(DWORD Msg, DWORD TimeStamp){
	console().write("Bad midi message recieved");
}

bool BajaMidi::init(){
	try
    {
        // Make sure there is a MIDI input device present
        if(CMIDIInDevice::GetNumDevs() > 0){
			Receiver=new MyReceiver();

			DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&Receiver->mainThread,0,FALSE,DUPLICATE_SAME_ACCESS);

			InDevice=new CMIDIInDevice(*Receiver);

            // We'll use the first device - we're not picky here
            InDevice->Open(0);

            // Start recording
            InDevice->StartRecording();
        }
        else
        {
            std::cout << "No MIDI input devices present!\n";
        }
    }
    catch(const std::exception &Err){
        std::cout << Err.what();
    }

	script().run("midi['devices']={}");

	for(int i=0; i<CMIDIInDevice::GetNumDevs(); i++){
		MIDIINCAPS caps;
		CMIDIInDevice::GetDevCaps(i,caps);

		script().run("midi['devices']["+String(i+1)+"]={name='"+String(caps.szPname)+"',mid="+String(caps.wMid)+",pid="+String(caps.wPid)+",driver="+String(caps.vDriverVersion)+"}");

		int lol=0;
	}

	return true;
}

bool BajaMidi::scriptInit(){
	script().run("midiCallbacks={}");
	script().run("midi={registerCallback=function(func) midiCallbacks[#midiCallbacks+1]=func end}");

	return true;
}

void BajaMidi::perFrame(){
	midi::CShortMsg *mess;

	WaitForSingleObject(queueMutex, INFINITE);

	while(messageQueue.size()>0){
		mess=messageQueue.front();

		String cmd="for k,v in pairs(midiCallbacks)  do v("+String(mess->GetCommand())+","+String(mess->GetChannel())+","+String(mess->GetData1())+","+String(mess->GetData2())+","+String(mess->GetTimeStamp())+") end";

		script().run(cmd);

		delete mess;
		messageQueue.pop();
	}

	ReleaseMutex(queueMutex);
}

BajaMidi::BajaMidi(){
	queueMutex=CreateMutex(NULL, false, NULL);
	Receiver=NULL;
	InDevice=NULL;
}

BajaMidi::~BajaMidi(){

	// Stop recording
	if(InDevice!=NULL){
            InDevice->StopRecording();

            // Close the device
            InDevice->Close();
		delete InDevice;
	}

	if(Receiver!=NULL){
		delete Receiver;
	}
}

#else
BajaMidi::~BajaMidi(){
}

BajaMidi::BajaMidi(){
}


bool BajaMidi::scriptInit(){
	script().run("midiCallbacks={}");
	script().run("midi={registerCallback=function(func) midiCallbacks[#midiCallbacks+1]=func end}");
	
	return true;
}

bool BajaMidi::init(){return true;}

void BajaMidi::perFrame(){
}	

#endif