#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    ofstream file;
    file.open("test.png", ios::binary | ios::out);
    if (file.is_open())
    {
        // PNG header
        char cbuf = 0x89;
        file.write(&cbuf, 1);

        {
            char buf[] = "PNG";
            file.write(buf, sizeof(buf)-1);
        }
        {
            char buf[] = { 0x0D, 0x0A, 0x1A, 0x0A };
            file.write(buf, sizeof(buf));
        }
    }
    file.close();
    return 0;
}