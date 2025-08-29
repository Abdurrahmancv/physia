// 4. BOYUT TESTİ

#include <iostream>
using namespace std;

int main(){
    // 4 DIMENTION TEST
    int arr[4][4][4][4];

    int sayi = 0;
    string test1 = "0" + sayi;

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            for(int k=0; k<4; k++){
                for(int l=0; l<4; l++){
                static int sayi=100;
                arr[i][j][k][l] = sayi;
                sayi++;
                }
            }
        }
    }


    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            for(int k=0; k<4; k++){
                for(int l=0; l<4; l++){
                cout << arr[i][j][k][l] << " ";
                }
                cout << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    return 0;
}
