package fan.akua.hakka.server;

import android.os.FileObserver;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

/**
 * 监听manager是否被卸载
 */
public class ApkChangedObservers {
    public interface ApkChangedListener {
        void onApkChanged();
    }

    private static final Map<String, ApkChangedObserver> observers = Collections.synchronizedMap(new HashMap<>());

    public static void start(String apkPath, ApkChangedListener listener) {
        String path = new File(apkPath).getParent();
        ApkChangedObserver observer = observers.get(path);
        if (observer == null) {
            observer = new ApkChangedObserver(path);
            observer.startWatching();
            observers.put(path, observer);
        }
        observer.addListener(listener);
    }

    public static void stop(ApkChangedListener listener) {
        List<String> pathToRemove = new ArrayList<>();
        observers.forEach((path, observer) -> {
            observer.removeListener(listener);
            if (!observer.hasListeners()) {
                pathToRemove.add(path);
            }
        });
        pathToRemove.forEach(path -> Objects.requireNonNull(observers.remove(path)).stopWatching());
    }

    private static class ApkChangedObserver extends FileObserver {

        public ApkChangedObserver(String path) {
            super(path, DELETE);
        }

        private final Set<ApkChangedListener> listeners = new HashSet<>();

        private void addListener(ApkChangedListener listener) {
            listeners.add(listener);
        }

        private void removeListener(ApkChangedListener listener) {
            listeners.remove(listener);
        }

        private boolean hasListeners() {
            return !listeners.isEmpty();
        }

        @Override
        public void onEvent(int event, String path) {
            if ((event & 0x00008000) != 0 || path == null) {
                return;
            }

            if (path.equals("base.apk")) {
                stopWatching();
                listeners.forEach(ApkChangedListener::onApkChanged);
            }
        }
    }
}
