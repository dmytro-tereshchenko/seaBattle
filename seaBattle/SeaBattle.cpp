#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)

#include <iostream>
#include <windows.h>
#include <time.h>
using namespace std;

//Выбор CONFIGURATION если игра будет предусматривать настройку (изменение параметров)
//#define CONFIGURATION

void initialFrameList(char**, int); //инициализация стартового пустого фрейма
void printList(char **, char **, int);//вывод поля игрока и поля противника для стрельбы
void printListWithBoat(char**, char**, int, int*);//вывод поля игрока и поля противника для стрельбы с перечнем кораблей только игрока
void printListWithBoatGame(char**, char**, int, int*, int*);//вывод поля игрока и поля противника для стрельбы с перечнем кораблей игрока и противника
void autoPutBoat(char**, int, int*);//автоматическая расстановка кораблей
void initialNullArrBoat(int[2][4]);//инициализация нулями массива с координатами корабля
int freeCells(char**, int); //определения количества возможных ячеек для установки корабля
int freeCellsEnemy(char**, int);//определения количества возможных ячеек для выстрела компьютера когда естьв наличии только однопалубные корабли
int freeAdvancedSearch(char **, int, int *);//определения количества возможных ячеек для выстрела компьютера когда естьв наличии только многоопалубные корабли
int sizeBoat(int *);//определения максимального размера живого корабля
bool checkPlaceBoat(char**, int, int[2][4], int); //проверка возможности установки корабля
bool checkCellBoat(char**, int, int, int);//проверка возможности установки корабля в конкретной ячейке
bool checkVerticalPlaceBoat(char**, int, int[2][4], int);//проверка возможности установки корабля при вертикальном расположении
bool checkHorizontalPlaceBoat(char**, int, int[2][4], int);//проверка возможности установки корабля при горизонтальном расположении
void manualPutBoat(char**, char**, int, int*);//ручная растановка кораблей
void inputPosition(int&, int&, int, bool&);//ввод даных координат поля от игрока
bool checkEndGame(int*);//проверка окончания игры
bool checkKillBoat(char**, int, int, int, int, int);//определение окончательного потопления корабля по последним координатам попадания
void placeCellKillBoat(char**, int, int, int, int, int, int &);//отрисовка на поле одной ячейки подбитого корабля
void placeKillBoat(char**, char**, int, int*, int, int);//отрисвка на полях в слечае потопления корабля игроком
void placeKillBoatEnemy(char**, int, int*, int, int);//отрисвка на поле в слечае потопления корабля противником
void playerMove(char**, char**, char**, int, int*, int*, char*);//ход игрока
void enemyMove(char**, int, int*, int[2][4], char*);//ход противника
void selectPosition(char**, int, int&, int&, int[2][4], int*);//выбор координат для выстрела компьютером
void inputBufBoat(int[2][4], int, int);//ввод в массив координаты подбитого корабля 
void copyStrEnemy(char*, int, int);//формирование строки с записью последних ходов противника
void exit(char**, char**, char**, int);//функция выхода из программы

#ifdef CONFIGURATION
void configuration(char **&, char **&, char **&, int &, int *);
void newFrame(char **&, int, int);
bool checkPossibleGame(int *, int);
void initialArrBoat(int*, int*);
int inputInt(char *);
void printProbed(int);
int myAbs(int);
int basicBoat[4];
#else
void initialArrBoat(int*);
#endif

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //для нескольких точек выхода
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(0));
	int _size = 13; //переменная для хранения размера поля (+3 за счет вспомагательных строчек на отрисовку)
	int woundedBoat[2][4]; //буфер для хранения координат недобитого корабля (для комьютера)
	bool end = 1; //маркер окончания игры
	char strEnemyMove[200]{ "\n" };//строка сообщения с ходом противника
	char **playerBoatList = new char*[_size]; //поле с кораблями игрока
	char **playerTargetList = new char*[_size];//поле игрока для стрельбы по противнику
	char **enemyBoatList = new char*[_size];//поле с кораблями противника
	for (int i = 0; i < _size; i++) { //инициализация базовых полей
		*(playerBoatList + i) = new char[_size * 2];
		*(playerTargetList + i) = new char[_size * 2];
		*(enemyBoatList + i) = new char[_size * 2];
	}
	int playerBoat[4], enemyBoat[4]; //массивы с количеством активных кораблей
	char menu; //буфер для ввода меню
	do { //начало программы (зациклено для возможности повторного начала)
		initialFrameList(playerBoatList, _size);
		initialFrameList(playerTargetList, _size);
		initialFrameList(enemyBoatList, _size);
		initialNullArrBoat(woundedBoat);
#ifdef CONFIGURATION
		configuration(playerBoatList, enemyBoatList, playerTargetList, _size, basicBoat); //запуск настройки игры для ввода размера поля и количества кораблей
		initialArrBoat(playerBoat, basicBoat); //инициализация массива с колмчеством кораблей игрока
		initialArrBoat(enemyBoat, basicBoat); //инициализация массива с колмчеством кораблей противника
#else
		initialArrBoat(playerBoat);//инициализация массива с колмчеством кораблей игрока
		initialArrBoat(enemyBoat);//инициализация массива с колмчеством кораблей противника
#endif
		do {
			printList(playerBoatList, playerTargetList, _size); //отрисовка полей
			cout << "\t\tДобро пожаловать в игру \"Морской бой\"" << endl;
			cout << "Выберите 1 - для автоматической расстановки кораблей\n\t 2 - для ручной расстановки кораблей\n\t 3 - для выхода" << endl;
			cin >> menu;
		} while ((int)menu < 49 || (int)menu > 51);
		switch ((int)menu) {
		case 49:
			autoPutBoat(playerBoatList, _size, playerBoat); //автоматическая растановка кораблей
			break;
		case 50:
			manualPutBoat(playerBoatList, playerTargetList, _size, playerBoat); //ручная растановка кораблей
			break;
		case 51:
			exit(playerBoatList, enemyBoatList, playerTargetList, _size); //выход (и высвобождение динамической памяти)
			break;
		}
		autoPutBoat(enemyBoatList, _size, enemyBoat);//автоматическое расположение кораблей противника
#ifdef CONFIGURATION
		initialArrBoat(playerBoat, basicBoat);//повторная инициализация, поскольку в процессе расположения кораблей все значения обнулились
		initialArrBoat(enemyBoat, basicBoat);//повторная инициализация, поскольку в процессе расположения кораблей все значения обнулились
#else
		initialArrBoat(playerBoat);//повторная инициализация, поскольку в процессе расположения кораблей все значения обнулились
		initialArrBoat(enemyBoat);//повторная инициализация, поскольку в процессе расположения кораблей все значения обнулились
#endif
		do {
			playerMove(playerBoatList, enemyBoatList, playerTargetList, _size, playerBoat, enemyBoat, strEnemyMove); //ход игрока
			if (checkEndGame(enemyBoat)) {//проверка на окончание игры
				enemyMove(playerBoatList, _size, playerBoat, woundedBoat, strEnemyMove); //ход противника
			}
		} while (checkEndGame(playerBoat) && checkEndGame(enemyBoat)); //играем до тех пор пока не закончились корабли
		do {
			printListWithBoatGame(playerBoatList, playerTargetList, _size, playerBoat, enemyBoat); //вывод финального экрана с результатами боя
			cout << strEnemyMove << endl; //вывод последнего хода противника
			if (checkEndGame(playerBoat) > checkEndGame(enemyBoat)) { //проверка, у кого остались живые корабли
				cout << "\n\t\tПобеда!!!" << endl;
			}
			else {
				cout << "\n\t\tПоражение!!!" << endl;
			}
			cout << "\nНачать заного? [да, нет]" << endl;
			cin >> menu; 
			if (menu == 'н') end = 0;
		} while ((int)menu != 'д' && (int)menu != 'н'); //вводить, пока не будет нужная команда 
	} while (end);
	exit(playerBoatList, enemyBoatList, playerTargetList, _size);//выход (и высвобождение динамической памяти)
}

void initialFrameList(char **list, int _size) {
	for (int i = 2; i < _size; i++) {
		*(*(list)+i * 2 - 1) = ' ';
		*(*(list)+i * 2) = (char)(i + 95);
		*(*(list + 1) + i * 2 - 1) = (char)150;
		*(*(list + 1) + i * 2) = (char)150;
		*(*(list + _size - 1) + i * 2 - 1) = (char)150;
		*(*(list + _size - 1) + i * 2) = (char)150;
		*(*(list + i) + 2) = (char)124;
		*(*(list + i) + _size * 2 - 2) = (char)124;
		*(*(list + i - 1) + 1) = (char)((i - 2) % 10 + 48);
		if (i < 11) *(*(list + i)) = ' ';
		else *(*(list + i)) = (char)((i - 1) / 10 + 48);
		*(*(list + i) + _size * 2 - 1) = '\0';
	}
	*(*(list)) = ' ';
	*(*(list)+1) = ' ';
	*(*(list)+2) = ' ';
	*(*(list)+_size * 2 - 2) = ' ';
	*(*(list)+_size * 2 - 1) = '\0';
	*(*(list + 1)) = ' ';
	*(*(list + 1) + 1) = ' ';
	*(*(list + _size - 1)) = ' ';
	*(*(list + _size - 1) + 1) = ' ';
	*(*(list + 1) + 2) = '+';
	*(*(list + 1) + _size * 2 - 2) = '+';
	*(*(list + _size - 1) + _size * 2 - 2) = '+';
	*(*(list + _size - 1) + 2) = '+';
	*(*(list + 1) + _size * 2 - 1) = '\0';
	for (int i = 2; i < _size - 1; i++) {
		for (int j = 3; j < _size * 2 - 2; j++) {
			*(*(list + i) + j) = ' ';
		}
	}
}

void autoPutBoat(char**list, int _size, int*boat) {
	int boatSize = 0;
	for (int i = 3; i >= 0; i--) {//определение размера еще не установленого корабля
		if (*(boat + i) > 0) {
			boatSize = i + 1;
			(*(boat + i))--;//сразу уменьшаем количество выбраного корабля
			break;
		}
	}
	if (boatSize == 0)return;
	int bufPosBoat[2][4];
	initialNullArrBoat(bufPosBoat);//очищаем координаты
	while (1) {//поиск возможного места под установку корабля (в связи с тем что поле 10х10, то вероятность нахождения пустого поля под установку корабля 100%)
		int position = rand() % (freeCells(list, _size) + 1);//случайный выбор ячейки под установку корабля
		for (int i = 2; i < _size - 1; i++) {
			for (int j = 4; j < _size * 2 - 3; j += 2) {
				if (*(*(list + i) + j) == ' ') {
					position--;
					if (position < 1) {
						bufPosBoat[0][0] = i;
						bufPosBoat[1][0] = j;
						if (checkPlaceBoat(list, _size, bufPosBoat, boatSize)) {//проверяем возможность установки корабля
							for (int i = 0; i < boatSize; i++) { //если можно, то отрисовуем в поле
								*(*(list + bufPosBoat[0][i]) + bufPosBoat[1][i]) = 'O';//метка не подбитого корабля 'O'
							}
							autoPutBoat(list, _size, boat);//запускаем повторно поиск для установки следующего корабля
							return;
						}
					}
				}
			}
		}
	}
}

void initialNullArrBoat(int boat[2][4]) {
	for (int i = 0; i < 4; i++) {//очищаем координаты ячеек корабля
		boat[0][i] = 0;
		boat[1][i] = 0;
	}
}

int freeCells(char **list, int _size) {
	int k = 0;
	for (int i = 2; i < _size - 1; i++) {
		for (int j = 4; j < _size * 2 - 3; j += 2) {
			if (*(*(list + i) + j) == ' ') k++;
		}
	}
	return k;
}

int freeCellsEnemy(char **list, int _size) {
	int k = 0;
	for (int i = 2; i < _size - 1; i++) {
		for (int j = 4; j < _size * 2 - 3; j += 2) {
			if (*(*(list + i) + j) == ' ' || *(*(list + i) + j) == 'O') k++;
		}
	}
	return k;
}

int freeAdvancedSearch(char **list, int _size, int *boat) { //поиск количества возможных расположений корабля максимального размера
	int k = 0, line;
	int sBoat = sizeBoat(boat);
	for (int i = 2; i < _size - 1; i++) { //поиск по строчкам
		line = 0;
		for (int j = 4; j < _size * 2 - 3; j += 2) {
			if (*(*(list + i) + j) == ' ' || *(*(list + i) + j) == 'O') line++;
			if (line == sBoat) {
				k++;
				line = 0;
			}
		}
	}
	for (int j = 4; j < _size * 2 - 3; j += 2) { //поиск по столбикам
		line = 0;
		for (int i = 2; i < _size - 1; i++) {
			if (*(*(list + i) + j) == ' ' || *(*(list + i) + j) == 'O') line++;
			if (line == sBoat) {
				k++;
				line = 0;
			}
		}
	}
	return k;
}

int sizeBoat(int *boat) {
	for (int i = 3; i >= 0; i--)
		if (*(boat + i) > 0) return i + 1;
	return 0;
}

bool checkPlaceBoat(char**list, int _size, int bufPosBoat[2][4], int boatSize) {
	if (!checkCellBoat(list, _size, bufPosBoat[0][0], bufPosBoat[1][0])) return 0;
	if (rand() % 2) { //случайный выбор расположение по вертикали или горизонтали
		if (checkVerticalPlaceBoat(list, _size, bufPosBoat, boatSize)) return 1;
		if (checkHorizontalPlaceBoat(list, _size, bufPosBoat, boatSize)) return 1;
	}
	else {
		if (checkHorizontalPlaceBoat(list, _size, bufPosBoat, boatSize)) return 1;
		if (checkVerticalPlaceBoat(list, _size, bufPosBoat, boatSize)) return 1;
	}
	return 0;
}

bool checkCellBoat(char **list, int _size, int posI, int posJ) { //проверка ячейки под корабль (смежные ячейки не должны быть заняты кораблями) 1 - свободно, 0 - занято
	if (*(*(list + posI) + posJ) != ' ' || posI < 2 || posI > _size - 2 || posJ < 4 || posJ > _size * 2 - 4) return 0;
	for (int i = 2; i < _size - 1; i++) {
		for (int j = 4; j < _size * 2 - 3; j += 2) {
			if (((abs(i - posI) == 1 && abs(j - posJ) == 2) || (i == posI && abs(j - posJ) == 2) || abs(i - posI) == 1 && j == posJ) && *(*(list + i) + j) != ' ') {
				return 0;
			}
		}
	}
	return 1;
}

bool checkVerticalPlaceBoat(char**list, int _size, int bufPosBoat[2][4], int boatSize) {
	int bufNumber = 0;
	int bufBoatSize = boatSize - 1;
	int checkPossiblePlaced = 0, up = 0, down = 0;
	while (bufBoatSize) { //расположение по вертикали
		if (bufPosBoat[0][0] - up - 1 > 1 && bufPosBoat[0][0] - up - 1 < _size - 1 && checkCellBoat(list, _size, bufPosBoat[0][0] - up - 1, bufPosBoat[1][0])) { //вверх
			bufPosBoat[0][++bufNumber] = bufPosBoat[0][0] - ++up;
			bufPosBoat[1][bufNumber] = bufPosBoat[1][0];
			bufBoatSize--;
		}
		else checkPossiblePlaced++;
		if (bufBoatSize && bufPosBoat[0][0] + down + 1 > 1 && bufPosBoat[0][0] + down + 1 < _size - 1 && checkCellBoat(list, _size, bufPosBoat[0][0] + down + 1, bufPosBoat[1][0])) { //вниз
			bufPosBoat[0][++bufNumber] = bufPosBoat[0][0] + ++down;
			bufPosBoat[1][bufNumber] = bufPosBoat[1][0];
			bufBoatSize--;
		}
		else {
			if (++checkPossiblePlaced == 2) {//проверка возможности дальнейшего расположения корабля
				initialNullArrBoat(bufPosBoat);//расположить нет возможности, очищаем координаты ячеек корабля
				break;
			}
			else checkPossiblePlaced = 0;
		}
	}
	if (!bufBoatSize) return 1;	
	return 0;
}

bool checkHorizontalPlaceBoat(char**list, int _size, int bufPosBoat[2][4], int boatSize) {
	int bufNumber = 0;
	int bufBoatSize = boatSize - 1;
	int checkPossiblePlaced = 0, left = 0, right = 0;
	while (bufBoatSize) { //расположение по горизонтали
		if (bufPosBoat[1][0] - 2 * (left + 1) > 3 && bufPosBoat[1][0] - 2 * (left + 1) < _size * 2 - 3 && checkCellBoat(list, _size, bufPosBoat[0][0], bufPosBoat[1][0] - 2 * (left + 1))) { //ввлево
			bufPosBoat[0][++bufNumber] = bufPosBoat[0][0];
			bufPosBoat[1][bufNumber] = bufPosBoat[1][0] - 2 * ++left;
			bufBoatSize--;
		}
		else checkPossiblePlaced++;
		if (bufBoatSize && bufPosBoat[1][0] + 2 * (right + 1) > 3 && bufPosBoat[1][0] + 2 * (right + 1) < _size * 2 - 3 && checkCellBoat(list, _size, bufPosBoat[0][0], bufPosBoat[1][0] + 2 * (right + 1))) { //вправо
			bufPosBoat[0][++bufNumber] = bufPosBoat[0][0];
			bufPosBoat[1][bufNumber] = bufPosBoat[1][0] + 2 * ++right;
			bufBoatSize--;
		}
		else {
			if (++checkPossiblePlaced == 2) {//проверка возможности дальнейшего расположения корабля
				initialNullArrBoat(bufPosBoat);//расположить нет возможности, очищаем координаты ячеек корабля
				break;
			}
			else checkPossiblePlaced = 0;
		}
	}
	if (!bufBoatSize) return 1;
	return 0;
}

void manualPutBoat(char**list, char **target, int _size, int*boat) {
	char menu;
	int sizeBoat = 0, posI = 0, posJ = 0;
	int numberOfBoats = 0;
	bool mistake = 0, mistakePos = 0;
	for (int i = 3; i >= 0; i--) {
		numberOfBoats += *(boat + i);
	}
	do {
		do {
			printListWithBoat(list, target, _size, boat);
			if (mistakePos) {
				cout << "Неправильное местоположение корабля. Повторите ввод." << endl;
				mistakePos = 0;
			}
			if (mistake) {
				cout << "Нет свободных кораблей. Повторите ввод." << endl;
				mistake = 0;
			}
			cout << "Выберите тип корабля 1 - OOOO\n\t\t     2 - OOO\n\t\t     3 - OO\n\t\t     4 - O" << endl;
			cin >> menu;
			sizeBoat = 53 - (int)menu;//запись в переменную размер выбраного коробля
			if (*(boat + sizeBoat - 1) < 1) {//проверка на наличие нераставленных кораблей
				menu = '0';
				mistake = 1;
			}
		} while ((int)menu < 49 || (int)menu > 52);
		sizeBoat = 53 - (int)menu;
		do {
			printListWithBoat(list, target, _size, boat);
			if (mistake) {
				cout << "Неверный ввод даных" << endl;
				mistake = 0;
			}
			cout << "Введите координаты позиции одной из сторон корабля с разделителем запятая\nна первом месте строчка, на втором столбец (пример: 4,с)" << endl;
			inputPosition(posI, posJ, _size, mistake);
		} while (posI == 0 || posJ == 0);
		if (sizeBoat == 1) {
			if (!checkCellBoat(list, _size, posI, posJ)) {
				mistakePos = 1;
			}
			else {
				*(*(list + posI) + posJ) = 'O';
			}
		}
		else {
			do {
				printListWithBoat(list, target, _size, boat);
				cout << "Введите направление размещения корабля:\n1 - вверх\n2 - вправо\n3 - вниз\n4 - влево" << endl;
				cin >> menu;
			} while ((int)menu < 49 || (int)menu > 52);
			switch ((int)menu - 48)
			{
			case 1:
				for (int i = 0; i < sizeBoat; i++) {
					if (!checkCellBoat(list, _size, posI - i, posJ)) {
						mistakePos = 1;
						break;
					}
				}
				if (!mistakePos) {
					for (int i = 0; i < sizeBoat; i++) {
						*(*(list + posI - i) + posJ) = 'O';
					}
				}
				break;
			case 2:
				for (int j = 0; j < sizeBoat * 2; j += 2) {
					if (!checkCellBoat(list, _size, posI, posJ + j)) {
						mistakePos = 1;
						break;
					}
				}
				if (!mistakePos) {
					for (int j = 0; j < sizeBoat * 2; j += 2) {
						*(*(list + posI) + posJ + j) = 'O';
					}
				}
				break;
			case 3:
				for (int i = 0; i < sizeBoat; i++) {
					if (!checkCellBoat(list, _size, posI + i, posJ)) {
						mistakePos = 1;
						break;
					}
				}
				if (!mistakePos) {
					for (int i = 0; i < sizeBoat; i++) {
						*(*(list + posI + i) + posJ) = 'O';
					}
				}
				break;
			case 4:
				for (int j = 0; j < sizeBoat * 2; j += 2) {
					if (!checkCellBoat(list, _size, posI, posJ - j)) {
						mistakePos = 1;
						break;
					}
				}
				if (!mistakePos) {
					for (int j = 0; j < sizeBoat * 2; j += 2) {
						*(*(list + posI) + posJ - j) = 'O';
					}
				}
				break;
			}
		}
		if (!mistakePos) {
			numberOfBoats--;
			(*(boat + sizeBoat - 1))--;
		}
	} while (numberOfBoats);
}

void inputPosition(int &posI, int &posJ, int _size, bool &mistake) {
	posI = 0;
	posJ = 0;
	mistake = 0;
	char str[10];
	cout << "Позиция: ";
	cin >> str;
	char *separator = strchr(str, ',');//находим разделитель ","
	if (separator == nullptr) {
		cout << "Неверный ввод даных" << endl;
		posI = 0;
		posJ = 0;
		mistake = 1;
		return;
	}
	separator--;
	int k = 0;
	bool error = 0;
	for (; separator >= str; separator--) {//определяем номер строки, слева от разделителя (число)
		if ((int)*separator > 47 && (int)*separator < 58) {
			posI += (*separator - 48)*pow(10, k++);
		}
		else {
			error = 1;
			break;
		}
	}
	separator = strchr(str, ',') + 1;
	k = 0;
	for (; *separator != '\0'; separator++) {//определяем номер столбца, справа от разделителя (буква)
		if ((int)*separator > 96 && (int)*separator < 123) {
			posJ += (*separator - 96)*pow(26, k++);
		}
		else {
			error = 1;
			break;
		}
	}
	if (error || posI > _size - 3 || posJ > _size - 3 || posI < 1 || posJ < 1) {
		cout << "Неверный ввод даных" << endl;
		posI = 0;
		posJ = 0;
		mistake = 1;
		return;
	}
	posI++;
	posJ = 2 + 2 * posJ;
}

bool checkEndGame(int* boat) {//проверка окончания игры: 0 - конец, 1 - продолжаем
	for (int i = 0; i < 4; i++)
		if (*(boat + i) > 0)
			return 1;
	return 0;
}

bool checkKillBoat(char **list, int _size, int posI, int posJ, int previousI, int previousJ) {//проверка окончательного потопления корабля 1 - убит, 0 - ранен
	bool mark = 1;
	/*for (int i = 2; i < _size - 1; i++) {
		for (int j = 4; j < _size * 2 - 3; j += 2) {
			if ((i == posI && abs(j - posJ) == 2) || (abs(i - posI) == 1 && j == posJ)) {
				if (*(*(list + i) + j) == 'O')
					return 0;
				else if (*(*(list + i) + j) == 'X' && (i !=previousI || j != previousJ))
					mark &= checkKillBoat(list, _size, i, j, posI, posJ);
			}
		}
	}*/
	int bufI = -1, bufJ = 0;//переменные для расчета смещения от базовой ячейки (по вертикали и горизонтали)
	for (int i = 0; i < 4; i++) {
		if (*(*(list + posI + bufI) + posJ + 2 * bufJ) == 'O') //проверка смежных ячеек по вертикали и горизонтали
			return 0;
		else if (*(*(list + posI + bufI) + posJ + 2 * bufJ) == 'X' && (posI + bufI != previousI || posJ + 2 * bufJ != previousJ))
			mark &= checkKillBoat(list, _size, posI + bufI, posJ + 2 * bufJ, posI, posJ);
		bufI += pow(-1, i / 2); //модификация координаты смещения от базовой по вертикали 
		bufJ += pow(-1, (i + 1) / 2);//модификация координаты смещения от базовой по горизонтали 
	}
	return mark;
}

void placeCellKillBoat(char**list, int _size, int posI, int posJ, int previousI, int previousJ, int &boatSize) {
	/*for (int i = 2; i < _size - 1; i++) {
		for (int j = 4; j < _size * 2 - 3; j += 2) {
			if (((abs(i - posI) == 1 && abs(j - posJ) == 2) || (i == posI && abs(j - posJ) == 2) || (abs(i - posI) == 1 && j == posJ)) && *(*(list + i) + j) == ' ') {
				*(*(list + i) + j) = (char)149; //метка промахнувшегося выстрела (точка)
			}
			if (((i == posI && abs(j - posJ) == 2) || (abs(i - posI) == 1 && j == posJ)) && *(*(list + i) + j) == 'X' && (i != previousI || j != previousJ)) {
				placeCellKillBoat(list, _size, i, j, posI, posJ, ++boatSize);
			}
		}
	}*/
	int bufI = -1, bufJ = 0;//переменные для расчета смещения от базовой ячейки (по вертикали и горизонтали)
	int bufI_diag = 0, bufJ_diag = 0;//переменные смещения от базовой ячейки (по диагоналям)
	for (int i = 0; i < 4; i++) {
		bufI_diag = pow(-1, i / 2);
		bufJ_diag = 2 * pow(-1, (i + 1) / 2);
		if (*(*(list + posI + bufI_diag) + posJ + bufJ_diag) == ' ') //проверка смежных ячеек по диагоналям
			*(*(list + posI + bufI_diag) + posJ + bufJ_diag) = (char)149; //(char)149 - метка промахнувшегося выстрела (точка)
		if (*(*(list + posI + bufI) + posJ + 2 * bufJ) == ' ') //проверка смежных ячеек по вертикали и горизонтали
			*(*(list + posI + bufI) + posJ + 2 * bufJ) = (char)149; //метка промахнувшегося выстрела (точка)
		if (*(*(list + posI + bufI) + posJ + 2 * bufJ) == 'X' && (posI + bufI != previousI || posJ + 2 * bufJ != previousJ))
			placeCellKillBoat(list, _size, posI + bufI, posJ + 2 * bufJ, posI, posJ, ++boatSize);
		bufI += pow(-1, i / 2); //модификация координаты смещения от базовой по вертикали 
		bufJ += pow(-1, (i + 1) / 2);//модификация координаты смещения от базовой по горизонтали 
	}
}

void placeKillBoat(char** list, char** target, int _size, int* boat, int posI, int posJ) {
	int boatSize = 1;
	placeCellKillBoat(list, _size, posI, posJ, 0, 0, boatSize);
	(*(boat + boatSize - 1))--;
	placeCellKillBoat(target, _size, posI, posJ, 0, 0, boatSize);
}

void placeKillBoatEnemy(char** list, int _size, int* boat, int posI, int posJ) {
	int boatSize = 1;
	placeCellKillBoat(list, _size, posI, posJ, 0, 0, boatSize);
	(*(boat + boatSize - 1))--;
}

void playerMove(char** playerBoatList, char** enemyBoatList, char** playerTargetList, int _size, int* playerBoat, int* enemyBoat, char* strEnemyMove) {
	bool mistake = 0;
	int posI = 0, posJ = 0;
	do {
		printListWithBoatGame(playerBoatList, enemyBoatList, _size, playerBoat, enemyBoat);
		cout << strEnemyMove << endl;
		strEnemyMove[0] = '\n';//очищаем строку с ходами противника
		strEnemyMove[1] = '\0';
		if (mistake) {
			cout << "Неверный ввод даных" << endl;
			mistake = 0;
		}
		cout << "Введите координаты выстрела с разделителем запятая\nна первом месте строчка, на втором столбец (пример: 4,с)" << endl;
		inputPosition(posI, posJ, _size, mistake);
	} while (posI == 0 || posJ == 0);
	if (*(*(enemyBoatList + posI) + posJ) == 'O') {
		*(*(enemyBoatList + posI) + posJ) = 'X';//'X'метка ячейки подбитого корабля
		*(*(playerTargetList + posI) + posJ) = 'X';
		if (checkKillBoat(enemyBoatList, _size, posI, posJ, 0 , 0)) {
			placeKillBoat(enemyBoatList, playerTargetList, _size, enemyBoat, posI, posJ);
		}
		if (checkEndGame(enemyBoat)) {
			playerMove(playerBoatList, enemyBoatList, playerTargetList, _size, playerBoat, enemyBoat, strEnemyMove);
		}
	}
	else if (*(*(enemyBoatList + posI) + posJ) == ' ') {
		*(*(enemyBoatList + posI) + posJ) = (char)149;
		*(*(playerTargetList + posI) + posJ) = (char)149;
	}
}

void enemyMove(char** playerBoatList, int _size, int* playerBoat, int woundedBoat[2][4], char * strEnemyMove) {
	int posI = 0, posJ = 0;
	selectPosition(playerBoatList, _size, posI, posJ, woundedBoat, playerBoat);
	copyStrEnemy(strEnemyMove, posI, posJ);
	cout << "Координаты врага " << posI - 1 << " " << (char)((posJ - 2)/2+96) << endl;
	cout << woundedBoat[0][0] << " " << woundedBoat[1][0] << " " << woundedBoat[0][1] << " " << woundedBoat[1][1] << endl;
	if (*(*(playerBoatList + posI) + posJ) == 'O') {
		*(*(playerBoatList + posI) + posJ) = 'X';//'X'метка ячейки подбитого корабля
		inputBufBoat(woundedBoat, posI, posJ); //запись координат подбитого корабля
		if (checkKillBoat(playerBoatList, _size, posI, posJ, 0, 0)) {//проверка потопления корабля
			placeKillBoatEnemy(playerBoatList, _size, playerBoat, posI, posJ); //прорисовуем ячейки с кораблем и зоной вокруг него
			initialNullArrBoat(woundedBoat);//очищаем буфер с координатами подбитого корабля
		}
		if (checkEndGame(playerBoat)) {
			enemyMove(playerBoatList, _size, playerBoat, woundedBoat, strEnemyMove);
		}
	}
	else if (*(*(playerBoatList + posI) + posJ) == ' ') {
		*(*(playerBoatList + posI) + posJ) = (char)149;
	}
}

void selectPosition(char** playerBoatlist, int _size, int &posI, int &posJ, int woundedBoat[2][4], int *boat) {
	if (woundedBoat[0][0]) { //проверка есть ли уже подбитый корабль, для поиска ячейки на его добивание
		if (woundedBoat[0][1]) { //проверка есть ли уже подбитая вторая ячейка корабля, если есть то можно сузить место поиска следующей
			int displacement = 0, k = 1; //переменные размера и направления смещения от базовой ячейки
			if (woundedBoat[0][0] == woundedBoat[0][1]) {
				do {	//поиск координаты при расположеном корабле горизонтально
					displacement++;
					if (*(*(playerBoatlist + woundedBoat[0][0]) + woundedBoat[1][0] + 2 * displacement * k) == 'O' || *(*(playerBoatlist + woundedBoat[0][0]) + woundedBoat[1][0] + 2 * displacement * k) == ' ') {
						break;//в случае нахождения возможного поля заканчиваем поиск координат ячейки
					}
					else if (*(*(playerBoatlist + woundedBoat[0][0]) + woundedBoat[1][0] + 2 * displacement * k) == (char)149 || *(*(playerBoatlist + woundedBoat[0][0]) + woundedBoat[1][0] + 2 * displacement * k) == (char)124) {
						k = -1; //если уперлись в ячейку с промахом или границу поля, то меняем направление
						displacement = 0; //и начинаем сначало
					}
				} while (1);
				posI = woundedBoat[0][0];
				posJ = woundedBoat[1][0] + 2 * displacement * k;
			}
			else {
				do {   //поиск координаты при расположеном корабле вертикально
					displacement++;
					if (*(*(playerBoatlist + woundedBoat[0][0] + displacement * k) + woundedBoat[1][0]) == 'O' || *(*(playerBoatlist + woundedBoat[0][0] + displacement * k) + woundedBoat[1][0]) == ' ') {
						break; //в случае нахождения возможного поля заканчиваем поиск координат ячейки
					}
					else if (*(*(playerBoatlist + woundedBoat[0][0] + displacement * k) + woundedBoat[1][0]) == (char)149 || *(*(playerBoatlist + woundedBoat[0][0] + displacement * k) + woundedBoat[1][0]) == (char)150) {
						k = -1; //если уперлись в ячейку с промахом или границу поля, то меняем направление
						displacement = 0; //и начинаем сначало
					}
				} while (1);
				posI = woundedBoat[0][0] + displacement * k;
				posJ = woundedBoat[1][0];
			}
		}
		else {//если в подбитом корабле найдена только 1 ячейка
			int bufI, bufJ;//переменные для расчета смещения от ячейки с попаданием по кораблю
			int buf = rand() % 4; // переменная для определения случайного старта перебора возможных позиций
			switch (buf)
			{
			case 0://начинаем сверху от ячейки
				bufI = -1;
				bufJ = 0;
				break;
			case 1://начинаем справа от ячейки
				bufI = 0;
				bufJ = 1;
				break;
			case 2://начинаем снизу от ячейки
				bufI = 1;
				bufJ = 0;
				break;
			case 3://начинаем слева от ячейки
				bufI = 0;
				bufJ = -1;
				break;
			}
			for (int i = buf; i < 4 + buf; i++) {
				if (*(*(playerBoatlist + woundedBoat[0][0] + bufI) + woundedBoat[1][0] + 2 * bufJ) == 'O' || *(*(playerBoatlist + woundedBoat[0][0] + bufI) + woundedBoat[1][0] + 2 * bufJ) == ' ') {
					break; // перебор до тех пор пока не найдем ячейку с нераненым кораблем или промах
				}
				bufI += pow(-1, i / 2); //модификация координаты смещения от базовой по вертикали 
				bufJ += pow(-1, (i + 1) / 2);//модификация координаты смещения от базовой по горизонтали 
			}
			posI = woundedBoat[0][0] + bufI;
			posJ = woundedBoat[1][0] + 2 * bufJ;
		}
	}
	else if (sizeBoat(boat) > 1) { //перебор кораблей размера "2" - "4"
		int position = rand() % (freeAdvancedSearch(playerBoatlist, _size, boat) + 1);//случайное определения номера ячейки
		int line;
		int sBoat = sizeBoat(boat);
		for (int i = 2; i < _size - 1; i++) { //поиск по строчкам
			line = 0;
			for (int j = 4; j < _size * 2 - 3; j += 2) {
				if (*(*(playerBoatlist + i) + j) == ' ' || *(*(playerBoatlist + i) + j) == 'O') line++;
				if (line == sBoat) {
					position--;
					if (position < 1) { //перебор всех возможных ячеек пока не закончится счетчик
						posI = i;
						posJ = j;
						return;
					}
					line = 0;
				}
			}
		}
		for (int j = 4; j < _size * 2 - 3; j += 2) { //поиск по столбикам
			line = 0;
			for (int i = 2; i < _size - 1; i++) {
				if (*(*(playerBoatlist + i) + j) == ' ' || *(*(playerBoatlist + i) + j) == 'O') line++;
				if (line == sBoat) {
					position--;
					if (position < 1) { //перебор всех возможных ячеек пока не закончится счетчик
						posI = i;
						posJ = j;
						return;
					}
					line = 0;
				}
			}
		}
	}
	else { //если остались только корабли размера "1"
		int position = rand() % (freeCellsEnemy(playerBoatlist, _size) + 1);//случайное определения номера ячейки
		for (int i = 2; i < _size - 1; i++) {
			for (int j = 4; j < _size * 2 - 3; j += 2) {
				if (*(*(playerBoatlist + i) + j) == ' ' || *(*(playerBoatlist + i) + j) == 'O') {
					position--;
					if (position < 1) { //перебор всех возможных ячеек пока не закончится счетчик
						posI = i;
						posJ = j;
						return;
					}
				}
			}
		}
	}
}

void inputBufBoat(int boat[2][4], int posI, int posJ) {
	for (int i = 0; i < 4; i++) {
		if (boat[0][i] == 0) {
			boat[0][i] = posI;
			boat[1][i] = posJ;
			return;
		}
	}
}

void copyStrEnemy(char*str, int posI, int posJ) {
	char buf[5];
	_itoa_s(posI - 1, buf, 10);
	strcat_s(str, 200, "Ход противника: ");
	strcat_s(str, 200, buf);
	strcat_s(str, 200, ",");
	int digit = 0;
	posJ = (posJ - 2) / 2;
	int buff = posJ;
	do {
		buff /= 26;
		digit++;
	} while (buff);
	buf[digit] = '\n';
	buf[digit + 1] = '\0';
	for (int i = digit - 1; i >= 0; i--) {
		buff = posJ % 26;
		buf[i] = (char)(buff + 96);
		posJ /= 26;
	}
	strcat_s(str, 200, buf);
}

void printListWithBoat(char**list, char**target, int _size, int*boat) {
	printList(list, target, _size);
	cout << "Корабли на установку:" << endl;
	cout << "\tOOOO - " << *(boat + 3) << "  OOO - " << *(boat + 2) << endl;
	cout << "\tOO   - " << *(boat + 1) << "  O   - " << *boat << endl;
}

void exit(char **playerBoatList, char **enemyBoatList, char **playerTargetList, int _size) {
	for (int i = 0; i < _size; i++) {
		delete[] * (playerBoatList + i);
		delete[] * (playerTargetList + i);
		delete[] * (enemyBoatList + i);
	}
	delete[] playerBoatList;
	delete[] playerTargetList;
	delete[] enemyBoatList;
	exit(0);
}

#ifdef CONFIGURATION

void configuration(char **&playerBoatList, char **&enemyBoatList, char **&playerTargetList, int &_size, int * boat) {
	char str[20]{ "\0" };
	int buf = 0;
	bool nonZone = 0;
	do {
		do {
			printList(playerBoatList, playerTargetList, _size);
			if (nonZone)
				cout << "\nВ поле заданого размера не поместится выбраное количество кораблей.\nПовторите ввод.\n" << endl;
			cout << "\t\tКонфигурация игры \"Морской бой\"" << endl;
			cout << "Введите размер поля [5..26]" << endl;
			cin >> str;
			buf = inputInt(str);
		} while (buf < 5 || buf > 26);
		buf += 3;
		newFrame(playerBoatList, _size, buf);
		newFrame(enemyBoatList, _size, buf);
		newFrame(playerTargetList, _size, buf);
		_size = buf;
		for (int i = 3; i >= 0; i--) {
			do {
				buf = -1;
				printList(playerBoatList, playerTargetList, _size);
				cout << "Введите количество " << i + 1 << " - палубных кораблей [0..99]" << endl;
				cin >> str;
				if (*str > 47 && *str < 58) {
					buf = inputInt(str);
				}
			} while (buf < 0 || buf > 99);
			*(boat + i) = buf;
		}
		nonZone = !checkPossibleGame(boat, _size);//проверка на возможность игры с даными параметрами (иначе повторный ввод)
	} while (nonZone);
}

void newFrame(char **&list, int _size, int newSize) {
	for (int i = 0; i < _size; i++) {
		delete[] * (list + i);
	}
	delete[] list;
	list = new char*[newSize];
	for (int i = 0; i < newSize; i++) {
		*(list + i) = new char[newSize * 2];
	}
	initialFrameList(list, newSize);
}

bool checkPossibleGame(int * boat, int _size) {
	int occupiedPlace = 0;
	for (int i = 0; i < 4; i++) {
		occupiedPlace += (6 + i * 2) * (*(boat + i));
	}
	if (occupiedPlace < (_size - 3) * (_size - 3)) return 1;
	return 0;
}

void initialArrBoat(int * boat, int * basicBoat) {
	for (int i = 0; i < 4; i++)
		*(boat + i) = *(basicBoat + i);
}

int inputInt(char *str) {
	int digit = 0, number = 0;
	for (int i = 0; *(str + i) != '\0'; i++) {
		if ((int)*(str + i) > 47 && (int)*(str + i) < 58)
			digit++;
		else break;
	}
	for (int i = 0; i < digit; i++)
		number += ((int)*(str + i) - 48) * pow(10, digit - i - 1);
	return number;
}

void printList(char **list, char ** target, int _size) {
	system("cls");
	printProbed(_size - 4);
	cout << "Ваше поле";
	printProbed(2 * _size - 9 - myAbs(_size - 4) + myAbs(31 - 2 * _size) + myAbs(_size - 8));
	cout << "Поле противника" << endl;
	for (int i = 0; i < _size; i++) {
		cout << *(list + i);
		printProbed(31 - 2 * _size);
		cout << *(target + i) << endl;
	}
}

void printListWithBoatGame(char**list, char**target, int _size, int*boatPlayer, int* boatEnemy) {
	printList(list, target, _size);
	cout << "   Ваши корабли";
	printProbed(2 * _size - 14 + myAbs(31 - 2 * _size));
	cout << " Корабли противника" << endl;
	cout << "   OOOO - " << *(boatPlayer + 3) << "  OOO - " << *(boatPlayer + 2);
	printProbed(2 * _size - 18 + myAbs(31 - 2 * _size));
	cout << "OOOO - " << *(boatEnemy + 3) << "  OOO - " << *(boatEnemy + 2) << endl;
	cout << "   OO   - " << *(boatPlayer + 1) << "  O   - " << *boatPlayer;
	printProbed(2 * _size - 18 + myAbs(31 - 2 * _size));
	cout << "OO   - " << *(boatEnemy + 1) << "  O   - " << *boatEnemy << endl;
}

void printProbed(int _size) {
	for (int i = 0; i < _size; i++) cout << " ";
}

int myAbs(int number) {
	return number < 0 ? 0 : number;
}

#else

void initialArrBoat(int *boat) {
	for (int i = 0; i < 4; i++)
		*(boat + i) = 4 - i;
}

void printList(char **list, char ** target, int _size) {
	system("cls");
	cout << "\tВаше поле\t\t    Поле противника" << endl;
	for (int i = 0; i < _size; i++) {
		cout << *(list + i) << "     " << *(target + i) << endl;
	}
}

void printListWithBoatGame(char**list, char**target, int _size, int*boatPlayer, int* boatEnemy) {
	printList(list, target, _size);
	cout << "   Ваши корабли\t\t\t Корабли противника" << endl;
	cout << "   OOOO - " << *(boatPlayer + 3) << "  OOO - " << *(boatPlayer + 2);
	cout << "             OOOO - " << *(boatEnemy + 3) << "  OOO - " << *(boatEnemy + 2) << endl;
	cout << "   OO   - " << *(boatPlayer + 1) << "  O   - " << *boatPlayer;
	cout << "             OO   - " << *(boatEnemy + 1) << "  O   - " << *boatEnemy << endl;
}

#endif