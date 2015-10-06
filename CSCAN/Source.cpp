#include <windows.h>  // базовые элементы WinAPI
#include <time.h>
#include <stdio.h>

#define N 50 // кол-во дорожек
HANDLE Semaphore;
HANDLE SemaphoreQueue;
HANDLE SemaphoreKiller;
HANDLE SemaphoreChoose;
HANDLE GenerateReq;
int currentPosition = 0;
int nq = 1;

struct Node
{
	int nt; // номер дорожки
	Node *next;
};

class List
{
private:
	Node *head;
	Node *tail;
	int count;
public:
	Node *GetHead() { return this->head; }
	Node *GetTail() { return this->tail; }
	int GetCount() { return this->count; }

	void SetHead(Node *head) { this->head = head; }
	void SetTail(Node *tail) { this->tail = tail; }
	void SetCount(int count) { this->count = count; }

	void insertToHead(int nt)
	{
		Node *q = new Node;
		q->next = head;
		head = q;
		q->nt = nt;
	}

	void insertToList(int nt)
	{
		Node *q = new Node;
		q->next = NULL;
		q->nt = nt;
		if(this->head == NULL)
		{
			this->head = q;
			this->tail = q;
		}
		else
		{
			this->tail->next = q;
			this->tail = q;
		}
		this->count++;
	}

	void DeleteFromList(Node *q)
	{
		Node *n=0;
		Node *prev=0;
		if(q == this->head)
		{
			this->head = q->next;
		}
		else if(q == this->tail)
		{
			prev=this->head;
			while(prev->next != this->tail)
			{
				prev=prev->next;
			}
			prev->next=NULL;
			this->tail = prev;
		}
		else
		{
			n = this->head;
			while(n != q)
			{
				prev=n;
				n=n->next;
			}
			prev->next = n->next;
		}
		this->count--;
	}

	int GetFromList(Node *qq) 
	{
		int numberTrack;
		while(qq->nt < currentPosition)
		{
			qq = qq->next;
		}
		numberTrack = qq->nt;
		currentPosition = numberTrack;
		this->DeleteFromList(qq);
		return numberTrack;
	}
	void SortListMin()
	{
		Node *q = this->GetHead();
		Node *n = this->GetHead();
		int tmp;
		while(n != NULL)
		{
			while(q != NULL)
			{
				if(n->nt < q->nt)
				{
					tmp = n->nt;
					n->nt = q->nt;
					q->nt = tmp;
				}
				q=q->next;
			}
			n = n->next;
			q = this->GetHead();
		}
	}
};

List *listFirstQueue;
List *listSecondQueue;
List *lst;
int numberOfQueue = 0;   //1 - первая очередь, 2 - вторая очередь

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
	return LineTo(hdc, x2, y2);
}

BOOL TextOutMod(HDC hdc, int x1, int y1, int x2, int y2, char *str, int c)
{
	MoveToEx(hdc, x1, y1, NULL);
	return TextOut(hdc, x2, y2, str, c);
}

void GenerateRequest()
{
	while(true)
	{
		WaitForSingleObject(SemaphoreKiller, INFINITE);
		int time = 0, nt = 0;
		time = (rand()%2)*300;
		Sleep(time);
		WaitForSingleObject(Semaphore, INFINITE);
		nt = rand()%N;
		lst->insertToList(nt);
		ReleaseSemaphore(SemaphoreKiller, 1, NULL);
		ReleaseSemaphore(Semaphore, 1, NULL);
	}
}

int ChooseRequest(Node *q)
{
	WaitForSingleObject(Semaphore, INFINITE);
	int nt;
	nt = lst->GetFromList(q);
	ReleaseSemaphore(Semaphore, 1, NULL);
	return nt;
}

// Дескриптор главного окна
HWND MainWindowHandle = 0;
HWND buttonNext;
HWND labelQueue;
int Koord[51];
bool wmPaint = false;

// Функция для создания и отображения окна 
bool InitWindowsApp(HINSTANCE instanceHandle, int show);
// Функция обработки сообщений приложения
int Run();

// Функция обработки сообщений главного окна
LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

// Входная функция - эквивалент main() в Windows
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, PSTR pCmdLine,int nShowCmd)
{
    // Создание и отображение окна
    // В случае неудачи выводим сообщение об ошибке и выходим из программы
    if(!InitWindowsApp(hInstance, nShowCmd)) 
    {
		MessageBox(0, "Init - FailedL", "ErrorL", MB_OK);
        return 0;
    }

    // Вызываем функцию обработки сообщений
    return Run();
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
    // Создание класса окна
    WNDCLASS wc;
	HDC hDC;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instanceHandle;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+0);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "FirstClass";

    // Регистрация класса окна
    // В случае неудачи выводим сообщение об ошибке и возвращаем false
    if(!RegisterClass(&wc)) 
    {
        MessageBox(0, "RegisterClass - FailedL", 0, 0);
        return false;
    }

    // Создаем окно

	hDC=GetDC(0);
 
	int x, y;
	int width = 1200, height = 500;
 
	x=GetDeviceCaps(hDC,HORZRES);
	y=GetDeviceCaps(hDC,VERTRES);
	ReleaseDC(0,hDC);
 
	int Left = (x - width) / 2;
	int Top = (y - height) / 2;

    MainWindowHandle = CreateWindow("FirstClass", "Магнитные диски",
    WS_OVERLAPPEDWINDOW, Left, Top, width, height, 0, 0, instanceHandle, 0);

    // Если окно не было создано, выводим сообщени об ошибке и возвращаем false
    if(MainWindowHandle == 0)
    {
        MessageBox(0, "CreateWindow - FailedL", 0, 0);
        return false;
    }

	//buttonNext = CreateWindow("button", "Next Step", WS_VISIBLE | WS_CHILD | WS_BORDER, 1050,420,120,30, MainWindowHandle,(HMENU)101, NULL, NULL);
	//labelQueue = CreateWindow("static", "Очередь запросов", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | BS_CENTER, 10,420,1010,30, MainWindowHandle,(HMENU)201, NULL, NULL);

    // Отображаем созданное окно
    ShowWindow(MainWindowHandle, show);
    UpdateWindow(MainWindowHandle);

    // Возвращаем true
    return true;
}

int Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // Обработка входящих сообщений в цикле
    // GetMessage будет возвращать истину до тех пор, пока не будет получено сообщение WM_QUIT
    while(GetMessage(&msg, 0, 0, 0))
    {
        // Транслируем сообщение и посылаем его функции обработке сообщений окна
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg,WPARAM wParam, LPARAM lParam)
{
    // Обработка сообщений

	Node *q;
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hPen;
	HFONT hFont;
	int x1, x2, y1, y2;
	int nc = 0;
	char str[10];
	char mainMessageStr[128];
	char labelStr[1024] = {0};
	char *st1 = "Очередь: ";
    char *st2 = "Дорожка: ";

    switch(msg)
    {

		case WM_CREATE:
			{
				srand(clock());
				Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
				SemaphoreChoose = CreateSemaphore(NULL, 1, 1, NULL);
				SemaphoreQueue = CreateSemaphore(NULL, 1, 1, NULL);
				SemaphoreKiller = CreateSemaphore(NULL, 1, 1, NULL);

				listFirstQueue = new List;
				listFirstQueue->SetHead(NULL);
				listFirstQueue->SetTail(NULL);
				listFirstQueue->SetCount(0);

				listSecondQueue = new List;
				listSecondQueue->SetHead(NULL);
				listSecondQueue->SetTail(NULL);
				listSecondQueue->SetCount(0);
				lst = listFirstQueue;

				GenerateReq = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GenerateRequest, NULL, 0, NULL);
			}
		break;

		case WM_COMMAND:
			if(wParam == 101)
			{
				
			}

		break;

		case WM_PAINT:
			{
				hdc=BeginPaint(MainWindowHandle, &ps);

				hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
				SelectObject(hdc, hPen);
				Line(hdc, 10, 20, 1159, 20);

				hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
				SelectObject(hdc, hPen);

				x1 = 10; y1 = 9; x2 = 10; y2 = 30;
				for(int i=0; i<=N; i++)
				{
					Line(hdc, x1, y1, x2, y2);
					x1+=23;
					x2+=23;
				}

				hFont = CreateFont(12, 7, 0, 0, FW_BOLD, 1, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, NULL);

				SelectObject(hdc, hFont);
				x1 = 6; y1 = 20; x2 = 6; y2 = 35;
				int c=1;
				int x0=19;
				for(int i=0; i<=N; i++)
				{
					if(i==10)
					{
						c++;
						x0=24;
					}
					itoa(i, str, 10);
					TextOutMod(hdc, x1, y1, x2, y2, str, c);
					x1+=x0;
					x2+=x0;
					Koord[i] = x1;
				}

				SendMessage(labelQueue, WM_SETTEXT, 0, (LPARAM)labelStr);
				SendMessage(buttonNext, WM_SETTEXT, 0, (LPARAM)"Next Step");
				currentPosition = rand()%N;
				x1 = Koord[currentPosition]; 
				y1 = 60;
				x2 = Koord[currentPosition]; 
				y2 = 65;

				int xPrev, yPrev;
				xPrev = Koord[currentPosition];
				yPrev = 60;
				int ic=0;
				lst = listFirstQueue;
				while(true)
				{
					Sleep(1500);
					lst->insertToList(50);
					lst->insertToList(0);
					WaitForSingleObject(SemaphoreKiller, INFINITE);
					q = lst->GetHead();
					lst->SortListMin();
					labelStr[0] = 0;
					//while(q != NULL)
					//{
					//	itoa(q->nt, str, 10);
					//	strcat(str, "  ");
					//	strcat(labelStr, str);
					//	q=q->next;
					//}
					//MessageBox(MainWindowHandle, labelStr, "ex", MB_ICONINFORMATION);
					//q = lst->GetHead();

					while(q != NULL)
					{
						if(nc == N || currentPosition == N) break;
						mainMessageStr[0] = 0;
						nc = ChooseRequest(q);
						x2 = Koord[nc];
						y2+=5*ic;
						Line(hdc, xPrev, yPrev, x2, y2);

						strcat(mainMessageStr, st1);
						itoa(nq, str, 10);
						strcat(mainMessageStr, str);
						strcat(mainMessageStr, "  ");
						strcat(mainMessageStr, st2);
						itoa(currentPosition, str, 10);
						strcat(mainMessageStr, str);
						MessageBox(MainWindowHandle, mainMessageStr, "ex", MB_ICONINFORMATION);
						xPrev = x2;
						yPrev = y2;
						y2=y1+30;
						ic++;
						q=q->next;
					}
					ReleaseSemaphore(SemaphoreKiller, 1, NULL);
					nc=0;
					if(nq == 1)
					{
						lst = listSecondQueue;
						currentPosition=0;
						nq = 2;
					}
					else if(nq == 2) 
					{
						lst = listFirstQueue;
						currentPosition=0;
						nq = 1;
					}
				}

				ValidateRect(MainWindowHandle, NULL);
				//заканчиваем рисовать
				EndPaint(MainWindowHandle, &ps);

			} break;

		// При уничтожении окна, отправить сообщение о выходе для прерывания обработки сообщений
        case WM_DESTROY: 

            PostQuitMessage(0); 
            return 0;
    }

    // Обработка сообщений по умолчанию
    return DefWindowProc(windowHandle, msg, wParam, lParam);
}