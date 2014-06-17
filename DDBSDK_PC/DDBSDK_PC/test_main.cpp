

#include <Windows.h>
#include <process.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>

#include "jscpp.io/BaseIO.h"
#include "TS_Sturct/TS_Tables.h"
#include "Data_Pack/Data_Pack.h"
#include "LocalLog.h"
#include "SyncCircleBuffer.h"
#include "dmb/TsDevic.h"
#include "Data_Pack/Data_Prase.h"

#include "ddbsdk.h"
//#include <WinBase.h>


using namespace std;

template <typename Object>
struct Node
{
	Object data;
	Node *prev;
	Node *next;

	Node(const Object & d = Object(), Node *p = NULL, Node *n = NULL)
		: data(d), prev(p), next(n){printf("Node Cont...\n");}
};



class Worker
{
public:
	Worker(int n)
	{
		this->i = n;
		InitializeCriticalSection(&(this->locker));
	}
	Worker()
	{
		printf("This is Worker...\n");
		this->i = 3;
		InitializeCriticalSection(&(this->locker));
	}

	CRITICAL_SECTION locker;
	int i;
	void dojob()
	{
		EnterCriticalSection(&(this->locker));
		printf("Begin doing job......!   %i\n", this->i);
		Sleep(3000);
		printf("Finish doing job......!   %i\n", this->i);
		LeaveCriticalSection(&(this->locker));
	}

	~Worker()
	{
		EnterCriticalSection(&(this->locker));
		printf("~Worker...\n");
		LeaveCriticalSection(&(this->locker));
		DeleteCriticalSection(&(this->locker));
	}
};


unsigned int WINAPI dojob(void* pram)
{
	list<Worker*>* workList = (list<Worker*>*)pram;
	printf("dojob fun...workList = %x\n", workList);
	list<Worker*>::iterator i;
	for(i=workList->begin(); i != workList->end(); ++i)
	{
		printf("work in list ADDR...%x\n", *i);
		(*i)->dojob();
	}

	return 0;
};

unsigned int WINAPI _threadStoreBuffer(void* pram)
{
	Sleep(2000);
	SyncCircleBuffer * syncBuffer = (SyncCircleBuffer*)pram;
	FILE* inFile = fopen("in.txt", "rb");
	if (inFile == NULL)
	{
		printf("in.txt文件无法打开！\n");
		return -1;
	}
	const int bufsize = 512;
	PBYTE buf = (PBYTE)malloc(bufsize);
	unsigned int redsize = 0;
	while ((redsize = fread(buf, 1, bufsize, inFile)) > 0)
	{
		syncBuffer->store(buf, redsize);
	}

	fclose(inFile);
	free(buf);

	printf("读文件，存buffer线程退出！\n");
	return 0;
};

class AInterface
{
	public:
		//AInterface(int a){printf("This is AInterface(int a)\n");}
		//AInterface(){printf("This is AInterface()\n");}
		virtual void getPrint() = 0;
		void p(){printf("AInterface's P\n");}
		virtual ~AInterface(){printf("~AInterface\n");}
};

class AImplement : public AInterface
{
public:
	//AImplement(){printf("This is AImplement()\n");}
	AImplement(int a){printf("This is AImplement(int a)\n");}
	void getPrint();
	void p(){printf("AImplement's P\n");}
	~AImplement(){printf("~AImplement\n");}
};
void AImplement::getPrint(){printf("This is A Implement!\n");}
//AImplement::AImplement(){printf("A\n");}

class AImpExtend : public AImplement
{
public:
	AImpExtend(int a):AImplement(a){printf("This is AImpExtend(int a)\n");}
// 	void getPrint();
// 	void p(){printf("AImpExtend's P\n");}
	~AImpExtend(){printf("~AImpExtend\n");}
};

class BImplement : public AInterface
{
public:
	//BImplement(){printf("This is BImplement()\n");}
	BImplement(int a){printf("This is BImplement(int a)\n");}
	void getPrint();
	void p(){printf("BImplement's P\n");}
	~BImplement(){printf("~BImplement\n");}
};
void BImplement::getPrint(){printf("This is B Implement!\n");}

class C
{
	void getHa();
};


class tempClass
{
public:
	tempClass(){cout << "tempClass initial..." << endl; num=13;}
	~tempClass(){cout << "tempClass Deinitial..." << endl;}
	void show(){cout << "tempClass Showing..." << endl;}
	int num;
};

void test_syncDeleteClass()
{

	    Worker* worker = new Worker();
	    Worker* worker2 = new Worker();
		printf("worker's ADDR...%x\n", worker);
		printf("worker2's ADDR...%x\n", worker2);
	 //    Worker& _worker2 = *worker2;
	 //    Worker& _worker = *worker;
	    list<Worker*>* workList = new list<Worker*>;
		printf("list<Worker*>* workList's ADDR...%x\n", workList);
	    workList->push_back(worker);
	    workList->push_back(worker2);
	 
	    HANDLE hThread;
	    unsigned threadID;
	    hThread = (HANDLE)_beginthreadex( NULL, 0, dojob, workList, 0, &threadID );
	    Sleep(500);
	    delete worker;
	    workList->pop_front();
	    Sleep(500);
	    delete worker2;
	    workList->pop_front();
}

unsigned int WINAPI dojob_B(void* pram)
{
	list<Worker> workList = *(list<Worker>*)pram;
	printf("dojob fun...workList = %x\n", &workList);
	list<Worker>::iterator i;
	for(i=workList.begin(); i != workList.end(); ++i)
	{
		printf("work in list ADDR...%x\n", &(*i));
		(*i).dojob();
	}

	return 0;
};

void test_syncDeleteClass_B()
{

	Worker worker;
	Worker worker2;
	printf("worker's ADDR...%x\n", &worker);
	printf("worker2's ADDR...%x\n", &worker2);
	//    Worker& _worker2 = *worker2;
	//    Worker& _worker = *worker;
	list<Worker> workList;
	printf("list<Worker> workList's ADDR...%x\n", &workList);
	workList.push_back(worker);
	workList.push_back(worker2);

	HANDLE hThread;
	unsigned threadID;
	hThread = (HANDLE)_beginthreadex( NULL, 0, dojob_B, &workList, 0, &threadID );
	Sleep(500);
	workList.pop_front();
	Sleep(500);
	workList.pop_front();
}

void test_SyncCircleBuffer()
{
	Buffer* syncBuffer = new SyncCircleBuffer(20*1024, 1000);

	HANDLE hThread;
	unsigned threadID;
	hThread = (HANDLE)_beginthreadex( NULL, 0, _threadStoreBuffer, syncBuffer, 0, &threadID );
	//Sleep(100);

	FILE* outFile = fopen("out.txt", "wb");

	const int bufsize = 512;
	PBYTE outBuf = (PBYTE)malloc(bufsize);
	unsigned int retByt = 0;

	int cout = 0;

	while(cout < 3)
	{
		syncBuffer->read(outBuf, bufsize, &retByt);
		if (retByt > 0)
		{
			fwrite(outBuf, 1, retByt, outFile);
		}
		else
		{
			cout++;
		}

	}
	fclose(outFile);
	free(outBuf);
	delete syncBuffer;
}


#include <stdarg.h>
int demo( char *fmt, ... ) 
{ 
	va_list argp; 
	int argno = 0;  
	char *para; 
	va_start(argp, fmt); 
	while (1) 
	{ 
		para = va_arg(argp, char *); 
		if (strcmp( para, "") == 0) 
			break; 
		printf("Parameter #%d is: %s\n", argno, para); 
		argno++; 
	} 
	va_end( argp ); 
	return 0; 
} 
void learn_varyPram()
{
	demo("DEMO", "This", "is", "a", "demo!", ""); 
}

void test_FunWithPram()
{
	int num = 100;
	int &$_num = num;

	printf("num's add = %x\n", &num);
	printf("$_num's add = %x\n", &$_num);


	int i = 3;
	Node<int>* a = new Node<int>(3, NULL, NULL);
	printf("i=%d, address:%x\n", i, &i);
	printf("i in Node = %d, address:%x\n", a->data, &a->data);

	AImplement ai(1);
	Node<AImplement>* b = new Node<AImplement>(ai, NULL, NULL);
	printf("ai=%d, address:%x\n", ai, &ai);
	printf("ai in Node = %d, address:%x\n", b->data, &b->data);
}

typedef struct _t_SERVICE_
{
	U8	stra[16];
	U16	BitRateKhz;
	U8	SrvCharSet;
}DAB_Struct$;

void initialStruct(list<DAB_Struct$>* alist)
{
	char* aaa = "aaa";
	DAB_Struct$ a;
	strcpy((char*)&a.stra[0], aaa);
	a.BitRateKhz = 1;
	a.SrvCharSet = 2;
	alist->push_back(a);
}
// tempClass& a
void test_StructInList()
{
	printf("\n");
	list<DAB_Struct$> alist;
	initialStruct(&alist);
	list<DAB_Struct$>::iterator i;
	for(i=alist.begin(); i != alist.end(); ++i)
	{
		printf("DAB_Struct$ in list...stra=%s\n", (*i).stra);
		printf("DAB_Struct$ in list...BitRateKhz=%d\n", (*i).BitRateKhz);
		printf("DAB_Struct$ in list...SrvCharSet=%d\n", (*i).SrvCharSet);
	}

}


//Fun()
// {
// 	tempClass tempclass;
// 	cout << &tempclass << endl;
// 	tempclass.show();
// // 	delete tempclass;
// // 	tempclass->show();
// 	return tempclass;
// }

void main()
{
   vector<string> SS;

   SS.push_back("The number is 10");
   SS.push_back("The number is 20");
   SS.push_back("The number is 30");

   cout << "Loop by index:" << endl;

   int ii;
   for(ii=0; ii < SS.size(); ii++)
   {
      cout << SS[ii] << endl;
   }

   cout << endl << "Constant Iterator:" << endl;

   vector<string>::const_iterator cii;
   for(cii=SS.begin(); cii!=SS.end(); cii++)
   {
      cout << *cii << endl;
   }

   cout << endl << "Reverse Iterator:" << endl;

   vector<string>::reverse_iterator rii;
   for(rii=SS.rbegin(); rii!=SS.rend(); ++rii)
   {
      cout << *rii << endl;
   }

   cout << endl << "Sample Output:" << endl;

   cout << SS.size() << endl;
   cout << SS[2] << endl;

   swap(SS[0], SS[2]);
   cout << SS[2] << endl;

//    tempClass& tclass = aFun();
//    cout << &tclass << endl;
//    tclass.show();
//    ((tempClass*)0)->show();


   int i = 1;
   cout << endl;
   AInterface* interfac = new AImplement(i);
   interfac->getPrint();
   interfac->p();
   delete interfac;
   interfac = new BImplement(i);
   interfac->getPrint();
   interfac->p();
   delete interfac;
   AImpExtend* aimpext = new AImpExtend(1);
   aimpext->getPrint();

   cout << endl;


   //assert(0);


   //test_syncDeleteClass();
   //test_syncDeleteClass_B();

   //test_SyncCircleBuffer();


	//learn_varyPram();

   //test_FunWithPram();
   test_StructInList();

//    wprintf(TEXT("abc"));
// 	int t = 3;
//     InitializeLog(TEXT("ddblog日志.txt"));
//     WriteLog("ddblog test! 日志  %d\n", t);
	
}
                

