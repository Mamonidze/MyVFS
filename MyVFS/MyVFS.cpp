#include <iostream>  // Подключаем заголовочный файл для ввода/вывода данных
#include <fstream>// Подключаем заголовочный файл для работы с файлами
#include <string>    // Подключаем заголовочный файл для работы со строками
#include <mutex>     // Подключаем заголовочный файл для работы с мьютексами
#include <unordered_map>  // Подключаем заголовочный файл для работы с хэш-картами
#include <filesystem>
using namespace  std;

    struct File
    {
        //default construct
        File()
        {
            this-> filename = "test";
            this-> IsReadMode = false;
            this-> IsWriteMode = false;
            this-> data = "data";
        }

        File(const string& filename, bool readOnly, bool writeOnly)
                : filename(filename), IsReadMode(readOnly), IsWriteMode(writeOnly)
        {
            if (IsReadMode)
            {
                this->IsReadMode = true;
            }
            else
            {
                this->IsWriteMode = true;
            }
        }
    
    
    
        //overloaded construct
        /*File(string filename, bool IsReadMode, bool IsReadMode, string data)
        {
            this-> filename = filename;
            this-> IsReadMode = IsReadMode;
            this-> IsReadMode = IsReadMode;
            this-> data = data;
        }*/
    
        string filename;
        bool IsReadMode;
        bool IsWriteMode;
        string data;

        void Print()
        {
            cout << "Filename: " << filename << "isReadMode: " << IsReadMode << "isWriteMode: " << IsWriteMode << "Data: " << data << endl;
        }
    
    };

    struct IVFS
    {
        /*virtual*/ File* Open(const char* name);             // Открыть файл в режиме только для чтения
        /*virtual*/ File* Create(const char* name);           // Открыть или создать файл в режиме только для записи
        /*virtual*/ size_t Read(File* f, char* buff, size_t len);    // Прочитать данные из файла. Возвращает количество прочитанных байт
        /*virtual*/ size_t Write(File* f, char* buff, size_t len);   // Записать данные в файл. Возвращает количество записанных байт
        /*virtual*/ void Close(File* f);                      // Закрыть файл
    };



    struct VFS: IVFS
    {
        unordered_map<string, File*> openFiles; //хэшка, где хранятся все файлы для корректной работы программы
    

        // Метод для проверки, является ли файл только для чтения
        bool IsFileReadOnly(const string &filename, bool readOnly) const
        {
            auto it = openFiles.find(filename); // находим файл в хранилище файлов.
            if (it != openFiles.end())
            {
                File* file = it->second;
                if (file->IsReadMode == readOnly) // проверяем является ли файл Readonly
                    return true;
            }
            return false;
        }

        
    
    
        // Метод для проверки, является ли файл только для записи
        bool IsFileWriteOnly(const string &filename, bool writeOnly) const
        {
            auto it = openFiles.find(filename);
            if (it != openFiles.end())
            {
                File* file = it->second;
                if (file->IsWriteMode == writeOnly)
                    return true;
            }
            return false;
        }


        //функция открывает файл, если файл уже есть с таким именем. Если такого файла нет, то создает его и открывает.
        File* Open(const char* name) /*override*/
        {
            /*
            std::lock_guard<std::mutex> lock(mutex); //блокируме мьютекс, пока работаем с файлом.
            */
            File* file;

            auto it = openFiles.find(name); // находим файл в хранилище файлов.
            if (it != openFiles.end())
            {
            
                cout << "File opened succesfully!";
            }
            else
            {
                file = new File(name, true, false);
                openFiles[name] = file;
                WritetoFile(file);
            }
            return openFiles[name];
        }




        //функция создает и открывает файл
        File* Create(const char* name) /*override*/
        {
            /*std::lock_guard<std::mutex> lock(mutex);*/

            //проверка на наличие файла
            File* file;
            
            auto it = openFiles.find(name); // находим файл в хранилище файлов.
            if (it != openFiles.end())
            {
                cout << "File with this name is already exist. " << endl;
                return openFiles[name];
            }
                file = new File();        // Создаем новый объект файла
                openFiles[name] = file;      // Добавляем файл в хранилище
                WritetoFile(file);
                return file;
        }

        //функа для чтения 1 файла из файла files
        size_t Read(File* file, char* buff, size_t len)
        {
            /*
            std::lock_guard<std::mutex> lock(f->mutex);    // Захватываем мьютекс файла для синхронизации доступа
            */

            fstream NewFile;
            NewFile.open("files.txt");

            NewFile.read(buff, len);
            size_t bytesRead = len;       // Получаем количество прочитанных байт

            return bytesRead;

        }

        void SearchInFiles(fstream fin,char* filename)/*initializing argument 1 of 'void search(std::fstream, char*)'*/
        {
            File* f;
            fin.open("files.txt");
            while(!fin){
                fin.read((char*)&f,sizeof(f));
                if(f->filename == filename){
                    cout<<"Record found !";
                    break;
                }
            }
            fin.close();
        }
    


        /*
        //функа для чтения данных из файлика со всеми файлами
        void ReadFromFile(File* file)
        {
            
        }
        */
        
        //функа для записи объекта в файл 
        void WritetoFile(File* file)
        {
            ofstream NewFile;
            NewFile.open("files.txt", ofstream::app);
            if (!(NewFile.is_open()))
            {
                cout << "Open File Error!" << endl;
            }
            else
            {
                NewFile.write((char*)&file, sizeof(File));
            }
            NewFile.close();
        }
    };












    File* temp;
    enum Options {Open = 1  , Create, Read, Write, Close, ExitProgram};



    void MainMenu()
    {
        std::cout << "Main Menu" << endl;
        std::cout << "---------" << endl;
        std::cout << "What do you want, bitch?" << endl;
        std::cout << " " << endl;
        std::cout << "1. Open file" << endl;
        std::cout << "2. Create new file" << endl;
        std::cout << "3. Read file" << endl;
        std::cout << "4. Write file" << endl;
        std::cout << "5. Close file" << endl;
        std::cout << "6. Exit IVFS" << endl;
        if (temp)
        {
            cout << "File " << temp->filename << " is opened" << endl;
        }
    }

    Options InputCheck()
    {
        char Input = '0';
        cin >> Input;

        while (!('0' < Input && Input < '7'))
        {
            cout << "Error, type the right option!";
            cin >> Input;
        }
        return static_cast<Options>((int)Input-'0');
    }








    //функа для записи в файлик со всеми файлами
  




    void main()
    {

        VFS MainIVFS;
        string filename;
        char buffer[256];
        
    
        int Input;
        do
        {
            MainMenu();
            Input = InputCheck();
       
            switch(Input) {
            case Create:
                {
                    cout << "Enter filename" << endl;
                    cin >> filename;
                
                    temp = MainIVFS.Open(filename.c_str());
                    MainIVFS.ReadFromFile(temp);

                    
                    break;
                }
            case Open:
                {
                    
                    /*if (temp != NULL)
                    {
                        cout << "Close the opened file"<< endl;
                        break;
                    }*/
                    /*size_t bytesRead;
                    bytesRead = MainIVFS.Read(temp, buffer, sizeof(buffer) - 1 );
                     cout << "Readed byte from file: " << bytesRead << endl;*/

                }
        
            }
        }while (Input != ExitProgram);
    }


// нужна мапа с открытыми файлами
// нужна мапа со всеми файлами
// нужен файлик куда будут записываться файлы
// нужны функи которые переводят файл в рид онли и райт онли
//функа для чтения 1 файла
//функа для вывода прочтенной инфы на экран