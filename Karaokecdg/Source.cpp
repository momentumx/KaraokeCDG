#include "Karaoke.h"



int main(int argc, char**argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1);

	//RESIZE SCREEN SIZE TO KARAOKE SIZE
	Console::SetBufferSize(50, 18);
	Console::SetWindowSize(50, 18);
	string fileName = "Karaoke tracks/";

	streampos size;
	char* unparsedLyrics;
	int i = -1;
	int j = -1;

	//////////////////////
	// parse text
	/////////////////
	{
		vector<char> parsedYlrics;
		Line line;
		//ifstream file(argv[1], ios::in | ios::binary | ios::ate);
		ifstream file("Karaoke tracks/Lyrics.txt", ios::in | ios::binary | ios::ate);
		if (file.is_open())
		{
			
			//create array of chars from the lyrics
			size = file.tellg();
			file.seekg(0, ios::beg);
			unparsedLyrics = new char[size];
			file.read(unparsedLyrics, size);
			file.close();
			bool found = false;
			char* name = new char[255];
			while (++i != size)//for every charcter
			{
				if (!found)
				{

					if (unparsedLyrics[i] == '\r')
					{
						found = true;
						name[i] = 0;
						fileName += name;
						fileName += '/';
						fileName += name;
						++i;
						delete[] name;
						continue;
					}
					name[i]= unparsedLyrics[i];
					continue;
				}
				char let = unparsedLyrics[i];
				//if we recognize the symbol
				if (let == '^' || let == ' ' || let == '@' || let == '*' || let == '_' || let == '\n' || ((let > 64 && let < 91) || (let > 96 && let < 123))) {
					//if it's caps make it regular
					if ((let > 64 && let < 91))
						let += 32;
					//put into vector
					parsedYlrics.push_back(let);
				}//since windows uses a \r\n we can get the last word of a newline by turning \r into a space since space is our delimeter
				else if (let == '\r')
					parsedYlrics.push_back(' ');
			}
			delete[] unparsedLyrics;
		}
		else
			cout << "Unable to open file";

		////////////////////////////
		// crate song
		///////////////////////////
		i = -1; while (++i != parsedYlrics.size())
		{
			switch (parsedYlrics[i])
			{
			case '@'://Opposite sex
				line.color = Red;
				line.color16 = RED;
				j = i;
				break;
			case '*'://Combined
				line.color = Green;
				line.color16 = BLUE;
				j = i;
				break;
			case '_'://hard space

				break;
			case '\n'://intentional line end
				line.CenterLine(Song);
				line.color = White;
				line.color16 = WHITE;
				j = i;
				break;
			case ' '://new word
			{
				unsigned wordSize = i - j - 1;
				if (wordSize > 0) {//prevents cases where more than 1 space is found back to back
					Word newWord;
					newWord.letters.resize(wordSize);
					newWord.word = new char[wordSize + 1];//this is for the timer 

					//for every letter since last space
					while (++j != i) {
						newWord.letters[wordSize - (i - j)] = alphebet[(parsedYlrics[j] - 97) % 26];//inserts cdg letter image 
						newWord.word[wordSize - (i - j)] = parsedYlrics[j];//inserts char for timer
					}
					//word needs a null terminator
					newWord.word[wordSize] = 0;
					//once we have newWord filled out add it to the line
					line.AddWord(newWord, Song);
				}
				break;
			}
			default:
				break;
			}
		}
		//the last line never actually gets pushd into song so do it here.
		line.CenterLine(Song);
		Console::SetCursorPosition(Song[0].words[0].x, ROW_START);
	}

	////////////////
	//draw first page
	////////////////
	i = -1; while (++i != NUMBER_OF_ROWS)
	{
		Console::ForegroundColor(Song[i].color);
		j = -1; while (++j != Song[i].words.size())
		{
			Console::SetCursorPosition(Song[i].words[j].x, ROW_START + 2 * i);
			//if its a hard space than insert white
			if (Song[i].words[j].word[0] == '_') {
				Console::BackgroundColor(White);
				cout << ' ';
				Console::BackgroundColor(Black);
			}
			else
				cout << Song[i].words[j].word;
		}
	}

	////////////////
	// start the timer
	////////////////
	{
		string name = fileName + ".mp3";
		string temp = "open \"\" type mpegvideo alias mp3";
		wchar_t songname[400] = {0};
		i = name.size(); while (--i!=-1)
		{
			temp.insert(6, 1, name[i]);
		}
		i = -1; while (++i!=temp.size())
		{
			songname[i] = temp[i];
		}
		mciSendString(songname, NULL, 0, NULL);
		mciSendString(L"play mp3", NULL, 0, NULL);
		beginTime = clock();
		Console::ForegroundColor(Blue);//over color
	}
	//the first 2 lines are special cases
	{
		Word* lastWord = nullptr;
		j = -1; while (++j != 2) {
			i = 0; while (i != Song[j].words.size()) {
				if (GetAsyncKeyState(VK_SPACE) & 1) {
					//set start time for current word in packet time
					Song[j].words[i].startTime = PressTime();

					// set speed of previous word in packet time
					if (i != 0)
						Song[j].words[i - 1].speed = (PressTime() - Song[j].words[i - 1].startTime) / (Song[j].words[i - 1].letters.size() * 2);
					else if (lastWord)
						lastWord->speed = (Song[j].words[i].startTime - lastWord->startTime) / (2 * lastWord->letters.size());

					//draw over in blue and increment
					Console::SetCursorPosition(Song[j].words[i].x, ROW_START + 2 * j);
					cout << Song[j].words[i].word;
					++i;
				}
			}
			lastWord = &Song[j].words[i - 1];
		}

		// for the rest of the song
		i = 1; while (++i != Song.size()) {
			Console::ForegroundColor(Blue);
			//for each word in the current line
			j = 0; while (j != Song[i].words.size())
			{
				if (GetAsyncKeyState(VK_SPACE) & 1) {
					Song[i].words[j].startTime = PressTime();//set start time
					if (j != 0)
						Song[i].words[j - 1].speed = (PressTime() - Song[i].words[j - 1].startTime) / (Song[i].words[j - 1].letters.size() * 2);
					else
						lastWord->speed = (Song[i].words[j].startTime - lastWord->startTime) / (2 * lastWord->letters.size());

					Console::SetCursorPosition(Song[i].words[j].x, ROW_START + 2 * ((i) % NUMBER_OF_ROWS));
					cout << Song[i].words[j].word;
					++j;
				}
			}
			lastWord = &Song[i].words[j - 1];
			unsigned index = i + NEW_LINE;
			//clear the line that is about to be drawn
			if (index < Song.size()) {
				Console::SetCursorPosition(2, ROW_START + 2 * ((index) % NUMBER_OF_ROWS));
				cout << EMPTY;
				//draw next line
				Console::ForegroundColor(Song[index].color);
				j = -1; while (++j != Song[index].words.size())
				{
					Console::SetCursorPosition(Song[index].words[j].x, ROW_START + 2 * ((index) % NUMBER_OF_ROWS));
					if (Song[index].words[j].word[0] == '_') {
						Console::BackgroundColor(White);
						cout << ' ';
						Console::BackgroundColor(Black);
					}
					else
						cout << Song[index].words[j].word;
				}
			}
		}
		lastWord->speed = LAST_WORD_LENGTH;
	}
	cout << '\n' << "Creating File";

	/////////////
	//Create File
	/////////////
	Packet packet;
	vector<Packet> packets;
	packets.resize(SONG_PACKETS+2000);
	string name = fileName + ".cdg";
	ofstream fileOut(name, ios::out | ios::binary);

	if (fileOut.is_open())
	{
		///////////////////////////
		//Insert Beggining packets
		///////////////////////////
		ifstream file("Karaoke tracks/BeginningCrownReg.cdg", ios::in | ios::binary | ios::ate);
		unsigned count;
		if (file.is_open())
		{
			size = file.tellg();
			file.seekg(0, ios::beg);
			unparsedLyrics = new char[size];
			file.read(unparsedLyrics, size);
			i = -1; while (++i!=size)
			{

			fileOut << unparsedLyrics[i];
			}
			count = size / PACKET_SIZE;
			if (Song[0].words[0].startTime > 1000) {
				//set intermission

			}
			file.close();
			delete[] unparsedLyrics;
		}
		/////////////////////
		//draw the first page
		///////////////////
		SetETC(Song[0].words[0].x, packets, count);
		packet.command = 9;
		packet.instruction = DRAW_TILE_BLEND;
		j = -1; while (++j != NUMBER_OF_ROWS)
		{
			packet.data.data[2] = ROW_START + (j % NUMBER_OF_ROWS) * 2;
			for each (Word word in Song[j].words) {
				i = -1; while (++i != word.letters.size()) {
					packet.data.SetImage(word.x + i, word.letters[i]);
					packets[++count] = packet;
					packet.data.SetImage(FILL, Song[j].color16);
					packets[++count] = packet;
				}
			}
		}
		if (!DrawETC(Song[0].words[0].x, packets, count, 0)) {
			system("Pause");
			return 0;
		}
		

		/////////////////////
		// first 2 lines are special
		///////////////////
		j = -1; while (++j != 2)
		{
			packet.data.data[2] = ROW_START + (j % NUMBER_OF_ROWS) * 2;//set y
			for each (Word word in Song[j].words) {
				count = word.startTime;
				i = -1; while (++i != word.letters.size()) {
					packet.data.SetImage(word.x + i, HALF_FILL1, ORANGE);
					packets[count] = packet;
					count += word.speed;
					packet.data.SetImage(HALF_FILL2, ORANGE);
					packets[count] = packet;
					count += word.speed;

				}
			}
		}
		///////////////
		// for the rest of the song
		///////////////////////////
		j = 1; while (++j != Song.size())
		{
			packet.data.data[2] = ROW_START + (j % NUMBER_OF_ROWS) * 2;//set y
			for each (Word word in Song[j].words) {
				count = word.startTime;
				i = -1; while (++i != word.letters.size()) {
					packet.data.SetImage(word.x + i, HALF_FILL1, ORANGE);
					packets[count] = packet;
					count += word.speed;
					packet.data.SetImage(HALF_FILL2, ORANGE);
					packets[count] = packet;
					count += word.speed;

				}
			}
			unsigned index = j + NEW_LINE;
			//clear the line that is about to be drawn
			if (index < Song.size()) {
				count = Song[j].words[0].startTime;

				//clears line to be drawn next
				packet.data.data[2] = ROW_START + (index % NUMBER_OF_ROWS) * 2;//set y
				packet.instruction = DRAW_TILE;
				packet.data.SetImage(EMPTY12);
				i = 2; while (i != 49)
					if (packets[++count].command == 0)
					{
						packet.data.data[3] = i;
						packets[count] = packet;
						++i;
					}

				//draw next line
				packet.instruction = DRAW_TILE_BLEND;
				for each (Word word in Song[index].words)
				{
					i = -1; while (++i!= word.letters.size())
					{
						while (packets[++count].command) {}
						packet.data.SetImage(word.x + i, word.letters[i]);
						packets[count] = packet;
						while (packets[++count].command) {}
						packet.data.SetImage(FILL, Song[index].color16);
						packets[count] = packet;
					}

				}
			}
		}

		//print out everything except for the beggining which we already printed out
		i = size / PACKET_SIZE - 1; while (++i != round( SONG_PACKETS + 2000))
			packets[i].printOut(fileOut);

		fileOut.close();
	}
	else
		cout << "File Failed To Open";

	for each (Line line in Song)
	{
		for each (Word word in line.words)
		{
			delete[] word.word;
		}
	}
	cout << "\nFile Created\n";

	system("Pause");

	return 0;
}