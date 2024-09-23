package fan.akua.hakka.demo;

//import fan.akua.hakka.Entry; // 导入 Test 类
parcelable fan.akua.hakka.PlayerEntry;

interface IClient {

    void searchStart() ;
    void searchEnd(int size);

    void wallHackStart();
    void wallHackEnd();
    void wallHackLoop(out List<fan.akua.hakka.PlayerEntry> entrys);
}