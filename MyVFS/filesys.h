#pragma once
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <string>
#include <fstream>
using namespace std;

struct File
{
    char filename[128];
    char data[4096];
    bool isReadOnly;
    mutex mutex;

    //конструктор
    File(const string& filename, bool ReadMode): isReadOnly(ReadMode)
    {
        copy_n(filename.c_str(), sizeof(this->filename) - 1, this->filename);
        this->filename[sizeof(this->filename) - 1] = '\0';
        this->data[0] = 0;
    }

    // Конструктор копирования
    File(const File& other)
    {
        copy_n(other.filename, sizeof(this->filename) - 1, this->filename);
        this->filename[sizeof(this->filename) - 1] = '\0';
        memcpy(data, other.data, sizeof(data));
        this->isReadOnly = other.isReadOnly;
    }

    void Print()
    {
        cout << "Filename: " << filename << endl;
        cout << "Data: " << data << endl;
    }
};

struct IVFS
{
    virtual File* Open(const char* name) = 0;             // Открыть файл в readonly режиме. Если нет такого файла или же он открыт во writeonly режиме - вернуть nullptr
    virtual File* Create(const char* name) = 0;           // Открыть или создать файл в writeonly режиме. Если нужно, то создать все нужные поддиректории, упомянутые в пути. Вернуть nullptr, если этот файл уже открыт в readonly режиме.
    virtual size_t Read(File* f, char* buff, size_t len) = 0;    // Прочитать данные из файла. Возвращаемое значение - сколько реально байт удалось прочитать
    virtual size_t Write(File* f, char* buff, size_t len) = 0;   // Записать данные в файл. Возвращаемое значение - сколько реально байт удалось записать
    virtual void Close(File* f) = 0;                // Закрыть файл*/

    virtual ~IVFS() = default;
};

struct VFS : IVFS
{
    ~VFS() override = default;
    unordered_map<string, File*> openFiles;
    mutex mutex;
    
    File* Open(const char* name) override;
    File* Create(const char* name) override;
    size_t Write(File* file, char* buff, size_t len) override;
    size_t Read(File* file, char* buff, size_t len) override;
    void Close(File* file);

    bool IsFileReadOnly(const std::string& filename) const;
    bool IsFileWriteOnly(const string& filename) const;
    void toReadOnly(File* file);
    void toWriteOnly(File* file);
    
    File* FileSearch(const char* filename);
    void WritetoFile(File* file);
    void DeleteFile(File* file);
    void AddFileToOpenedFiles(File* file);
    void ListOfOpenedFiles();
    void ListOfAllFiles();
    
};

void CreateFile(VFS &vfs);
void ViewFile(VFS& vfs);
void WriteToFile(VFS& vfs);
void FileToWriteOnly(VFS& vfs);
void FileToReadOnly(VFS& vfs);
void CloseFile(VFS &vfs);


