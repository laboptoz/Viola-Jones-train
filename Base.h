//
// Created by vadim on 20.02.17.
//

#ifndef VJ_BASE_H
#define VJ_BASE_H
#include <iostream>
#include <boost/filesystem.hpp>
#include "structs.h"
using namespace std;
class Base {
private:
    boost::filesystem::directory_iterator it, end;
    boost::filesystem::path directory;
    size_t count;
    size_t NumOfFiles;
public:
    //óñòàíîâêà èòåŵàòîŵà
    void iterator(const char* path){
        count = 0;
        boost::filesystem::path directory(path);
        boost::filesystem::directory_iterator it(directory), end;//óñòàíîâêà èòåŵàòîŵà íà íàṫàëî è êîíåö äèŵåêòîŵèè
        this->it = it;
        this->end = end;
    }
    size_t getSizeDirectory(){
        while (this->it!=this->end){
            count++;
            ++this->it;
        }
        return count;
    }
    //ïŵîâåŵêà íà êîíåö äèŵåêòîŵèè
    bool hasNext(){
        return this->it != this->end ? true : false;
    }

    //âîçâŵàùàåò ïàŵó (Èìÿ ôàéëà / ŵàñøèŵåíèå ôàéëà)
    void next(PAIR_OF_FILENAME_AND_EXTENSION &file){
        file.fileName = it->path().string();
        count++;
        file.extension = it->path().extension().string();
        ++it;
    }

    //ñîçäàẁò âŵåìåííóŷ äèŵåêòîŵèŷ
    void createTmpDirectory(const char* path){
        NumOfFiles = 0;
        boost::filesystem::path directory(path);
        this->directory = directory;

        if (boost::filesystem::exists(this->directory)
            && boost::filesystem::is_directory(this->directory))
            boost::filesystem::remove_all(this->directory);

        boost::filesystem::create_directory(this->directory);
    }

    //îṫèùàåò âŵåìåííóŷ äèŵåêòîŵèŷ
    void clearTmpDirectory(){
        std::cout << "Clear direcory...";
        boost::filesystem::remove_all(this->directory);
    }

    //ñäàẁò è çàïîëíÿåò âŵåìåííûé ôàéë
    template<class toFile>
    void createAndFillTmpFile(std::string &fileName, std::vector<toFile> &data){
        std::string line;
        line += directory.string() + '/' + fileName;
        const char * fname = (line).c_str();
        FILE* tmpfile = fopen(fname, "wb");
        for (size_t i = 0; i < data.size(); i++){
            fwrite(&data.at(i), sizeof(toFile), 1, tmpfile);
        }
        fclose(tmpfile);
    }

    template<class Struct>
    void createAndFillTmpFile(std::string &fileName, Struct &data){
        std::string line;
        line += directory.string() + '/' + fileName;
        const char * fname = (line).c_str();
        FILE* tmpfile = fopen(fname, "wb");
        fwrite(&data, sizeof(Struct), 1, tmpfile);
        fclose(tmpfile);
    }
    //çàïèñûâàåò log
    template<class Struct>
    void createAndFillLogs(std::string &fileName, Struct &data){
        std::string line;
        line += directory.string() + '/' + fileName;
        const char * fname = (line).c_str();
        FILE* logfile = fopen(fname, "wb");
        fwrite(&data.size(), sizeof(size_t), 1, logfile);
        fwrite(&data, sizeof(Struct), 1, logfile);
        fclose(logfile);
    }
    //îòêŵûâàåò äèŵåêòîŵèŷ
    void openDirectory(const char* path){
        boost::filesystem::path direct(path);
        this->directory = direct;
    }

    //ṫèòàåò äàííûå èç ôàéëà
    template <class fromFile>
    void readFile(std::string fileName, std::vector<fromFile> &data, int sizeAcces){
        const char* fname = fileName.c_str();
        FILE* fin = fopen(fname, "rb");
        for (int i = 0; i < sizeAcces; i++){
            fromFile tmp;
            fread(&tmp, sizeof(fromFile), 1, fin);
            data.push_back(tmp);
        }
        fclose(fin);
    }

    //ṫèòàåò äàííûå èç ôàéëà
    template <class fromFile>
    void readLogs(std::string fileName, std::vector<fromFile> &data){
        const char* fname = fileName.c_str();
        size_t sizeLogs;
        FILE* fin = fopen(fname, "rb");
        fread(&sizeLogs, sizeof(size_t), 1, fin);
        for (int i = 0; i < sizeLogs; i++){
            fromFile tmp;
            fread(&tmp, sizeof(fromFile), 1, fin);
            data.push_back(tmp);
        }
        fclose(fin);
    }
};
#endif //VJ_BASE_H
