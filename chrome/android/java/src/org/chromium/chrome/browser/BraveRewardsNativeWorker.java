/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser;

import org.chromium.base.Log;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.chrome.browser.BraveRewardsObserver;

import java.util.ArrayList;
import java.util.List;

@JNINamespace("chrome::android")
public class BraveRewardsNativeWorker {
    private List<BraveRewardsObserver> observers_;
    private long mNativeBraveRewardsNativeWorker;

    private static BraveRewardsNativeWorker instance;
    private static final Object lock = new Object();

    public static  BraveRewardsNativeWorker getInstance(){
        synchronized(lock) {
          if(instance == null){
              instance = new BraveRewardsNativeWorker();
              instance.Init();
          }
        }
        return instance;
    }

    private BraveRewardsNativeWorker() {
        observers_ = new ArrayList<BraveRewardsObserver>();
    }

    private void Init() {
      if (mNativeBraveRewardsNativeWorker == 0) {
          nativeInit();
      }
    }

    @Override
    protected void finalize() {
        Destroy();
    }

    private void Destroy() {
        if (mNativeBraveRewardsNativeWorker != 0) {
            nativeDestroy(mNativeBraveRewardsNativeWorker);
            mNativeBraveRewardsNativeWorker = 0;
        }
    }

    public void AddObserver(BraveRewardsObserver observer) {
        synchronized(lock) {
            observers_.add(observer);
        }
    }

    public void RemoveObserver(BraveRewardsObserver observer) {
        synchronized(lock) {
            observers_.remove(observer);
        }
    }

    public void CreateWallet() {
        synchronized(lock) {
            nativeCreateWallet(mNativeBraveRewardsNativeWorker);
        }
    }

    public boolean WalletExist() {
        synchronized(lock) {
            return nativeWalletExist(mNativeBraveRewardsNativeWorker);
        }
    }

    public void GetWalletProperties() {
        synchronized(lock) {
            nativeGetWalletProperties(mNativeBraveRewardsNativeWorker);
        }
    }

    public double GetWalletBalance() {
        synchronized(lock) {
            return nativeGetWalletBalance(mNativeBraveRewardsNativeWorker);
        }
    }

    public double GetWalletRate(String rate) {
        synchronized(lock) {
            return nativeGetWalletRate(mNativeBraveRewardsNativeWorker, rate);
        }
    }

    public void GetPublisherInfo(int tabId, String host) {
        synchronized(lock) {
            nativeGetPublisherInfo(mNativeBraveRewardsNativeWorker, tabId, host);
        }
    }

    public String GetPublisherURL(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherURL(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public String GetPublisherFavIconURL(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherFavIconURL(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public String GetPublisherName(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherName(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public String GetPublisherId(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherId(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public int GetPublisherPercent(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherPercent(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public boolean GetPublisherExcluded(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherExcluded(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public boolean GetPublisherVerified(int tabId) {
        synchronized(lock) {
            return nativeGetPublisherVerified(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public void IncludeInAutoContribution(int tabId, boolean exclude) {
        synchronized(lock) {
            nativeIncludeInAutoContribution(mNativeBraveRewardsNativeWorker, tabId, exclude);
        }
    }

    public void RemovePublisherFromMap(int tabId) {
        synchronized(lock) {
            nativeRemovePublisherFromMap(mNativeBraveRewardsNativeWorker, tabId);
        }
    }

    public void GetCurrentBalanceReport() {
        synchronized(lock) {
            nativeGetCurrentBalanceReport(mNativeBraveRewardsNativeWorker);
        }
    }

    public void Donate(String publisher_key, int amount, boolean recurring) {
        synchronized(lock) {
            nativeDonate(mNativeBraveRewardsNativeWorker, publisher_key, amount, recurring);
        }
    }

    @CalledByNative
    public void OnGetCurrentBalanceReport(String[] report) {
        for(BraveRewardsObserver observer : observers_) {
            observer.OnGetCurrentBalanceReport(report);
        }
    }

    @CalledByNative
    private void setNativePtr(long nativePtr) {
        assert mNativeBraveRewardsNativeWorker == 0;
        mNativeBraveRewardsNativeWorker = nativePtr;
    }

    @CalledByNative
    public void OnWalletInitialized(int error_code) {
        for(BraveRewardsObserver observer : observers_) {
            observer.OnWalletInitialized(error_code);
        }
    }

    @CalledByNative
    public void OnPublisherInfo(int tabId) {
        for(BraveRewardsObserver observer : observers_) {
            observer.OnPublisherInfo(tabId);
        }
    }

    @CalledByNative
    public void OnWalletProperties(int error_code) {
        for(BraveRewardsObserver observer : observers_) {
            observer.OnWalletProperties(error_code);
        }
    }

    private native void nativeInit();
    private native void nativeDestroy(long nativeBraveRewardsNativeWorker);
    private native void nativeCreateWallet(long nativeBraveRewardsNativeWorker);
    private native boolean nativeWalletExist(long nativeBraveRewardsNativeWorker);
    private native void nativeGetWalletProperties(long nativeBraveRewardsNativeWorker);
    private native double nativeGetWalletBalance(long nativeBraveRewardsNativeWorker);
    private native double nativeGetWalletRate(long nativeBraveRewardsNativeWorker, String rate);
    private native void nativeGetPublisherInfo(long nativeBraveRewardsNativeWorker, int tabId, String host);
    private native String nativeGetPublisherURL(long nativeBraveRewardsNativeWorker, int tabId);
    private native String nativeGetPublisherFavIconURL(long nativeBraveRewardsNativeWorker, int tabId);
    private native String nativeGetPublisherName(long nativeBraveRewardsNativeWorker, int tabId);
    private native String nativeGetPublisherId(long nativeBraveRewardsNativeWorker, int tabId);
    private native int nativeGetPublisherPercent(long nativeBraveRewardsNativeWorker, int tabId);
    private native boolean nativeGetPublisherExcluded(long nativeBraveRewardsNativeWorker, int tabId);
    private native boolean nativeGetPublisherVerified(long nativeBraveRewardsNativeWorker, int tabId);
    private native void nativeIncludeInAutoContribution(long nativeBraveRewardsNativeWorker, int tabId,
      boolean exclude);
    private native void nativeRemovePublisherFromMap(long nativeBraveRewardsNativeWorker, int tabId);
    private native void nativeGetCurrentBalanceReport(long nativeBraveRewardsNativeWorker);
    private native void nativeDonate(long nativeBraveRewardsNativeWorker, String publisher_key, 
        int amount, boolean recurring);
}
