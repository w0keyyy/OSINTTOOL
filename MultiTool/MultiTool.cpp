#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

using namespace std;

const string PHONE_API_TOKEN = "0b46363467f36e18694fb9425f69788284c4dc10";
const string IP_API_KEY = "a44ab571c637431aa77efaba7d330cc5";
const string USER_DATA_FILE = "users.txt";

void printOSINTLogo() {
    cout << "\n\n";
    cout << string(30, ' ') << "  ██████  ███████ ██ ███    ██ ████████     ████████  ██████   ██████  ██      \n";
    cout << string(30, ' ') << " ██       ██      ██ ████   ██    ██           ██    ██    ██ ██    ██ ██      \n";
    cout << string(30, ' ') << " ██   ███ █████   ██ ██ ██  ██    ██           ██    ██    ██ ██    ██ ██      \n";
    cout << string(30, ' ') << " ██    ██ ██      ██ ██  ██ ██    ██           ██    ██    ██ ██    ██ ██      \n";
    cout << string(30, ' ') << "  ██████  ███████ ██ ██   ████    ██           ██     ██████   ██████  ███████ \n";
    cout << "\n\n";
}


//очистка экрана и центрирование текста
void clearScreen() {
    system("cls");
    printOSINTLogo();  // Вывод ASCII-арта
}




// Функция для отправки HTTP-запроса
string sendRequest(const string& url) {
    HINTERNET hInternet = InternetOpenA("MultiTool", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "Ошибка InternetOpen";

    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "Ошибка InternetOpenUrl";
    }

    char buffer[4096];
    DWORD bytesRead;
    string response;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return response;
}

// тут написал парсер для json вроде ворк хз
string getValue(const string& json, const string& key) {
    size_t start = json.find("\"" + key + "\":");
    if (start == string::npos) return "Не найдено";

    start += key.length() + 3;
    if (json[start] == '"') {
        start++;
        size_t end = json.find("\"", start);
        return json.substr(start, end - start);
    }
    else {
        size_t end = json.find(",", start);
        if (end == string::npos) end = json.find("}", start);
        return json.substr(start, end - start);
    }
}

// тут регатся нужно
void registerUser() {
    clearScreen();
    string username, password;
    cout << "Введите логин: ";
    cin >> username;
    cout << "Введите пароль: ";
    cin >> password;

    ofstream file(USER_DATA_FILE, ios::app);
    file << username << " " << password << endl;
    file.close();

    cout << "Регистрация успешна! Нажмите ENTER для продолжения...";
    cin.ignore();
    cin.get();
}

// тут логинн
bool loginUser() {
    clearScreen();
    string username, password, fileUser, filePass;
    cout << "Введите логин: ";
    cin >> username;
    cout << "Введите пароль: ";
    cin >> password;

    ifstream file(USER_DATA_FILE);
    while (file >> fileUser >> filePass) {
        if (fileUser == username && filePass == password) {
            file.close();
            return true;
        }
    }
    file.close();

    cout << "Ошибка входа! Нажмите ENTER для выхода...";
    cin.ignore();
    cin.get();
    return false;
}

// гетинфо функция для получения инфы про номер тела
void getPhoneInfo() {
    clearScreen();
    string phone;
    cout << "Введите номер телефона (без +7): ";
    cin >> phone;

    string url = "https://api.regius.name/iface/phone-number.php?phone=" + phone + "&token=" + PHONE_API_TOKEN;
    string response = sendRequest(url);

    cout << "\nОператор: " << getValue(response, "company") << " (" << getValue(response, "brand") << ")" << endl;
    cout << "Регион: " << getValue(response, "region") << endl;
    cout << "Часовой пояс: UTC" << getValue(response, "utc") << endl;

    cout << "\nНажмите ENTER для продолжения...";
    cin.ignore();
    cin.get();
}

// тут гетайпи для получения данных об ИП
void getIPInfo() {
    clearScreen();
    string ip;
    cout << "Введите IP-адрес: ";
    cin >> ip;

    string url = "https://api.ipgeolocation.io/ipgeo?apiKey=" + IP_API_KEY + "&ip=" + ip;
    string response = sendRequest(url);

    cout << "\nСтрана: " << getValue(response, "country_name") << endl;
    cout << "Город: " << getValue(response, "city") << endl;
    cout << "Провайдер: " << getValue(response, "isp") << endl;

    cout << "\nНажмите ENTER для продолжения...";
    cin.ignore();
    cin.get();
}

int main() {
    setlocale(LC_ALL, "RU");

  
    int choice;
    while (true) {
        clearScreen();
        cout << "1. Регистрация\n";
        cout << "2. Вход\n";
        cout << "3. Выход\n";
        cout << "Выберите опцию: ";
        cin >> choice;

        if (choice == 1) registerUser();
        else if (choice == 2 && loginUser()) break;
        else if (choice == 3) return 0;
    }

    // main menu sigma
    while (true) {
        clearScreen();
        cout << "1. Пробить номер телефона\n";
        cout << "2. Пробить IP-адрес\n";
        cout << "3. Выйти\n";
        cout << "Выберите опцию: ";
        cin >> choice;

        if (choice == 1) getPhoneInfo();
        else if (choice == 2) getIPInfo();
        else if (choice == 3) return 0;
    }
}
