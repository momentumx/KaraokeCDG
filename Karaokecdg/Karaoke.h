#pragma once

#include "Console.h"
using namespace System;

#define CLEAR_SCREEN 1
#define SET_BORDER 2
#define DRAW_TILE 6
#define MOVE_IMAGE 20
#define WRAP_IMAGE 24
#define SET_TRANSPARENT_COLOR 28
#define LOAD_BOTTOM_COLORS 30
#define LOAD_UPPER_COLORS 31
#define DRAW_TILE_BLEND 38
#define PACKET_SIZE 24
#define ONE_SEC_OF_PACKETS_IN_BYTES 7200

#define AA { 30,33,45,45,45,45,33,45,45,45,45,63 }
#define BB { 60,34,41,45,45,34,34,45,45,41,33,62 }
#define CC { 31,17,17,23,22,22,22,22,23,17,17,31 }
#define DD { 62,35,33,41,45,45,45,45,41,33,35,62 }
#define EE { 62,34,34,46,46,34,34,46,46,34,34,62 }
#define FF { 63,33,33,39,38,34,34,38,38,38,38,62 }
#define GG { 14,19,33,45,47,46,41,41,45,33,49,30 }
#define HH { 63,45,45,45,45,33,33,45,45,45,45,63 }
#define II { 63,35,35,55,22,22,22,22,55,35,35,63 }
#define JJ { 63,33,33,50,17,57,41,41,41,33,33,63 }
#define KK { 63,45,41,34,38,38,38,34,42,41,45,63 }
#define LL { 28,38,38,38,38,38,38,38,39,33,33,31 }
#define MM { 63,45,33,33,45,45,45,45,45,45,45,63 }
#define NN { 63,45,45,37,37,33,41,41,45,45,45,63 }
#define OO { 30,51,33,45,45,45,45,45,45,33,51,30 }
#define PP { 28,35,45,45,41,34,38,38,38,38,38,60 }
#define QQ { 28,34,34,42,42,42,42,42,42,34,33,31 }
#define RR { 30,35,45,45,41,34,38,38,34,42,41,63 }
#define SS { 12,18,33,37,39,18,18,57,41,33,18,12 }
#define TT { 63,35,35,55,22,22,22,22,22,22,22,30 }
#define UU { 63,45,45,45,45,45,45,45,45,33,37,63 }
#define VV { 63,45,45,45,45,45,41,41,34,18,22,12 }
#define WW { 63,45,45,45,45,45,45,45,33,33,45,63 }
#define XX {  0,63,45,45,51,18,18,18,51,45,45,63 }
#define YY { 63,45,45,45,33,33,18,18,18,18,18,30 }
#define ZZ { 63,33,33,61,57,51,39,47,47,33,33,63 }

#define TEAL 0
#define WHITE 1
#define BLACK 2
#define RED 4
#define BLUE 5
#define ORANGE 8

#define EMPTY "                                                "

#define SONG_MILLSEC 182428.0f
#define SONG_PACKETS 54728.4f

#define ROW_START 4
#define NUMBER_OF_ROWS 5
#define LAST_WORD_LENGTH 4
#define NEW_LINE NUMBER_OF_ROWS - 2

struct ClearScreen
{
	char color = 0;    //4 bits
	char repeat = 0;   //4 bits
	char filler[14] = { 0 };
	ClearScreen() {}
	ClearScreen(char _color) {
		color = _color;
	}
};

struct SetBorder
{
	char color = 0;      //4 bits
	char filler[15] = { 0 };
	SetBorder() {}
	SetBorder(char _color){
		color = _color;
	}
};

struct DrawTile
{
	char color0 = 0;				// Only lower 4 bits are used, mask with 0x0F
	char color1 = 0;				// Only lower 4 bits are used, mask with 0x0F
	char y = 5;						// Only lower 5 bits are used, mask with 0x1F
	char x = 2;						// Only lower 6 bits are used, mask with 0x3F
	char tilePixels[12]{ 0 };		// Only lower 6 bits of each byte are used
	DrawTile(){}
	DrawTile(char* _image) {
		unsigned i = -1; while (++i!=12)
			tilePixels[i] = _image[i];
	}
	void SetImage(char* _image) {
		unsigned i = -1; while (++i != 12)
			tilePixels[i] = _image[i];
	}
};

struct Data
{
	char data[16] = { 0 };

	void SetImage(char _x, char* _data, char _color1 = BLACK, char _color0 = TEAL) {
		data[0] = _color0;
		data[1] = _color1;
		data[3] = _x;
		unsigned i = 3; while (++i != 16)
			data[i] = _data[i - 4];
	}
	void SetImage(char* _data, char _color1 = BLACK, char _color0 = TEAL) {
		data[0] = _color0;
		data[1] = _color1;
		unsigned i = 3; while (++i != 16)
			data[i] = _data[i - 4];
	}
};

struct Word
{
	char x = 0;
	char* word;
	unsigned startTime;
	unsigned speed;
	vector<char*> letters;
};

struct Line
{
	ConsoleColor color = White;
	char color16 = WHITE;
	char totalChars = 0;
	vector<Word> words;

	void CenterLine(vector<Line>& _song) {
		char start = 25 - totalChars*.5f;
		int i = -1; while (++i != words.size())
		{
			words[i].x = start;//x6
			start += words[i].letters.size() + 1;
		}
		Line copy = *this;
		if (words.size()>0)
			_song.push_back(copy);
		words.clear();
		totalChars = 0;
	}

	void AddWord(Word _word, vector<Line>& _song) {
		//2 ->47  = 46 letters
		if ((2 + totalChars + _word.letters.size() + 1) < 48) {
			totalChars += _word.letters.size() + 1;
			words.push_back(_word);
		}
		else {
			CenterLine(_song);
			totalChars += _word.letters.size() + 1;
			words.push_back(_word);
		}
	}
};


struct Packet //24 bytes (4 in a channel)
{
	char command = 0;
	char instruction = 0;
	char parityQ[2] = { 0 };
	char parityP[4] = { 0 };
	Data data;
	void operator=(char* _packet) {

		command = _packet[0];
		instruction = _packet[1];

		parityQ[0] = _packet[2];
		parityQ[1] = _packet[3];

		parityP[0] = _packet[20];
		parityP[1] = _packet[21];
		parityP[2] = _packet[22];
		parityP[3] = _packet[23];


		int i = -1; while (++i != 16)
			data.data[i] = _packet[i + 4];
	}

	void printOut(ofstream& _fileOut) {
		_fileOut << command;
		_fileOut << instruction;
		int i = -1; while (++i != 2)
		{
			_fileOut << parityQ[i];
		}

		i = -1; while (++i != 16)
		{
			_fileOut << data.data[i];
		}

		i = -1; while (++i != 4)
		{
			_fileOut << parityP[i];
		}
	}
};

vector<Line> Song;
char alphebet[26][12] = { AA,BB,CC,DD,EE,FF,GG,HH,II,JJ,KK,LL,MM,NN,OO,PP,QQ,RR,SS,TT,UU,VV,WW,XX,YY,ZZ };
char FILL[12] = { 0,30,30,30,30,30,30,30,30,30,30,0 }; 
char EMPTY12[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
char HALF_FILL1[12] = { 56,56,56,56,56,56,56,56,56,56,56,56 };
char HALF_FILL2[12] = { 7,7,7,7,7,7,7,7,7,7,7,7 };
float slope = SONG_PACKETS / SONG_MILLSEC;
clock_t beginTime = 0;

void RemoveEmptySpace() {

	char* reader;
	ifstream file("E:/downloads/BeginningCrown.cdg", ios::in | ios::binary);
	if (file.is_open())
	{
		vector<Packet> packeeee;
		//create array of chars from the lyrics
		reader = new char[ONE_SEC_OF_PACKETS_IN_BYTES];
		file.read(reader, ONE_SEC_OF_PACKETS_IN_BYTES);

		ofstream fileOut("E:/downloads/BeginningCrownReg.cdg", ios::out | ios::binary);

		if (fileOut.is_open())
		{
			int i = -1; while (++i != 300 * PACKET_SIZE)
				fileOut << reader[i];

			delete[] reader;
			reader = new char[PACKET_SIZE];
			Packet pack;
			i = -1; while (++i != 4500)
			{
				file.read(reader, PACKET_SIZE);
				pack = reader;
				if (pack.command == 9)
					packeeee.push_back(pack);
			}
			for each (Packet packs in packeeee)
				packs.printOut(fileOut);

			fileOut.close();
		}
		delete[] reader;
		file.close();
	}
}

unsigned PressTime() {
	return slope*(float(clock() - beginTime)-100);
}

void SetETC(char _x, vector<Packet>& _packets, unsigned& _index) {
	Packet packet;
	packet.command = 9;
	packet.data.data[2] = 4;
	packet.instruction = DRAW_TILE_BLEND;
	char halfETC1[12] = { 0,0,0,0,0,0,0,0,25,38,38,25 };
	char halfETC2[12] = { 0,0,0,0,0,0,0,0,38,25,25,38 };
	char filler[12] =	{ 0,0,0,0,0,0,0,0, 0,63,63, 0 };
	packet.data.SetImage(_x - 2, halfETC1);
	_packets[_index] = packet;
	packet.data.SetImage(filler, WHITE);
	_packets[++_index] = packet;
	packet.data.SetImage(_x - 1, halfETC2);
	_packets[++_index] = packet;
	packet.data.SetImage(filler, WHITE);
	_packets[++_index] = packet;
}

bool DrawETC(char _x, vector<Packet>& _packets, unsigned& _index, unsigned _indexOfSong) {
   	if (_index > Song[_indexOfSong].words[0].startTime) {
		cout << "\nNeed more start time\nReturning";
		return false;
	}
	float speed = (Song[_indexOfSong].words[0].startTime - _index) / 4;
	Packet packet;
	packet.command = 9;
	packet.data.data[2] = 4;

	packet.instruction = DRAW_TILE_BLEND;
	packet.data.SetImage(_x - 2, HALF_FILL1,ORANGE);
	_packets[++_index] = packet;
	_index += speed;
	packet.data.SetImage(HALF_FILL2, ORANGE);
	_packets[_index] = packet;
	_index += speed;
	packet.data.SetImage(_x - 1, HALF_FILL1, ORANGE);
	_packets[_index] = packet;
	_index += speed;
	packet.data.SetImage(HALF_FILL2, ORANGE);
	_packets[_index] = packet;
	return true;
}

bool DrawBreak(char _x, vector<Packet>& _packets, unsigned& _index, unsigned _indexOfSong) {
	float length = (Song[_indexOfSong].words[0].startTime - _index) ;
	Packet packet;
	packet.command = 9;
	packet.data.data[2] = 4;
	streampos size;
	char* reader;

	ifstream file("E:/downloads/DrinkBreak.cdg", ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		file.seekg(0, ios::beg);
		reader = new char[size];

		file.read(reader, PACKET_SIZE);
		file.close();
		delete[] reader;
	}

	//packet.instruction = DRAW_TILE_BLEND;
	//packet.data.SetImage(_x - 2, HALF_FILL1, ORANGE);
	//_packets[++_index] = packet;
	//_index += speed;
	//packet.data.SetImage(HALF_FILL2, ORANGE);
	//_packets[_index] = packet;
	//_index += speed;
	//packet.data.SetImage(_x - 1, HALF_FILL1, ORANGE);
	//_packets[_index] = packet;
	//_index += speed;
	//packet.data.SetImage(HALF_FILL2, ORANGE);
	//_packets[_index] = packet;
	return true;
}