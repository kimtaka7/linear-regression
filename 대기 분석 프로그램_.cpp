/*=======================================
** 프로그램명: 대기 정보 예측 및 분석 프로그램 
** 입력 데이터출처 : https://www.airkorea.or.kr/web/realSearch?pMENU_NO=97 
**=====================================*/ 

#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <stdlib.h> //atoi, atof 함수 헤더파일 

#define days 1095 //전체 데이터 개수 (이상) 
#define all_days 150 //선형 회귀할때 참고할 데이터(날) 수  
using namespace std;

void arrangement(); //구조체에 파일에서 입력받은 값을 대입하는 함수
int find(char *wh_1, char *wh_2); //입력한 날짜가 파일 안에 존재하는 지 확인하고 위치를 반환하여 주는 함수  
int file_check(); //파일이 비어있는지 비어있지 않는지 확인하는 함수 (있으면 1, 없으면 0 리턴) 
void hold(); 

void compare();  //과거와의 대기정보 비교 기능 함수  
int select_compare(); //비교하고 싶은 기체를 입력받고 판별하여주는 함수  
int percent_check(); //증가인지 감소인지 판별하고 증가량, 감소량을 계산하여 주는 함수 

void expect();  //대기정보 예측 기능 함수   
void regression(); //선형회귀하는 함수, 최소 제곱법 
double calcul(int a, int b); //회귀한 값을 계산하는 함수 

void variance(); //변화량 그래프를 그려주는 기능 함수  
int max_check(int i); //값들 중 가장 큰 값을 찾는 함수 
void graph(int i, int max);
void res_check();
 
void menu();
void compare_menu();
int expect_menu();
int expect_menu2(int i);
void variance_menu();


struct infor{ //구조체 (파일에 있는 정보들을 가져다오기 위함) 
	char when[15];
	int VF_dust, F_dust;
	float Ozone, NO_2, CO_2, SO_2;
};

char VF_dust[10], F_dust[10], Ozone[10], NO_2[10], CO_2[10], SO_2[10]; //불러온 값을 잠시 저장하는 공간(정수형, 실수형으로 변환 전) 

infor p[days];

//wh_idx : 날짜의 인덱스 값 저장공간 / compare_air1 : 비교할 기체 정보 보관 장소(int 형) / times : 몇번 반복했는가?, main 함수에서 사용 
int wh_idx[2], compare_air1[2], times; 
float compare_air2[2]; //compare_air2 : 비교할 기체 정보 보관 장소(float 형)
double dx_dy; //증가량, 감소량 계산할 때 쓰이는 변수  
double dx_dy_2, y_intercept; //dx_dy_2 : 선형회귀에서의 기울기 변수 / y_intercept : y 절편 변수 
char air_sort[20]; //공기 이름 저장공간 
int value[11]; //그래프 그릴때 값들을 저장하는 공간  
int gaesu; //2번 기능에서 막대의 개수 


int main(){
	int i = 1, j;  // i : 반복문 조건에서 True, False 값을 나타내는 변수, j : scanf 구문에서 데이터가 정수형이면 1을, 문자형이면 0을 반환 
	
	//소수점을 2번째자리에서 반올림 
	cout << fixed; 
	cout.precision(1);
	 
	if(file_check == 0){
		printf("입력 파일이 잘못 지정되었거나 문제가 있습니다. \n");
		hold(); //종료 
	}
	
	if(!times){ //최초로 실행될때만 실행 (후에 main함수를 호출할 때 파일에 있는 정보를 불어오는 것과 회귀는 굳이 한번 더 할 필요가 없음) 
		arrangement(); 
		regression();
		times++;
	}
	
	while(i){ //i가 0일때까지 실행 
		int func; //func : 입력 받은 번호를 저장하는 변수 
		
		menu();
		fflush(stdin);
		j = scanf("%d", &func); // 입력한 값 func에 저장
					
		if(func == 1 && j == 1){ // 1을 입력하면 실행 
			compare(); //대기 정보 비교 기능의 함수 
			i = 0;
		}
		else if(func == 2 && j == 1){ // 2를 입력하면 실행 
			expect(); //대기 정보 예측 기능의 함수  
			i = 0;
		}
		else if(func == 3 && j == 1){ // 3을 입력하면 실행 
			variance(); //대기 정보 그래프를 그려주는 기능의 함수  
			i = 0;
		}
		else if(func == 4 && j == 1){ // 4을 입력하면 실행 
			hold();
			i = 0;
		}
	}
	return 0; 
}


void arrangement(){
	int j = 0;
	
	FILE *pFile = NULL;
	
	//데이터 구조 : 시간 초미세먼지량 미세먼지량 오존량 이산화질소량 이산화탄소량 이산화황산량
	//ex) 2022-05-28 30 11 0.057 0.01 0.2 0.002 
	pFile = fopen("대기정보(2020 ~ 2022).txt", "r");
	
	while( !feof( pFile ) ){        
        fscanf(pFile, "%s %s %s %s %s %s %s\n", p[j].when, VF_dust, F_dust, Ozone, NO_2, CO_2, SO_2); //문자열로 받아오기 (데이터 가공중 문제 발생 가능성 방지) 

		p[j].VF_dust = atoi(VF_dust); //int형으로 변환후 구조체에 저장 
		p[j].F_dust = atoi(F_dust);
		p[j].Ozone = atof(Ozone); //float형으로 변환후 구조체에 저장 
		p[j].NO_2 = atof(NO_2);
		p[j].CO_2 = atof(CO_2);
		p[j].SO_2 = atof(SO_2);
		
		j++;
	    }
	
	fclose(pFile);  
}


int find(char *wh_1, char *wh_2){ 
	int i = days, cnt; //i : 전체 데이터 개수, cnt : 위치를 바꾸기 위한 변수 
	
	wh_idx[0] = -1; //초깃값 
	wh_idx[1] = -1;
	
	while(i >= 0){ //모든 정보를 다 탐색할 때까지 반복 
		if(!strcmp(p[i].when, wh_1)) //같다면 실행 
			wh_idx[0] = i;
		
		if(!strcmp(p[i].when, wh_2)) //같다면 실행 
			wh_idx[1] = i;
		
		i--;
	}
	
	if(wh_idx[0] < wh_idx[1]){ //데이터의 특수성 이용, 나중에 읽어오는 정보가 더 옛날 데이터여서 인덱스 값이 크면 더 오래된 데이터임을 활용
		//바꿔주는 과정 
		cnt = wh_idx[0]; 
		wh_idx[0] = wh_idx[1];
		wh_idx[1] = cnt;
	}
	
	if(wh_idx[0] == -1 || wh_idx[1] == -1) { //탐색을 못했을 시에 
		printf("파일에 존재하지 않는 파일이거나 날짜 형식이 잘못되었습니다.");
		fflush(stdin);
		Sleep(1000); 
		return 0;
	}
	
	if(wh_idx[0] == wh_idx[1]){ //두 개의 날짜가 같을시에  
		printf("같은 날짜는 입력할 수 없습니다.");
		fflush(stdin);
		Sleep(1000); 
		return 0;
	}
	return 1; //탐색을 성공했을 시에 
}

int file_check(){ 
	FILE *pFile = NULL;
	
	pFile = fopen("대기정보(2020 ~ 2022).txt", "r"); 
	
    if( pFile == NULL ) {
		return 0;
	}
	
	fclose(pFile);
	
	return 1;	
}

void hold(){
	fflush(stdin);
	printf("\n\n");
	printf("종료하시려면 임의의 키를 입력하세요.\n");
	getchar();
	exit(0);
}


void compare(){
	int i = 0, j = 0;  //i : 올바른 이름이 입력되었는지 확인, j : 증가 감소 판단 함수 
	char store[2][10] = {"감소량은 ", "증가량은 "}; //증가 감소 출력할때 사용 
	
	compare_menu();
	
	i = select_compare(); //올바른 값 입력시 0을 제외한 값 반환, 올바르지 않은 값 입력시 0 반환
	while(!i){ //만약 올바른 이름이 입력이 되었으면 반복문 중단 
		printf("\n올바른 값을 입력해주세요.\n\n\n");
		Sleep(700);
		i = select_compare();
	}
	
	if(i <= 2){ //값들이 정수형일 때   
		printf("\n\n\n%s일 때 %s의 양 : %d ㎍/㎥\n", p[wh_idx[0]].when, air_sort, compare_air1[0]); //날짜가 더 과거인 것을 출력
		printf("%s일 때 %s의 양 : %d ㎍/㎥\n\n", p[wh_idx[1]].when, air_sort, compare_air1[1]);
		
		dx_dy = (double) compare_air1[1] / compare_air1[0];
		
		j = percent_check();
	}
	else if(i > 2){ //값들이 실수형일 때  
		printf("\n%s일 때 %s의 양 : %.3lf ppm\n", p[wh_idx[0]].when, air_sort, compare_air2[0]); //날짜가 더 과거인 것을 출력
		printf("%s일 때 %s의 양 : %.3lf ppm\n\n", p[wh_idx[1]].when, air_sort, compare_air2[1]);
		
		dx_dy = (double) compare_air2[1] / compare_air2[0];			
	
		j = percent_check();
	}
	
	//ex) '초미세먼지의 증가량은 35.1 % 입니다.'  
	cout << air_sort;
	cout << "의 ";
	cout << store[j];
	cout << dx_dy;
	cout << " % 입니다.";
	Sleep(700);
	
	res_check();
}

int select_compare(){
	printf("*선택할 수 있는 것 : 초미세먼지, 미세먼지, 오존, 이산화질소, 이산화탄소, 이산화황산");
	printf("\n\n어떠한 것을 선택할지 입력하여 주세요 : ");
	
	fflush(stdin);
	scanf("%s", air_sort);
	
	if(!strcmp(air_sort, "초미세먼지")){ //'초미세먼지'가 입력되었을 때  
		compare_air1[0] = p[wh_idx[0]].VF_dust;
		compare_air1[1] = p[wh_idx[1]].VF_dust;
		return 1; 
	}	
	else if(!strcmp(air_sort, "미세먼지")){ //'미세먼지'가 입력되었을 때 
		compare_air1[0] = p[wh_idx[0]].F_dust;
		compare_air1[1] = p[wh_idx[1]].F_dust;
		return 2;
	}	
	else if(!strcmp(air_sort, "오존")){ //'오존'가 입력되었을 때 
		compare_air2[0] = p[wh_idx[0]].Ozone;
		compare_air2[1] = p[wh_idx[1]].Ozone;
		return 3; 
	}	
	else if(!strcmp(air_sort, "이산화질소")){ //'이산화질소'가 입력되었을 때 
		compare_air2[0] = p[wh_idx[0]].NO_2;
		compare_air2[1] = p[wh_idx[1]].NO_2;
		return 4;
	}	
	else if(!strcmp(air_sort, "이산화탄소")){ //'이산화탄소'가 입력되었을 때 
		compare_air2[0] = p[wh_idx[0]].CO_2;
		compare_air2[1] = p[wh_idx[1]].CO_2;
		return 5;
	}	
	else if(!strcmp(air_sort, "이산화황산")){ //'이산화황산'가 입력되었을 때 
		compare_air2[0] = p[wh_idx[0]].SO_2;
		compare_air2[1] = p[wh_idx[1]].SO_2;
		return 6;
	}
	return 0;
}

int percent_check(){
	if(dx_dy > 1){ // (이후의 값/이전의 값)이 1을 넘을 때, 즉 값이 증가할 때  
		//퍼센트로 변환  
		dx_dy = (double) dx_dy - 1;
		dx_dy *= 100;
		return 1;
	}	
	else{ // (이후의 값/이전의 값)이 1을 넘지 않을 때, 즉 값이 감소할 때  
		dx_dy = (double) 1 - dx_dy;
		dx_dy *= 100;
		return 0;
	}
}


void expect(){	
	int i = 0, max = 0; //i : 반복 확인 및 어떤 기체를 예측하는지 확인하는 변수, max : 최댓값 
	double result = 0; //결괏값을 저장하는 변수 
	
	while(!i) //i가 0이 아닐때 까지 반복 
		i = expect_menu(); //i = 0 -> 잘못된 값 입력, i = 1 -> 초미세먼지 값 예측, i = 2 -> 미세먼지 값 예측
	
	expect_menu2(i); 
	
	max = max_check(0); //max_check에 0을 넣어주면 최댓값만 구할 수 있음 
	graph(0, max); //3번 기능과 분리해야 되기에 0을 넣어줌 
	
	res_check();
}

void regression(){
	//avg_F : 미세먼지의 평균, avg_VF : 초미세먼지의 평균, x_xavg : x-x평균, y_yvg : y-y평균 
	double avg_F = 0, avg_VF = 0, x_xavg, y_yavg, x_y_avg = 0, x_x_avg = 0; // x_y_avg : Σ(x-x평균)(y-y평균) , x_x_avg : Σ(x-x평균)^2
	int all_F = 0, all_VF = 0, i; //all_f : 모든 미세먼지의 합, all_VF : 모든 초미세먼지의 합 
	
	for(i = 0; i < all_days; i++){ //모든 미세먼지&초미세먼지의 합을 구하는 과정 
		all_VF += p[i].VF_dust;
		all_F += p[i].F_dust;
	}
	
	avg_F = (double) all_F / all_days;
	avg_VF = (double) all_VF / all_days;
	
	for(i = 0; i < all_days; i++){
		x_xavg =  (double) p[i].VF_dust - avg_VF;
		y_yavg = (double) p[i].F_dust - avg_F;
		
		x_y_avg += (double) x_xavg * y_yavg;
		x_x_avg += (double) x_xavg * x_xavg;
	}
	
	dx_dy_2 = x_y_avg / x_x_avg; //최소제곱법에서 기울기 
	y_intercept = (double) avg_F - (avg_VF * dx_dy_2); //최소제곱법에서 y절편 
}

double calcul(int i, int k){ //i = 1이면 초미세먼지 값 구하기 i = 2이면 미세먼지 값 구하기, k : 입력된 값
	// * y = ax + b에서 y 값은 미세먼지, x 값은 초미세먼지  
	double result = 0;
	
	if(i == 1){ // x = (y - b)/a
		result = (double) (k - y_intercept)/dx_dy_2;
		return result;
	}
	else{ // y = ax + b
		result = (double) dx_dy_2 * k + y_intercept;
		return result;
	}
}


void variance(){
	int i = 0;
	int max = 0; //최댓값을 저장하는 공간 
	
	variance_menu();
	
	if(abs(wh_idx[0] - wh_idx[1]) >= 10){ //두 값의 인덱스의 차가 10을 넘을 경우 (그래프가 너무 좌우로 길어지기 때문에) 
		printf("두 날짜의 차이가 10을 넘지 않도록 해주세요.");
		Sleep(1000);
		variance_menu();
	}
	
	i = select_compare();
	
	while(!i){ //만약 올바른 이름이 입력이 되었으면 반복문 중단 
		printf("\n\n올바른 이름을 입력해주세요.\n\n");
		Sleep(1000); 
		i = select_compare();  //다시 호출 
	}
	
	max = max_check(i);
	graph(i, max);
	Sleep(1000);
	
	res_check();
}

int max_check(int i){ 
	int j = 0, k = wh_idx[0], max = 0;
	double cnt;
	
	while(j <= 10){
		if(i == 1){ //초미세먼지를 사용할 때  
			value[j] = p[k].VF_dust * 100;
			k--;
		}
		else if(i == 2){ //미세먼지를 사용할 때 
			value[j] = p[k].F_dust * 100;
			k--;
		}
		else if(i == 3){ //오존을 사용할 때 
			cnt = p[k].Ozone * 100000;
			value[j] = (int) cnt;
			k--;
		}
		else if(i == 4){ //이산화질소를 사용할 때 
			cnt = p[k].NO_2 * 100000;
			value[j] = (int) cnt;
			k--;
		}
		else if(i == 5){ //이산화탄소를 사용할 때 
			cnt = p[k].CO_2 * 100000;
			value[j] = (int) cnt;
			k--;
		}
		else if(i == 6){ //이산화황산을 사용할 때 
			cnt = p[k].SO_2 * 100000;
			value[j] = (int) cnt;
			k--;
		}
		if(k < wh_idx[1]) //모든 날짜 범위를 다 읽었을 때  
			break;
			
		if(max < value[j]) //max 초기화 
			max = value[j];
			
		j++;
	}	
	return max;
}

void graph(int i, int max){ //그래프를 25칸으로 나누어서 출력하기 때문에 하나씩 세세히 그려주지 못함 -> 버림을 해서 그래프를 그려줌 
	//maxx : 그래프에서 축을 나타날 때 축에서 가장 큰 값(기준이 되는 값), max_x = maxx, s_unit : 작은 1칸, unit : 5칸(축에서 값을 나타낼 값) 
	//c_unit : 그래프 그릴 때 사용하는 변수, num_in : 수치를 저장하는 변수 
	//len : 막대의 개수, k/j : 반복문을 사용하기 위한 변수, x : 0이면 ppm값, 1이면 ㎍/㎥값
	//store : 단위를 저장하는 공간, storage : 실제값을 저장할때 인트형을 더블로 받아오기 위한 공간
	int maxx = 0, max_x, unit, s_unit, c_unit = 0;
	int num_in, lens = wh_idx[0] - wh_idx[1] + 1;
	int k, j, x = 0, y[2] = {100,100000}, z[2] = {0, 2};
	double storage = 0;
	char store[2][10] = {"ppm", "㎍/㎥"};
	
	if(i > 2){
		x = 1; //소수점 자릿수 맞추기 (3자리) 
		cout << fixed; 
		cout.precision(3);
	}
	if(i == 0) lens = gaesu; //2번 기능에서 그래프로 넘어올 때  
	
	maxx = max + (1000 - (max % 1000)); //축에서 최댓값 계산 
	
	unit = maxx/5;
	s_unit = maxx/25; //가장 작은 단위를 25로 나누어서 만들기 
	max_x = maxx;
	
	system("mode con cols=170 lines=40");
	
	cout.width(85);
	cout << air_sort;
	
	printf("\n\n");
	
	//단위 표시  
	cout << "단위 : "; 
	cout.width(5);
	cout << store[x] << endl << endl; 
	
	//그래프 그리기 시작 
	for (k = 25; k > 0; k--) {
		if(k % 5 == 0){ //5칸마다 숫자 있을 때 표시  
			cout.width(12);
			storage = (double) maxx/y[x];
			cout << storage;
			cout.width(2);
			cout << "┃";
			maxx -= unit;
		}
		else{ //숫자 없을 때  
			cout.width(14);
			cout << "┃";
		}
		
		for (j = 0; j < lens; j++) {
			num_in = value[j]; //지금 막대로 그려야되는 값 
			int p = max_x - c_unit; //한칸씩 내려가면서 p 값을 정해줌 ex) 최댓값이 250이라면 p는 250, 240, 230, 220, 210 ... 0 이렇게 바뀜
			  
			if (num_in >= p) {  
				if (num_in - p >= 0 && num_in - p < s_unit) { //값 - p 값이 0이상, 제일 작은 단위 미만 
					cout.width(16);
				 	cout << "┏┓"; //시작을 표시하는 ┏┓ 기호 출력
				 }  
				else { 
					cout.width(16);
					cout << "┃┃"; // ┃┃ 기호 출력
				}  
			}
			else { //막대를 비워야되는 경우  
				cout.width(14);
				cout << "";
			}
		}
		c_unit += s_unit;
		cout << endl;
	}
	
	cout.width(12);
	cout << 0; //축에서 0
	 
	cout.width(1);
	cout << "┗━━━━━";
	
	for (k = 0; k < lens; k++) {
		if (value[k] != 0) cout << "━━━━━━━┺┺━━━━━"; //값이 0이아닌 값이 존재한다면  
		else cout << "━━━━━━━━━━━━━━"; //값이 0이라면 
	}
	cout << "━━━━>" << endl;
	
	cout.width(17);
	cout << "";
	
	for (k = 0; k < lens; k++) {
		if(i == 0) break; //2번 기능에서 그래프를 그리면 날짜를 표시할 필요가 없음 
		cout.width(14);
		cout << p[wh_idx[0] - k].when; //날짜 출력 
	}
	printf("\n");
	
	cout.width (15+z[x]); //z변수를 쓴 이유 : 그래프를 예쁘게 그리기위해 들여쓰기를 맞춰줌 
	cout << "실제 값:";
	
	for (k = 0; k < lens; k++) {
		cout.width(11-z[x]); //위의 이유와 동일 
		storage = (double) value[k]/y[x]; 
		cout << storage; //실제 값 
		cout << store[x]; //단위  
	}
}

void res_check(){
	char i[10];
	
	printf("\n\n\n\n");
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); //노란색으로 변경  
	printf("다른 기능 사용 - 1번\n");
	printf("프로그램 종료 2번\n\n");
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //흰색으로 변경  
	printf("값을 입력해주세요 : "); 
	fflush(stdin);
	scanf("%s",  i);
	
	if(i[0] == '1' && i[1] == '\0'){ // 1을 입력하면 실행 
		main();
		return;
	}	
	else if(i[0] == '2' && i[1] == '\0'){ // 2를 입력하면 실행 
		hold();
		return;
	}
	
	printf("\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	printf("올바른 값을 입력해주세요.");
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	
	Sleep(1000);
	res_check(); //재귀(올바르지 않은 값이 입력되었기 때문) 
} 


void menu(){
	system("mode con cols=80 lines=35"); //콘솔창 크기 조정 - 칸(cols):80, 줄(lines):35
	
	printf("* 본 프로그램은 구미시 원평동을 기준으로 제작하였습니다.\n");
	printf("* 2020-01-01부터 2022-05-31까지의 값만 존재합니다.\n\n\n");
	
	printf("===================================================\n");
	printf("             대기 정보 분석 프로그램               \n");
	printf("===================================================\n\n\n"); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // 글씨 초록색 

	printf("다음 기능 중 사용하고 싶은 기능의 번호를 입력해 주세요\n\n"); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // 글씨 흰색

	printf("1. 과거와의 대기 정보와 비교\n");
	printf("2. 미래 대기 정보 예측\n"); 
	printf("3. 일정 날짜 사이에 일정 기체의 변화량 그래프\n");
	printf("4. 프로그램 종료\n\n\n");
	
	printf("사용 하고 싶은 기능 : ");
}

void compare_menu(){
	char wh_1[15], wh_2[15]; //날짜를 받아들이는 배열 
	int check; //날짜가 잘 입력되었으면 1, 잘 입력되지 않았으면 0을 저장하는 변수 
	
	system("mode con cols=90 lines=30");
	
	printf("\n=============================================================\n");
	printf("                   과거와의 대기 정보 비교                   \n");
	printf("=============================================================\n\n\n");
	
	printf("비교하고 싶은 날짜를 연도-월-일 순으로 입력해 주세요\n");
	printf("ex) 2020년 6월 1일 -> 2020-06-01\n\n\n");
	fflush(stdin);
	
	printf("날짜를 입력하여 주세요 : ");
	scanf("%s", wh_1);
	printf("비교하고 싶은 날짜를 입력하여 주세요 : " );
	scanf("%s", wh_2);
	printf("\n\n");
	
	check = find(wh_1, wh_2); 
	
	if(!check) //날짜가 올바르게 찾아지지 않았을 경우 
		compare_menu();
} 

int expect_menu(){
	int func, i; //func : 입력 받은 번호를 저장하는 변수
	
	system("mode con cols=80 lines=30");

	printf("*대기는 외부 요인을 많이 받기 때문에 정확하지 않을 수도 있습니다.\n");
	printf("*초미세먼지와 미세먼지에 한해서만 위 기능을 제공합니다.\n\n\n");
		
	printf("=============================================================\n");
	printf("                        대기 정보 예측                       \n");
	printf("=============================================================\n\n\n");
	
	printf("초미세먼지나 미세먼지의 정보를 입력시키면 예측해주는 기능입니다.\n\n");
	printf("초미세먼지를 예측하고 싶으면 1번, \n미세먼지를 예측하고 싶으면 2번을 입력해주세요.\n\n\n");
	
	printf("값을 입력하여 주세요 : ");
	fflush(stdin);
	i = scanf("%d", &func); 
	
	if(func == 1 && i == 1) return 1; //받은 값이 1일때  
	if(func == 2 && i == 1) return 2; //받은 값이 2일때
	
	printf("\n1 또는 2를 입력해주세요");
	Sleep(800);
}

int expect_menu2(int i){
	//cal : 값들을 받아오는 변수, n : 갯수를 받아오는 변수, p : 처음에는 문자열인지 숫자인지 판단, 후에 인덱스 값으로 활용 
	//q : 정수인지 문자열인지 판단  
	int cal, n, p, q;
	double result;
	
	system("mode con cols=65 lines=30");
	
	do{
		printf("예측하고 싶은 데이터의 개수(최대 10)를 입력해주세요\n");
		printf("-->");
		fflush(stdin);
		p = scanf("%d",&n); //문자열이 입력되면 p = 0, 숫자가 입력되면 p = 1 
		if(n > 10)
			p = 0;
	}
	while(!p);
	
	gaesu = n; 
	p = 0; //밑에 value 배열에 집어넣을 때 인덱스 값으로 활용하기 위해  
	
	printf("*주의사항 : 값의 차이가 너무 크면 그래프가 이상하게 그려져요!");
	printf("*소수점은 인식이 되지 않습니다.\n\n"); 
	
	while(n > 0){
		if(i == 1){ //1일때 즉, 앞선 값에서 초미세먼지를 선택했을 때   
			printf("%d번째 미세먼지 값(ppm)을 입력해주세요 : ",p+1); 
			
			fflush(stdin);
			q = scanf("%d", &cal);
			
			strcpy(air_sort, "초미세먼지"); 
			
			if(cal < 5 || cal >= 1500 || q == 0){ //q = 0 : 문자열이 입력되었을 때  
				printf("\n");
				printf("5ppm 이상 1500ppm 미만의 값을 입력하여 주세요\n\n"); 
				Sleep(1000);
				continue;
			}
		}
		else{ //그외에 상황(i가 2일때) 
			printf("%d번째 초미세먼지 값(ppm)을 입력해주세요 : ",p+1); 
			
			fflush(stdin);
			q = scanf("%d", &cal);
			
			strcpy(air_sort, "미세먼지"); 
			
			if(cal < 5 || cal >= 1500 || q == 0){
				printf("\n");
				printf("5ppm 이상 1500ppm 미만의 값을 입력하여 주세요\n\n"); 
				Sleep(1000);
				continue;
			}
		}
		result = calcul(i, cal); //값을 받아오고 이 값을 바탕으로 회귀분석 
		value[p] = (int) result * 100; //100을 곱해서 저장 
		p++;
		n--;
	}
}

void variance_menu(){
	char wh_1[15], wh_2[15]; //날짜를 받아들이는 배열 
	int check; //날짜가 잘 입력되었으면 1, 잘 입력되지 않았으면 0을 저장하는 변수
	
	system("mode con cols=80 lines=30");
	
	printf("*두 날짜의 차이가 10일을 넘지 않도록 해주세요.\n\n\n");
	
	printf("\n=============================================================\n");
	printf("            일정 날짜 사이에 대기정보 변화량 그래프        \n");
	printf("=============================================================\n\n\n");
	
	printf("날짜 2개와 기체를 입력하면 대기 정보 변화량을 보여주는 기능입니다.\n\n"); 
	printf("비교하고 싶은 날짜를 연도-월-일 순으로 입력해 주세요\n");
	printf("ex) 2020년 6월 1일 -> 2020-06-01\n\n\n");
	
	printf("날짜를 입력하여 주세요 : ");
	scanf("%s", wh_1);
	printf("두번 날짜를 입력하여 주세요 : " );
	scanf("%s", wh_2);
	printf("\n\n");
	
	check = find(wh_1, wh_2); 
	
	if(!check) //날짜가 올바르게 찾아지지 않았을 경우 
		variance_menu();
} 
