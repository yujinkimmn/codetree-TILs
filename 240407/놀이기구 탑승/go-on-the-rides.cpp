#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;

int n;
// 놀이기구 격자
int seats[20][20];
// 좋아하는 학생 정보를 저장(탑승 학생 번호 + 학생당 4명) {n0, n1, n2, n3, n4}
vector<vector<int>> likes;
// 학생들이 앉은 자리 기록해놓기
vector<pair<int, int>> pos;

// 좋아하는 학생 수에 따른 점수
int scores[5] = {0, 1, 10, 100, 1000};

// 상하좌우
int dx[4] = {-1, 1, 0, 0}; 
int dy[4] = {0, 0, -1, 1};

// 칸의 상태
// 좋아하는 친구 수, 빈칸수, -행, -열
typedef tuple<int, int, int, int> Cell;

bool InRange(int x, int y){
    return 0 <= x && x < n && 0 <= y && y < n;
}


// num번째 학생이 앉을 자리 찾기 
void StudentSit(int num){
    // 초기화 
    Cell best_cell = make_tuple(0, 0, -(n+1), -(n+1));
    
    // 격자 순서대로 탐색
    for(int r = 0; r < n ; r++){
        for(int c = 0; c < n ; c++){
             // 비어있지 않은 자리 패스
            if(seats[r][c] != 0) continue; 
            
            // 현재 자리에서 네 방향 탐색하며 개수 세기
            int like_cnt = 0;
            int empty_cnt = 0;
            for(int d = 0; d < 4; d++){
                int nx = r + dx[d];
                int ny = c + dy[d];
                if (!InRange(nx, ny)) continue; // 격자 벗어나면 패스
                // 빈 자리 
                if(!seats[nx][ny]) 
                    empty_cnt++; 
                // 좋아하는 친구인지 확인(n1 ~ n4 중에)
                else if(find(likes[num].begin() + 1, likes[num].end(), seats[nx][ny]) != likes[num].end()){
                    like_cnt++;
                }
            }
            
            Cell curr = make_tuple(like_cnt, empty_cnt, -r, -c);

            if (curr > best_cell){
                best_cell = curr;
            }
        }
    }
    int x, y;
    tie(ignore, ignore, x, y) = best_cell;
    // 최종 자리에 앉히기
    int n0 = likes[num][0]; 
    seats[-x][-y] = n0;   
    pos.push_back(make_pair(-x, -y));
}

// 현재 학생의 점수를 계산
int CalcScore(int num){
    int r = pos[num].first;
    int c = pos[num].second;

    // 사방 탐색하며 좋아하는 친구 수 카운트
    int like_cnt = 0;
    for(int d = 0; d < 4; d++){
        int nx = r + dx[d];
        int ny = c + dy[d];
        if (!InRange(nx, ny)) continue; // 격자 벗어나면 패스
        // 좋아하는 친구인지 확인
        if(find(likes[num].begin() + 1, likes[num].end(), seats[nx][ny]) != likes[num].end()){
            like_cnt++;
        }
    }
    // 좋아하는 친구 수에 따라 점수 반환
    return scores[like_cnt];
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n;
    for(int i = 0 ; i < n*n; i++){
        int n0, n1, n2, n3, n4;
        cin >> n0 >> n1 >> n2 >> n3 >> n4;
        likes.push_back({n0, n1, n2, n3, n4});
    }

    int dum = -1;
    // 순서대로 학생 탑승
    for(int i = 0 ; i < n*n ; i++){
        StudentSit(i);
    }

    // 점수 계산도 학생 탑승 순서대로! 
    int ans = 0;
    for(int i = 0 ; i < n*n ; i++){
        ans += CalcScore(i);
    }
    cout << ans << '\n';
}