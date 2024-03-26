#include <iostream>
#include <vector>
#include <map>
using namespace std;

int Q, L, cur_time;

struct Sushi{
    string name;
    int num;
};

struct Person{
    string name;
    int num;
};
map<int, vector<Sushi>> sushies;    // (x 위치, 초밥 정보들)
map<int, Person> people; 



// 스시 만들기
void MakeSushi(int pos, string name){
    // 해당 자리에 초밥이 있는지 확인
    auto s = sushies.find(pos);
    
    // 같은 이름의 초밥이 있으면 숫자만 추가
    int flag = 0;
    if (s != sushies.end()){
       std::vector<Sushi>::iterator it;
        for(auto i = s->second.begin(); i != s->second.end(); i++){
            if (i->name == name) {
                i->num++;
                flag = 1;
                break;
            }
        }
        // 해당 자리에 없는 이름이면 push 
        if(flag==0) s->second.push_back({name, 1});
    }
    // 같은 이름의 초밥이 없으면 새로 추가
    else sushies[pos].push_back({name, 1});
}


void EatSushi(){
    // 사람이 있는 자리만 확인
    if(people.size() == 0) return;
    for (auto p = people.begin(); p != people.end();){
        int x = p->first;
        int flag = 0;   // 먹을 초밥 있었는지를 표시 
        string p_name = p->second.name;

        auto su = sushies.find(x);
        // 해당 위치에 초밥이 있다면 
        if (su != sushies.end()){

            // 같은 이름의 초밥이 있는지 확인 
            for(auto s = su->second.begin(); s != su->second.end(); s++){
                // 여러 개 동시에 먹을 수 있음!
                if (s->name == p_name){
                    flag = 1;
                    // 초밥의 개수가 더 많은 경우 사람 다먹고 떠나기
                    if(s->num > p->second.num) p = people.erase(p);

                    // 개수가 같은 경우 사람 떠나기 & 초밥 없애기
                    else if (s->num == p->second.num) {
                        p = people.erase(p);
                        su->second.erase(s);
                    }
                    // 초밥의 개수가 더 적은 경우 사람 개수 줄이기 & 초밥 없애기
                    else if(s->num < p->second.num){
                        p->second.num -= s->num;
                        su->second.erase(s);
                        if(su->second.size() == 0) sushies.erase(su);
                    }
                    break;
                }
            }
            if (flag == 0) p++; // 다음 사람
        }
        else p++;
    }
}


// time 만큼 벨트를 시계방향으로 회전
void Rotate(int time){
    // 맨 처음엔 회전할 거 없음
    if(time == 1) {
        cur_time = 1;
        return;
    }

    // 회전할 초밥 없으면 패스
    if(sushies.size() != 0){

        // 1초마다 회전 + 먹기 수행
        int gap = time - cur_time;

        for(int i = 0 ; i < gap ; i++){
            map<int, vector<Sushi>> tmp;
            for(auto pos : sushies){
                int newPos = (pos.first + i) % L;
                tmp[newPos] = pos.second;
            }
            sushies = tmp;
            EatSushi();
        }
    }
    // 현재 시간 업데이트 
    cur_time = time;
}

// 사람 수와 초밥의 수를 출력
void TakePicture(){
    int sushi_cnt = 0;
    for(auto it = sushies.begin(); it!= sushies.end(); it++){
        for(auto s = it->second.begin(); s != it->second.end(); s++){
            sushi_cnt += s->num;
        }
    }
    cout << people.size() << " " << sushi_cnt << '\n';
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> L >> Q;

    // 벨트 길이 만큼 초기화
    

    // Q개의 명령 실행
    for(int i = 0 ; i < Q; i++){
        int q, t, x, n; string name;
        cin >> q;
        
        // 초밥 만들기
        if(q == 100){
            cin >> t >> x >> name;
            Rotate(t);
            MakeSushi(x, name);
            EatSushi();
        }

        // 손님 입장
        if(q == 200){
            cin >> t >> x >> name >> n;
            Rotate(t);
            people[x] = {name, n};  // 사람 앉히기
            EatSushi(); 
        }

        // 사진 촬영
        if (q == 300){
            cin >> t;
            Rotate(t);
            EatSushi();
            TakePicture();
        }
    }
}