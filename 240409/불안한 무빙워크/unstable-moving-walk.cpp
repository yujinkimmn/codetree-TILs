#include <iostream>
#include <vector>

using namespace std;

int n, k;
int turn;

// 0번째 칸부터 2n - 1 번째 칸까지 안정성 저장
vector<int> safety;
vector<int> people;

void Rotate(vector<int> &v){
    vector <int> tmp;
    for(int i = 0; i < 2 * n; i++){
        tmp.push_back(v[(i - 1 + 2 * n) % (2 * n)]);
    }
    v = tmp;
}

void MovePeople(){
    // 사람들은 0 ~ n-1번째 칸에만 있음 
    // 가장 먼저 올라간 사람 부터 하려면 n-2 부터 0까지 탐색
    for(int i = n - 2;  i >= 0 ; i--){
        if(!people[i]) continue;
        // 앞 칸에 사람이 없고 안정성 1 이상인 경우에만 이동 가능 
        if(!people[i + 1] && safety[i + 1] >= 1) {
            people[i + 1] = 1;
            people[i] = 0;
            safety[i + 1]--;
        }
        // 앞 칸이 n-1 번째 칸이면 즉시 내리기
        if (i + 1 == n - 1) people[i + 1] = 0;
    }
}

void PeopleRide(){
    // 0번칸에 사람 없고 안정성 0 아니면 탑승
    if(!people[0] && safety[0] >= 1){
        people[0] = 1;
        safety[0]--;
    }
}

bool IsPassed(){
    int cnt = 0;
    for(int i = 0; i < 2 * n ; i++){
        if(safety[i] == 0) cnt++;
    }
    if(cnt >= k) return false;
    else return true;
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n >> k;
    for(int i = 0 ; i < 2 * n ; i++){
        int s; cin >> s;
        safety.push_back(s);
    }
    people.resize(2 * n, 0);    // 사람들 없는 상태로 초기화

    int dum = -1;

    while(true){
        turn++;
        // 무빙워크 + 사람 같이 시계방향 한 칸 회전
        Rotate(safety);
        Rotate(people);
        // n-1 번째 칸에 사람이 가게 되면 내리기 
        if(people[n-1]) people[n-1] = 0;

        // 무빙워크의 사람들이 회전 방향으로 한 칸 이동
        MovePeople();

        // 1번칸에 사람 탑승
        PeopleRide();

        // 안정성이 0인 칸이 k개 이상이면 종료
        if(!IsPassed()) break;
    }
    cout << turn << "\n";

}