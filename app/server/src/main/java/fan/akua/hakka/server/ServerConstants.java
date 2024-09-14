package fan.akua.hakka.server;

import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public interface ServerConstants {
    /**
     * [Server]
     **/
    String SERVER_NAME = "hakka_server";
    String CLASS_NAME = HakkaServer.class.getPackage().getName() + "." + HakkaServer.class.getSimpleName();
    String LOG_PATH = "/data/local/tmp";
    /**
     * [Manager]
     */
    String PACKAGE_NAME_MANAGER = "fan.akua.hakka.demo";


    ArrayList<String> logs = new ArrayList<>();

    static void log(String message) {
        synchronized (logs) {
            logs.add(message);
        }
        Log.e("simonServer", message);
    }

    static void printLog(File file) {
        synchronized (logs) {
            try (BufferedWriter writer = new BufferedWriter(new FileWriter(file))) {
                // 将列表中的每个字符串逐行写入文件
                for (String s : logs) {
                    writer.write(s);
                    writer.newLine();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    static void clearLog() {
        synchronized (logs) {
            logs.clear();
        }
    }
}