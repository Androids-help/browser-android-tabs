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

    public BraveRewardsNativeWorker() {
        observers_ = new ArrayList<BraveRewardsObserver>();
    }

    public void Init() {
      if (mNativeBraveRewardsNativeWorker == 0) {
          nativeInit();
      }
    }

    public void Destroy() {
        if (mNativeBraveRewardsNativeWorker != 0) {
            nativeDestroy(mNativeBraveRewardsNativeWorker);
            mNativeBraveRewardsNativeWorker = 0;
        }
    }

    public void AddObserver(BraveRewardsObserver observer) {
        // TODO add synchronization if ever use it from diff threads
        observers_.add(observer);
    }

    public void RemoveObserver(BraveRewardsObserver observer) {
        // TODO add synchronization if ever use it from diff threads
        observers_.remove(observer);
    }

    public void CreateWallet() {
      nativeCreateWallet(mNativeBraveRewardsNativeWorker);
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

    private native void nativeInit();
    private native void nativeDestroy(long nativeBraveRewardsNativeWorker);
    private native void nativeCreateWallet(long nativeBraveRewardsNativeWorker);
}
