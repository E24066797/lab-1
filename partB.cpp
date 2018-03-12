#include <iostream>
using namespace std;

int main(void)
{
	int n;
	cout << "Please enter an integer:\n";
		cin >> n;
	while(n != 1){
		int m = n;
		if(m % 2 != 0) {
			n = 3 * n + 1;
		}
		else {n = n / 2;}
		cout << n << endl ;
	}
	cout << "n = 1   stop";
	
	return 0;
}


