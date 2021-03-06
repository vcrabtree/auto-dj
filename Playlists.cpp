#include "Playlists.h"

Playlists::Playlists(int initialCapacity) {
    playlistList = new ArrayList<Playlist>(initialCapacity);
}

Playlists::~Playlists() { delete playlistList; }

void Playlists::add(Playlist* playlist) { playlistList->add(playlist); }

Playlist* Playlists::remove(std::string title) { return playlistList->remove(title); }

int Playlists::find(std::string title) { return playlistList->find(title); }

Playlist* Playlists::getPlaylistAt(int index) { return playlistList->getItemAt(index); }

Playlist* Playlists::getPlaylist(std::string title) { return playlistList->getItem(title); }

std::string Playlists::playlistString(std::string title) { 
    int index = playlistList->find(title);
    return "    **"+title+"**\n"+playlistList->getItemAt(index)->toString(); 
}

std::string Playlists::allPlaylistsString() {
    std::string playlistsString = "";
    Playlist* playlist;
    for (int i = 0; i < playlistList->getLength(); i++) {
        playlist = playlistList->getItemAt(i);
        playlistsString += "title: "+playlist->getTitle()+
                           "\t("+std::to_string(playlist->getDuration())+")\n";
    }
    playlist = nullptr;
    return playlistsString;
};

std::string Playlists::toFileString() { 
    Playlist *currPlaylist;
    std::string fileString = "";
    for (int i = 0; i < getLength(); i++) {
        currPlaylist = getPlaylistAt(i);
        fileString += "*"+currPlaylist->getTitle()+"*\n";
        for (int j = 0; j < currPlaylist->getLength(); j++) {
            fileString += currPlaylist->getSongAt(j)->toFileString(true);
        }
    }
    fileString += "!";
    return fileString;
}

void Playlists::clear() { 
    int originalCapacity = getLength();
    delete playlistList;
    playlistList = new ArrayList<Playlist>(originalCapacity);
}

bool Playlists::isEmpty() { return playlistList->isEmpty(); }

int Playlists::getLength() { return playlistList->getLength(); }
