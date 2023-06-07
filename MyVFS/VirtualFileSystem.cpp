#include "filesys.h"
#include  <thread>
using namespace std;

VFS IVFS1;


enum Options {AllFiles = 1, Open, Create, Read, Write, Writeonly, ReadOnly, Close, ExitProgram = 0};

    void MainMenu()
    {
        system("cls");
        cout << "Main Menu" << endl;
        cout << "---------" << endl;
        cout << "What do you want?" << endl;
        cout << "1. List all files " << endl;
        cout << "2. Open file" << endl;
        cout << "3. Create new file" << endl;
        cout << "4. Read file" << endl;
        cout << "5. Write file" << endl;
        cout << "6. Switch to WriteOnly" << endl;
        cout << "7. Switch to ReadOnly" << endl;
        cout << "8. Close file" << endl;
        cout << "0. Exit VFS" << endl;

        IVFS1.ListOfOpenedFiles();
        
    }

    Options InputCheck()
    {
        char Input = '0';
        cin >> Input;

        while (!('0' < Input && Input < '9'))
        {
            cout << "Error, type the right option!" << Input << endl;
            cin >> Input;
        }
        return static_cast<Options>((int)Input-'0');
    }

    int main()
    {
        /*
        setlocale(LC_ALL, "Rus");
        */
        
        string filename;

        int Input;
        do
        {
            MainMenu();
            Input = InputCheck();
       
            switch(Input) {
            case AllFiles:
                {
                    cout << "List of all files: " << endl; 
                    IVFS1.ListOfAllFiles();
                    break;
                }
            case Open:
                {
                    cout << "Enter filename" << endl;
                    cin >> filename;
                    IVFS1.Open(filename.c_str());
                    break;
                }
            case Create:
                {
                    CreateFile(IVFS1);
                    break;
                }
            case Read:
                {
                    ViewFile(IVFS1);
                    break;
                }
            case Write:
                {
                    WriteToFile(IVFS1);
                    break;
                }
            case Writeonly:
                {
                    FileToWriteOnly(IVFS1);
                    break;
                }
            case ReadOnly:
                {
                    FileToReadOnly(IVFS1);
                    break;
                }
            case Close:
                {
                    CloseFile(IVFS1);
                    break;
                }
            default:
                {
                    cout << "Incorrect choice. Try again" << endl;
                    break;
                }
            }
            if (Input != ExitProgram)
                system("pause");
        } while (Input != ExitProgram);

        return 0;
    }























