#include <iostream>
#include <queue>

using namespace std;

#define MAX_L 42
#define MAX_N 32

int board[MAX_L][MAX_L];      // 체스판 상태
int knight[MAX_L][MAX_L];   // 기사 영역
int first_hp[MAX_N];      // 기사들의 초기 체력 
int hp[MAX_N];            // 기사들의 현재 체력
int is_dead[MAX_N];       // 탈락 여부
pair<int, int> knight_pos[MAX_N];  // 기사들의 위치
pair<int, int> knight_h_w[MAX_N];   // 기사들의 방패 크기 

int dx[] = {-1, 0, 1, 0};   // 위, 오, 아래, 왼
int dy[] = {0, 1, 0, -1};

int L, N, Q;

int main(){
    //freopen("input.txt", "r", stdin);

    cin >> L >> N >> Q;

    // 체스판 정보
    for(int i = 1; i <= L; i++){
        for(int j = 1; j <= L ; j++){
            cin >> board[i][j];
       }
    }

    // 1번 ~ N명의 기사 정보
    for(int i = 1; i <= N; i++){
        int r, c, h, w, k;
        cin >> r >> c >> h >> w >> k;
        knight_pos[i] = {r, c};
        knight_h_w[i] = {h, w};
        first_hp[i] = k;
        hp[i] = k;
        for(int x = r; x < r + h ; x++){
            for(int y = c; y < c + w; y++){
                knight[x][y] = i;
            }
        }
    }


    // Q개의 명령
    for(int q = 1; q <= Q; q++){
        int first_knight_num, dir;
        cin >> first_knight_num >> dir;

        // 명령 받은 기사
        int h = knight_h_w[first_knight_num].first;
        int w = knight_h_w[first_knight_num].second;

        // 이동할 칸
        int new_x = knight_pos[first_knight_num].first + dx[dir];
        int new_y = knight_pos[first_knight_num].second + dy[dir];


        // 명령 받은 기사가 이동할 영역이 벽이거나 격자 밖이진 않은지 & 충돌 발생한 기사 확인
        queue<int> crashed_num;
        int if_crashed[MAX_N];     // 중복 방지용
        fill(if_crashed , if_crashed + MAX_N, 0);

        int flag = 0;
        int tmp[MAX_L][MAX_L];
        fill(&tmp[0][0], &tmp[MAX_L][MAX_L], 0);

        for(int x = new_x ; x < new_x + h; x++){
            for(int y = new_y ; y < new_y + w; y++){
                // 벽에 부딪힘
                if (x < 1 || y < 1 || x > L || y > L || board[x][y] == 2) {
                    flag = 1;
                    break;
                }
                // 다른 기사와 충돌
                else if(knight[x][y] > 0 && knight[x][y] != first_knight_num){
                    if(!if_crashed[knight[x][y]]) { // 중복 방지용
                        crashed_num.push(knight[x][y]);
                        if_crashed[knight[x][y]] = 1;
                    }
                }
                tmp[x][y] = first_knight_num;
            }
        }

        if (flag == 1) continue;    // 명령을 받은 기사도 이동할 수 없음
        if(crashed_num.size() == 0) continue;   // 연쇄 충돌한 기사 없다면 패스


        // 가장 마지막의 연쇄 이동 기사가 벽에 부딪히지 않는지 확인하기 -> 모든 이동 무산
        // int last_knight_num = first_knight_num;
        flag = 0;

        while(!crashed_num.empty() && !flag){
            int last_knight_num = crashed_num.front();
            crashed_num.pop();

            int last_knight_new_x = knight_pos[last_knight_num].first + dx[dir];
            int last_knight_new_y = knight_pos[last_knight_num].second + dy[dir];
            int last_knight_h =  knight_h_w[last_knight_num].first;
            int last_knight_w =  knight_h_w[last_knight_num].second;

            // 연쇄 충돌한 기사의 영역도 충돌이 발생하는지 확인하기
            for(int i = last_knight_new_x; i < last_knight_new_x + last_knight_h; i++){
                for(int j = last_knight_new_y; j < last_knight_new_y + last_knight_w; j++){
                    // 벽이 있다면 모든 이동 무산!
                    if(board[i][j] == 2 || i < 1 || j < 1 || i > L || j > L){
                        flag = 1;
                        break;
                    }
                    // 또 다른 기사와 충돌
                    else if (knight[i][j] > 0 && knight[i][j] != last_knight_num){
                        if (!if_crashed[knight[i][j]]){
                            crashed_num.push(knight[i][j]);
                            tmp[i][j] = last_knight_num;

                        }
                    }
                    // 충돌 없음
                    else{
                        tmp[i][j] = last_knight_num;
                    }
                }
            }

        }
        if (flag == 1) continue;    
        
        // 벽 없이 모든 이동 완료시 대미지 깎고,
        // knight_map에 이동 결과 저장하기
        int updated[MAX_N]; 
        fill(updated, updated + MAX_N, 0);

        for(int i = 1; i<= L ; i++){
            for(int j = 1; j <= L ; j++){
                
                int num = tmp[i][j];
                if(num == 0) {
                    knight[i][j] = 0;
                    continue;
                }

                // 처음 명령을 받은 기사를 제외하고 함정(1) 과 겹치면 -1 체력
                if(board[i][j] == 1 && num > 0 && num != first_knight_num) hp[num] -= 1;

                // 체력 다쓰면 체스판에서 사라짐
                if (hp[num] <= 0) {
                    is_dead[num] = 1;
                    continue;
                }

                // 이동 결과 업데이트
                knight[i][j] = tmp[i][j];
                if(updated[num] == 1) continue;

                // 영역의 제일 좌측 상단 정보도 업데이트
                knight_pos[num] = {i, j};
                updated[num] = 1;

            }
        }
    }

    // 살아있는 기사들의 대미지 합 구하기 
    int ans = 0; 
    for(int i = 1; i <= N; i++){
        if(is_dead[i]) continue;
        ans += (first_hp[i] - hp[i]);
    }

    cout << ans << endl;
}