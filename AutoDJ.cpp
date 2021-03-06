#include "AutoDJ.h"
#include <fstream>
using namespace std;

AutoDJ::AutoDJ() {
    _library = nullptr;
    _playlists = nullptr;
    _fileManager = new FileManager();

    readSongsFile("library.txt", "initialize");
    readPlaylistsFile("playlists.txt");
}

AutoDJ::~AutoDJ() {
    cout << "...saving updated library to ./library.txt"<< endl;
    _fileManager->writeToFile("library.txt", _library->toFileString());
    std::cout << "done" << std::endl;
    std::cout << "...saving updated playlists to ./playlists.txt"<< std::endl;
    _fileManager->writeToFile("playlists.txt", _playlists->toFileString());
    std::cout << "done" << std::endl;
    delete _playlists;
    delete _library;
    delete _fileManager;
}

void AutoDJ::readPlaylistsFile(std::string filename) {
    std::string playlistsString = _fileManager->readFromFile(filename), *playlistTitles;
    int *songCountPerPlaylist, playlistCount = 0;
    std::string ***playlistsArgs = playlistsStringToArray(playlistsString, playlistCount, 
                                                            songCountPerPlaylist, playlistTitles);

    _playlists = new Playlists(playlistCount);
    loadPlaylistsToPlaylists(playlistsArgs, playlistTitles, playlistCount, songCountPerPlaylist);


    delete[] playlistTitles;
    delete[] playlistsArgs;
    delete[] songCountPerPlaylist;
    playlistTitles = nullptr;
    playlistsArgs = nullptr;
    songCountPerPlaylist = nullptr;
}

void AutoDJ::readSongsFile(std::string filename, std::string operation) { 
    std::string songsString = _fileManager->readFromFile(filename);
    int songCount = 0;
    std::string **songArgs = songsStringToArray(songsString, songCount), 
        duplicates = "",
        notFound = "";

    if (operation == "import") {
        addNewSongsToLibraryFile(songArgs, songsString, songCount, duplicates);
        if (duplicates.length()) std::cout << "the following duplicates were found: \n" << duplicates << endl;
        else loadSongsToLibrary(songArgs, songCount);
    } 
    else if (operation == "discontinue") {
        removeSongsFromLibraryAndRewrite(songArgs, songsString, songCount, notFound);
        if (notFound.length()) std::cout << "the following songs do not exist: \n" << notFound << endl;
    } 
    else if (operation == "initialize") {
        _library = new Library(songCount);
        loadSongsToLibrary(songArgs, songCount);
    }
    delete songArgs;
    songArgs = nullptr;
}

void AutoDJ::loadSongsToLibrary(std::string **songArgs, int songCount) {
        std::cout << "loading " << songCount << " songs..." << std::endl;
        std::string *currArgs;
        Song *song;
        for (int i = 0; i < songCount; i++) {
            currArgs = songArgs[i];
            song = new Song(currArgs[0], currArgs[1], std::stof(currArgs[2]));
            std::cout << song->getTitle() << endl;
            _library->add(song);
        }
        currArgs = nullptr;
        std::cout << "\ndone\n\n";
}
void AutoDJ::addNewSongsToLibraryFile(std::string **songArgs, std::string &songsString, int songCount, std::string &duplicates) {
    std::cout << "checking for duplicate songs..." << std::endl;
    std::string *currArgs;
    for (int i = 0; i < songCount; i++) {
        currArgs = songArgs[i];
        if (_library->find(currArgs[0], currArgs[1]) >= 0) {
           duplicates += currArgs[0]+"\n"+currArgs[1]+"\n\n";
        }
    } 
    currArgs = nullptr;
    if (duplicates.length()) return;
    _fileManager->appendToFile("library.txt", songsString);  
}

void AutoDJ::removeSongsFromLibraryAndRewrite(std::string **songArgs, std::string &songsString, int songCount, std::string &notFound) {
    std::cout << "checking that songs exist..." << std::endl;
    std::string *currArgs;
    Playlist *currPlaylist;
    for (int i = 0; i < songCount; i++) {
        currArgs = songArgs[i];
        if (_library->find(currArgs[0], currArgs[1]) < 0) {
           notFound += currArgs[0]+"\n"+currArgs[1]+"\n\n";
        }
    } 
    if (notFound.length()) return;

    std::cout << "removing " << songCount << " songs..." << std::endl;
    Song *song;
    for (int i = 0; i < songCount; i++) {
        currArgs = songArgs[i];
        song = _library->remove(currArgs[0], currArgs[1]);

        // remove if in any playlists
        for (int j = 0; j < _playlists->getLength(); j++) {
            currPlaylist = _playlists->getPlaylistAt(j);
            for (int k = 0; k < currPlaylist->getLength(); k++) {
                if (currPlaylist->find(currArgs[0], currArgs[1]) > -1) {
                    currPlaylist->remove(currArgs[0], currArgs[1]);
                }
            }
        }
        std::cout << song->getTitle() << endl;
        delete song;
    }
    song = nullptr;
    currArgs = nullptr;
    currPlaylist = nullptr;
    std::cout << "\ndone\n\n";

    _fileManager->writeToFile("library.txt", _library->toFileString());
}

void AutoDJ::loadPlaylistsToPlaylists(std::string ***playlistsArgs, std::string *playlistTitles,
                                        int playlistCount, int *songCountPerPlaylist) {
    std::string **currPlaylist, 
                currPlaylistTitle,
                currSongTitle,
                currSongArtist;

    Playlist *playlist;
    Song *song;

    bool currDuplicate = false;

    std::cout << "loading " << playlistCount << " playlist(s)..." << std::endl;
    //[playlist][song][title/artist]
    for (int p = 0; p < playlistCount; p++) {
        currPlaylist = playlistsArgs[p];
        currPlaylistTitle = playlistTitles[p];

        if (_playlists->find(currPlaylistTitle) < 0) {
            playlist = new Playlist(currPlaylistTitle);
        } else {
            std::cout << "did not add duplicate: ";
            currDuplicate = true;
        }
        std::cout << currPlaylistTitle << std::endl;

        if (currDuplicate) {
            currDuplicate = false;
        } else {
            for (int s = 0; s < songCountPerPlaylist[p]; s++) {
                currSongTitle = currPlaylist[s][0];
                currSongArtist = currPlaylist[s][1];
                song = _library->getSong(currSongTitle, currSongArtist);
                if (song) {
                    if (playlist->find(currSongTitle, currSongArtist) < 0) {
                        playlist->add(song);
                    } else {
                        std::cout << "did not add duplicate song: " 
                            "\""<< currSongTitle << "\", " << "\"" 
                                << currSongArtist << "\"" << endl;
                    }
                } else {
                    std::cout << "song not found: \"" << currSongTitle << "\", " 
                        << "\"" << currSongArtist << "\"" << endl;
                }
            }
            _playlists->add(playlist);
        }

    }
    std::cout << "\ndone\n\n";
}

std::string*** AutoDJ::playlistsStringToArray(std::string playlistsString, int &playlistCount, 
                                                int *&songCountPerPlaylist, std::string *&playlistTitles) {
    std::string ***playlistArgs,
        currBlock = "",
        currChar = "",
        currArg = "";
    int argSep = 0, 
        songArgsIdx = 0,
        songCountPerIdx = 0,
        playlistTitlesIdx = 0,
        playlistArgsIdx = 0,
        songCount = 0;

    //allocate for playlist data
    for (int i = 0; i < playlistsString.length()-1; i++) {
        if (playlistsString[i] == *"*" && playlistsString[i+1] != *"\n") playlistCount++;
    }
    playlistArgs = new std::string**[playlistCount];
    playlistTitles = new std::string[playlistCount];
    songCountPerPlaylist = new int[playlistCount];

    for (int i = 0; i < playlistsString.length()-1; i++) {
        currChar = playlistsString[i];
        if (currChar == "*") {
            int j = i+1;
            currChar = playlistsString[j++];

            // get current playlist title
            while (currChar != "*" && currChar != "\n") {
                currBlock += currChar;
                currChar = playlistsString[j++];
            }
            std::string currTitle = currBlock;
            playlistTitles[playlistTitlesIdx++] = currTitle;

            // look ahead to get song count in current playlist, then allocate 
            int k = j+1;
            currChar = playlistsString[k++];
            while (currChar != "*" && currChar != "!") {
                if (currChar == ";" && currChar != "\n") songCount++;
                currChar = playlistsString[k++];
            }
            songCountPerPlaylist[songCountPerIdx] = songCount;
            //cout << *songCountPerPlaylist[songCountPerIdx] << " song(s)" <<endl;
            songCountPerIdx++;
            std::string **songArgs = new std::string*[songCount];
            int songArgsIdx = 0;
            songCount = 0;

            // grab artists & titles in current playlist until next
            k = j+1;
            currChar = playlistsString[k++];
            std::string *args = new std::string[2];
            while (currChar != "*" && currChar != "!") {
                if (currChar != "~" && currChar != ";" && currChar != "\n") currArg += currChar;
                else if (currChar == "~"){
                    args[argSep++] = currArg;
                    currArg = "";
                    if (argSep == 2) {
                        songArgs[songArgsIdx++] = args;
                        args = new std::string[2];
                        argSep = 0;
                    }
                }
                currChar = playlistsString[k++];
            }
            playlistArgs[playlistArgsIdx++] = songArgs;
            i = j-1;
        }
        currBlock = "";
        currChar = "";
    }
    return playlistArgs;
}

std::string** AutoDJ::songsStringToArray(std::string songsString, int &songCount) {
    std::string **songArgs,
        currBlock = "",
        currChar = "";
    int argSep = 0, 
        songArgsIdx = 0;

    for (int i = 0; i < songsString.length(); i++) {
        if (songsString[i] == *";") songCount++;
    }
    songArgs = new std::string*[songCount];

    for (int i = 0; i < songsString.length(); i++) {
        currChar = songsString[i];
        // build block to ;
        if (currChar != ";" && currChar != "\n") currBlock += currChar;
        else if (currChar == ";") {
            std::string *args = new std::string[3],
                currArg = "";
            for (int j = 0; argSep < 3; j++) {
                if (currBlock[j] != *"~") currArg += currBlock[j];
                else {
                    args[argSep++] = currArg;
                    currArg = "";
                }
            }
            songArgs[songArgsIdx++] = args;
            currBlock = "";
            argSep = 0;
        }
    }
    return songArgs;
}

std::string AutoDJ::library() { 
    return _library->toString(); 
}

std::string AutoDJ::artist(std::string artist) {
    return _library->findByArtist(artist);
}

std::string AutoDJ::song(std::string title, std::string artist) {
    std::string songString =  _library->findBySong(title, artist);
    if (songString.length()) return songString;
    else return title+", "+artist+" does not exist";
}

std::string AutoDJ::playlists() { 
    return _playlists->allPlaylistsString(); 
}

std::string AutoDJ::playlist(std::string title) {
    return _playlists->playlistString(title);
}

void AutoDJ::import(std::string filename) {
    readSongsFile(filename, "import");
}

void AutoDJ::discontinue(std::string filename) {
    readSongsFile(filename, "discontinue");
}

std::string AutoDJ::newPlaylist(std::string name) { 
    std::string status = "playlist: \"";
    if (_playlists->find(name) < 0) {
        _playlists->add(new Playlist(name)); 
        status += name+"\""+", added\n";
    } else status = name + "\" is already a playlist\n";
    return status;
}

std::string AutoDJ::add(std::string name, std::string title, std::string artist) {
    Song *song = _library->getSong(title, artist);
    Playlist *playlistFound = _playlists->getPlaylist(name);
    if (playlistFound) {
        std::string songStatus = "song: \""+title+"\", "+"\""+artist+"\"";
        if (song) {
            if (playlistFound->find(title, artist) < 0) {
                playlistFound->add(song);
                return songStatus+" added";
            } else return songStatus+" already exists in "+name;
        } else return songStatus+" not found";
        
    } else return "playlist: \""+name+"\" not found";
}

std::string AutoDJ::remove(std::string name, std::string title, std::string artist) {
    Song *song = _library->getSong(title, artist);
    Playlist *playlistFound = _playlists->getPlaylist(name);
    if (playlistFound) {
        std::string songStatus = "song: \""+title+"\", "+"\""+artist+"\"";
        if (song) {
            if (playlistFound->find(title, artist) > -1) {
                playlistFound->remove(title, artist);
                return songStatus+" removed";
            } else return songStatus+" not in "+name;
        } else return songStatus+" not found";
        
    } else return "playlist: \""+name+"\" not found";
}

std::string AutoDJ::playNext(std::string title) {
    Playlist *selectedPlaylist = _playlists->getPlaylist(title);
    std::string songString;
    if (selectedPlaylist) {
        try {
            songString = "    --now playing--\n"+
                selectedPlaylist->playNext()->toString();
            if (selectedPlaylist->isEmpty()) _playlists->remove(title);
            return songString;
        } catch (std::out_of_range &e) {
            songString = "playlist: \""+title+"\", is empty";
        }
    } else {
        songString = "playlist: \""+title+"\", not found";
    }
    return songString;
}

void AutoDJ::newRandom(std::string title, float duration) {
    _playlists->add(new Playlist(title, duration, *_library));
}
