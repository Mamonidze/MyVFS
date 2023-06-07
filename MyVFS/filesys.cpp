#include "filesys.h"

//Check if file is ReadOnly
bool VFS::IsFileReadOnly(const std::string& filename) const
{
    auto it = openFiles.find(filename);
    if (it != openFiles.end())
    {
        File* file = it->second;
        if (file->isReadOnly == true)
            return true;
    }
    return false;
}


//Check if file is WriteOnly
bool VFS::IsFileWriteOnly(const string& filename) const
{
    auto it = openFiles.find(filename);
    if (it != openFiles.end())
    {
        File* file = it->second;
        if (file->isReadOnly == false)
            return true;
    }

    return false;
}


//Add file to unordered map of open files
File* VFS::Open(const char* name) 
{
    lock_guard<std::mutex> lock(mutex);
    
    File* file;
    auto it = openFiles.find(name);
    if (it != openFiles.end())
    {
        file = it->second;
        cout << "File " << file->filename << " is already opened!" << endl;
    }
    else
    {
        file = FileSearch(name); //Looking for a file in files.txt

        if (file)
        {
            openFiles[name] = file;
            toReadOnly(file);
            cout << "File " << file->filename << " is opened!" << endl;

        }
        else
        {
            cout << "File doesn't exist!"<< endl;
        }
        return file;
    }
    return NULL;
}


//Create a file
File* VFS::Create(const char* name) 
{
    File* file = new File(name,false);
    WritetoFile(file);
    return file;
}


//Write data to existing file
size_t VFS::Write(File* file, char* buff, size_t len) 
{
    lock_guard<std::mutex> lock(file->mutex);
    File* NewFile(file);
    DeleteFile(file);
        
    for (int i = 0; i < len; ++i)
    {
        NewFile->data[i] = buff[i];
    }
    NewFile->data[len] = '\0';

    WritetoFile(NewFile);
    size_t byteswritten = len;
    return byteswritten;
}


//Read from opened file
size_t VFS::Read(File* file, char* buff, size_t len) 
{
    lock_guard<std::mutex> lock(file->mutex);
    size_t bytesRead = 0;
    for (int i = 0; i < sizeof file->data - 1; ++i)
    {
        if (file->data[i] != '\0')
        {
            buff[i] = file->data[i];
            bytesRead++;
        }
        else
        {
            break;
        }
    }
    return bytesRead;
}


//Search in files.txt
File* VFS::FileSearch(const char* filename)
{
    fstream fin;
    fin.open("files.txt", fstream::in);
    File* file = new File(filename,true);
    while (fin.read((char*)file, sizeof(File)))
    {
        if (fin.eof())
            break;
        if (!(strcmp(filename,file->filename)))
        {
            fin.close();
            cout << "File was found!" <<  endl;
            return file;
        }
    }
    fin.close();
    /*delete file;*/
    return NULL;
}


//Write to files.txt
void VFS::WritetoFile(File* file)
{
    ofstream NewFile;
    NewFile.open("files.txt", ofstream::app|ofstream::binary);
    NewFile.write((char*)file,sizeof(File));
    AddFileToOpenedFiles(file);
    NewFile.close();
}


//Delete file from files.txt
void VFS::DeleteFile(File* file)
{
    fstream fin;
    fin.open("files.txt", fstream::in);
    vector<File> files;
    File* ReadFile = new File("name",1);
    while (fin.read((char*)ReadFile, sizeof(File)))
    {
        files.push_back(*ReadFile);
    }
    fin.close();

    ofstream fout;
    fout.open("files.txt");

    for (auto item:files)
    {
        if (!strcmp(item.filename,file->filename))
        {
            continue;
        }
        fout.write((char*)&item,sizeof(File));
    }
    Close(file);
}


//Remove file from Opened files
void VFS::Close(File* file)
{
    lock_guard<std::mutex> lock(mutex);
    auto it = openFiles.find(file->filename);
    if (it != openFiles.end())
    {
        openFiles.erase(file->filename);
    }
}


//Add file to openedfiles
void VFS::AddFileToOpenedFiles(File* file)
{
    auto it = openFiles.find(file->filename);
    if (it == openFiles.end())
    {
        openFiles[file->filename] = file;
    }
}


//List of opened files
void VFS::ListOfOpenedFiles()
{
    cout << "Currently opened files: " << endl;
    for(auto& item : openFiles)
    {
        string Mode;
        if (item.second->isReadOnly)
        {
            Mode = "ReadOnly";
        }
        else
        {
            Mode = "WriteOnly";
        }
        cout << item.first << " : " << item.second->data << " : " << Mode << endl;
    }
    cout << "______________________" << endl;
}


//Switch file to ReadOnly
void VFS::toReadOnly(File* file)
{
    file->isReadOnly = true; 
}


//Switch file to WriteOnly
void VFS::toWriteOnly(File* file)
{
    file->isReadOnly = false;
}


//Switch file to WriteOnly
void VFS::ListOfAllFiles()
{
    ifstream AllFiles;
    AllFiles.open("files.txt");
    
    vector<File> Files;
    File* file = new File("name",1);
    while (AllFiles.read((char*)file, sizeof(File)))
    {
        Files.push_back(*file);
    }
    AllFiles.close();

    
    for (auto value : Files)
    {
        cout << value.filename << endl;
    }
}


//Create a file
void CreateFile(VFS &vfs)
{
    string filename, content;

    cout << "Enter file name: ";
    cin.ignore();
    getline(cin,filename);

    

    while (vfs.FileSearch(filename.c_str()))
    {
        cout << "File with the same name already exists! Type another name!" << endl;
    }


    auto it = vfs.openFiles.find(filename);
    if (it != vfs.openFiles.end())
    {
        cout << "File " << filename << " is already opened!" << endl;
    }

    
    if (vfs.IsFileReadOnly(filename))
        {
            cout << "File is already opened in ReadOnly mode!" << endl;
            return;
        }
   vfs.Create(filename.c_str());
}


//View File
void ViewFile(VFS& vfs)
{
    lock_guard<std::mutex> lock(mutex);
    string filename;
    cout << "Enter file name: " << endl;
    cin >> filename;
    
    auto it = vfs.openFiles.find(filename);
    if (it != vfs.openFiles.end())
    {
        
        if (vfs.IsFileWriteOnly(filename))
        {
            cout << "File is in WriteOnly mode!" << endl;
            return;
        }
        
        char buffer[4096];
        size_t bytesRead = vfs.Read(it->second, buffer, sizeof(buffer) - 1); 
        buffer[bytesRead] = '\0';
        cout << "Read: " << bytesRead << " bytes!"<< endl;
        /*vfs.Close(it->second);*/
    }
    else
    {
        cout << "Failed to open a file. It doesn't exist or file is in WriteOnly mode!" << endl;
    }
}


//Write new data to file
void WriteToFile(VFS& vfs)
{
    string filename, content;

    cout << "Enter file name: " << endl;
    cin >> filename;

    auto it = vfs.openFiles.find(filename);
    if (it != vfs.openFiles.end())
    {
        if (vfs.IsFileReadOnly(filename))
        {
            cout << "File is in ReadOnly mode!" << endl;
            return;
        }
        
        cout << "Введите новые данные: ";
        cin.ignore(); // Очистить буфер ввода
        getline(std::cin, content);
        const char* data = content.c_str();
        
        size_t bytesWritten = vfs.Write(it->second, const_cast<char*>(data), content.length());
        cout << bytesWritten << " bytes were written to the file!" << endl;
        
        /*
        vfs.Close(it->second); // - закрывает файл
    */
    }
 
}


//Switch file to WriteOnly
void FileToWriteOnly(VFS& vfs)
{
    string filename;
    cout << "Enter file name: ";
    cin >> filename;

    auto it = vfs.openFiles.find(filename); // ищем в 
    if (it != vfs.openFiles.end())
    {
        if (vfs.IsFileReadOnly(filename))
        {
            vfs.toWriteOnly(it->second);
            cout << "File " << filename << " was switched to WriteOnly!" << endl;
        }
        else if (vfs.IsFileWriteOnly(filename))
        {
            cout << "File " << filename << " is already in WriteOnly!" << endl;
        }
    }
    else
    {
        cout << "Failed to access the file!" << endl; 
    }
}


//Switch file to Readonly
void FileToReadOnly(VFS& vfs)
{
    string filename;
    cout << "Enter file name: ";
    cin >> filename;

    auto it = vfs.openFiles.find(filename); // ищем в 
    if (it != vfs.openFiles.end())
    {
        if (vfs.IsFileWriteOnly(filename))
        {
            vfs.toReadOnly(it->second);
            cout << "File " << filename << " was switched to ReadOnly!" << endl;
        }
        else if (vfs.IsFileReadOnly(filename))
        {
            cout << "File " << filename << " is already in ReadOnly!" << endl;
        }
    }
    else
    {
        cout << "Failed to access the file!" << endl; 
    }
}


//Close file
void CloseFile(VFS &vfs)
{
    string filename;
    cout << "Enter file name: " << endl;
    cin >> filename;

    
    auto it = vfs.openFiles.find(filename); // ищем в 
    if (it != vfs.openFiles.end())
    {
        vfs.openFiles.erase(it);
    }
}

