#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

int D, W, K;
int film[13][20];
int tmp[13][20];

vector<pair<int, int>> inject;  // (몇 번째 행에, 0또는 1주입)

// 해당 inject 조합으로 통과할 수 있는지를 판단
bool CheckIfPassed(){
    // tmp 초기화
    memcpy(tmp, film, sizeof(tmp));
    
    for (auto i : inject) {
        fill(tmp[i.first], tmp[i.first] + W, i.second);
        // memset(tmp[i.first], i.second, sizeof(tmp[i.first]));
    }
    
    // 통과 가능한지
    int passed = 0;
    // W개 열의 필름이 모두 통과해야 함
    for(int j = 0 ; j < W; j++){
        int now = tmp[0][j];
        int cnt = 1;
        bool pass = false;
        for(int i = 1 ; i < D; i++){
            if(tmp[i][j] == now){
                cnt++;
                if(cnt == K) {
                    passed++;
                    pass = true;
                    break;
                }
            }
            else{
                now = tmp[i][j];
                cnt = 1;
            }
        }
        if(!pass) return false; // 하나의 필름이라도 통과하지 못하면 false
    }
    // W개 열이 모두 통과했으면 true를 반환
    if (passed == W){
        return true;
    }
    else return false;
}


bool DFS(int cnt, int start){
    // cnt 개수 만큼의 조합을 만들었으면 통과 가능한지 확인하기
    if ((int)inject.size() == cnt){
        if(CheckIfPassed()){
            return true;
        }
        return false;
    }

    // 아니면 start 행부터 조합 만들기
    for(int row = start; row < D; row++){
        // start 행에 0 주입 시도
        inject.push_back({row, 0});
        // start+1 행부터 조합 만들기
        if(DFS(cnt, row + 1)){
            return true;
        }
        // 원상 복구
        inject.pop_back();
        // start 행에 1 주입 시도
        inject.push_back({row, 1});
        // start+1 번째 행부터 조합 만들기
        if(DFS(cnt, row + 1)){
            return true;
        }
        // 원상복구
        inject.pop_back();
    }
    return false;
}

int main(int argc, char** argv){
    int test_case;
    int T;
    //freopen("input.txt", "r", stdin);
    cin>>T;
    for(test_case = 1; test_case <= T; ++test_case)
    {
        cin >> D >> W >> K;
        for(int i = 0; i < D ; i ++){
            for(int j = 0; j < W; j++){
                cin >> film[i][j];
            }
        }

        // K가 1이면 그냥 0을 리턴
        int ans = K;
        if(K == 1) ans = 0;
        else if(K > 1){
            // 0개부터 K개까지 DFS로 조합 만들어보면서 찾기
            for(int cnt = 0; cnt < K; cnt++){
                // 가능한 조합을 담을 변수 초기화
                inject.clear();
                if(DFS(cnt, 0)){    // (몇개 조합, 시작 행)
                    ans = cnt;
                    break;
                }
            }
        }
        cout << "#" << test_case << " " << ans << "\n";
    }
    return 0;//정상종료시 반드시 0을 리턴해야합니다.
}
