package fan.akua.hakka.demo;

//import fan.akua.hakka.PlayerEntry.aidl; // 导入 Test 类
import fan.akua.hakka.PlayerEntry;

interface IClient {

    void searchStart() ;
    void searchEnd(int size);
    void wallHackStart();
    void wallHackEnd();
    void wallHackLoop(in List<PlayerEntry> entrys);
}