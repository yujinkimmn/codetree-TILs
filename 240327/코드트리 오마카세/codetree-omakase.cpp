#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>

using namespace std;

int L, Q;
struct CMD{
    int cmd, x, t, n;
    string name;
};

vector <CMD> commands;
unordered_map<string, int> sitting_position;    // 손님이 앉은 자리
unordered_set<string> visitting_customer_names; // 방문한 손님 목록(재방문은 X)
unordered_map<string, int> visit_customer_time;     // 손님이 방문한 시간대
unordered_map<string, int> eat_cnt; // 손님이 먹어야 할 초밥 갯수
vector<CMD> v;

int sushi_cnt; // 남은 초밥 갯수
int customer_cnt;   // 남은 손님 명수

void input(){
    cin >> L >> Q; 

    // 명령어 입력 
    for(int i = 0; i < Q; i++){
        int cmd = -1, t = -1, x = -1, n = -1;
        string name = "";

        cin >> cmd;

        if (cmd == 100) cin >> t >> x >> name;

        else if (cmd == 200) {
            cin >> t >> x >> name >> n;
            
            visitting_customer_names.insert(name);
            sitting_position[name] =  x;
            visit_customer_time[name] = t;
            eat_cnt[name] = n;
        }
        else if (cmd == 300) cin >>  t;

        commands.push_back({cmd, x, t, n, name});
    }
}

bool cmp(CMD left, CMD right){
    if (left.t < right.t) return true;
    if (left.t > right.t) return false;
    if (left.cmd < right.cmd) return true;
    if (left.cmd > right.cmd) return false;
    return false;
}

void RecordEatingSushi(){
    // 생성된 초밥이 언제 없어질지 기록하기
    for(int i = 0; i < commands.size(); i++){
        if(commands[i].cmd != 100 ) continue;      // 초밥 생성된 경우만 !
        
        CMD sushi = commands[i];

        int meet_sushi_customer_time = 0;   // 초밥과 손님이 만나게 할 수 있는 보정 시간 
        int eat_sushi_time = 0 ;    // 초밥을 먹는 시간

        // 초밥이 만들어진 후에 손님이 들어온다면 
        if (sushi.t < visit_customer_time[sushi.name]){

            // 현재 초밥의 위치
            int now_sushi_position = sushi.x;

            // 손님이 방문 할 때 까지 기다려야 하는 시간 
            int diff_t = visit_customer_time[sushi.name] - sushi.t;

            // 손님이 입장했을 때 바뀌는 초밥의 위치
            now_sushi_position = (now_sushi_position + diff_t) % L;

            // 손님이 초밥보다 시계방향으로 더 먼쪽에 있는 경우
            if (sitting_position[sushi.name] >= now_sushi_position){
                meet_sushi_customer_time = sitting_position[sushi.name] - now_sushi_position;
            }

            // 반시계방향으로 더 먼 쪽에 있는 경우
            else if (sitting_position[sushi.name] < now_sushi_position){
                meet_sushi_customer_time = L - (now_sushi_position - sitting_position[sushi.name]);
            }
            // 손님이 초밥을 먹을 수 있는 시간 = 손님이 방문한 시점 + 초밥 먹는데 걸리는 시간
            eat_sushi_time = visit_customer_time[sushi.name] + meet_sushi_customer_time;
        }

        // 초밥 만들어졌을 때 손님이 이미 있는 경우 
        else{
            int now_sushi_position = sushi.x;

            if (sitting_position[sushi.name] >= now_sushi_position){
                meet_sushi_customer_time = sitting_position[sushi.name] - now_sushi_position;
            }
            else if (sitting_position[sushi.name] < now_sushi_position){
                meet_sushi_customer_time = L - (now_sushi_position - sitting_position[sushi.name]);
            }
            eat_sushi_time = sushi.t + meet_sushi_customer_time;
        }
        // 초밥이 사라지는 시점을 기록 
        // cmd , x, t, n ,name
        v.push_back({101, -1, eat_sushi_time, -1, sushi.name});

    }

    // 전체 명령과 초밥 사라지는 기록 다 합치기 
    commands.insert(commands.end(), v.begin(), v.end());

    // t가 작을 수록(시간순), 명령이 작을수록 우선순위를 부여해 정렬
    // 명령어 우선순위 100 -> 101 -> 200 -> 300
    sort(commands.begin(), commands.end(), cmp);
}


void DoCmd(){
    // 실제 명령대로 수행하면서 초밥 & 사람 개수 세기
    for(int i = 0; i < commands.size(); i++ ){
        
        // 초밥 생성
        if (commands[i].cmd == 100){
            ++sushi_cnt;
        }

        // 초밥 사라짐
        else if (commands[i].cmd == 101){
            --sushi_cnt;

            // 손님이 먹을 초밥 갯수 줄이기
            string now_name = commands[i].name;
            eat_cnt[now_name] -= 1;
            if(eat_cnt[now_name] <= 0) --customer_cnt;  // 개수만큼 다 먹으면 손님 떠나기
        }

        // 손님 방문
        else if(commands[i].cmd == 200){
            ++customer_cnt;
        }

        // 사진 촬영
        else if (commands[i].cmd == 300){
            cout << customer_cnt << " " << sushi_cnt << '\n';
        }
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    input();
    RecordEatingSushi();
    DoCmd();

    return 0;
}