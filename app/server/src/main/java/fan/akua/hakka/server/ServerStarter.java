package fan.akua.hakka.server;

import android.content.Context;
import android.os.Build;
import android.os.FileUtils;
import android.os.UserManager;
import android.system.ErrnoException;
import android.system.Os;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public final class ServerStarter {
    private static volatile boolean hasWritten;
    private static volatile String command;

    public static String writeFiles(Context context) throws IllegalStateException, IOException, ErrnoException {
        if (hasWritten) {
            return command;
        }

        UserManager um = (UserManager) context.getSystemService(Context.USER_SERVICE);
        boolean unlocked = um.isUserUnlocked();
        if (!unlocked) {
            throw new IllegalStateException("User is locked");
        }
        File filesDir;
        filesDir = context.createDeviceProtectedStorageContext().getFilesDir();
        Os.chmod(filesDir.getAbsolutePath(), 457);
        String starterPath = dumpAssets(context, "/starter", new File(filesDir, "starter"));

        command = writeStartSH(context, new File(filesDir, "start.sh"), starterPath);
        Os.chmod(starterPath, 420);
        Os.chmod(command, 420);
        hasWritten = true;
        return command;
    }

    private static String writeStartSH(Context context, File outPut, String startPath) throws IOException {
        if (!outPut.exists()) {
            outPut.createNewFile();
        }
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(context.getResources().openRawResource(R.raw.start)));
        PrintWriter printWriter = new PrintWriter(new FileWriter(outPut));
        while (true) {
            String readLine = bufferedReader.readLine();
            if (readLine != null) {
                printWriter.println(
                        readLine.replace("@IN_START@", startPath)

                                .replace("@STARTER_PATH@", "/data/local/tmp/simonStarter")
                                .replace("@APP_PATH@", context.getApplicationInfo().sourceDir)
                                .replace("@SERVER_NAME@", ServerConstants.SERVER_NAME)
                                .replace("@CLASS_NAME@", ServerConstants.CLASS_NAME)
                );
            } else {
                printWriter.flush();
                printWriter.close();
                return outPut.getAbsolutePath();
            }
        }
    }

    private static String dumpAssets(Context context, String inPut, File outPut) throws IOException {
        String str = "assets/" + Build.SUPPORTED_ABIS[0] + inPut;
        ZipFile zipFile = new ZipFile(context.getApplicationInfo().sourceDir);
        Enumeration<? extends ZipEntry> entries = zipFile.entries();
        while (entries.hasMoreElements()) {
            ZipEntry zipEntry = entries.nextElement();
            if (zipEntry.getName().equals(str)) {
                InputStream inputStream = zipFile.getInputStream(zipEntry);
                FileOutputStream fileOutputStream = new FileOutputStream(outPut);
                if (Build.VERSION.SDK_INT < 29 || !(inputStream instanceof FileInputStream)) {
                    byte[] bf = new byte[2048];
                    int len;
                    while ((len = inputStream.read(bf)) > 0) {
                        fileOutputStream.write(bf, 0, len);
                    }
                    inputStream.close();
                    fileOutputStream.close();
                } else {
                    FileUtils.copy(((FileInputStream) inputStream).getFD(), fileOutputStream.getFD());
                }
            }
        }
        return outPut.getAbsolutePath();
    }
}