#include <thread>
#include <iostream>
#include <chrono>

#include <Windows.h>

#include <hook.h>

using namespace std;
using namespace std::literals;

int main() {
	cout << HookAllApps(TRUE, 0) << endl;
	this_thread::sleep_for(5min);
	cout << HookAllApps(FALSE, 0) << endl;
}
