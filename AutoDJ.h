#ifndef AUTO_DJ_AUTO_DJ
#define AUTO_DJ_AUTO_DJ

#include "Library.h"
#include "Playlists.h"
#include "FileManager.h"

class AutoDJ {
    private:
        Library *_library;
        Playlists *_playlists;
        FileManager *_fileManager;

        void readSongsFile(std::string filename, std::string operation);

        void readPlaylistsFile(std::string filename);

        void addNewSongsToLibraryFile(std::string **songArgs, std::string &songsString, int songCount, std::string &duplicates);

        void removeSongsFromLibraryAndRewrite(std::string **songArgs, std::string &songsString, int songCount, std::string &notFound);

        void loadSongsToLibrary(std::string **songArgs, int songCount);

        void loadPlaylistsToPlaylists(std::string ***playlistsArgs, std::string *playlistTitles,
                                                int playlistCount, int *songCountPerPlaylist);

        std::string** songsStringToArray(std::string songsString, int &songCount);

        std::string*** playlistsStringToArray(std::string playlistsString, int &playlistCount, 
                                                int *&songCountPerPlaylist, std::string *&playlistTitles);

        

    public:
        AutoDJ();

        ~AutoDJ();

        std::string library();

        std::string playlists();

        std::string playlist(std::string playlistName);

        std::string artist(std::string artist);

        std::string song(std::string artist, std::string title);

        void import(std::string filename);

        void discontinue(std::string fileName);

        std::string newPlaylist(std::string name);

        std::string add(std::string name, std::string artist, std::string title);

        std::string remove(std::string name, std::string artist, std::string title);

        std::string playNext(std::string playlistName);

        void newRandom(std::string playlistName, float duration);
};

#endif
