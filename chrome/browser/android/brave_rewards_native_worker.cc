/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "brave_rewards_native_worker.h"
#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "brave/components/brave_rewards/browser/rewards_service.h"
#include "brave/components/brave_rewards/browser/rewards_service_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "jni/BraveRewardsNativeWorker_jni.h"

namespace chrome {
namespace android {

BraveRewardsNativeWorker::BraveRewardsNativeWorker(JNIEnv* env, const base::android::JavaRef<jobject>& obj):
  weak_java_brave_rewards_native_worker_(env, obj),
  brave_rewards_service_(nullptr) {
    Java_BraveRewardsNativeWorker_setNativePtr(env, obj, reinterpret_cast<intptr_t>(this));

  brave_rewards_service_ = brave_rewards::RewardsServiceFactory::GetForProfile(
      ProfileManager::GetActiveUserProfile()->GetOriginalProfile());
  if (brave_rewards_service_) {
    brave_rewards_service_->AddObserver(this);
  }
}

BraveRewardsNativeWorker::~BraveRewardsNativeWorker() {
}

void BraveRewardsNativeWorker::Destroy(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller) {
  if (brave_rewards_service_) {
    brave_rewards_service_->RemoveObserver(this);
  }
  delete this;
}

void BraveRewardsNativeWorker::CreateWallet(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller) {
  if (brave_rewards_service_) {
    brave_rewards_service_->CreateWallet();
  }
}

void BraveRewardsNativeWorker::OnWalletInitialized(brave_rewards::RewardsService* rewards_service,
        int error_code) {
  JNIEnv* env = base::android::AttachCurrentThread();
  return Java_BraveRewardsNativeWorker_OnWalletInitialized(env, 
        weak_java_brave_rewards_native_worker_.get(env), error_code);
}

static void JNI_BraveRewardsNativeWorker_Init(JNIEnv* env, const
    base::android::JavaParamRef<jobject>& jcaller) {
  new BraveRewardsNativeWorker(env, jcaller);
}

}
}
