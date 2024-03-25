#include <iostream>
#include <queue>
using namespace std;

// 왜 블록 값이 2의 제곱꼴일까?????

// DFS -> 1024 경우의 수 모두 따져보기? 
// DFS + Backtracking 혹은 
// DP를 이용하면 계산을 더 줄일 수 있을 것 같다.

int N; 
int MAP[21][21];
int max_value = 0;

void input(){
    //freopen("input.txt", "r", stdin);
    cin >> N;
    for(int i = 0; i < N; i++){
        for(int j = 0 ; j < N; j++){
            int n; cin >> n;
            MAP[i][j]= n; 
            max_value = max(max_value, n);
        }
    }
}

void findMax(){
    int cur_max = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0 ; j < N; j++){
            cur_max = max(cur_max, MAP[i][j]);
        }
    }
    //cout << "최대값: " << cur_max;
    if (cur_max > max_value) max_value = cur_max;
}

void move(int dir){
    // 위 - 열고정 & 행 탐색
    //cout << dir << '\n';
    if(dir == 0){
        for(int i = 0 ; i < N ; i++){
            queue <int> qu;
            for(int j = 0; j < N ; j++){
                qu.push(MAP[j][i]);
                MAP[j][i] = 0;
            }
            int idx = 0;
            while(!qu.empty()){
                int now = qu.front();
                qu.pop();
                // 빈자리면 그냥 삽입
                if(MAP[idx][i] == 0) {
                    MAP[idx][i] = now;
                }
                // 같은 값인 경우 누적 & 인덱스 증가(두번 합쳐짐 방지)
                else if (MAP[idx][i] == now){
                    MAP[idx][i] += now;
                    idx++;
                }
                // 값이 다른 경우 다음 칸에 삽입
                else if(MAP[idx][i] != now){
                    idx++;
                    MAP[idx][i] = now;
                }
            }
        }
    }

    // 오른쪽 -> 행고정 & 열탐색
    if(dir == 1){
        for(int i = 0 ; i < N ; i++){
            queue <int> qu;
            for(int j = N-1; j >=0 ; j--){
                qu.push(MAP[i][j]);
                MAP[i][j] = 0;
            }
            int idx = N-1;
            while(!qu.empty()){
                int now = qu.front();
                qu.pop();
                // 빈자리면 그냥 삽입
                if(MAP[i][idx] == 0) {
                    MAP[i][idx] = now;
                }
                // 같은 값인 경우 누적 & 인덱스 증가(두번 합쳐짐 방지)
                else if (MAP[i][idx] == now){
                    MAP[i][idx] += now;
                    idx--;
                }
                // 값이 다른 경우 다음 칸에 삽입
                else if(MAP[i][idx] != now){
                    idx--;
                    MAP[i][idx] = now;
                }
            }
        }
    }

    // 아래쪽 -> 열 고정 & 행 탐색
    if(dir == 2){
        for(int i = 0 ; i < N ; i++){
            queue <int> qu;
            for(int j = N-1; j >=0 ; j--){
                qu.push(MAP[j][i]);
                MAP[j][i] = 0;
            }
            int idx = N-1;
            while(!qu.empty()){
                int now = qu.front();
                qu.pop();
                // 빈자리면 그냥 삽입
                if(MAP[idx][i] == 0) {
                    MAP[idx][i] = now;
                }
                // 같은 값인 경우 누적 & 인덱스 증가(두번 합쳐짐 방지)
                else if (MAP[idx][i] == now){
                    MAP[idx][i] += now;
                    idx--;
                }
                // 값이 다른 경우 다음 칸에 삽입
                else if(MAP[idx][i] != now){
                    idx--;
                    MAP[idx][i] = now;
                }
            }
        }
    }
    // 왼쪽 -> 행 고정 & 열 탐색

    if (dir == 3){
        for(int i = 0 ; i < N ; i++){
            queue <int> qu;
            for(int j = 0; j < N ; j++){
                qu.push(MAP[i][j]);
                MAP[i][j] = 0;
            }
            int idx = 0;
            while(!qu.empty()){
                int now = qu.front();
                qu.pop();
                // 빈자리면 그냥 삽입
                if(MAP[i][idx] == 0) {
                    MAP[i][idx] = now;
                }
                // 같은 값인 경우 누적 & 인덱스 증가(두번 합쳐짐 방지)
                else if (MAP[i][idx] == now){
                    MAP[i][idx] += now;
                    idx++;
                }
                // 값이 다른 경우 다음 칸에 삽입
                else if(MAP[i][idx] != now){
                    idx++;
                    MAP[i][idx] = now;
                }
            }
        }
    }
}


void dfs(int cnt){
    if (cnt > 5){   // 종료 조건
        findMax();
        //cout << '\n';
        return;
    }
    for(int d = 0 ; d < 4 ; d++){
        // 원본 복사 해놓기
        int tmp [21][21];
        for(int i = 0 ; i < N; i++){
            for(int j = 0 ; j < N; j++){
                tmp[i][j] = MAP[i][j];
            }
        }

        // 재귀
        move(d);
        dfs(cnt + 1);

        // 원상 복구 
        for(int i = 0 ; i < N; i++){
            for(int j = 0 ; j < N; j++){
                MAP[i][j] = tmp[i][j];
            }
        }
    }
}

int main(){
    input();
    dfs(1);
    cout << max_value;
}